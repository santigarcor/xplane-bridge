# CLAUDE.md

Knowledge for working on this repository. Keep it current when the architecture, protocol or rules change.

## What this is

A home-built Boeing-style MCP + lights panel driven by X-Plane 12. The panel is an **Arduino Mega 2560** (shows up as `Arduino Mega 2560 (COMx)`, `USB\VID_2341&PID_0042`, usually COM3) with:

- 6 rotary encoders (course 1/2, speed, heading, altitude, vertical speed)
- 31 switches: MCP buttons (momentary), FD/AT/disengage and exterior lights (toggles)
- 5 MAX7219 8-digit displays (LedControl on pins DIN 48, CLK 52, CS 50)
- 17 LEDs (MCP annunciators, FD lights)

Supported aircraft: Zibo 737, FlightFactor 757, FlightFactor 777, Laminar C172 (steam gauges) and C172 G1000. The author previously built [xplane_bae146_mcu](https://github.com/santigarcor/xplane_bae146_mcu) for the BAe 146. The user is the only developer, uses Windows (also macOS for the Node bridge) and flies with X-Plane launched from Steam.

## Two generations, one repo

1. **Node bridge** (`src/`, TypeScript): a separate process. It talks to X-Plane's web API (WebSocket + REST at `ws://HOST:PORT/api/v2`, configured in `.env`) and to the Arduino with `serialport`. FlyWithLua starts it via `XplaneBridge.lua` → `start_bridge.ps1` / `start_bridge.sh`. Built with `npm run bundle` (esbuild → `dist/bridge.js`).
2. **Native plugin** (`plugin/`, C++23): the replacement and the target going forward. It runs inside X-Plane, talks to the SDK directly and opens the serial port itself, which removes the WebSocket/REST/process/Lua failure modes behind the disconnections.

The Node bridge stays until the plugin is proven in real flights. Then remove `src/`, `XplaneBridge.lua`, the start scripts and the npm setup. **Never run both**: only one can open the COM port (the plugin reports `COMx in use (is the Node bridge running?)`).

Mapping changes currently have to be made in both `src/mappings/*.ts` and `plugin/src/include/mappings/profiles/*.cpp`. The C++ profiles are line-by-line ports, so keep them in sync until Node is removed.

## Serial protocol (contract with `arduino/arduino.ino`)

57600 baud, 8N1, line-delimited JSON, `\n` terminated.

Arduino → host:
- `{"user_input":"<name>"}`. Momentary switches send `<name>`, toggles send `<name>_on` / `<name>_off`, encoders send `<encoder>_increment` / `<encoder>_decrement`.
- `{"hb":<uptime seconds>}` every 2 s. Proves the link is alive. An uptime that goes backwards means the board reset.
- `Ready for input...` at the end of `setup()`. **Don't change this text**: the host waits for it (substring match, since bootloader garbage can precede it).
- Plain text lines (e.g. `POLLING system initialized`) are logged and ignored.

Host → Arduino: `{"cmd":"<name>","value":<number>}`
- `set_*` drives a display (`set_speed`, `set_heading`, `set_altitude`, `set_vertical_speed`, `set_course_1`, `set_course_2`)
- `toggle_display_set_*` blanks (0) or shows (1) a display
- `power` turns the whole panel off/on (displays shut down, LEDs held off, state kept)
- any other name is an LED (`n1_led`, `flight_director_1_led`, ...)

Numbers are sent like JavaScript's `JSON.stringify` (integers without a decimal point). Opening the port resets the board through DTR, and it takes ~2 s to boot.

The sketch reads one line per `loop()` and has a 64-byte receive buffer (under two lines). A display update is several ms of bit-banged SPI, so lines written back to back get lost silently: corrupt JSON is ignored, and there are no acks. The plugin therefore writes at most one line every 25 ms, and replaces a queued line for the same `cmd` instead of queueing another. (A periodic resend of the stored values was tried and removed at the user's request.)

When X-Plane quits or the plugin is disabled, the plugin sends `power 0` right before closing the port, so the panel goes dark until the next session connects. Nothing is sent on restarts, reconnects or unsupported aircraft (the user's choice). The sketch also boots dark (displays cleared, LEDs off), so a reset caused by closing the port leaves the panel dark too.

## Mappings

Each aircraft has an initializer that calls the bridge's mapping API (same names in TS and C++):

| Call | Meaning |
|---|---|
| `addDataRef(name, {cmd, threshold, parser})` | Send `cmd` when the parsed dataref value changes by at least `threshold` |
| `addBooleanDataRef(name, cmd)` | Value > 0 → 1, else 0 |
| `addValueMapDataRef(name, cmd, map)` | Replace mapped values (broken in TS, see known issues) |
| `addToggleSwitchInputDataRef(switch, refs, inverse)` | `_on`/`_off` write 1/0 to the datarefs |
| `addToggleSwitchInputCommands(switch, on, off?)` | `_on`/`_off` run commands; without `off`, `on` runs both ways |
| `addMomentarySwitchInputCommand(switch, cmds, duration)` | Run commands, held `duration` s (0 = press and release) |
| `addMomentarySwitchInputDataRef(switch, refs, value \| TOGGLE)` | Write a value, or flip 0↔1 |
| `addRotaryEncoderCommands(encoder, inc, dec)` | `_increment`/`_decrement` run commands |

Parsers: `toBoolean`, `round` (JS `Math.round`), `toDegrees`, `valueMap`, `atIndex(i)` for array datarefs, `pipe(...)` to chain, or any custom lambda. Stateful parsers are created inside the initializer so they reset on re-detection. Example: the 757 FD master LED, where only the first FD switched on lights up.

Aircraft detection matches the aircraft path against case-insensitive patterns, **in order**: `737` → zibo_737, `757` → ff_757, `777` → ff_777, `Cessna_172SP_G1000` → c172_g1000 (must precede the next one), `Cessna_172SP.acf` → c172. Node matches the `sim/aircraft/view/acf_relative_path` dataref. The plugin matches the path from `XPLMGetNthAircraftModel(0)` made relative to the X-Plane folder, so a digit in the install path can't match.

**Adding an aircraft:**
- Node: add `src/mappings/<id>.ts`, a pattern in `detector.ts`, and an entry in `types.ts` and `index.ts`.
- Plugin: add `plugin/src/include/mappings/profiles/<id>-mappings.cpp`, declare its init function in `aircraft-mappings.h`, and add a row to `kProfiles` in `aircraft-mappings.cpp`. New `.cpp` files are picked up automatically, but re-run cmake (`build.ps1` does).

## Native plugin

### Rules (non-negotiable)

- **The sim never blocks.** Nothing on X-Plane's main thread may wait on I/O, sleep or hold a lock across I/O. All serial work (discovery, open, read, write, close, reconnect) happens on the single worker thread in `SerialLink`. The main thread only swaps queues under short locks.
- **XPLM calls only on the main thread.** That includes `XPLMDebugString`, which aborts X-Plane if called from another thread. Log through `Logger` (`logger.h`), which queues off-main-thread lines and flushes them from a flight loop.
- **No leaks.** No raw `new`/`delete`; RAII for every Win32 handle (`UniqueHandle`, `DevInfoList`). Every XPLM resource has a symmetric release: flight loops destroyed, command handlers unregistered, menus destroyed. Everything is owned by `AppState`, created in `XPluginEnable` and destroyed in `XPluginDisable`, so Plugin Admin disable/enable cycles start from scratch.
- **Commands are never left held**: `Bridge` ends held commands when their time is up, on aircraft change/unload and in its destructor.
- **Exceptions never cross into X-Plane**: every callback (flight loop, menu, command, plugin entry points) catches.
- XPLM420+ only (X-Plane 12.3.0+), Windows only, 64-bit.
- The structure follows [winctrl-xplane-plugin](https://github.com/rswilem/winctrl-xplane-plugin) (local checkout: `C:\Users\Santiago\Repos\winctrl-xplane-plugin`): CMake with the `add_xplane_plugin` shape, MinGW-w64 GCC, `src/main.cpp` + `src/include/**` with flat includes, kebab-case file names, `win_x64/<name>.xpl` output, `.clang-format`. **winctrl is GPL-3 and this repo is MIT: follow its patterns, never copy its code.**

### Layout

```
plugin/
  CMakeLists.txt, toolchain-win.cmake, build.ps1, .clang-format
  SDK/                         X-Plane SDK, not committed
  src/main.cpp                 XPlugin* entry points, routes plane/dataref messages to AppState
  src/include/
    config.h                   names, version, default baud
    appstate.*                 owns everything; flight loop (every frame), menu, commands, status text
    serial/serial-link.*       worker thread + state machine (port of ArduinoSerialCommunicator.ts)
    serial/serial-ports.*      SetupAPI COM port discovery
    bridge/bridge.*            port of XPlaneBridge.ts: mapping API, dataref polling, inputs
    bridge/value.h, parsers.h  Value = variant<double, vector<double>, string>; parser library
    bridge/protocol.*          line JSON encode/decode
    mappings/                  aircraft table, detection, one profile per aircraft
    utils/                     logger, preferences (INI), plugins menu, Win32 helpers
  tests/                       host test: stub XPLM + real serial code
```

### Serial state machine (same constants as the Node version)

Discover the port (manufacturer or instance ID contains `arduino|usb|serial`, or the `port=` preference) → open (DTR on, so the board resets) → **Connecting** → the boot banner or first heartbeat → **Connected** (state is sent to the board) → a banner or backwards uptime while connected means **reset** (state resent) → 6 s without data closes the port → reconnect every 5 s. If `CloseHandle` hangs for more than 3 s (wedged USB driver), the status shows "not responding, replug the USB cable". `stop()` waits at most 3 s. If the worker is stuck, it is left running: it shares ownership of its state and pins the DLL (`GetModuleHandleEx` + `FreeLibraryAndExitThread`), so destroying or unloading the plugin is still safe.

### Behaviour differences from the Node bridge (intentional)

- Datarefs are **polled every frame** instead of pushed by X-Plane. An unchanged value is never resent, even with threshold 0. Node relied on X-Plane only sending changes.
- Parsers run every frame, so stateful parsers see the latest values. This fixes a Node case where the 757 FO FD LED didn't light when the captain FD (master) was switched off.
- Datarefs that don't exist yet are retried every 5 s and on `XPLM_MSG_DATAREFS_ADDED`; the rest work right away. Node waited until all of them existed. Missing ones are reported after a 10 s grace period.
- An unsupported aircraft clears the mappings. Node kept the previous aircraft's mappings active.
- Writes to the board are paced and coalesced (see the serial protocol). Node wrote everything back to back, which lost lines in the burst when an aircraft finished loading. The panel then showed stale values until a restart or reload resent everything.
- Byte datarefs are read as raw text. The web API sent them base64 encoded, so `BASE64DECODE` has no C++ equivalent.

### Build, test, install

- Toolchain: MSYS2 MINGW64 (`C:\msys64\mingw64\bin`, GCC 16, cmake, ninja). The runtime is statically linked, so the `.xpl` only imports system DLLs and `XPLM_64.dll`.
- `plugin\build.ps1` builds `build\win\win_x64\xplane_bridge.xpl` (with debug info) plus a `.stripped.xpl`. With `$env:XPLANE_DIR` set, it installs the stripped one to `<X-Plane>\Resources\plugins\xplane_bridge\win_x64\` (X-Plane must be closed).
- Each build also checks the stripped `.xpl` (5 `XPlugin*` exports, only system DLLs plus `XPLM_64.dll`) and creates `plugin\dist\xplane_bridge\` (`win_x64\xplane_bridge.xpl` + `README.txt` from `plugin\INSTALL.txt`) and `plugin\dist\xplane_bridge.zip`. That's the copy-and-paste install folder, the same layout as the verdant-grass repo.
- `plugin\build.ps1 -Test` builds and runs `host-test.exe`: protocol/parser unit checks, bridge logic against a fake sim, and, with the Arduino plugged in, connect/threshold/FD/restart/stop-latency/leak checks against the real board. It resets the board and shows values on its displays.
- PowerShell 5.1 gotchas: quote every `-DNAME=value` argument (an unquoted `-DX=a.b` is split at the dot), and don't run native tools under `$ErrorActionPreference = 'Stop'` (stderr warnings become exceptions).

### Runtime

- Menu: **Plugins → XPlane Bridge**: Status line, Aircraft line (`label (found/mapped datarefs)`), Restart serial connection, Reload aircraft mappings, Debug logging (persisted).
- Commands: `xplane_bridge/restart_serial`, `xplane_bridge/reload_mappings`.
- Status meanings: `Connected (COMx)`; `Searching for Arduino...` (no matching port); `Connecting (COMx, waiting for boot)...`; `ERROR - COMx in use (is the Node bridge running?)`; `ERROR - no data from COMx for 6s, reconnecting` (board silent: sketch not running or USB stalled); `ERROR - connection lost on COMx, reconnecting` (unplugged); `ERROR - COMx not responding, replug the USB cable`; `ERROR - N datarefs not found (see log)`; `ERROR - aircraft not supported: <acf>`. Link errors stay until the next successful connection or a manual restart.
- Logs: X-Plane's `Log.txt` (lines prefixed `[xplane_bridge]`) and `Resources\plugins\xplane_bridge\logs\bridge-<start>.log` (last 10 kept).
- Preferences: `Output\preferences\xplane_bridge.ini` (`baud`, `port`, `debug_logging`).

## Lessons learned

- node launched by FlyWithLua's `os.execute` inherited X-Plane's handles, kept `xplane.replay` open and broke replay mode. It was fixed by launching through `Start-Process` without redirection (commit cbdef6c). The plugin has no child process.
- X-Plane launched from Steam has a stale PATH, so `start_bridge.ps1` finds node via `NVM_SYMLINK` in the registry.
- USB-serial drivers can stop responding: `close()` may never return, and reads may return nothing forever without an error. Hence the heartbeat liveness check, and closing on the worker with a watchdog.
- The bytes right after a DTR reset can be garbage, hence the substring match on the boot banner and dropping over-long lines.
- Add-on datarefs (FF, Zibo) only exist after the aircraft's own plugin has loaded, sometimes seconds after the plane.
- The FF 777's `bus_load_amps[0]` reads 56 with power on and 55 off, which is why its power parser uses `> 55`.

## Known issues

- `src/mappings/ff_757.ts` maps `co`, but the sketch sends `c_o`, so the C/O button does nothing on the 757 in Node. Fixed in the C++ profile.
- 757: `speed_intv`, `alt_intv` and `cws_b` are unmapped (pressing them logs "Unknown Arduino input"), and `cws_a` runs the CMD L command, the same as `cmd_a`.
- TS `valueMap` in `src/bridge/helpers.ts` returns a function instead of a value (unused today). The C++ version works.
- 777: `hdg_sel` might need `mcpHdgHoldButton_button` instead of `mcpHdgCelButton_button` (open question in the mapping).
- C172 profiles reuse panel switches for unrelated systems (fuel pump, avionics, pitot heat), on purpose.
- The plugin hasn't been flown yet. Open checks: function in each aircraft, unplug/replug mid-flight, Arduino reset button, Plugin Admin disable/enable leak check (Process Explorer handles/threads), clean quit with the USB cable pulled.

## Conventions

- English for code, comments, logs and docs.
- Commit subjects are short and imperative ("Map 757 vertical speed display label", "Fix wrong names").
- C++: 4 spaces, braces always, `.clang-format` from winctrl, comments explain why rather than what.
- TypeScript: Prettier config in `.prettierrc`.

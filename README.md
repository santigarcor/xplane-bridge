# XPlane 12 - Arduino interface

This project provides an interface between X-Plane 12 and Arduino, enabling custom hardware panels, controls, and displays for your flight simulator setup. It allows you to send and receive datarefs and commands between X-Plane and Arduino-compatible microcontrollers, making it easier to build immersive and interactive cockpit experiences.

> **Inspiration:**
> This repository was inspired by [xplane_bae146_mcu](https://github.com/santigarcor/xplane_bae146_mcu), which implements a similar interface for the BAe 146 aircraft. Many concepts and ideas have been adapted and extended for broader use with X-Plane 12 and various Arduino projects.

## Features

- Native X-Plane plugin (Windows), or a Node bridge using X-Plane's Websockets & REST API
- Read and write inputs from Arduino
- Send X-Plane commands/datarefs from physical switches and buttons
- Easy integration with custom panels and hardware

## Getting Started

1. Clone this repository.
2. Run `npm install`.
3. Inside the `mappings/` are all the supported aircraft, you can extend the code to add new planes.
4. Run `npm run bundle`
5. Start X-Plane.
6. Run `npm start`.
7. Fly and enjoy.

## Launching from X-Plane (FlyWithLua)

`XplaneBridge.lua` starts the bridge when FlyWithLua loads, stops it when X-Plane closes and adds a `FlyWithLua/Bridge/Restart` command. It works on Windows (`start_bridge.ps1`) and macOS (`start_bridge.sh`).

1. Run `npm run bundle`.
2. The repo paths are hardcoded at the top of `XplaneBridge.lua` and in `start_bridge.sh`. Update them if you cloned the repo somewhere else.
3. Symlink the script into the FlyWithLua `Scripts` folder so `git pull` keeps it up to date:
   - Windows (admin PowerShell, or Developer Mode enabled):
     ```powershell
     New-Item -ItemType SymbolicLink -Path "<X-Plane 12>\Resources\plugins\FlyWithLua\Scripts\XplaneBridge.lua" -Target "<repo>\XplaneBridge.lua"
     ```
   - macOS:
     ```sh
     ln -s <repo>/XplaneBridge.lua "<X-Plane 12>/Resources/plugins/FlyWithLua/Scripts/XplaneBridge.lua"
     ```

On Windows the bridge runs without a window. Each session writes its output to `logs\bridge-<start time>.log` and `logs\bridge.err-<start time>.log`, and the last 10 sessions are kept. To follow the current session:

```powershell
Get-ChildItem logs\bridge-*.log | Sort-Object Name | Select-Object -Last 1 | Get-Content -Wait -Tail 50
```

## Native X-Plane plugin (Windows)

`plugin/` is a native X-Plane 12 plugin that does the same job as the Node bridge, without the separate process, the web API or FlyWithLua. It talks to the Arduino directly over serial and to X-Plane through the SDK. Requires X-Plane 12.3.0 or newer (XPLM420).

**Use either the plugin or the Node bridge, not both:** only one can open the COM port. While using the plugin, remove `XplaneBridge.lua` from the FlyWithLua `Scripts` folder.

### Build

1. Install [MSYS2](https://www.msys2.org/), then in the MSYS2 shell:
   ```sh
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
   ```
2. Download the [X-Plane SDK](https://developer.x-plane.com/sdk/plugin-sdk-downloads/) (4.2.0 or newer) and unzip it so that `plugin/SDK/CHeaders` exists.
3. Build, and optionally install into X-Plane:
   ```powershell
   cd plugin
   $env:XPLANE_DIR = 'D:\X-Plane 12'   # optional, installs to Resources\plugins\xplane_bridge\win_x64
   .\build.ps1
   ```
   X-Plane must be closed to install, since it locks the plugin file while it's loaded.

   Every build also creates `plugin\dist\xplane_bridge\`, a ready folder to copy into `X-Plane 12\Resources\plugins\` (with a `README.txt` taken from `plugin\INSTALL.txt`), and `plugin\dist\xplane_bridge.zip`.

`.\build.ps1 -Test` builds and runs `host-test.exe`, which runs the bridge outside X-Plane against a fake sim and, if the Arduino is plugged in, against the real board.

### Use

The plugin starts with X-Plane, finds the Arduino on its own and loads the mappings for the current aircraft. Its menu is under **Plugins → XPlane Bridge**:

- **Status**: `Connected (COM3)`, `Searching for Arduino...`, `Connecting...`, or `ERROR - <reason>` (for example the port is in use, the board stopped sending data, or an aircraft's datarefs are missing).
- **Aircraft**: the loaded mapping and how many of its datarefs were found.
- **Restart serial connection** and **Reload aircraft mappings**, also available as the `xplane_bridge/restart_serial` and `xplane_bridge/reload_mappings` commands, so they can be bound to a key or button.
- **Debug logging**: logs every message to and from the Arduino.

Logs go to X-Plane's `Log.txt` and to `Resources\plugins\xplane_bridge\logs\bridge-<start time>.log` (the last 10 sessions are kept). Settings are in `Output\preferences\xplane_bridge.ini`:

```ini
baud=57600      # must match BAUD_RATE in arduino.ino
port=COM3       # optional, skips autodetection
debug_logging=0
```

## License

See [LICENSE](./LICENSE) for details.

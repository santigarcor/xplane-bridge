# XPlane 12 - Arduino interface

This project provides an interface between X-Plane 12 and Arduino, enabling custom hardware panels, controls, and displays for your flight simulator setup. It allows you to send and receive datarefs and commands between X-Plane and Arduino-compatible microcontrollers, making it easier to build immersive and interactive cockpit experiences.

> **Inspiration:**
> This repository was inspired by [xplane_bae146_mcu](https://github.com/santigarcor/xplane_bae146_mcu), which implements a similar interface for the BAe 146 aircraft. Many concepts and ideas have been adapted and extended for broader use with X-Plane 12 and various Arduino projects.

## Features

- Communicate with X-Plane 12 via Websockets & REST API
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

On Windows the bridge runs without a window: its output goes to `logs\bridge.log` and `logs\bridge.err.log`.

## License

See [LICENSE](./LICENSE) for details.

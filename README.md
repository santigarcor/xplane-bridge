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
6. Run `node dist/bridge.js`.
7. Fly and enjoy.

If you want to use it with the lua script you will need to update the paths in the `start_bridge.sh` and inside the XplaneBridge.lua and paste the latter into the Lua Scripts folder.

## License

See [LICENSE](./LICENSE) for details.

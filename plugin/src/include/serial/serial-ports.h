#ifndef SERIAL_PORTS_H
#define SERIAL_PORTS_H

#include <optional>
#include <string>
#include <vector>

struct SerialPortInfo {
        std::string port;         // COM5
        std::string manufacturer; // Arduino LLC (www.arduino.cc)
        std::string instanceId;   // USB\VID_2341&PID_0042\...
        std::string friendlyName; // Arduino Mega 2560 (COM5)
};

// Present COM ports, in the order Windows enumerates them. Uses SetupAPI, which can take tens of
// milliseconds, so only call it from the serial worker thread.
std::vector<SerialPortInfo> listSerialPorts();

// First port whose manufacturer or instance ID mentions arduino, usb or serial. Same rule as
// autodiscoverPort() in src/communicators/ArduinoSerialCommunicator.ts (pnpId there is the
// instance ID here).
std::optional<SerialPortInfo> findArduinoPort(const std::vector<SerialPortInfo> &ports);

#endif

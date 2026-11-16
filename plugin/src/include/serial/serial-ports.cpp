#include "serial-ports.h"

#include "win-utils.h"

#include <algorithm>
#include <cwctype>
#include <setupapi.h>
#include <windows.h>

namespace {

    // GUID_DEVCLASS_PORTS (devguid.h), defined here so we don't need INITGUID tricks
    const GUID kGuidDevClassPorts = {0x4d36e978, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};

    // Owns an HDEVINFO list
    class DevInfoList {
        public:
            DevInfoList() : handle(SetupDiGetClassDevsW(&kGuidDevClassPorts, nullptr, nullptr, DIGCF_PRESENT)) {}
            ~DevInfoList() {
                if (valid()) {
                    SetupDiDestroyDeviceInfoList(handle);
                }
            }

            DevInfoList(const DevInfoList &) = delete;
            DevInfoList &operator=(const DevInfoList &) = delete;

            bool valid() const {
                return handle != INVALID_HANDLE_VALUE;
            }

            HDEVINFO get() const {
                return handle;
            }

        private:
            HDEVINFO handle;
    };

    std::string registryProperty(HDEVINFO devices, SP_DEVINFO_DATA &device, DWORD property) {
        wchar_t buffer[512] = {};
        if (!SetupDiGetDeviceRegistryPropertyW(devices, &device, property, nullptr, reinterpret_cast<BYTE *>(buffer), sizeof(buffer) - sizeof(wchar_t), nullptr)) {
            return "";
        }
        return wideToUtf8(buffer);
    }

    std::string portName(HDEVINFO devices, SP_DEVINFO_DATA &device) {
        HKEY key = SetupDiOpenDevRegKey(devices, &device, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (key == INVALID_HANDLE_VALUE || key == nullptr) {
            return "";
        }

        wchar_t buffer[64] = {};
        DWORD size = sizeof(buffer) - sizeof(wchar_t);
        DWORD type = 0;
        LONG result = RegQueryValueExW(key, L"PortName", nullptr, &type, reinterpret_cast<BYTE *>(buffer), &size);
        RegCloseKey(key);

        if (result != ERROR_SUCCESS || type != REG_SZ) {
            return "";
        }
        return wideToUtf8(buffer);
    }

    std::string lowercase(std::string text) {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return text;
    }

    bool looksLikeArduino(const std::string &text) {
        std::string lower = lowercase(text);
        return lower.find("arduino") != std::string::npos || lower.find("usb") != std::string::npos ||
               lower.find("serial") != std::string::npos;
    }

}

std::vector<SerialPortInfo> listSerialPorts() {
    std::vector<SerialPortInfo> ports;

    DevInfoList devices;
    if (!devices.valid()) {
        return ports;
    }

    SP_DEVINFO_DATA device{};
    device.cbSize = sizeof(device);
    for (DWORD index = 0; SetupDiEnumDeviceInfo(devices.get(), index, &device); index++) {
        SerialPortInfo info;
        info.port = portName(devices.get(), device);
        // Parallel ports (LPT) share this device class
        if (!info.port.starts_with("COM")) {
            continue;
        }

        info.manufacturer = registryProperty(devices.get(), device, SPDRP_MFG);
        info.friendlyName = registryProperty(devices.get(), device, SPDRP_FRIENDLYNAME);

        wchar_t instanceId[512] = {};
        if (SetupDiGetDeviceInstanceIdW(devices.get(), &device, instanceId, static_cast<DWORD>(std::size(instanceId) - 1), nullptr)) {
            info.instanceId = wideToUtf8(instanceId);
        }

        ports.push_back(std::move(info));
    }

    return ports;
}

std::optional<SerialPortInfo> findArduinoPort(const std::vector<SerialPortInfo> &ports) {
    for (const auto &port : ports) {
        if (looksLikeArduino(port.manufacturer) || looksLikeArduino(port.instanceId)) {
            return port;
        }
    }
    return std::nullopt;
}

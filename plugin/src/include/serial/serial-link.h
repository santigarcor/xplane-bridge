#ifndef SERIAL_LINK_H
#define SERIAL_LINK_H

#include <memory>
#include <string>
#include <vector>
#include <windows.h>

enum class LinkState {
    Stopped,
    Searching,  // looking for the Arduino's COM port
    Connecting, // port open, waiting for the boot banner or a heartbeat
    Connected,
};

struct LinkStatus {
        LinkState state = LinkState::Stopped;
        std::string port;
        // Last problem, kept until the next successful connection or a manual restart. Empty if none.
        std::string error;
};

struct SerialEvent {
        enum class Type {
            Connected,    // the sketch is up: send the full state
            Reset,        // the board rebooted while connected: send the full state again
            Disconnected, // the link dropped, a reconnect is already scheduled
            Input,        // a {"user_input":...} from the sketch, the name is in `text`
        };

        Type type;
        std::string text;
};

struct SerialConfig {
        int baudRate;
        // When set, autodiscovery is skipped and this port is always used (e.g. "COM5")
        std::string forcedPort;
};

class SerialWorker;

// The serial connection to the Arduino. Port of src/communicators/ArduinoSerialCommunicator.ts.
//
// All serial I/O happens on one worker thread owned by this class: discovery, opening, reading,
// writing, liveness checks, closing and reconnecting. The main thread only exchanges queued data
// with it through short locks, so nothing here can block X-Plane:
//   - send() queues a line, the worker writes it
//   - drainEvents() returns what the worker received since the last call
//   - requestRestart() only sets a flag, the worker closes and reopens the port
//
// stop() (also called by the destructor) is bounded: it waits at most 3 s. If the USB driver is
// wedged inside CloseHandle, the worker is left to finish on its own. It shares ownership of its
// state and holds a reference to this DLL, so the plugin can be destroyed and even unloaded safely
// while it's still running.
class SerialLink {
    public:
        explicit SerialLink(SerialConfig config);
        ~SerialLink();

        SerialLink(const SerialLink &) = delete;
        SerialLink &operator=(const SerialLink &) = delete;

        bool start();
        void stop();

        void requestRestart();

        // A line written right before the port closes when stop() is called (not on restarts or
        // reconnects), if the board is connected. Used to switch the panel off when X-Plane quits.
        void setShutdownLine(const std::string &line);

        // Queues a line (without the newline) for the Arduino. Dropped unless connected, so nothing
        // piles up while the board is booting or unplugged. A line still queued with the same key
        // is replaced, since only the latest value of a display or LED matters.
        void send(const std::string &key, const std::string &line);

        std::vector<SerialEvent> drainEvents();
        LinkStatus getStatus() const;
        bool isConnected() const;

    private:
        std::shared_ptr<SerialWorker> worker;
        HANDLE thread = nullptr;
};

#endif

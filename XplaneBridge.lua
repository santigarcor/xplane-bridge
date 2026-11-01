-- xplane_bridge_launcher.lua

local bridge_started = false

-- SYSTEM is set by FlyWithLua: "IBM" on Windows, "APL" on macOS
local IS_WINDOWS = SYSTEM == "IBM"
local BRIDGE_DIR = IS_WINDOWS and "C:\\Users\\Santiago\\Repos\\xplane-bridge" or "/Users/santiago/Repos/xplane-bridge"

-- Total flight time dataref. If it's > 0, the sim has already loaded the aircraft and the scenery.
DataRef("flight_time", "sim/time/total_flight_time_sec", "readonly")

local function launch_bridge()
    if IS_WINDOWS then
        -- 'start /B' returns control right away so X-Plane doesn't freeze.
        -- start_bridge.ps1 launches node hidden and saves its PID to bridge.pid
        os.execute('start "" /B powershell -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File "'
            .. BRIDGE_DIR .. '\\start_bridge.ps1"')
    else
        -- The ' > /dev/null 2>&1 &' at the end is CRITICAL.
        -- It redirects the output and sends the process to the background so X-Plane doesn't freeze.
        os.execute(BRIDGE_DIR .. "/start_bridge.sh > /dev/null 2>&1 &")
    end
end

local function stop_bridge()
    if IS_WINDOWS then
        -- Use the PID saved by start_bridge.ps1 (the cmd.exe wrapping node);
        -- /T also kills the child node.exe process
        local pid_path = BRIDGE_DIR .. "\\bridge.pid"
        local pid_file = io.open(pid_path, "r")
        if not pid_file then return end
        local pid = tonumber(pid_file:read("*l"))
        pid_file:close()
        if pid then
            os.execute('taskkill /F /T /PID ' .. pid .. ' /FI "IMAGENAME eq cmd.exe" > nul 2>&1')
        end
        os.remove(pid_path)
    else
        -- Find and kill the node process running the bridge's bridge.js
        os.execute("pkill -f 'node dist/bridge.js'")
    end
end

local function wait_a_moment()
    if IS_WINDOWS then
        -- 'timeout' fails without an interactive console, ping waits ~1 second
        os.execute("ping -n 2 127.0.0.1 > nul")
    else
        os.execute("sleep 1")
    end
end

function start_node_bridge()
    -- Make sure the bridge hasn't already been launched in this session
    if not bridge_started then
        launch_bridge()
        bridge_started = true

        -- Write a message to X-Plane's Log.txt to confirm it ran
        logMsg("X-Plane Bridge: Node.js ejecutado exitosamente en el background.")
    end
end

function kill_node_bridge()
    if bridge_started then
        stop_bridge()
        logMsg("X-Plane Bridge: Proceso de Node.js detenido.")
    end
end

function restart_node_bridge()
    -- 1. Kill any previous instance that's still running
    stop_bridge()

    -- 2. Give the system a moment to release the serial port
    wait_a_moment()

    -- 3. Launch the script in the background again
    launch_bridge()
    bridge_started = true

    logMsg("X-Plane Bridge: Reinicio manual ejecutado.")
end

-- This creates an official command inside X-Plane
create_command("FlyWithLua/Bridge/Restart",
               "Reiniciar el bridge de Node.js",
               "restart_node_bridge()",
               "",
               "")

-- Runs right before FlyWithLua shuts down (when the sim closes)
do_on_exit("kill_node_bridge()")
start_node_bridge()

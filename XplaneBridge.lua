-- xplane_bridge_launcher.lua

local bridge_started = false

-- SYSTEM lo define FlyWithLua: "IBM" en Windows, "APL" en macOS
local IS_WINDOWS = SYSTEM == "IBM"
local BRIDGE_DIR = IS_WINDOWS and "C:\\Users\\Santiago\\Repos\\xplane-bridge" or "/Users/santiago/Repos/xplane-bridge"

-- Dataref que indica el tiempo total de vuelo. Si es > 0, el simulador ya cargó el avión y el escenario.
DataRef("flight_time", "sim/time/total_flight_time_sec", "readonly")

local function launch_bridge()
    if IS_WINDOWS then
        -- 'start /B' devuelve el control enseguida para que X-Plane no se cuelgue.
        -- start_bridge.ps1 lanza node oculto y guarda su PID en bridge.pid
        os.execute('start "" /B powershell -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File "'
            .. BRIDGE_DIR .. '\\start_bridge.ps1"')
    else
        -- El ' > /dev/null 2>&1 &' al final es CRÍTICO.
        -- Redirige la salida y envía el proceso al background para que X-Plane no se cuelgue.
        os.execute(BRIDGE_DIR .. "/start_bridge.sh > /dev/null 2>&1 &")
    end
end

local function stop_bridge()
    if IS_WINDOWS then
        -- Usamos el PID guardado por start_bridge.ps1 para no matar otros procesos de node
        local pid_path = BRIDGE_DIR .. "\\bridge.pid"
        local pid_file = io.open(pid_path, "r")
        if not pid_file then return end
        local pid = tonumber(pid_file:read("*l"))
        pid_file:close()
        if pid then
            os.execute('taskkill /F /T /PID ' .. pid .. ' /FI "IMAGENAME eq node.exe" > nul 2>&1')
        end
        os.remove(pid_path)
    else
        -- Busca y destruye el proceso de node que esté corriendo el bridge.js de tu bridge
        os.execute("pkill -f 'node dist/bridge.js'")
    end
end

local function wait_a_moment()
    if IS_WINDOWS then
        -- 'timeout' falla sin una consola interactiva, ping espera ~1 segundo
        os.execute("ping -n 2 127.0.0.1 > nul")
    else
        os.execute("sleep 1")
    end
end

function start_node_bridge()
    -- Verificamos que el vuelo haya iniciado y que el bridge no se haya lanzado ya en esta sesión
    if not bridge_started then
        launch_bridge()
        bridge_started = true

        -- Imprime un mensaje en el archivo Log.txt de X-Plane para confirmar la ejecución
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
    -- 1. Matar cualquier instancia previa que esté corriendo
    stop_bridge()

    -- 2. Darle un momento al sistema para liberar el puerto serial
    wait_a_moment()

    -- 3. Volver a lanzar el script en el background
    launch_bridge()
    bridge_started = true

    logMsg("X-Plane Bridge: Reinicio manual ejecutado.")
end

-- Esto crea un comando oficial dentro de X-Plane
create_command("FlyWithLua/Bridge/Restart",
               "Reiniciar el bridge de Node.js",
               "restart_node_bridge()",
               "",
               "")

-- Se ejecuta justo antes de que FlyWithLua se apague (al cerrar el simulador)
do_on_exit("kill_node_bridge()")
start_node_bridge()

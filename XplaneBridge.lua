-- xplane_bridge_launcher.lua

local bridge_started = false

-- Dataref que indica el tiempo total de vuelo. Si es > 0, el simulador ya cargó el avión y el escenario.
DataRef("flight_time", "sim/time/total_flight_time_sec", "readonly")

function start_node_bridge()
    -- Verificamos que el vuelo haya iniciado y que el bridge no se haya lanzado ya en esta sesión
    if not bridge_started then

        -- El ' > /dev/null 2>&1 &' al final es CRÍTICO.
        -- Redirige la salida y envía el proceso al background para que X-Plane no se cuelgue.
        local cmd = "/Users/santiago/Repos/xplane-bridge/start_bridge.sh > /dev/null 2>&1 &"

        os.execute(cmd)
        bridge_started = true

        -- Imprime un mensaje en el archivo Log.txt de X-Plane para confirmar la ejecución
        logMsg("X-Plane Bridge: Node.js ejecutado exitosamente en el background.")
    end
end

function kill_node_bridge()
    if bridge_started then
        -- Busca y destruye el proceso de node que esté corriendo el bridge.js de tu bridge
        local kill_cmd = "pkill -f 'node dist/bridge.js'"
        os.execute(kill_cmd)
        logMsg("X-Plane Bridge: Proceso de Node.js detenido.")
    end
end

-- Se ejecuta justo antes de que FlyWithLua se apague (al cerrar el simulador)
do_on_exit("kill_node_bridge()")
start_node_bridge()
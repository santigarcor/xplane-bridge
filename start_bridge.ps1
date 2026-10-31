# Lanzador del bridge para Windows (equivalente a start_bridge.sh).
# Ejecuta node en segundo plano sin ventana, guarda los logs en logs\ y el PID en bridge.pid
# para que XplaneBridge.lua pueda detenerlo.
#
# IMPORTANTE: node NO debe heredar los handles abiertos de X-Plane (os.execute los pasa a cmd/powershell).
# Si los hereda, mantiene abierto xplane.replay y X-Plane no puede abrir el modo replay.
# Por eso no usamos -RedirectStandardOutput/-RedirectStandardError (fuerzan herencia de handles):
# Start-Process sin redireccion usa ShellExecute, que no hereda, y la redireccion la hace cmd /c.

Set-Location $PSScriptRoot

# 1. Buscar node.exe. X-Plane (via Steam) puede tener un PATH antiguo, asi que leemos
#    NVM_SYMLINK directamente del registro y usamos la ruta por defecto de nvm-windows como ultimo recurso.
$node = (Get-Command node -ErrorAction SilentlyContinue).Source
if (-not $node) {
  foreach ($scope in 'User', 'Machine') {
    $symlink = [Environment]::GetEnvironmentVariable('NVM_SYMLINK', $scope)
    if ($symlink -and (Test-Path "$symlink\node.exe")) { $node = "$symlink\node.exe"; break }
  }
}
if (-not $node) { $node = 'C:\nvm4w\nodejs\node.exe' }

# 2. Detener una instancia previa si sigue corriendo (p. ej. tras recargar los scripts Lua)
if (Test-Path bridge.pid) {
  $old = Get-Process -Id (Get-Content bridge.pid) -ErrorAction SilentlyContinue
  # El PID guardado es el cmd.exe que envuelve a node; /T mata tambien a node
  if ($old -and $old.ProcessName -eq 'cmd') { taskkill /F /T /PID $old.Id | Out-Null }
  Remove-Item bridge.pid
}

# 3. Lanzar el bridge oculto (via cmd /c para redirigir los logs) y guardar el PID de cmd.exe
New-Item -ItemType Directory -Force logs | Out-Null
$cmdLine = "/c `"`"$node`" dist\bridge.js > logs\bridge.log 2> logs\bridge.err.log`""
$process = Start-Process cmd.exe -ArgumentList $cmdLine -WorkingDirectory $PSScriptRoot `
  -WindowStyle Hidden -PassThru
$process.Id | Set-Content bridge.pid

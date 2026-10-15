# Lanzador del bridge para Windows (equivalente a start_bridge.sh).
# Ejecuta node en segundo plano sin ventana, guarda los logs en logs\ y el PID en bridge.pid
# para que XplaneBridge.lua pueda detenerlo.

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
  if ($old -and $old.ProcessName -eq 'node') { Stop-Process -Id $old.Id -Force }
  Remove-Item bridge.pid
}

# 3. Lanzar el bridge oculto y guardar su PID
New-Item -ItemType Directory -Force logs | Out-Null
$process = Start-Process $node -ArgumentList 'dist\bridge.js' -WindowStyle Hidden -PassThru `
  -RedirectStandardOutput logs\bridge.log -RedirectStandardError logs\bridge.err.log
$process.Id | Set-Content bridge.pid

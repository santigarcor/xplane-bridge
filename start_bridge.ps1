# Bridge launcher for Windows (equivalent to start_bridge.sh).
# Runs node in the background without a window, writes the logs to logs\ and the PID to bridge.pid
# so XplaneBridge.lua can stop it.
#
# IMPORTANT: node must NOT inherit X-Plane's open handles (os.execute passes them on to cmd/powershell).
# If it does, it keeps xplane.replay open and X-Plane can't enter replay mode.
# That's why we don't use -RedirectStandardOutput/-RedirectStandardError (they force handle inheritance):
# Start-Process without redirection uses ShellExecute, which doesn't inherit, and cmd /c does the redirection.

Set-Location $PSScriptRoot

# 1. Find node.exe. X-Plane (via Steam) may have a stale PATH, so we read NVM_SYMLINK
#    straight from the registry and fall back to the nvm-windows default path.
$node = (Get-Command node -ErrorAction SilentlyContinue).Source
if (-not $node) {
  foreach ($scope in 'User', 'Machine') {
    $symlink = [Environment]::GetEnvironmentVariable('NVM_SYMLINK', $scope)
    if ($symlink -and (Test-Path "$symlink\node.exe")) { $node = "$symlink\node.exe"; break }
  }
}
if (-not $node) { $node = 'C:\nvm4w\nodejs\node.exe' }

# 2. Stop a previous instance if it's still running (e.g. after reloading the Lua scripts)
if (Test-Path bridge.pid) {
  $old = Get-Process -Id (Get-Content bridge.pid) -ErrorAction SilentlyContinue
  # The saved PID is the cmd.exe wrapping node; /T kills node too
  if ($old -and $old.ProcessName -eq 'cmd') {
    taskkill /F /T /PID $old.Id | Out-Null
    # Wait for it to exit so it releases COM3 before the new bridge opens it
    $old.WaitForExit(5000) | Out-Null
  }
  Remove-Item bridge.pid
}

# 3. Each session writes to its own log files, named after its start time. Keep the last 10 of each.
New-Item -ItemType Directory -Force logs | Out-Null
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
foreach ($name in 'bridge', 'bridge.err') {
  # Leave room for the new session's file. A file still held by the old process just stays until next time.
  Get-ChildItem "logs\$name-*.log" | Sort-Object Name -Descending | Select-Object -Skip 9 |
    Remove-Item -ErrorAction SilentlyContinue
}

# 4. Launch the bridge hidden (via cmd /c to redirect the logs) and save cmd.exe's PID
$cmdLine = "/c `"`"$node`" dist\bridge.js > logs\bridge-$stamp.log 2> logs\bridge.err-$stamp.log`""
$process = Start-Process cmd.exe -ArgumentList $cmdLine -WorkingDirectory $PSScriptRoot `
  -WindowStyle Hidden -PassThru
$process.Id | Set-Content bridge.pid

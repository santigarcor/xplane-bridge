# Builds the plugin with MSYS2's MinGW-w64 toolchain and, if XPLANE_DIR is set, installs it.
#
#   .\build.ps1                      build to build\win\win_x64\xplane_bridge.xpl
#   .\build.ps1 -Clean               delete build\win first
#   .\build.ps1 -Test                build and run the host test (stub X-Plane, real Arduino if plugged in)
#   $env:XPLANE_DIR = 'D:\X-Plane 12'; .\build.ps1   also copy it into X-Plane
#
# Needs MSYS2 with: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
# and the X-Plane SDK (4.2.0 or newer) unzipped into plugin\SDK.

param(
  [switch]$Clean,
  [switch]$Test,
  [string]$Msys2Bin = $(if ($env:MSYS2_MINGW64_BIN) { $env:MSYS2_MINGW64_BIN } else { 'C:\msys64\mingw64\bin' })
)

$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot

if (-not (Test-Path "$Msys2Bin\x86_64-w64-mingw32-g++.exe")) {
  throw "MinGW-w64 not found in $Msys2Bin. Install MSYS2 and the packages listed at the top of this script, or pass -Msys2Bin."
}
$env:PATH = "$Msys2Bin;$env:PATH"

$defs = 'SDK\CHeaders\XPLM\XPLMDefs.h'
if (-not (Test-Path $defs)) {
  throw "X-Plane SDK not found. Unzip it into $PSScriptRoot\SDK (so that $defs exists)."
}
$match = Select-String -Path $defs -Pattern '#define\s+kXPLM_Version\s+\((\d+)\)'
if (-not $match) { throw "Could not read kXPLM_Version from $defs" }
$sdkVersion = $match.Matches[0].Groups[1].Value
Write-Host "Building with SDK version $sdkVersion"

$buildDir = if ($Test) { 'build\win-tests' } else { 'build\win' }
if ($Clean -and (Test-Path $buildDir)) {
  Remove-Item -Recurse -Force $buildDir
}

# Native tools write warnings to stderr, which PowerShell 5.1 would turn into terminating errors.
# Their exit codes are checked instead.
$ErrorActionPreference = 'Continue'

$hostTests = if ($Test) { 'ON' } else { 'OFF' }
# Every -D argument is quoted: PowerShell 5.1 splits an unquoted -DX=file.ext at the dot
& cmake -G Ninja "-DCMAKE_TOOLCHAIN_FILE=toolchain-win.cmake" "-DCMAKE_BUILD_TYPE=Release" "-DSDK_VERSION=$sdkVersion" "-DXPLANE_BRIDGE_HOST_TESTS=$hostTests" -S . -B $buildDir
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed' }

& cmake --build $buildDir
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }

if ($Test) {
  & "$buildDir\host-test.exe"
  $testExit = $LASTEXITCODE
  $ErrorActionPreference = 'Stop'
  if ($testExit -ne 0) { throw 'Host test failed' }
  return
}

$ErrorActionPreference = 'Stop'

$output = "$buildDir\win_x64\xplane_bridge.xpl"
# The build keeps its debug info (-g1) so crashes can be symbolicated; the installed copy is stripped
$stripped = "$buildDir\win_x64\xplane_bridge.stripped.xpl"
& "$Msys2Bin\strip.exe" -s -o $stripped $output
if ($LASTEXITCODE -ne 0) { throw 'strip failed' }
Write-Host "Built $output (stripped copy: $stripped)"

# Sanity checks on what X-Plane will load: the five entry points, and only DLLs every Windows has
$ErrorActionPreference = 'Continue'
$dump = & "$Msys2Bin\objdump.exe" -p $stripped
$ErrorActionPreference = 'Stop'
$exports = ($dump | Select-String -Pattern 'XPlugin(Start|Stop|Enable|Disable|ReceiveMessage)$').Count
if ($exports -ne 5) { throw "Expected 5 XPlugin exports, found $exports" }
$allowedDlls = 'KERNEL32.dll', 'ADVAPI32.dll', 'SETUPAPI.dll', 'msvcrt.dll', 'XPLM_64.dll'
$dlls = $dump | Select-String -Pattern 'DLL Name: (.+)$' | ForEach-Object { $_.Matches[0].Groups[1].Value.Trim() }
$unexpected = $dlls | Where-Object { $allowedDlls -notcontains $_ }
if ($unexpected) { throw "Unexpected DLL dependencies: $($unexpected -join ', ')" }

# Copy-and-paste folder for Resources\plugins, plus a zip of it (same layout as verdant-grass)
$dist = 'dist\xplane_bridge'
if (Test-Path 'dist') { Remove-Item -Recurse -Force 'dist' }
New-Item -ItemType Directory -Force "$dist\win_x64" | Out-Null
Copy-Item $stripped "$dist\win_x64\xplane_bridge.xpl"
Copy-Item 'INSTALL.txt' "$dist\README.txt"
Compress-Archive -Path $dist -DestinationPath 'dist\xplane_bridge.zip'
Write-Host "Distribution: $PSScriptRoot\dist\xplane_bridge (and dist\xplane_bridge.zip)"

if ($env:XPLANE_DIR) {
  $target = Join-Path $env:XPLANE_DIR 'Resources\plugins\xplane_bridge\win_x64'
  New-Item -ItemType Directory -Force $target | Out-Null
  try {
    Copy-Item $stripped (Join-Path $target 'xplane_bridge.xpl') -Force
    Write-Host "Installed to $target"
  } catch {
    throw "Could not copy to $target. Is X-Plane running? Quit it first, the plugin file is locked while loaded."
  }
}

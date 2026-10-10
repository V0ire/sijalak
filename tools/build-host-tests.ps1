# Si Jalak Bedas — host test build untuk Windows (PowerShell, tanpa ESP-IDF).
#
# Menjalankan build + ctest untuk library portable. Tidak butuh toolchain Xtensa.
# Butuh: CMake, Ninja, dan compiler C++ (MSVC atau MinGW-w64) di PATH.
#
#   .\tools\build-host-tests.ps1
#
param(
    [string]$BuildDir = "$PSScriptRoot\..\.buildtmp\fw_build_win"
)

$ErrorActionPreference = "Stop"
$repo = Resolve-Path "$PSScriptRoot\.."
Push-Location $repo
try {
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    Write-Host "== configure ==" -ForegroundColor Cyan
    cmake -S firmware\test -B $BuildDir -G Ninja -DCMAKE_BUILD_TYPE=Debug
    Write-Host "== build ==" -ForegroundColor Cyan
    cmake --build $BuildDir
    Write-Host "== ctest ==" -ForegroundColor Cyan
    ctest --test-dir $BuildDir --output-on-failure
}
finally {
    Pop-Location
}

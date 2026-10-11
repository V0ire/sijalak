# =====================================================================
#  Si Jalak Bedas — Windows Installer & Builder (fail-fast)
#
#  Untuk teman: anggap BELUM ada ESP-IDF, Git, CMake, atau Python.
#
#  Cara pakai (jalankan dari root repo, yaitu folder yang berisi 'firmware',
#  'backend', dan 'tools'):
#     powershell -ExecutionPolicy Bypass -File .\tools\install-windows.ps1
#   atau klik dua kali: tools\install-windows.bat
#
#  Script ini BERHENTI pada kegagalan pertama dan TIDAK akan menyarankan flash
#  bila build target gagal. Semua langkah memeriksa $LASTEXITCODE segera.
# =====================================================================

[CmdletBinding()]
param(
    [string]$IdfVersion = "v5.4",
    [string]$IdfPath    = "$HOME\esp\esp-idf",
    [string]$Target     = "esp32s3",
    [switch]$AutoInstall,
    [switch]$SkipFirmware,
    [switch]$SkipBackend
)

$ErrorActionPreference = "Stop"
$script:RepoRoot = $null
$script:BuildOk  = $false

function Say  ($m) { Write-Host $m }
function Head ($m) { Write-Host ""; Write-Host "== $m ==" -ForegroundColor Cyan }
function Ok   ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function Warn ($m) { Write-Host "  [WARN] $m" -ForegroundColor Yellow }
function Fail ($m) { Write-Host "  [FAIL] $m" -ForegroundColor Red }

function Have ($cmd) { return [bool](Get-Command $cmd -ErrorAction SilentlyContinue) }

# Run a command; throw with a clear message on non-zero exit.
function Invoke-Checked {
    param([string]$What, [scriptblock]$Body)
    & $Body
    if ($LASTEXITCODE -ne 0) { throw "$What failed (exit $LASTEXITCODE)" }
}

# --- Locate the repo root (folder containing firmware + backend + tools) ----
function Resolve-RepoRoot {
    $candidates = @(
        (Resolve-Path (Join-Path $PSScriptRoot "..") -ErrorAction SilentlyContinue),
        (Get-Location).Path
    )
    foreach ($c in $candidates) {
        if ($null -eq $c) { continue }
        if ((Test-Path (Join-Path $c "firmware")) -and
            (Test-Path (Join-Path $c "backend")) -and
            (Test-Path (Join-Path $c "tools"))) {
            return (Resolve-Path $c).Path
        }
    }
    return $null
}

Head "Locating repository"
$repo = Resolve-RepoRoot
if ($null -eq $repo) {
    Fail "Cannot find the repo root."
    Say  "  This script must live in <repo>\tools\ and be run with <repo> containing"
    Say  "  'firmware', 'backend' and 'tools' folders."
    Say  "  Current dir : $(Get-Location)"
    Say  "  Script dir  : $PSScriptRoot"
    Say  ""
    Say  "  Fix: clone the full repo and run from it, e.g."
    Say  "       git clone https://github.com/V0ire/sijalak.git"
    Say  "       cd sijalak"
    Say  "       powershell -ExecutionPolicy Bypass -File .\tools\install-windows.ps1"
    exit 2
}
$script:RepoRoot = $repo
Ok "repo root: $repo"

# --- Prerequisites ---------------------------------------------------------
Head "Checking prerequisites"
function Ensure-Tool([string]$Cmd, [string]$WingetId, [string]$Friendly) {
    if (Have $Cmd) { Ok "$Friendly ($Cmd) found"; return $true }
    Fail "$Friendly ($Cmd) not found"
    if ($WingetId -and $AutoInstall -and (Have "winget")) {
        Say "     installing $Friendly via winget..."
        winget install --id $WingetId -e --accept-source-agreements --accept-package-agreements
        if (-not (Have $Cmd)) {
            Say "     '$Cmd' still missing after winget — you may need to restart the shell."
            return $false
        }
        return $true
    }
    return $false
}

$missing = @()
if (-not (Ensure-Tool "git" "Git.Git" "Git")) { $missing += "git (winget install --id Git.Git -e)" }
if (-not (Ensure-Tool "cmake" "Kitware.CMake" "CMake")) { $missing += "cmake (winget install --id Kitware.CMake -e)" }
if (-not (Ensure-Tool "ninja" "Ninja-build.Ninja" "Ninja")) { $missing += "ninja (winget install --id Ninja-build.Ninja -e)" }
$pyOk = (Have "py") -or (Have "python")
if (-not $pyOk) { $missing += "python (winget install --id Python.Python.3.12 -e)" }
$cxxOk = (Have "cl") -or (Have "g++") -or (Have "clang++")
if (-not $cxxOk) { $missing += "a C++ compiler (Visual Studio Build Tools 'Desktop C++' or mingw-w64)" }

if ($missing.Count -gt 0) {
    Head "Missing prerequisites — install these, then re-run"
    $missing | ForEach-Object { Say "  - $_" }
    Say ""
    Say "  Tip: re-run with  -AutoInstall  to let winget install the winget-managed ones."
    if (-not $cxxOk) {
        Fail "A C++ compiler is required for the host tests and the ESP-IDF build. Stopping."
        exit 3
    }
}

# --- Backend ---------------------------------------------------------------
if (-not $SkipBackend) {
    Head "Backend — install deps + run tests"
    Push-Location (Join-Path $repo "backend")
    try {
        if ($pyOk) {
            $py = if (Have "py") { "py" } else { "python" }
            Invoke-Checked "pip upgrade"        { & $py -m pip install --quiet --upgrade pip }
            Invoke-Checked "backend deps"       { & $py -m pip install --quiet starlette uvicorn pytest httpx anyio }
            Ok "deps installed"
            Invoke-Checked "backend pytest"     { & $py -m pytest -q }
            Ok "backend tests passed"
        } else {
            Warn "skipping backend (no python launcher)"
        }
    } finally { Pop-Location }
}

# --- ESP-IDF ---------------------------------------------------------------
if ($SkipFirmware) {
    Warn "SkipFirmware set — not installing/building ESP-IDF"
} else {
    Head "ESP-IDF ($IdfVersion)"
    if (-not (Have "git")) { Fail "git missing — cannot install ESP-IDF"; exit 3 }

    if (Test-Path (Join-Path $IdfPath "export.ps1")) {
        Ok "ESP-IDF already present at $IdfPath"
    } else {
        Say "  cloning ESP-IDF $IdfVersion (recursive) into $IdfPath ..."
        New-Item -ItemType Directory -Force -Path (Split-Path $IdfPath) | Out-Null
        Invoke-Checked "git clone esp-idf" {
            git clone -b $IdfVersion --recursive https://github.com/espressif/esp-idf.git $IdfPath
        }
        Ok "cloned"
    }

    Say "  running install.ps1 $Target (downloads Xtensa toolchain; several minutes)..."
    Push-Location $IdfPath
    try {
        Invoke-Checked "esp-idf install.ps1" { & .\install.ps1 $Target }
        Ok "toolchain installed"
    } finally { Pop-Location }

    Head "Firmware — host tests (no hardware needed)"
    Push-Location (Join-Path $repo "firmware")
    try {
        $bd = Join-Path $repo ".buildtmp\fw_build_win"
        New-Item -ItemType Directory -Force -Path $bd | Out-Null
        Invoke-Checked "cmake configure (host)" { cmake -S test -B $bd -G Ninja -DCMAKE_BUILD_TYPE=Debug }
        Invoke-Checked "cmake build (host)"     { cmake --build $bd }
        Invoke-Checked "ctest (host)"           { ctest --test-dir $bd --output-on-failure }
        Ok "host tests passed"
    } finally { Pop-Location }

    Head "Firmware — build ESP32-S3 target image"
    Push-Location (Join-Path $repo "firmware")
    try {
        Invoke-Checked "dot-source export.ps1" {
            . "$IdfPath\export.ps1"
            if ($LASTEXITCODE -ne 0 -and $null -ne $LASTEXITCODE) { throw "export.ps1 failed" }
        }
        Invoke-Checked "idf.py set-target" { idf.py set-target $Target }
        Invoke-Checked "idf.py build" {
            idf.py build 2>&1 | Tee-Object -FilePath (Join-Path $repo ".buildtmp\idf_build.log")
        }
        $script:BuildOk = $true
        Ok "target build succeeded -> firmware\build\sijalak_bedas.bin"
    } catch {
        Fail "TARGET BUILD FAILED: $_"
        Say "  Full log: $repo\.buildtmp\idf_build.log"
        Say "  First errors above are the ones to fix."
        Say "  Do NOT flash: the image was not produced."
        exit 4
    } finally { Pop-Location }
}

# --- Done (only reached when nothing threw) --------------------------------
Head "Done"
if (-not $SkipFirmware -and $script:BuildOk) {
    Say "Next steps:"
    Say "  1. Connect the ESP32-S3 board via USB."
    Say "  2. Open the 'ESP-IDF PowerShell' (or run export.ps1) and:"
    Say "         cd `"$repo\firmware`""
    Say "         idf.py -p COMx flash monitor      # replace COMx with your port"
    Say ""
    Say "REMINDER: a successful build does NOT mean the device works. Pins, the I2C"
    Say "address, I2S clocking and the HTTPS endpoint are UNVERIFIED until tested on"
    Say "a real board."
} else {
    Say "Setup finished (no target build requested or build skipped)."
}

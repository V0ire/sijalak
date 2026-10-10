# =====================================================================
#  Si Jalak Bedas — Windows Installer & Builder
#  Untuk teman: anggap BELUM ada ESP-IDF, Git, CMake, atau Python.
#
#  Cara pakai:
#   1. Buka PowerShell (bukan cmd) di folder repo ini.
#   2. Jalankan:
#         powershell -ExecutionPolicy Bypass -File .\tools\install-windows.ps1
#      atau klik dua kali: tools\install-windows.bat
#
#  Script ini:
#   - Memeriksa prasyarat (Git, CMake, Ninja, Python, compiler C++).
#   - Jika ada yang kurang, menampilkan perintah winget untuk memasangnya
#     (atau memakainya otomatis bila -AutoInstall diberikan).
#   - Meng-Clone + memasang ESP-IDF v5.4 untuk target esp32s3 (bila belum ada).
#   - Menjalankan build test host (CTest) — tidak butuh hardware.
#   - Menjalankan build target esp32s3 (idf.py build).
#
#  Script TIDAK mem-flash board, TIDAK menyentuh konfigurasi global, dan TIDAK
#  memakai API berbayar. Flashing dilakukan manual di akhir (perintah dicetak).
# =====================================================================

[CmdletBinding()]
param(
    [string]$IdfVersion   = "v5.4",
    [string]$IdfPath      = "$HOME\esp\esp-idf",
    [string]$Target       = "esp32s3",
    [switch]$AutoInstall,            # pasang prasyarat via winget tanpa tanya
    [switch]$SkipFirmware,           # hanya backend + host tests
    [switch]$SkipBackend
)

$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..")
$script:Problems = @()

function Say   ($m) { Write-Host $m }
function Head  ($m) { Write-Host ""; Write-Host "== $m ==" -ForegroundColor Cyan }
function Ok    ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function Warn  ($m) { Write-Host "  [WARN] $m" -ForegroundColor Yellow }
function Fail  ($m) { Write-Host "  [MISS] $m" -ForegroundColor Red }

function Have ($cmd) { return [bool](Get-Command $cmd -ErrorAction SilentlyContinue) }

function Ensure-Tool {
    param([string]$Cmd, [string]$WingetId, [string]$Friendly)
    if (Have $Cmd) { Ok "$Friendly ($Cmd) found"; return $true }
    Fail "$Friendly ($Cmd) not found"
    if ($WingetId) {
        $script:Problems += "winget install --id $WingetId -e"
        if ($AutoInstall -and (Have "winget")) {
            Say "     installing $Friendly via winget..."
            try { winget install --id $WingetId -e --accept-source-agreements --accept-package-agreements }
            catch { Warn "winget install failed for $Friendly; install manually." }
            return (Have $Cmd)
        }
    }
    return $false
}

# ---------------------------------------------------------------------
Head "Checking prerequisites"
# ---------------------------------------------------------------------
$gitOk    = Ensure-Tool "git"    "Git.Git"                              "Git"
$cmakeOk  = Ensure-Tool "cmake"  "Kitware.CMake"                        "CMake"
$ninjaOk  = Ensure-Tool "ninja"  "Ninja-build.Ninja"                    "Ninja"
$pyOk     = Ensure-Tool "py"     "Python.Python.3.12"                   "Python launcher (py)"

# A C++ compiler: prefer cl (MSVC), else g++/clang++ (MinGW/LLVM).
$clOk = (Have "cl")
$gOk  = (Have "g++") -or (Have "clang++")
if ($clOk) { Ok "MSVC (cl) found" }
elseif ($gOk) { Ok "GCC/Clang C++ compiler found" }
else {
    Fail "No C++ compiler found"
    $script:Problems += "Install 'Visual Studio Build Tools 2022' (Desktop C++ workload) or 'mingw-w64'"
}

if ($script:Problems.Count -gt 0 -and -not $AutoInstall) {
    Head "Missing prerequisites — run these, then re-run this script"
    $script:Problems | ForEach-Object { Say "  $_" }
    Say ""
    Say "  Or re-run with:  -AutoInstall"
    # Continue anyway if git/cmake exist; only hard-stop if compiler missing.
}

# ---------------------------------------------------------------------
Head "Backend — install deps + run tests"
# ---------------------------------------------------------------------
if (-not $SkipBackend) {
    Push-Location (Join-Path $repo "backend")
    try {
        if ($pyOk) {
            Say "  installing backend Python deps..."
            & py -m pip install --quiet --upgrade pip
            & py -m pip install --quiet starlette uvicorn pytest httpx anyio
            Ok "deps installed"
            Say "  running pytest..."
            & py -m pytest -q
            if ($LASTEXITCODE -eq 0) { Ok "backend tests passed" } else { Warn "backend tests reported failures" }
        } else {
            Warn "skipping backend (python 'py' launcher missing)"
        }
    } finally { Pop-Location }
}

# ---------------------------------------------------------------------
Head "ESP-IDF ($IdfVersion)"
# ---------------------------------------------------------------------
if ($SkipFirmware) {
    Warn "SkipFirmware set — not installing/building ESP-IDF"
} elseif (-not $gitOk) {
    Warn "git missing — cannot install ESP-IDF"
} else {
    if (Test-Path (Join-Path $IdfPath "export.ps1")) {
        Ok "ESP-IDF already present at $IdfPath"
    } else {
        Say "  cloning ESP-IDF $IdfVersion (recursive) into $IdfPath ..."
        New-Item -ItemType Directory -Force -Path (Split-Path $IdfPath) | Out-Null
        git clone -b $IdfVersion --recursive https://github.com/espressif/esp-idf.git $IdfPath
        if ($LASTEXITCODE -ne 0) { throw "git clone of ESP-IDF failed" }
        Ok "cloned"
    }

    # Install the esp32s3 toolchain + python env.
    if (Test-Path "$HOME\.espressif\tools") { Ok "~\.espressif tools dir present" }
    Say "  running install.ps1 $Target (downloads Xtensa toolchain; several minutes)..."
    Push-Location $IdfPath
    try {
        & .\install.ps1 $Target
        if ($LASTEXITCODE -ne 0) { throw "ESP-IDF install.ps1 failed" }
        Ok "toolchain installed"
    } finally { Pop-Location }

    # -----------------------------------------------------------------
    Head "Firmware — host tests (no hardware needed)"
    # -----------------------------------------------------------------
    Push-Location (Join-Path $repo "firmware")
    try {
        $bd = Join-Path $repo ".buildtmp\fw_build_win"
        New-Item -ItemType Directory -Force -Path $bd | Out-Null
        cmake -S test -B $bd -G Ninja -DCMAKE_BUILD_TYPE=Debug
        cmake --build $bd
        ctest --test-dir $bd --output-on-failure
        if ($LASTEXITCODE -eq 0) { Ok "host tests passed" } else { Warn "host tests reported failures" }
    } finally { Pop-Location }

    # -----------------------------------------------------------------
    Head "Firmware — build ESP32-S3 target image"
    # -----------------------------------------------------------------
    Push-Location (Join-Path $repo "firmware")
    try {
        # Import ESP-IDF into THIS session. export.ps1 must be DOT-SOURCED so its
        # environment changes (PATH, IDF_PATH) persist for idf.py below.
        . "$IdfPath\export.ps1"
        idf.py set-target $Target
        idf.py build
        if ($LASTEXITCODE -eq 0) {
            Ok "target build succeeded -> firmware\build\sijalak_bedas.bin"
        } else {
            Warn "idf.py build failed. The 4 IDF HAL drivers + app_main are"
            Warn "UNVERIFIED against a real toolchain; capture the error log."
        }
    } finally { Pop-Location }
}

# ---------------------------------------------------------------------
Head "Done"
# ---------------------------------------------------------------------
Say "Next steps:"
Say "  1. Connect the ESP32-S3 board via USB."
Say "  2. Open the 'ESP-IDF PowerShell' (or run export.ps1) and:"
Say "         cd `"$repo\firmware`""
Say "         idf.py -p COMx flash monitor      # replace COMx with your port"
Say "  3. Replace placeholder pins in app_main.cpp (currently -1) with your wiring."
Say ""
Say "REMINDER: no hardware validation has been done. A successful build does NOT"
Say "mean the device works — pins, I2C address, I2S clocking and the HTTPS endpoint"
Say "are still unknown until tested on a real board."

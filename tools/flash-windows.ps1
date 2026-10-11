# =====================================================================
#  Si Jalak Bedas — WINDOWS: install -> build -> flash (all-in-one)
#
#  Satu skrip, dari nol sampai firmware masuk board ESP32-S3.
#
#  Cara pakai:
#     powershell -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1
#   atau klik dua kali: tools\flash-windows.bat
#
#  Yang dilakukan (berurutan, BERHENTI di error pertama):
#     1. Cek prasyarat (Git; instal Git kalau -AutoInstall).
#     2. Install ESP-IDF v5.4 + toolchain esp32s3 kalau belum ada.
#     3. Siapkan folder repo (pakai yang sekarang; clone bila dijalankan tanpa repo).
#     4. (opsional) Konversi aset Mochi -> firmware\preview\assets.
#     5. idf.py set-target esp32s3 + idf.py build.
#     6. Deteksi port COM, lalu idf.py flash (dan monitor bila diminta).
#
#  TIDAK mengganti konfigurasi global, TIDAK commit/push, TIDAK mengubah repo
#  orang lain. Aset pihak ketiga hanya ditulis ke folder git-ignored lokal.
# =====================================================================

[CmdletBinding()]
param(
    [string]$IdfVersion = "v5.4",
    [string]$IdfPath    = "$HOME\esp\esp-idf",
    [string]$RepoUrl    = "https://github.com/V0ire/sijalak.git",
    [string]$RepoDir    = "",            # default: script-\.. (repo tempat skrip berada)
    [string]$MochiRef   = "",            # path ke clone watcher-mochi (opsional)
    [string]$Port       = "",            # COM port; kosong = auto-deteksi
    [int]   $Baud       = 460800,
    [switch]$AutoInstall,                # pasang Git via winget bila hilang
    [switch]$SkipAssets,                 # lewati konversi aset Mochi
    [switch]$Monitor,                    # buka serial monitor setelah flash
    [switch]$NoFlash                     # hanya build, jangan flash
)

$ErrorActionPreference = "Stop"

function Say  ($m) { Write-Host $m }
function Head ($m) { Write-Host ""; Write-Host "== $m ==" -ForegroundColor Cyan }
function Ok   ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function Warn ($m) { Write-Host "  [WARN] $m" -ForegroundColor Yellow }
function Fail ($m) { Write-Host "  [FAIL] $m" -ForegroundColor Red; exit 1 }
function Have ($c) { return [bool](Get-Command $c -ErrorAction SilentlyContinue) }

# Run a command; abort with the FIRST failing step clearly.
function Invoke-Checked {
    param([string]$What, [scriptblock]$Body)
    & $Body
    if ($LASTEXITCODE -ne 0) { Fail "$What failed (exit $LASTEXITCODE). Fix the FIRST error above." }
}

# ---------------------------------------------------------------------
Head "Locating repository"
# ---------------------------------------------------------------------
if ($RepoDir -eq "") {
    $cand = Resolve-Path (Join-Path $PSScriptRoot "..") -ErrorAction SilentlyContinue
    if ($cand -and (Test-Path (Join-Path $cand "firmware")) -and (Test-Path (Join-Path $cand "tools"))) {
        $RepoDir = $cand.Path
    }
}
if (-not $RepoDir -or -not (Test-Path (Join-Path $RepoDir "firmware"))) {
    # Not inside a repo: clone one next to the current directory.
    if (-not (Have "git")) { Fail "git not found and no local repo. Install Git, or run with -AutoInstall." }
    $RepoDir = Join-Path (Get-Location) "sijalak"
    if (-not (Test-Path $RepoDir)) {
        Say "  cloning $RepoUrl -> $RepoDir"
        Invoke-Checked "git clone" { git clone $RepoUrl $RepoDir }
    }
}
$repo = (Resolve-Path $RepoDir).Path
Ok "repo: $repo"

# ---------------------------------------------------------------------
Head "Prerequisites"
# ---------------------------------------------------------------------
if (-not (Have "git")) {
    Fail "Git is required. Install from https://git-scm.com/download/win or re-run with -AutoInstall."
}
Ok "git found"

# ---------------------------------------------------------------------
Head "ESP-IDF ($IdfVersion)"
# ---------------------------------------------------------------------
$exportPs1 = Join-Path $IdfPath "export.ps1"
if (Test-Path $exportPs1) {
    Ok "ESP-IDF already present at $IdfPath"
} else {
    Say "  cloning ESP-IDF $IdfVersion (recursive) into $IdfPath ..."
    New-Item -ItemType Directory -Force -Path (Split-Path $IdfPath) | Out-Null
    Invoke-Checked "git clone esp-idf" {
        git clone -b $IdfVersion --recursive https://github.com/espressif/esp-idf.git $IdfPath
    }
    Say "  installing esp32s3 toolchain (several minutes, ~1-2 GB download)..."
    Push-Location $IdfPath
    try { Invoke-Checked "esp-idf install.ps1" { & .\install.ps1 esp32s3 } }
    finally { Pop-Location }
}
Ok "ESP-IDF ready"

# Activate ESP-IDF in THIS shell (dot-source so PATH/IDF_PATH persist).
. $exportPs1

# ---------------------------------------------------------------------
Head "(optional) Convert Mochi assets"
# ---------------------------------------------------------------------
if (-not $SkipAssets) {
    $conv = Join-Path $repo "tools\mochi_assets.py"
    if ($MochiRef -ne "" -and (Test-Path $MochiRef) -and (Test-Path $conv)) {
        if (Have "py") {
            $out = Join-Path $repo "firmware\preview\assets"
            Say "  converting GIFs + pop sound -> $out"
            Invoke-Checked "mochi_assets.py" {
                py $conv --mochi $MochiRef --out $out
            }
            Ok "assets converted (copy firmware\preview\assets\* to the SD card root)"
        } else {
            Warn "python 'py' not found; skipping asset conversion"
        }
    } else {
        Warn "no -MochiRef given (or tools\mochi_assets.py missing); skipping assets"
        Warn "the demo still runs with built-in eyes and UI tones only"
    }
}

# ---------------------------------------------------------------------
Head "Build firmware"
# ---------------------------------------------------------------------
Push-Location (Join-Path $repo "firmware")
try {
    Invoke-Checked "idf.py set-target" { idf.py set-target esp32s3 }
    Invoke-Checked "idf.py build" { idf.py build }
    $bin = Join-Path (Get-Location) "build\sijalak_bedas.bin"
    if (Test-Path $bin) { Ok "built: $bin" } else { Warn "build finished but $bin not found" }
}
finally { Pop-Location }

if ($NoFlash) {
    Head "Done (build only)"
    Say "-NoFlash set: not flashing. To flash later:"
    Say "  idf.py -p COMx flash monitor"
    exit 0
}

# ---------------------------------------------------------------------
Head "Detect serial port"
# ---------------------------------------------------------------------
if ($Port -eq "") {
    # Prefer USB-serial bridges commonly used by ESP dev boards.
    $devs = Get-CimInstance Win32_PnPEntity -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match "\((COM\d+)\)" }
    $usb = $devs | Where-Object { $_.Name -match "CP210|CH34|CH91|USB-SERIAL|Silicon|Espressif|UART" }
    if ($usb) {
        $Port = [regex]::Match($usb[0].Name, "\((COM\d+)\)").Groups[1].Value
        Ok "auto-detected $Port ($($usb[0].Name))"
    } elseif ($devs) {
        $Port = [regex]::Match($devs[0].Name, "\((COM\d+)\)").Groups[1].Value
        Warn "no known USB-serial bridge; using first port $Port ($($devs[0].Name))"
    } else {
        Fail "No COM port found. Plug in the board (data cable) and check Device Manager, then pass -Port COMx."
    }
}

# ---------------------------------------------------------------------
Head "Flash"
# ---------------------------------------------------------------------
Push-Location (Join-Path $repo "firmware")
try {
    if ($Monitor) {
        Say "  flashing + opening monitor ($Port @ $Baud). Press Ctrl+] to exit."
        Invoke-Checked "idf.py flash monitor" { idf.py -p $Port -b $Baud flash monitor }
    } else {
        Say "  flashing ($Port @ $Baud)..."
        Invoke-Checked "idf.py flash" { idf.py -p $Port -b $Baud flash }
        Ok "flash complete"
    }
}
finally { Pop-Location }

Head "Done"
Say "If the board did not behave as expected:"
Say "  - Hold BOOT, press RST, release BOOT, then flash again (download mode)."
Say "  - Check OLED wiring (SDA8/SCL9), touch polarity (GPIO2), and SD formatting."
Say "  - Re-run:  idf.py -p $Port monitor"
Say ""
Say "REMINDER: hardware pins/controller/polarity are UNVERIFIED. First-run success"
Say "depends on your actual module matching the assumed wiring."

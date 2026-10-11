# =====================================================================
#  Si Jalak Bedas — Windows Installer & Builder (fail-fast)
#
#  Requires Git for a new SDK clone; Python for optional backend setup.
#  This installer downloads dependencies. flash-windows.ps1 does not.
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
    [ValidateSet('esp32s3')][string]$Target = "esp32s3",
    [string]$RepoDir = "",
    [string]$BuildDir = "",
    [string]$Sdkconfig = "",
    [switch]$SkipFirmware,
    [switch]$SkipBackend
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot 'windows-common.ps1')
$transcriptStarted = $false
try {
$script:BuildOk  = $false

function Say  ($m) { Write-Host $m }
function Head ($m) { Write-Host ""; Write-Host "== $m ==" -ForegroundColor Cyan }
function Ok   ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function Warn ($m) { Write-Host "  [WARN] $m" -ForegroundColor Yellow }
function Fail ($m) { Write-Host "  [FAIL] $m" -ForegroundColor Red }

function Have ($cmd) { return [bool](Get-Command $cmd -CommandType Application -ErrorAction SilentlyContinue) }

Head "Locating repository"
$repo = Resolve-JalakRepo $RepoDir
Ok "repo root: $repo"
New-Item -ItemType Directory -Force -Path (Join-Path $repo '.buildtmp') | Out-Null
$log = Join-Path $repo '.buildtmp/install-windows.log'
Start-Transcript -Path $log -Append | Out-Null
$transcriptStarted = $true

# --- Prerequisites ---------------------------------------------------------
Head "Checking prerequisites"
$pyOk = (Have "py") -or (Have "python")
if (-not $SkipBackend -and -not $pyOk) { throw 'Python is required for the requested backend setup/tests.' }
# Host C++ compiler checks belong to build-host-tests.ps1, not an IDF target build.

# --- Backend ---------------------------------------------------------------
if (-not $SkipBackend) {
    Head "Backend — install deps + run tests"
    Push-Location (Join-Path $repo "backend")
    try {
        if ($pyOk) {
            $py = if (Have "py") { "py" } else { "python" }
            Invoke-NativeChecked 'backend deps' $py @('-m', 'pip', 'install', 'starlette', 'uvicorn', 'pytest', 'httpx', 'anyio')
            Ok "deps installed"
            Invoke-NativeChecked 'backend pytest' $py @('-m', 'pytest', '-q')
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
    if (Test-Path (Join-Path $IdfPath "export.ps1")) {
        Ok "ESP-IDF source present at $IdfPath; installation/activation still must pass"
    } else {
        if (-not (Have 'git')) { throw 'Git is required to clone ESP-IDF.' }
        Say "  cloning ESP-IDF $IdfVersion (recursive) into $IdfPath ..."
        New-Item -ItemType Directory -Force -Path (Split-Path $IdfPath) | Out-Null
        if (Test-Path -LiteralPath $IdfPath) { throw "IdfPath exists but has no export.ps1: $IdfPath. Choose a new installation directory." }
        Invoke-NativeChecked 'git clone esp-idf' 'git' @('clone', '-b', $IdfVersion, '--recursive', 'https://github.com/espressif/esp-idf.git', $IdfPath)
        Ok "cloned"
    }

    Say "  running install.ps1 $Target (downloads Xtensa toolchain; several minutes)..."
    Push-Location $IdfPath
    try {
        $LASTEXITCODE = 0
        & .\install.ps1 $Target
        if ($LASTEXITCODE -ne 0) { throw "install.ps1 failed (exit $LASTEXITCODE)" }
        Ok "toolchain installed"
    } finally { Pop-Location }

    Say 'Host tests are separate: tools\build-host-tests.ps1 (requires a host C++ compiler).'

    # Activation must persist here, not inside a helper's scriptblock scope.
    $LASTEXITCODE = 0
    . (Join-Path $IdfPath 'export.ps1')
    if ($LASTEXITCODE -ne 0) { throw "export.ps1 failed (exit $LASTEXITCODE)" }
    $sdk = Get-ActivatedIdf $IdfPath $IdfVersion
    $build = Initialize-JalakBuild $repo $sdk.Path $BuildDir $Sdkconfig $sdk.Compiler
    $idfArgs = @($sdk.IdfPy, '-C', $build.Source, '-B', $build.Directory, '-D', 'IDF_TARGET=esp32s3', '-D', "SDKCONFIG=$($build.Sdkconfig)")

    Head "Firmware — build ESP32-S3 target image"
    Push-Location (Join-Path $repo "firmware")
    try {
        Invoke-NativeChecked 'IDF build' $sdk.Python ($idfArgs + @('build'))
        Assert-JalakArtifacts $build.Directory
        $script:BuildOk = $true
        Ok "target build succeeded -> $($build.Directory)\sijalak_bedas.bin"
    } catch {
        Fail "TARGET BUILD FAILED: $_"
        Say "  Full log: $log"
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
    Say "         powershell -ExecutionPolicy Bypass -File `"$repo\tools\flash-windows.ps1`" -Port COMx"
    Say ""
    Say "REMINDER: a successful build does NOT mean the device works. Pins, the I2C"
    Say "address, I2S clocking and SD behavior are UNVERIFIED until tested on"
    Say "a real board."
} else {
    Say "Setup finished (no target build requested or build skipped)."
}
} catch {
    Write-Host "[FAIL] $_" -ForegroundColor Red
    if ($log) { Write-Host "Log: $log" }
    exit 1
} finally {
    if ($transcriptStarted) { Stop-Transcript | Out-Null }
}

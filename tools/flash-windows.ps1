# =====================================================================
#  Si Jalak Bedas — WINDOWS: build -> explicit-port flash
#
#  Requires an installed ESP-IDF 5.4 S3 environment. No automatic installation.
#
#  Cara pakai:
#     powershell -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -NoFlash
#   atau klik dua kali: tools\flash-windows.bat
#
#  Yang dilakukan (berurutan, BERHENTI di error pertama):
#     1. Validate repository and activate the installed SDK in script scope.
#     2. Check activated Python, IDF version and S3 compiler availability.
#     3. Use a separate owned build directory; preserve menuconfig/cache.
#     4. (opsional) Konversi aset Mochi -> firmware\preview\assets.
#     5. Build with explicit Python + full tools/idf.py path.
#     6. Validate output, then flash only the explicitly supplied COM port.
#
#  TIDAK mengganti konfigurasi global, TIDAK commit/push, TIDAK mengubah repo
#  orang lain. Aset pihak ketiga hanya ditulis ke folder git-ignored lokal.
# =====================================================================

[CmdletBinding()]
param(
    [string]$IdfVersion = "v5.4",
    [string]$IdfPath    = "$HOME\esp\esp-idf",
    [string]$RepoDir    = "",            # default: script-\.. (repo tempat skrip berada)
    [string]$MochiRef   = "",            # path ke clone watcher-mochi (opsional)
    [string]$Port       = "",            # explicit board port; never auto-pick
    [ValidateRange(1, 2000000)][int]$Baud = 460800,
    [string]$BuildDir   = "",            # default: repo/.buildtmp/idf-esp32s3
    [string]$Sdkconfig  = "",            # separate persistent menuconfig file
    [switch]$SkipAssets,                 # lewati konversi aset Mochi
    [switch]$Monitor,                    # buka serial monitor setelah flash
    [switch]$NoFlash                     # hanya build, jangan flash
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot 'windows-common.ps1')
$transcriptStarted = $false
try {
if (-not $NoFlash -and $Port -notmatch '^COM[1-9][0-9]*$') {
    throw 'Flash requires an explicit -Port COMx verified in Device Manager. Use -NoFlash to build only.'
}
if ($Monitor -and $NoFlash) { throw '-Monitor cannot be used with -NoFlash.' }

function Say  ($m) { Write-Host $m }
function Head ($m) { Write-Host ""; Write-Host "== $m ==" -ForegroundColor Cyan }
function Ok   ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function Warn ($m) { Write-Host "  [WARN] $m" -ForegroundColor Yellow }

# ---------------------------------------------------------------------
Head "Locating repository"
# ---------------------------------------------------------------------
$repo = Resolve-JalakRepo $RepoDir
Ok "repo: $repo"
New-Item -ItemType Directory -Force -Path (Join-Path $repo '.buildtmp') | Out-Null
$log = Join-Path $repo '.buildtmp/flash-windows.log'
Start-Transcript -Path $log -Append | Out-Null
$transcriptStarted = $true

# ---------------------------------------------------------------------
Head "ESP-IDF ($IdfVersion)"
# ---------------------------------------------------------------------
$exportPs1 = Join-Path $IdfPath "export.ps1"
if (-not (Test-Path -LiteralPath $exportPs1 -PathType Leaf)) {
    throw "ESP-IDF not installed at $IdfPath. Run the Espressif installer or tools/install-windows.ps1 separately."
}

# Activate ESP-IDF in THIS shell (dot-source so PATH/IDF_PATH persist).
$LASTEXITCODE = 0
. $exportPs1
if ($LASTEXITCODE -ne 0) { throw "export.ps1 failed (exit $LASTEXITCODE). Run the SDK install.ps1 esp32s3 separately if tools are missing." }
$sdk = Get-ActivatedIdf $IdfPath $IdfVersion

# ---------------------------------------------------------------------
Head "(optional) Convert Mochi assets"
# ---------------------------------------------------------------------
if (-not $SkipAssets) {
    $conv = Join-Path $repo "tools\mochi_assets.py"
    if ($MochiRef -ne "" -and (Test-Path $MochiRef) -and (Test-Path $conv)) {
            $out = Join-Path $repo "firmware\preview\assets"
            Say "  converting GIFs + pop sound -> $out"
            Invoke-NativeChecked 'mochi_assets.py' $sdk.Python @($conv, '--mochi', $MochiRef, '--out', $out)
            Ok "assets converted (copy firmware\preview\assets\* to the SD card root)"
    } else {
        Warn "no -MochiRef given (or tools\mochi_assets.py missing); skipping assets"
        Warn "the demo still runs with built-in eyes and UI tones only"
    }
}

# ---------------------------------------------------------------------
Head "Build firmware"
# ---------------------------------------------------------------------
$build = Initialize-JalakBuild $repo $sdk.Path $BuildDir $Sdkconfig $sdk.Compiler
$idfArgs = @($sdk.IdfPy, '-C', $build.Source, '-B', $build.Directory, '-D', 'IDF_TARGET=esp32s3', '-D', "SDKCONFIG=$($build.Sdkconfig)")

Push-Location (Join-Path $repo "firmware")
try {
    Invoke-NativeChecked 'IDF build' $sdk.Python ($idfArgs + @('build'))
    Assert-JalakArtifacts $build.Directory
    Ok "built: $($build.Directory)\sijalak_bedas.bin"
}
finally { Pop-Location }

if ($NoFlash) {
    Head "Done (build only)"
    Say "-NoFlash set: not flashing. To flash later:"
    Say "  powershell -ExecutionPolicy Bypass -File `"$PSCommandPath`" -Port COMx"
} else {

# ---------------------------------------------------------------------
Head "Explicit serial port: $Port"
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
Head "Flash"
# ---------------------------------------------------------------------
Push-Location (Join-Path $repo "firmware")
try {
    if ($Monitor) {
        Say "  flashing + opening monitor ($Port @ $Baud). Press Ctrl+] to exit."
        Invoke-NativeChecked 'IDF flash/monitor' $sdk.Python ($idfArgs + @('-p', $Port, '-b', "$Baud", 'flash', 'monitor'))
    } else {
        Say "  flashing ($Port @ $Baud)..."
        Invoke-NativeChecked 'IDF flash' $sdk.Python ($idfArgs + @('-p', $Port, '-b', "$Baud", 'flash'))
        Ok "flash complete"
    }
}
finally { Pop-Location }
}

Head "Done"
Say "If the board did not behave as expected:"
Say "  - Hold BOOT, press RST, release BOOT, then flash again (download mode)."
Say "  - Check OLED wiring (SDA8/SCL9), touch polarity (GPIO2), and SD formatting."
Say "Manual commands need export.ps1 in the PARENT terminal; child activation never propagates back."
Say ""
Say "REMINDER: hardware pins/controller/polarity are UNVERIFIED. First-run success"
Say "depends on your actual module matching the assumed wiring."
} catch {
    Write-Host "[FAIL] $_" -ForegroundColor Red
    if ($log) { Write-Host "Log: $log" }
    exit 1
} finally {
    if ($transcriptStarted) { Stop-Transcript | Out-Null }
}

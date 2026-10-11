# Offline parser + behavioral mocks. Never invokes real IDF, serial or installers.
param([string]$TestDir = '')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
if (-not $TestDir) { $TestDir = Join-Path $repo ('.buildtmp/windows-mock-' + [guid]::NewGuid().ToString('N')) }
New-Item -ItemType Directory -Force -Path $TestDir | Out-Null
function Assert([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
foreach ($file in Get-ChildItem (Join-Path $repo 'tools') -Filter '*.ps1') {
    $tokens = $null; $errors = $null
    [void][Management.Automation.Language.Parser]::ParseFile($file.FullName, [ref]$tokens, [ref]$errors)
    Assert ($errors.Count -eq 0) ("Parser errors in $file : $errors")
}
. (Join-Path $repo 'tools/windows-common.ps1')
$shell = (Get-Process -Id $PID).Path
$failed = $false
try { Invoke-NativeChecked 'deliberate fail' $shell @('-NoProfile', '-Command', 'exit 7') }
catch { $failed = $_ -match 'exit 7' }
Assert $failed 'Native exit code was swallowed'
Invoke-NativeChecked 'success after stale exit' $shell @('-NoProfile', '-Command', 'exit 0')
$failed = $false
try { Resolve-JalakRepo (Join-Path $TestDir 'nonexistent') } catch { $failed = $true }
Assert $failed 'Invalid RepoDir must fail, not clone'
$sdkDir = Join-Path $TestDir 'sdk'
$fakeRepo = Join-Path $TestDir 'repo with spaces'
foreach ($dir in @($sdkDir, (Join-Path $fakeRepo 'tools'), (Join-Path $fakeRepo 'firmware/components/jalak_app/src'))) {
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
}
foreach ($name in @('windows-common.ps1', 'flash-windows.ps1', 'install-windows.ps1')) {
    Copy-Item -LiteralPath (Join-Path $repo "tools/$name") -Destination (Join-Path $fakeRepo "tools/$name")
}
foreach ($name in @('firmware/CMakeLists.txt', 'firmware/sdkconfig.defaults', 'firmware/components/jalak_app/Kconfig.projbuild', 'firmware/components/jalak_app/src/app_main.cpp')) {
    Set-Content -LiteralPath (Join-Path $fakeRepo $name) -Value '# mock source'
}
Set-Content -LiteralPath (Join-Path $sdkDir 'install.ps1') -Value '$global:LASTEXITCODE = 0'
# This export defines shell state/functions, deliberately testing persistent script scope.
$mockExport = @'
$script:ActivationProof = 'active'
$env:IDF_PATH = $PSScriptRoot
function Get-ActivatedIdf {
    param($IdfPath, $IdfVersion)
    if ($script:ActivationProof -ne 'active') { throw 'activation scope lost' }
    return [pscustomobject]@{ Python = 'mock-python'; IdfPy = 'mock-idf.py'; Path = $IdfPath }
}
function Invoke-NativeChecked {
    param($What, $Command, $Arguments)
    if ($script:ActivationProof -ne 'active') { throw 'activation scope lost' }
    Add-Content -LiteralPath $env:JALAK_MOCK_CALLS -Value ($Arguments -join '|')
    if ($env:JALAK_MOCK_CASE -eq 'native-fail') { throw 'mock native exit 7' }
    if ($Arguments -contains 'build') {
        $dir = $Arguments[[array]::IndexOf($Arguments, '-B') + 1]
        Set-Content (Join-Path $dir 'flash_args') -Value 'mock'
        Set-Content (Join-Path $dir 'flasher_args.json') -Value '{"extra_esptool_args":{"chip":"esp32s3"},"flash_files":{"0x10000":"sijalak_bedas.bin"}}'
        if ($env:JALAK_MOCK_CASE -ne 'missing-bin') { Set-Content (Join-Path $dir 'sijalak_bedas.bin') -Value 'mock' }
    }
}
$global:LASTEXITCODE = 0
'@
Set-Content -LiteralPath (Join-Path $sdkDir 'export.ps1') -Value $mockExport
foreach ($case in @('success', 'native-fail', 'missing-bin')) {
    $env:JALAK_MOCK_CASE = $case
    $env:JALAK_MOCK_CALLS = Join-Path $TestDir "$case-calls.txt"
    & $shell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $fakeRepo 'tools/flash-windows.ps1') -RepoDir $fakeRepo -IdfPath $sdkDir -NoFlash -SkipAssets -BuildDir (Join-Path $TestDir "$case-build")
    $code = $LASTEXITCODE
    Assert (($case -eq 'success' -and $code -eq 0) -or ($case -ne 'success' -and $code -ne 0)) "Unexpected exit for $case : $code"
    $calls = Get-Content -LiteralPath $env:JALAK_MOCK_CALLS -Raw
    Assert ($calls -notmatch 'flash|monitor|set-target|fullclean') 'NoFlash reached flashing/destructive action'
}
# Empty/invalid COM must fail before activation/build, without even mock flash.
foreach ($port in @('', 'COM0', 'COMx', 'COM3 invalid', 'LPT1')) {
    & $shell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $fakeRepo 'tools/flash-windows.ps1') -RepoDir $fakeRepo -IdfPath $sdkDir -Port $port -SkipAssets
    Assert ($LASTEXITCODE -ne 0) "Invalid port accepted: $port"
}
# Installer uses a dummy install.ps1 and skips backend: no downloads/deps touched.
$env:JALAK_MOCK_CASE = 'success'
$env:JALAK_MOCK_CALLS = Join-Path $TestDir 'installer-calls.txt'
& $shell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $fakeRepo 'tools/install-windows.ps1') -RepoDir $fakeRepo -IdfPath $sdkDir -SkipBackend -BuildDir (Join-Path $TestDir 'installer-build')
Assert ($LASTEXITCODE -eq 0) 'Installer mock activation/build failed'
$buildDir = Join-Path $TestDir 'owned-build'
$config = Join-Path $TestDir 'config'
$build = Initialize-JalakBuild $fakeRepo $sdkDir $buildDir $config
Set-Content -LiteralPath $config -Value 'CONFIG_IDF_TARGET="esp32s3"'
$before = Get-Content -LiteralPath $config -Raw
$again = Initialize-JalakBuild $fakeRepo $sdkDir $buildDir $config
Assert ((Get-Content -LiteralPath $config -Raw) -eq $before) 'menuconfig overwritten'
Set-Content -LiteralPath (Join-Path $buildDir 'CMakeCache.txt') -Value 'IDF_TARGET:STRING=esp32'
$failed = $false
try { Initialize-JalakBuild $fakeRepo $sdkDir $buildDir $config } catch { $failed = $true }
Assert $failed 'Wrong cached target accepted'
$failed = $false
try { Initialize-JalakBuild $fakeRepo $sdkDir $TestDir $config } catch { $failed = $true }
Assert $failed 'Unowned directory accepted'
Set-Content -LiteralPath (Join-Path $buildDir 'CMakeCache.txt') -Value 'CMAKE_HOME_DIRECTORY:INTERNAL=/wrong/source'
$failed = $false
try { Initialize-JalakBuild $fakeRepo $sdkDir $buildDir $config } catch { $failed = $true }
Assert $failed 'Wrong source cache accepted'
Set-Content -LiteralPath (Join-Path $buildDir 'CMakeCache.txt') -Value 'CMAKE_C_COMPILER:FILEPATH=/wrong/compiler'
$failed = $false
try { Initialize-JalakBuild $fakeRepo $sdkDir $buildDir $config '/expected/compiler' } catch { $failed = $true }
Assert $failed 'Wrong compiler cache accepted'
# Validate metadata even when a seemingly successful build left an old .bin.
$artifactDir = Join-Path $TestDir 'artifact-tests'
New-Item -ItemType Directory -Path $artifactDir | Out-Null
Set-Content (Join-Path $artifactDir 'sijalak_bedas.bin') -Value 'mock'
Set-Content (Join-Path $artifactDir 'flash_args') -Value 'mock'
foreach ($metadata in @(
    '{"extra_esptool_args":{"chip":"esp32"},"flash_files":{"0x0":"sijalak_bedas.bin"}}',
    '{"extra_esptool_args":{"chip":"esp32s3"},"flash_files":{}}',
    '{"extra_esptool_args":{"chip":"esp32s3"},"flash_files":{"0x0":"missing.bin"}}',
    '{"extra_esptool_args":{"chip":"esp32s3"},"flash_files":{"0x0":"sijalak_bedas.bin","0x1":"missing.bin"}}'
)) {
    Set-Content (Join-Path $artifactDir 'flasher_args.json') -Value $metadata
    $failed = $false
    try { Assert-JalakArtifacts $artifactDir } catch { $failed = $true }
    Assert $failed 'Invalid/incomplete flash metadata accepted'
}
Write-Host 'Windows parser/native-failure/scope/NoFlash/artifacts/config mocks passed (NOT real SDK/COM validation)'

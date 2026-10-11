# Shared native-command and non-destructive build checks. Activation is deliberately
# NOT wrapped here: callers must dot-source export.ps1 at their own script scope.
function Invoke-NativeChecked {
    param([string]$What, [string]$Command, [string[]]$Arguments = @())
    $LASTEXITCODE = 0
    & $Command @Arguments
    $code = $LASTEXITCODE
    if ($code -ne 0) { throw "$What failed (exit $code). Fix the first error above." }
}

function Resolve-JalakRepo {
    param([string]$RepoDir)
    if (-not $RepoDir) { $RepoDir = Join-Path $PSScriptRoot '..' }
    if (-not (Test-Path -LiteralPath $RepoDir -PathType Container)) { throw "RepoDir does not exist: $RepoDir" }
    $repo = (Resolve-Path -LiteralPath $RepoDir).Path
    foreach ($file in @('firmware/CMakeLists.txt', 'firmware/sdkconfig.defaults',
                        'firmware/components/jalak_app/src/app_main.cpp',
                        'firmware/components/jalak_app/Kconfig.projbuild', 'tools/windows-common.ps1')) {
        if (-not (Test-Path -LiteralPath (Join-Path $repo $file) -PathType Leaf)) {
            throw "Incomplete/wrong RepoDir: $repo (missing $file). Use the full repository root, not firmware."
        }
    }
    return $repo
}

function Get-ActivatedIdf {
    param([string]$IdfPath, [string]$IdfVersion)
    $expected = (Resolve-Path -LiteralPath $IdfPath).Path
    if (-not $env:IDF_PATH -or -not (Test-Path -LiteralPath $env:IDF_PATH) -or
        (Resolve-Path -LiteralPath $env:IDF_PATH).Path -ne $expected) {
        throw 'export.ps1 did not activate the requested IDF_PATH.'
    }
    $pythonCmd = Get-Command python -CommandType Application -ErrorAction Stop
    $python = $pythonCmd.Source
    $idf = Join-Path $expected 'tools/idf.py'
    if (-not (Test-Path -LiteralPath $idf -PathType Leaf)) { throw "Missing $idf" }
    $version = @(Invoke-NativeChecked 'IDF version/dependencies' $python @($idf, '--version')) -join "`n"
    $wanted = [regex]::Escape($IdfVersion)
    if ($version -notmatch "ESP-IDF $wanted(?:\.\d+)?(?:[-\s]|$)") {
        throw "Expected ESP-IDF $IdfVersion; activated: $version"
    }
    Write-Host "Activated: $version; Python: $python"
    foreach ($tool in @('cmake', 'ninja')) { Get-Command $tool -CommandType Application -ErrorAction Stop | Out-Null }
    # Use this SDK's actual S3 compiler name, not a hardcoded obsolete prefix.
    $toolchain = Join-Path $expected 'tools/cmake/toolchain-esp32s3.cmake'
    $text = Get-Content -LiteralPath $toolchain -Raw
    if ($text -notmatch 'set\(CMAKE_C_COMPILER\s+([\w-]+)\)') { throw "Cannot identify S3 compiler in $toolchain" }
    $compiler = (Get-Command $Matches[1] -CommandType Application -ErrorAction Stop).Source
    Invoke-NativeChecked 'S3 compiler availability (not final-link health)' $compiler @('--version') | Out-Host
    return [pscustomobject]@{ Python = $python; IdfPy = $idf; Path = $expected; Compiler = $compiler }
}

function Initialize-JalakBuild {
    param([string]$Repo, [string]$IdfPath, [string]$BuildDir, [string]$Sdkconfig, [string]$Compiler = '')
    $source = (Resolve-Path -LiteralPath (Join-Path $Repo 'firmware')).Path
    if (-not $BuildDir) { $BuildDir = Join-Path $Repo '.buildtmp/idf-esp32s3' }
    if (-not $Sdkconfig) { $Sdkconfig = Join-Path $Repo '.buildtmp/sdkconfig-esp32s3' }
    # PowerShell's provider location can differ from .NET's process cwd.
    if (-not [IO.Path]::IsPathRooted($BuildDir)) { $BuildDir = Join-Path (Get-Location).Path $BuildDir }
    if (-not [IO.Path]::IsPathRooted($Sdkconfig)) { $Sdkconfig = Join-Path (Get-Location).Path $Sdkconfig }
    $build = [IO.Path]::GetFullPath($BuildDir)
    $config = [IO.Path]::GetFullPath($Sdkconfig)
    $ownerFile = Join-Path $build '.jalak-build.json'
    $owner = [pscustomobject]@{ Source = $source; IdfPath = $IdfPath; Target = 'esp32s3'; Sdkconfig = $config }
    if (Test-Path -LiteralPath $build) {
        if (-not (Test-Path -LiteralPath $ownerFile -PathType Leaf)) {
            throw "BuildDir already exists and is not owned by this launcher: $build. Choose a NEW -BuildDir; do not delete old caches."
        }
        $old = Get-Content -LiteralPath $ownerFile -Raw | ConvertFrom-Json
        foreach ($key in @('Source', 'IdfPath', 'Target', 'Sdkconfig')) {
            if ($old.$key -ne $owner.$key) { throw "BuildDir ownership mismatch ($key): choose a new -BuildDir." }
        }
        $cache = Join-Path $build 'CMakeCache.txt'
        if (Test-Path -LiteralPath $cache) {
            $text = Get-Content -LiteralPath $cache -Raw
            foreach ($entry in @(@('CMAKE_HOME_DIRECTORY', $source), @('IDF_TARGET', 'esp32s3'), @('IDF_PATH', $IdfPath))) {
                if ($text -match "(?m)^$($entry[0]):[^=]+=(.+)\r?$") {
                    $actual = $Matches[1].Trim().Replace('\', '/')
                    if ($actual -ne $entry[1].Replace('\', '/')) { throw "Cache $($entry[0]) mismatch: choose a new -BuildDir." }
                }
            }
            if ($Compiler -and $text -match '(?m)^CMAKE_C_COMPILER:[^=]+=(.+)\r?$') {
                if ($Matches[1].Trim().Replace('\', '/') -ne $Compiler.Replace('\', '/')) {
                    throw 'Cached compiler differs from the activated S3 compiler. Choose a new -BuildDir.'
                }
            }
        }
    } else {
        New-Item -ItemType Directory -Path $build -Force | Out-Null
        $owner | ConvertTo-Json | Set-Content -LiteralPath $ownerFile -Encoding UTF8
    }
    New-Item -ItemType Directory -Path (Split-Path $config -Parent) -Force | Out-Null
    if (Test-Path -LiteralPath $config) {
        $text = Get-Content -LiteralPath $config -Raw
        if ($text -notmatch '(?m)^CONFIG_IDF_TARGET="esp32s3"\s*$') { throw "Sdkconfig is not esp32s3: $config. Choose a new -Sdkconfig; existing configuration is preserved." }
    }
    return [pscustomobject]@{ Directory = $build; Sdkconfig = $config; Source = $source }
}

function Assert-JalakArtifacts {
    param([string]$BuildDir)
    foreach ($file in @('sijalak_bedas.bin', 'flasher_args.json', 'flash_args')) {
        $path = Join-Path $BuildDir $file
        if (-not (Test-Path -LiteralPath $path -PathType Leaf) -or (Get-Item -LiteralPath $path).Length -eq 0) {
            throw "Build output missing/empty: $path. Do not flash."
        }
    }
    $metadata = Get-Content -LiteralPath (Join-Path $BuildDir 'flasher_args.json') -Raw | ConvertFrom-Json
    if ($metadata.extra_esptool_args.chip -ne 'esp32s3' -or -not $metadata.flash_files -or
        @($metadata.flash_files.PSObject.Properties).Count -eq 0) {
        throw 'Flash metadata is not an ESP32-S3 image.'
    }
    if (@($metadata.flash_files.PSObject.Properties.Value) -notcontains 'sijalak_bedas.bin') {
        throw 'Flash metadata does not reference sijalak_bedas.bin.'
    }
    foreach ($file in $metadata.flash_files.PSObject.Properties.Value) {
        $path = Join-Path $BuildDir $file
        $full = [IO.Path]::GetFullPath($path)
        $prefix = [IO.Path]::GetFullPath($BuildDir).TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
        if (-not $full.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) { throw "Flash image outside BuildDir: $file" }
        if (-not (Test-Path -LiteralPath $path -PathType Leaf) -or (Get-Item -LiteralPath $path).Length -eq 0) {
            throw "Referenced flash image missing/empty: $path"
        }
    }
}

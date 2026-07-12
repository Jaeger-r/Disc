param(
    [string]$QtRoot = $env:Qt6_DIR,
    [string]$BuildDir = "",
    [string]$Generator = "Ninja",
    [string]$Config = "Release",
    [string]$PackageName = "DiskClient-windows-x64",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

function Resolve-RequiredTool {
    param(
        [string]$ToolName,
        [string]$HintPath = ""
    )

    $command = Get-Command $ToolName -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    if ($HintPath -and (Test-Path $HintPath)) {
        return (Resolve-Path $HintPath).Path
    }

    throw "Required tool '$ToolName' was not found."
}

function Convert-ToQtRoot {
    param([string]$Path)

    if (-not $Path) {
        return ""
    }

    $resolved = [System.IO.Path]::GetFullPath($Path)

    if (Test-Path (Join-Path $resolved "Qt6Config.cmake")) {
        $resolved = [System.IO.Path]::GetFullPath((Join-Path $resolved "../../.."))
    }

    if ((Split-Path -Leaf $resolved) -eq "bin" -and (Test-Path (Join-Path $resolved "windeployqt.exe"))) {
        $resolved = Split-Path -Parent $resolved
    }

    if (Test-Path (Join-Path $resolved "bin/windeployqt.exe")) {
        return $resolved
    }

    return ""
}

function Resolve-QtRoot {
    param([string]$PreferredRoot)

    $candidates = @(
        $PreferredRoot,
        $env:Qt6_DIR,
        $env:QT_ROOT_DIR,
        $env:Qt6_ROOT,
        $env:QTDIR
    )

    foreach ($candidate in $candidates) {
        $root = Convert-ToQtRoot $candidate
        if ($root) {
            return $root
        }
    }

    $searchRoots = @(
        "C:\Qt",
        $env:RUNNER_WORKSPACE,
        (Split-Path -Parent $env:GITHUB_WORKSPACE)
    ) | Where-Object { $_ -and (Test-Path $_) }

    foreach ($searchRoot in $searchRoots) {
        $windeployqt = Get-ChildItem -Path $searchRoot -Filter "windeployqt.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($windeployqt) {
            $root = Split-Path -Parent (Split-Path -Parent $windeployqt.FullName)
            if (Test-Path (Join-Path $root "bin/windeployqt.exe")) {
                return $root
            }
        }
    }

    throw "QtRoot is empty or invalid. Pass -QtRoot, set Qt6_DIR/QTDIR, or install Qt for Windows."
}

$ProjectRoot = Split-Path -Parent $PSScriptRoot
if (-not $BuildDir) {
    $BuildDir = Join-Path $ProjectRoot "build/windows-release"
}
$BuildDir = [System.IO.Path]::GetFullPath($BuildDir)

$OutRoot = Join-Path $ProjectRoot "out"
$PackageDir = Join-Path $OutRoot $PackageName
$ZipPath = Join-Path $OutRoot ($PackageName + ".zip")

New-Item -ItemType Directory -Force -Path $OutRoot | Out-Null

$QtRoot = Resolve-QtRoot $QtRoot
$QtBinDir = if (Test-Path (Join-Path $QtRoot "bin")) { Join-Path $QtRoot "bin" } else { $QtRoot }

$cmake = Resolve-RequiredTool "cmake"
$windeployqt = Resolve-RequiredTool "windeployqt" (Join-Path $QtBinDir "windeployqt.exe")

if (-not $SkipBuild) {
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    & $cmake -S $ProjectRoot -B $BuildDir -G $Generator "-DCMAKE_BUILD_TYPE=$Config" "-DCMAKE_PREFIX_PATH=$QtRoot"
    & $cmake --build $BuildDir --config $Config --target DiskClientApp
}

$exePath = Join-Path $BuildDir "DiskClient.exe"
if (-not (Test-Path $exePath)) {
    $exePath = Join-Path $BuildDir $Config | Join-Path -ChildPath "DiskClient.exe"
}
if (-not (Test-Path $exePath)) {
    throw "DiskClient.exe was not found under $BuildDir. Build may have failed or uses a different output layout."
}

if (Test-Path $PackageDir) {
    Remove-Item -Recurse -Force $PackageDir
}
New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null

Copy-Item $exePath (Join-Path $PackageDir "DiskClient.exe")

& $windeployqt `
    --release `
    --compiler-runtime `
    --no-translations `
    --no-system-d3d-compiler `
    --no-opengl-sw `
    (Join-Path $PackageDir "DiskClient.exe")

$tlsDir = Join-Path $PackageDir "tls"
New-Item -ItemType Directory -Force -Path $tlsDir | Out-Null

Copy-Item (Join-Path $ProjectRoot "diskclient.ini") (Join-Path $PackageDir "diskclient.ini") -Force
Copy-Item (Join-Path $ProjectRoot "icon.png") (Join-Path $PackageDir "icon.png") -Force
Copy-Item (Join-Path $ProjectRoot "file.png") (Join-Path $PackageDir "file.png") -Force
Copy-Item (Join-Path $ProjectRoot "tls/ca.crt") (Join-Path $tlsDir "ca.crt") -Force

if (Test-Path $ZipPath) {
    Remove-Item -Force $ZipPath
}
Compress-Archive -Path (Join-Path $PackageDir "*") -DestinationPath $ZipPath

Write-Host "Windows package ready:"
Write-Host "  Directory: $PackageDir"
Write-Host "  Zip file : $ZipPath"

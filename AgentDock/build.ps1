param(
    [string]$ZigPath = "zig",
    [string]$Configuration = "release"
)

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $projectRoot "build"
$distDir = Join-Path $projectRoot "dist"
$source = Join-Path $projectRoot "source\AgentDock.c"
$resourceScript = Join-Path $projectRoot "assets\AgentDock.rc"
$resource = Join-Path $buildDir "AgentDock.res"
$output = Join-Path $distDir "AgentDock.exe"

New-Item -ItemType Directory -Force -Path $buildDir, $distDir | Out-Null
$env:ZIG_GLOBAL_CACHE_DIR = Join-Path $buildDir "zig-global-cache"
$env:ZIG_LOCAL_CACHE_DIR = Join-Path $buildDir "zig-local-cache"

if ($Configuration -notin @("release", "debug")) {
    throw "Configuration must be 'release' or 'debug'."
}

Push-Location $projectRoot
try {
    & $ZigPath rc /nologo "/fo$resource" $resourceScript
    if ($LASTEXITCODE -ne 0) { throw "Resource compilation failed ($LASTEXITCODE)." }

    $optimization = if ($Configuration -eq "release") { "-Oz" } else { "-O0" }
    $strip = if ($Configuration -eq "release") { @("-s") } else { @() }
    $arguments = @(
        "cc", $source, $resource,
        "-target", "x86_64-windows-gnu",
        $optimization,
        "-fno-stack-protector",
        "-fno-unwind-tables",
        "-fno-asynchronous-unwind-tables",
        "-nostdlib",
        "-Wl,--subsystem,windows",
        "-Wl,--entry,mainCRTStartup",
        "-Wl,--gc-sections"
    ) + $strip + @(
        "-lkernel32", "-luser32", "-lgdi32", "-lshell32", "-lole32",
        "-ladvapi32", "-lcomctl32", "-lcomdlg32", "-ldwmapi", "-lwinhttp",
        "-o", $output
    )
    & $ZigPath @arguments
    if ($LASTEXITCODE -eq 0) {
        return
    }

    # Fallback: zig 0.16's lld-link no longer bundles chkstk for windows-gnu target.
    # Provide clang_rt.builtins (contains __chkstk_ms) explicitly via lld-link.
    $builtinsLib = Join-Path $env:USERPROFILE "scoop\apps\llvm\current\lib\clang\22\lib\windows\clang_rt.builtins-x86_64.lib"
    if (-not (Test-Path $builtinsLib)) {
        throw "C compilation or link failed ($LASTEXITCODE). clang_rt.builtins not found at $builtinsLib"
    }
    Write-Host "Link fallback: adding $builtinsLib for __chkstk_ms"
    & $ZigPath @arguments $builtinsLib
    if ($LASTEXITCODE -ne 0) { throw "C compilation or link failed ($LASTEXITCODE)." }
} finally {
    Pop-Location
}

Get-Item $output | Select-Object FullName, Length, LastWriteTime

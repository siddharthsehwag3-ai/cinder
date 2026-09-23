$ErrorActionPreference = "Stop"

$projectRoot = (
    Resolve-Path (
        Join-Path $PSScriptRoot ".."
    )
).Path

$compilerSource = Join-Path `
    $projectRoot `
    "compiler\cinder.c"

$buildDirectory = Join-Path `
    $projectRoot `
    "build"

$webDirectory = Join-Path `
    $projectRoot `
    "web"

$nativeOutput = Join-Path `
    $buildDirectory `
    "cinder.exe"

$wasmOutput = Join-Path `
    $webDirectory `
    "cinder.wasm"

New-Item `
    -ItemType Directory `
    -Force `
    -Path $buildDirectory |
    Out-Null

New-Item `
    -ItemType Directory `
    -Force `
    -Path $webDirectory |
    Out-Null

Write-Host ""
Write-Host `
    "Building Cinder..." `
    -ForegroundColor Cyan

Write-Host ""

$gccCommand =
    Get-Command `
        gcc `
        -ErrorAction SilentlyContinue

if (-not $gccCommand) {
    throw (
        "GCC was not found. " +
        "Add GCC to PATH before building Cinder."
    )
}

$zigCommand =
    Get-Command `
        zig `
        -ErrorAction SilentlyContinue

if (-not $zigCommand) {
    throw (
        "Zig was not found. " +
        "Install Zig and reopen the terminal."
    )
}

Write-Host `
    "[1/2] Building native compiler..." `
    -ForegroundColor Yellow

& $gccCommand.Source `
    $compilerSource `
    -std=c11 `
    -Wall `
    -Wextra `
    -Wpedantic `
    -Werror `
    -O2 `
    -o $nativeOutput

if ($LASTEXITCODE -ne 0) {
    throw (
        "The native Cinder build failed."
    )
}

Write-Host `
    "      Created build\cinder.exe" `
    -ForegroundColor Green

$env:ZIG_GLOBAL_CACHE_DIR =
    Join-Path `
        $buildDirectory `
        "zig-global-cache"

$env:ZIG_LOCAL_CACHE_DIR =
    Join-Path `
        $buildDirectory `
        "zig-local-cache"

Write-Host `
    "[2/2] Building browser WebAssembly..." `
    -ForegroundColor Yellow

& $zigCommand.Source `
    cc `
    $compilerSource `
    -target wasm32-freestanding `
    -Os `
    -nostdlib `
    -Wall `
    -Wextra `
    -Wpedantic `
    -Werror `
    "-Wl,--no-entry" `
    "-Wl,--export=input_ptr" `
    "-Wl,--export=stdin_ptr" `
    "-Wl,--export=input_capacity" `
    "-Wl,--export=stdin_capacity" `
    "-Wl,--export=set_stdin_len" `
    "-Wl,--export=report_ptr" `
    "-Wl,--export=report_len" `
    "-Wl,--export=compile" `
    "-Wl,--export-memory" `
    "-Wl,--initial-memory=33554432" `
    "-Wl,--max-memory=33554432" `
    -o $wasmOutput

if ($LASTEXITCODE -ne 0) {
    throw (
        "The WebAssembly build failed."
    )
}

$nativeSize = (
    Get-Item -LiteralPath $nativeOutput
).Length

$wasmSize = (
    Get-Item -LiteralPath $wasmOutput
).Length

Write-Host `
    "      Created web\cinder.wasm" `
    -ForegroundColor Green

Write-Host ""
Write-Host `
    "Cinder built successfully." `
    -ForegroundColor Cyan

Write-Host "Native size: $nativeSize bytes"
Write-Host "Wasm size:   $wasmSize bytes"
Write-Host "Wasm memory: 33554432 bytes"
Write-Host ""
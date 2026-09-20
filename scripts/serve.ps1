$ErrorActionPreference = "Stop"

$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$webDirectory = Join-Path $projectRoot "web"
$wasmFile = Join-Path $webDirectory "cinder.wasm"

if (-not (Test-Path -LiteralPath $wasmFile)) {
    Write-Host "The WebAssembly compiler has not been built." -ForegroundColor Yellow
    Write-Host "Running scripts\build.ps1 first..." -ForegroundColor Yellow

    & (Join-Path $PSScriptRoot "build.ps1")
}

$pythonCommand = Get-Command python -ErrorAction SilentlyContinue

if (-not $pythonCommand) {
    throw "Python was not found. Install Python or serve the web folder another way."
}

Write-Host ""
Write-Host "Cinder is available at:" -ForegroundColor Cyan
Write-Host "http://localhost:4173" -ForegroundColor Green
Write-Host ""
Write-Host "Press Ctrl+C to stop the server." -ForegroundColor DarkGray
Write-Host ""

& $pythonCommand.Source `
    -m http.server `
    4173 `
    --directory $webDirectory
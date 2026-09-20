$ErrorActionPreference = "Stop"

$projectRoot = (
    Resolve-Path (
        Join-Path $PSScriptRoot ".."
    )
).Path

$compiler = Join-Path `
    $projectRoot `
    "build\cinder.exe"

$buildScript = Join-Path `
    $PSScriptRoot `
    "build.ps1"

Write-Host ""
Write-Host `
    "Building before tests..." `
    -ForegroundColor Cyan

& $buildScript

if ($LASTEXITCODE -ne 0) {
    throw "Cinder could not be built before testing."
}

if (
    -not (
        Test-Path -LiteralPath $compiler
    )
) {
    throw "The Cinder executable was not created."
}

$tests = @(
    @{
        Name = "Arithmetic"
        File = "tests\arithmetic.c"
        ExpectedOutput = "result = 68`n"
        ExpectedResult = 68
    },
    @{
        Name = "Functions and recursion"
        File = "tests\functions.c"
        ExpectedOutput =
            "7! = 5040`n" +
            "fib(10) = 55`n"
        ExpectedResult = 0
    },
    @{
        Name = "Control flow"
        File = "tests\control-flow.c"
        ExpectedOutput = "total = 62`n"
        ExpectedResult = 62
    },
    @{
        Name = "Extended language features"
        File = "tests\extended.c"
        ExpectedOutput =
            "char=A value=8 bits=3 total=10`n" +
            "formats=255 ff FF 377`n"
        ExpectedResult = 8
    }
)

$diagnosticTests = @(
    @{
        Name = "Division by zero diagnostic"
        File = "tests\diagnostics.c"
        ExpectedError = "Division by zero."
    }
)

Write-Host ""
Write-Host `
    "Running successful-program tests..." `
    -ForegroundColor Cyan

Write-Host ""

$passed = 0
$total =
    $tests.Count +
    $diagnosticTests.Count

foreach ($test in $tests) {
    $testPath = Join-Path `
        $projectRoot `
        $test.File

    $rawReport =
        & $compiler $testPath

    if (-not $rawReport) {
        throw (
            "$($test.Name) did not produce " +
            "a compiler report."
        )
    }

    try {
        $report =
            $rawReport |
            ConvertFrom-Json
    } catch {
        throw @"
$($test.Name) produced invalid JSON.

Received:
$rawReport
"@
    }

    if (-not $report.ok) {
        throw (
            "$($test.Name) failed: " +
            "$($report.error)"
        )
    }

    if (
        $report.stdout -ne
        $test.ExpectedOutput
    ) {
        throw @"
$($test.Name) produced incorrect output.

Expected:
$($test.ExpectedOutput)

Received:
$($report.stdout)
"@
    }

    if (
        $report.result -ne
        $test.ExpectedResult
    ) {
        throw @"
$($test.Name) returned the wrong value.

Expected: $($test.ExpectedResult)
Received: $($report.result)
"@
    }

    $passed++

    Write-Host `
        "PASS  $($test.Name)" `
        -ForegroundColor Green

    Write-Host (
        "      " +
        "$($report.tokens.Count) tokens"
    )

    Write-Host (
        "      " +
        "$($report.instructions.Count) instructions"
    )

    Write-Host (
        "      " +
        "$($report.steps) execution steps"
    )
}

Write-Host ""
Write-Host `
    "Running diagnostic tests..." `
    -ForegroundColor Cyan

Write-Host ""

foreach ($test in $diagnosticTests) {
    $testPath = Join-Path `
        $projectRoot `
        $test.File

    $rawReport =
        & $compiler $testPath

    if (-not $rawReport) {
        throw (
            "$($test.Name) did not produce " +
            "a compiler report."
        )
    }

    try {
        $report =
            $rawReport |
            ConvertFrom-Json
    } catch {
        throw @"
$($test.Name) produced invalid JSON.

Received:
$rawReport
"@
    }

    if ($report.ok) {
        throw (
            "$($test.Name) was expected " +
            "to fail, but it succeeded."
        )
    }

    if (
        $report.error -notlike
        "*$($test.ExpectedError)*"
    ) {
        throw @"
$($test.Name) returned the wrong diagnostic.

Expected to contain:
$($test.ExpectedError)

Received:
$($report.error)
"@
    }

    $passed++

    Write-Host `
        "PASS  $($test.Name)" `
        -ForegroundColor Green

    Write-Host (
        "      $($report.error)"
    )
}

Write-Host ""

if ($passed -ne $total) {
    throw (
        "$passed of $total tests passed."
    )
}

Write-Host `
    "$passed of $total tests passed." `
    -ForegroundColor Cyan

Write-Host ""
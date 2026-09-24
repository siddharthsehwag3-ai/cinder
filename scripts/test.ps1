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

function Read-CinderReport {
    param(
        [Parameter(Mandatory = $true)]
        [string]$TestName,

        [Parameter(Mandatory = $true)]
        [string]$SourceFile,

        [string]$InputFile = ""
    )

    $sourcePath = Join-Path `
        $projectRoot `
        $SourceFile

    if (
        -not (
            Test-Path -LiteralPath $sourcePath
        )
    ) {
        throw (
            "$TestName source file does not exist: " +
            $sourcePath
        )
    }

    if ($InputFile) {
        $inputPath = Join-Path `
            $projectRoot `
            $InputFile

        if (
            -not (
                Test-Path -LiteralPath $inputPath
            )
        ) {
            throw (
                "$TestName input file does not exist: " +
                $inputPath
            )
        }

        $rawReport = & $compiler `
            $sourcePath `
            $inputPath
    } else {
        $rawReport = & $compiler `
            $sourcePath
    }

    if (-not $rawReport) {
        throw (
            "$TestName did not produce " +
            "a compiler report."
        )
    }

    try {
        return (
            $rawReport |
                ConvertFrom-Json
        )
    } catch {
        throw @"
$TestName produced invalid JSON.

Received:
$rawReport
"@
    }
}

function Assert-SuccessfulProgram {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Test
    )

    $arguments = @{
        TestName = $Test.Name
        SourceFile = $Test.File
    }

    if ($Test.ContainsKey("InputFile")) {
        $arguments.InputFile =
            $Test.InputFile
    }

    $report = Read-CinderReport `
        @arguments

    if (-not $report.ok) {
        $diagnosticText = ""

        if ($report.diagnostics) {
            $diagnosticText = (
                $report.diagnostics |
                    ForEach-Object {
                        (
                            "line {0}, column {1}: {2}" -f
                            $_.line,
                            $_.column,
                            $_.message
                        )
                    }
            ) -join "`n"
        }

        throw @"
$($Test.Name) failed.

Error:
$($report.error)

Diagnostics:
$diagnosticText
"@
    }

    if (
        $report.stdout -ne
        $Test.ExpectedOutput
    ) {
        throw @"
$($Test.Name) produced incorrect output.

Expected:
$($Test.ExpectedOutput)

Received:
$($report.stdout)
"@
    }

    if (
        [long]$report.result -ne
        [long]$Test.ExpectedResult
    ) {
        throw @"
$($Test.Name) returned the wrong value.

Expected: $($Test.ExpectedResult)
Received: $($report.result)
"@
    }

    if (
        -not $report.tokens -or
        $report.tokens.Count -eq 0
    ) {
        throw (
            "$($Test.Name) did not report tokens."
        )
    }

    if (
        -not $report.instructions -or
        $report.instructions.Count -eq 0
    ) {
        throw (
            "$($Test.Name) did not report instructions."
        )
    }

    if (
        $null -eq $report.steps -or
        [long]$report.steps -lt 0
    ) {
        throw (
            "$($Test.Name) reported an invalid " +
            "execution-step count."
        )
    }

    Write-Host `
        "PASS  $($Test.Name)" `
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

function Assert-Diagnostic {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Test
    )

    $report = Read-CinderReport `
        -TestName $Test.Name `
        -SourceFile $Test.File

    if ($report.ok) {
        throw (
            "$($Test.Name) was expected " +
            "to fail, but it succeeded."
        )
    }

    $combinedDiagnostics = @(
        $report.error
    )

    if ($report.diagnostics) {
        $combinedDiagnostics += @(
            $report.diagnostics |
                ForEach-Object {
                    $_.message
                }
        )
    }

    $diagnosticText =
        $combinedDiagnostics -join "`n"

    if (
        $diagnosticText -notlike
        "*$($Test.ExpectedError)*"
    ) {
        throw @"
$($Test.Name) returned the wrong diagnostic.

Expected to contain:
$($Test.ExpectedError)

Received:
$diagnosticText
"@
    }

    if (
        -not $report.diagnostics -or
        $report.diagnostics.Count -eq 0
    ) {
        throw (
            "$($Test.Name) did not include " +
            "structured diagnostic information."
        )
    }

    $firstDiagnostic =
        $report.diagnostics[0]

    if (
        [int]$firstDiagnostic.line -lt 1 -or
        [int]$firstDiagnostic.column -lt 1
    ) {
        throw (
            "$($Test.Name) reported an invalid " +
            "diagnostic source location."
        )
    }

    Write-Host `
        "PASS  $($Test.Name)" `
        -ForegroundColor Green

    Write-Host (
        "      line " +
        "$($firstDiagnostic.line), " +
        "column " +
        "$($firstDiagnostic.column)"
    )

    Write-Host (
        "      $($firstDiagnostic.message)"
    )
}

Write-Host ""
Write-Host `
    "Building Cinder before tests..." `
    -ForegroundColor Cyan

& $buildScript

if ($LASTEXITCODE -ne 0) {
    throw (
        "Cinder could not be built " +
        "before testing."
    )
}

if (
    -not (
        Test-Path -LiteralPath $compiler
    )
) {
    throw (
        "The Cinder executable " +
        "was not created."
    )
}

$successfulTests = @(
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
        Name = "Extended expressions"
        File = "tests\extended.c"
        ExpectedOutput =
            "char=A value=8 bits=3 total=10`n" +
            "formats=255 ff FF 377`n"
        ExpectedResult = 8
    },
    @{
        Name = "Arrays, pointers, strings, and globals"
        File = "tests\advanced-memory.c"
        ExpectedOutput =
            "Cinder memory works`n" +
            "arrays + pointers + strings`n" +
            "global=50 local=30 matrix=21`n" +
            "pointer[2]=6 sizeof-array=20`n"
        ExpectedResult = 101
    },
    @{
        Name = "Structures, enums, switch, and types"
        File = "tests\advanced-types.c"
        ExpectedOutput =
            "point=12,30 total=42`n" +
            "record=4,9 code=65530 grade=A`n" +
            "enum=3 switch=200 const=7`n" +
            "sizes=1,2,4,8`n" +
            "integers=250,-12,65000,100000,15`n"
        ExpectedResult = 255
    },
    @{
        Name = "Standard input"
        File = "tests\advanced-stdin.c"
        InputFile = "tests\advanced-stdin.txt"
        ExpectedOutput =
            "input=Cinder browser input`n" +
            "length=20`n"
        ExpectedResult = 20
    }
)

$diagnosticTests = @(
    @{
        Name = "Division by zero diagnostic"
        File = "tests\diagnostics.c"
        ExpectedError = "Division by zero."
    }
)

$total =
    $successfulTests.Count +
    $diagnosticTests.Count

$passed = 0

Write-Host ""
Write-Host `
    "Running successful-program tests..." `
    -ForegroundColor Cyan
Write-Host ""

foreach ($test in $successfulTests) {
    Assert-SuccessfulProgram `
        -Test $test

    $passed++
}

Write-Host ""
Write-Host `
    "Running diagnostic tests..." `
    -ForegroundColor Cyan
Write-Host ""

foreach ($test in $diagnosticTests) {
    Assert-Diagnostic `
        -Test $test

    $passed++
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

$global:LASTEXITCODE = 0
exit 0
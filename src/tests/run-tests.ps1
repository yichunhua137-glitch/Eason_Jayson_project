$ErrorActionPreference = "Stop"

$testDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$sourceDirectory = Split-Path -Parent $testDirectory
$buildDirectory = Join-Path $testDirectory "build"

if (Test-Path -LiteralPath $buildDirectory) {
    Remove-Item -LiteralPath $buildDirectory -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDirectory | Out-Null

$compiler = Get-Command g++ -ErrorAction Stop
$flags = @("-std=c++20", "-fmodules-ts", "-Wall", "-Wextra", "-pedantic")

$interfaces = @(
    "types.cc",
    "tile.cc",
    "residence.cc",
    "vertex.cc",
    "edge.cc",
    "dice.cc",
    "board.cc",
    "builder.cc",
    "boardsetupstrategy.cc",
    "gamestateio.cc",
    "textdisplay.cc",
    "gamecontroller.cc"
)

$implementations = @(
    "tile-impl.cc",
    "residence-impl.cc",
    "vertex-impl.cc",
    "edge-impl.cc",
    "dice-impl.cc",
    "board-impl.cc",
    "builder-impl.cc",
    "boardsetupstrategy-impl.cc",
    "gamestateio-impl.cc",
    "textdisplay-impl.cc",
    "gamecontroller-impl.cc"
)

Push-Location $buildDirectory
try {
    & $compiler @flags -x c++-system-header vector
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to build the <vector> header unit."
    }

    foreach ($file in $interfaces + $implementations) {
        & $compiler @flags -c (Join-Path $sourceDirectory $file)
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to compile $file."
        }
    }

    $objects = Get-ChildItem -LiteralPath $buildDirectory -Filter "*.o" |
        Select-Object -ExpandProperty Name

    $tests = @("board-smoke.cc", "all-tests.cc")
    foreach ($test in $tests) {
        $executable = [System.IO.Path]::GetFileNameWithoutExtension($test) + ".exe"
        & $compiler @flags (Join-Path $testDirectory $test) @objects -o $executable
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to link $test."
        }

        Write-Host "==> $executable"
        & (Join-Path $buildDirectory $executable)
        if ($LASTEXITCODE -ne 0) {
            throw "$executable failed."
        }
    }

    & $compiler @flags (Join-Path $sourceDirectory "main.cc") @objects -o "constructor-cli.exe"
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to link the command-line executable."
    }

    $layout = @()
    for ($i = 0; $i -lt 19; ++$i) {
        $layout += "4 12"
    }
    Set-Content -LiteralPath "layout.txt" -Value ($layout -join " ")

    $placementInput = "0`n2`n5`n6`n11`n18`n23`n52`n"

    $defaultOutput = $placementInput | & ".\constructor-cli.exe"
    if (
        ($defaultOutput -join "`n") -notmatch "Builder Blue's turn\." -or
        ($defaultOutput -join "`n") -cmatch "BRICK"
    ) {
        throw "Default layout.txt command-line test failed."
    }

    $randomOutput = $placementInput |
        & ".\constructor-cli.exe" -seed 1 -random-board
    if (
        ($randomOutput -join "`n") -notmatch "Builder Blue's turn\." -or
        ($randomOutput -join "`n") -cnotmatch "BRICK"
    ) {
        throw "-random-board command-line test failed."
    }

    $savedErrorPreference = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $seedOutput = & ".\constructor-cli.exe" -seed invalid 2>&1
    $seedExitCode = $LASTEXITCODE
    $ErrorActionPreference = $savedErrorPreference
    if (
        $seedExitCode -eq 0 -or
        ($seedOutput -join "`n") -notmatch "Invalid seed\."
    ) {
        throw "Invalid -seed command-line test failed."
    }

    Write-Host "==> command-line"
    Write-Host "command-line tests passed"
} finally {
    Pop-Location
}

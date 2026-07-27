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
} finally {
    Pop-Location
}

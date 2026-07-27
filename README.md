# Constructor

A four-player C++20 implementation of Constructor, the CS246 variant of
Settlers of Catan.

## Build

GCC with C++20 Modules support is required.

```bash
make
```

This creates the `constructor` executable.

## Run

By default, the game reads `layout.txt`:

```bash
./constructor
```

Supported options may be supplied in any order:

```text
-seed <number>
-load <save-file>
-board <layout-file>
-random-board
```

`-load` takes precedence over board options. A custom `-board` takes
precedence over `-random-board`.

## Tests

Linux/macOS:

```bash
make test
```

Windows PowerShell:

```powershell
.\src\tests\run-tests.ps1
```

The test suite covers board topology and display, building rules, resources,
dice, initial placement, trading, Geese, saving/loading, controller phases,
replay/reset behavior, and command-line options.

## Commands

Before rolling:

```text
load
fair
roll
```

After rolling:

```text
board
status
residences
build-road <edge#>
build-res <housing#>
improve <housing#>
trade <colour> <give> <take>
next
save <file>
help
```

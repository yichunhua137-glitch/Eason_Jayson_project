# Constructor tests

The test directory contains:

- `board-smoke.cc`: the original board connectivity smoke test.
- `all-tests.cc`: broad unit and integration coverage for every current
  public module.
- `run-tests.ps1`: Windows PowerShell test runner.
- `run-tests.sh`: Linux/macOS Bash test runner.

## Run on Windows

From the repository root:

```powershell
.\src\tests\run-tests.ps1
```

## Run on Linux

From the repository root:

```bash
./src/tests/run-tests.sh
```

Both runners require a GCC version that supports C++20 Modules and
`-fmodules-ts`.

## Coverage

`all-tests.cc` checks:

- `Tile`, `Vertex`, `Edge`, and `Residence` state transitions.
- Builder resources, affordability, points, and dice switching.
- Loaded dice validation/EOF and repeated fair-dice range checks.
- All default board tile, edge, vertex, and reverse adjacency invariants.
- Initial and normal road/residence rules, ownership, blocking, and upgrades.
- Production amounts and Geese production suppression.
- File and random board setup, including invalid input and distributions.
- Formal, debug, and stream display behavior.
- Save/load resources, roads, residences, points, turn, Geese, and topology.
- Game-controller phases, rolling, status/help output, invalid commands,
  next-turn behavior, and failed trades.

The suite reports every failed assertion before exiting with a non-zero status.
An assertion should only be changed when the expected behavior in the project
specification changes; implementation bugs should be fixed in the relevant
production module.

#!/usr/bin/env bash
set -euo pipefail

test_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
src_dir="$(cd "$test_dir/.." && pwd)"
build_dir="$test_dir/build"

rm -rf "$build_dir"
mkdir -p "$build_dir"
cd "$build_dir"

cxx="${CXX:-g++}"
flags=(-std=c++20 -fmodules-ts -Wall -Wextra -pedantic)

"$cxx" "${flags[@]}" -x c++-system-header vector

interfaces=(
    types.cc tile.cc residence.cc vertex.cc edge.cc dice.cc
    board.cc builder.cc boardsetupstrategy.cc gamestateio.cc
    textdisplay.cc gamecontroller.cc
)

implementations=(
    tile-impl.cc residence-impl.cc vertex-impl.cc edge-impl.cc
    dice-impl.cc board-impl.cc builder-impl.cc
    boardsetupstrategy-impl.cc gamestateio-impl.cc
    textdisplay-impl.cc gamecontroller-impl.cc
)

for file in "${interfaces[@]}" "${implementations[@]}"; do
    "$cxx" "${flags[@]}" -c "$src_dir/$file"
done

for test in board-smoke.cc all-tests.cc; do
    executable="${test%.cc}"
    "$cxx" "${flags[@]}" "$test_dir/$test" ./*.o -o "$executable"
    echo "==> $executable"
    "./$executable"
done

"$cxx" "${flags[@]}" -c "$src_dir/main.cc"
"$cxx" "${flags[@]}" ./*.o -o constructor-cli

for _ in {1..19}; do
    printf '4 12 ' >> layout.txt
done
printf '\n' >> layout.txt

default_output="$(./constructor-cli </dev/null)"
if [[ "$default_output" != *"Builder Blue's turn."* ]] ||
   [[ "$default_output" == *"BRICK"* ]]; then
    echo "default layout.txt test failed" >&2
    exit 1
fi

random_output="$(./constructor-cli -seed 1 -random-board </dev/null)"
if [[ "$random_output" != *"Builder Blue's turn."* ]] ||
   [[ "$random_output" != *"BRICK"* ]]; then
    echo "-random-board test failed" >&2
    exit 1
fi

if ./constructor-cli -seed invalid > seed-output.txt 2>&1; then
    echo "invalid -seed test failed" >&2
    exit 1
fi
if ! grep -q "Invalid seed." seed-output.txt; then
    echo "invalid -seed message test failed" >&2
    exit 1
fi

rm -f backup.sv layout.txt seed-output.txt
echo "==> command-line"
echo "command-line tests passed"

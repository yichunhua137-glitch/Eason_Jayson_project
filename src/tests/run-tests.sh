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

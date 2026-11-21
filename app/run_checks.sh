#!/bin/sh
set -eu
cd "$(dirname "$0")"
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/d3-checks.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM
c++ -std=c++17 -O2 -Wall -Wextra -pedantic gcd_segment_tree.cpp -o "$build_dir/replay"
"$build_dir/replay" < sample.in > "$build_dir/actual.out"
diff -u sample.out "$build_dir/actual.out"
"$build_dir/replay" --test
"$build_dir/replay" --scale

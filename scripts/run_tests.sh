#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INPUT_DIR="$ROOT_DIR/tests/input"
EXPECTED_DIR="$ROOT_DIR/tests/expected"
ACTUAL_DIR="$ROOT_DIR/tests/actual"
REFERENCE_EXE="${RPAL_REFERENCE:-}"

mkdir -p "$ACTUAL_DIR" "$EXPECTED_DIR"

make -C "$ROOT_DIR" >/dev/null

for file in "$INPUT_DIR"/*.rpal; do
  base="$(basename "$file" .rpal)"
  expected="$EXPECTED_DIR/$base.out"
  actual="$ACTUAL_DIR/$base.out"

  if [[ -n "$REFERENCE_EXE" ]]; then
    "$REFERENCE_EXE" "$file" > "$expected"
  fi

  if [[ ! -f "$expected" ]]; then
    echo "Missing expected output: $expected" >&2
    echo "Set RPAL_REFERENCE=/path/to/rpal.exe to generate it from the reference interpreter." >&2
    exit 1
  fi

  "$ROOT_DIR/rpal20" "$file" > "$actual"
  diff -u "$expected" "$actual"
done

echo "All RPAL tests passed."

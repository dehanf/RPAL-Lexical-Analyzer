#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ZIP_NAME="${1:-rpal20_submission.zip}"

cd "$ROOT_DIR"

make clean >/dev/null
make >/dev/null
make test >/dev/null

rm -f "$ZIP_NAME"
zip -q "$ZIP_NAME" \
  Makefile \
  scripts/run_tests.sh \
  docs/REPORT.md \
  docs/REPORT.pdf \
  src/*.cpp \
  src/*.h \
  src/README.md \
  tests/input/*.rpal \
  tests/expected/*.out

echo "Created $ZIP_NAME"

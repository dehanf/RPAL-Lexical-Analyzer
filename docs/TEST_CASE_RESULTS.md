# Extended RPAL Test Cases

This file records the test script added in `scripts/extended_test_cases.sh` and the
output from running it.

## Script

```bash
#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
WORK_DIR="$(mktemp -d /tmp/rpal_extended_tests.XXXXXX)"
trap 'rm -rf "$WORK_DIR"' EXIT

make -C "$ROOT_DIR" >/dev/null

write_case() {
  local name="$1"
  local program="$2"
  local expected="$3"

  printf "%s\n" "$program" > "$WORK_DIR/$name.rpal"
  printf "%s\n" "$expected" > "$WORK_DIR/$name.expected"
}

write_case "01_arithmetic_precedence" \
  "Print (1 + 2 * 3 ** 2)" \
  "19"

write_case "02_boolean_conditional" \
  "Print ((5 gr 3) & not false -> 'yes' | 'no')" \
  "yes"

write_case "03_where_scope" \
  "Print (x + 7 where x = 10)" \
  "17"

write_case "04_within_scope" \
  "let x = 3 within y = x * 4 in Print y" \
  "12"

write_case "05_recursive_sum" \
  "let rec sum n = n eq 0 -> 0 | n + sum (n - 1) in Print (sum 5)" \
  "15"

write_case "06_tuple_parameter" \
  "let addPair (x, y) = x + y in Print (addPair (8, 13))" \
  "21"

write_case "07_tuple_aug_index" \
  "Print (((1, 2) aug 3) 3)" \
  "3"

write_case "08_string_builtins" \
  "Print (Conc (Stem 'zebra') (Stern 'ebra'))" \
  "zbra"

write_case "09_type_predicate" \
  "Print (Isinteger (Order (1, 2, 3)))" \
  "true"

write_case "10_comments_and_strings" \
  "let s = 'hello' // this comment should be ignored
in Print s" \
  "hello"

for program in "$WORK_DIR"/*.rpal; do
  base="$(basename "$program" .rpal)"
  actual="$WORK_DIR/$base.actual"
  expected="$WORK_DIR/$base.expected"

  "$ROOT_DIR/rpal20" "$program" > "$actual"

  if diff -u "$expected" "$actual" >/dev/null; then
    echo "PASS $base"
  else
    echo "FAIL $base"
    diff -u "$expected" "$actual" || true
    exit 1
  fi
done

echo "All extended tests passed."
```

## Output

```text
PASS 01_arithmetic_precedence
PASS 02_boolean_conditional
PASS 03_where_scope
PASS 04_within_scope
PASS 05_recursive_sum
PASS 06_tuple_parameter
PASS 07_tuple_aug_index
PASS 08_string_builtins
PASS 09_type_predicate
PASS 10_comments_and_strings
All extended tests passed.
```

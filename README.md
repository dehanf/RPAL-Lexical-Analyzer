# RPAL Interpreter

This repository contains a C++ implementation of an RPAL interpreter. It includes
a hand-written lexer, recursive descent parser, AST standardizer, and CSE-style
evaluator with lexical scoping, closures, recursion, tuples, and common RPAL
built-in functions.

The final executable is named `rpal20` and runs with:

```bash
./rpal20 file_name
```

## Project Structure

```text
.
├── Makefile
├── src/                  # C++ source and header files
├── tests/
│   ├── input/            # RPAL test programs
│   ├── expected/         # Expected output files
│   └── actual/           # Generated test outputs
├── scripts/              # Test and packaging scripts
└── docs/                 # Project PDFs, report, and notes
```

## Build

From the project root:

```bash
make
```

This compiles the source files and creates:

```bash
./rpal20
```

## Run

Run any RPAL source file:

```bash
./rpal20 tests/input/01_let_print.rpal
```

Example:

```bash
./rpal20 tests/input/08_project_sample.rpal
```

## Development Flags

```bash
./rpal20 -tokens file_name
./rpal20 -ast file_name
./rpal20 -st file_name
```

- `-tokens` prints the token stream.
- `-ast` prints the raw AST.
- `-st` prints the standardized tree.

Normal execution does not print tokens, ASTs, or debug information.

## Tests

Run the main diff-based test suite:

```bash
make test
```

Run the extended test script:

```bash
./scripts/extended_test_cases.sh
```

The test workflow writes generated outputs to `tests/actual/` and compares them
against files in `tests/expected/`.

## Clean

Remove generated build files:

```bash
make clean
```

## Package

Create a submission zip:

```bash
make package
```

## Implemented Features

- Lexical analysis according to the RPAL lexicon
- Recursive descent parsing according to the RPAL grammar
- AST printing and standardized tree printing
- Standardization for `let`, `where`, `within`, `rec`, `and`, function forms,
  multi-parameter lambdas, and `@`
- Runtime evaluation for integers, strings, booleans, `nil`, `dummy`, tuples,
  closures, recursive functions, and conditionals
- Built-ins including `Print`, `Conc`, `Stem`, `Stern`, `Order`, `Null`,
  `ItoS`, `Isinteger`, `Istruthvalue`, `Isstring`, `Istuple`, `Isfunction`,
  and `Isdummy`

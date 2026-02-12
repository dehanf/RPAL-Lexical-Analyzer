# RPAL Compiler - Phase 1: Lexical Analysis

This repository contains the first phase of building an **RPAL (Right-reference Pedagogic Algorithmic Language)** compiler. This phase implements the **Lexical Analyzer** (Scanner) which converts RPAL source code into a stream of tokens.

## Overview

The lexical analyzer is the first step in the compilation process. It reads RPAL source code character by character and groups them into meaningful tokens that will be used by the parser in subsequent phases.

## Features

### Token Types
The lexical analyzer recognizes the following token types:

- **IDENTIFIER**: Variable and function names (e.g., `x`, `factorial`, `my_var`)
- **INTEGER**: Numeric literals (e.g., `42`, `123`, `0`)
- **STRING**: String literals (future implementation)
- **OPERATOR**: RPAL operators (e.g., `+`, `-`, `*`, `<`, `>`, `&`, `.`, `|`, `:`, `!`, `#`, `%`, `^`, `@`, `$`)
- **KEYWORD**: Reserved RPAL keywords
- **PUNCTUATION**: Delimiters (e.g., `(`, `)`, `;`, `,`)
- **ERROR**: Invalid tokens

### RPAL Keywords
The analyzer recognizes the following RPAL keywords:
```
let, where, true, false, not, fn, Is, gr, ge, aug, le, nil, 
dummy, or, in, eq, ne, and, rec, within
```

## Implementation Details

### Finite State Machine
The lexical analyzer uses a **Finite State Machine (FSM)** approach with the following states:
- `STATE_START`: Initial state
- `STATE_IDENTIFIER`: Processing identifiers/keywords
- `STATE_INTEGER`: Processing numeric literals
- `STATE_OPERATOR`: Processing operators
- `STATE_PUNCTUATION`: Processing punctuation marks
- `STATE_COMMENT`: Processing comments (future)
- `STATE_ERROR`: Error state for invalid characters

### Algorithm
1. Read the input file character by character
2. Transition between states based on current character and state
3. Accumulate characters to form complete tokens
4. Classify tokens (e.g., distinguish keywords from identifiers)
5. Output token stream in the format: `<TOKEN_TYPE, "value">`

## Compilation and Usage

### Prerequisites
- C++ compiler (g++, clang++, or MSVC)
- C++11 or later

### Compilation
```bash
g++ -o lexical_analyzer lexical_analyzer.cpp -std=c++11
```

### Running the Lexical Analyzer
```bash
./lexical_analyzer <input_file>
```

### Example
Given an RPAL source file `test.rpal`:
```rpal
let x = 5 in x + 10
```

Run:
```bash
./lexical_analyzer test.rpal
```

Output:
```
<KEYWORD, "let">
<IDENTIFIER, "x">
<OPERATOR, "=">
<INTEGER, "5">
<KEYWORD, "in">
<IDENTIFIER, "x">
<OPERATOR, "+">
<INTEGER, "10">
```

## Project Structure
```
RPAL_compiler/
├── lexical_analyzer.cpp    # Main lexical analyzer implementation
├── README.md                # This file
└── a.out                    # Compiled executable (if present)
```

## Future Phases

This project is under active development. Upcoming phases include:

- **Phase 2**: Syntax Analysis (Parser)
  - Build Abstract Syntax Tree (AST)
  - Implement grammar rules for RPAL

- **Phase 3**: Semantic Analysis
  - Type checking
  - Scope resolution
  - Symbol table construction

- **Phase 4**: Code Generation
  - Generate intermediate representation
  - Optimization
  - Target code generation

## RPAL Language

RPAL (Right-reference Pedagogic Algorithmic Language) is a functional programming language designed for educational purposes. It features:
- Functional programming constructs
- Lambda expressions (`fn` keyword)
- Let expressions with local bindings
- Recursive definitions (`rec` keyword)
- Pattern matching and conditional expressions

## Known Limitations

- String literals are not yet fully implemented
- Comment handling is planned but not yet active
- Error recovery could be improved
- Line and column number tracking not implemented

## Contributing

This is an academic project for the Programming Languages course (4th semester). Further enhancements and additional compiler phases will be added incrementally.

## Author

Computer Science Student - ACA, 4th Semester

## License

Academic Project - For Educational Purposes

---

**Note**: This is Phase 1 of a multi-phase compiler construction project. Stay tuned for updates as more phases are implemented!


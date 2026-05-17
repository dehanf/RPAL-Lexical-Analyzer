# RPAL Interpreter

This project is being built into a C++ RPAL interpreter. The lexer is reusable,
the recursive descent parser builds the raw RPAL AST, the standardizer converts
it to CSE-ready form, and the evaluator runs standardized trees with lexical
closures, recursive bindings, tuples, and built-ins.

## Build

From the project root:

```bash
make
```

This creates the required executable:

```bash
./rpal20 file_name
```

Normal execution prints only RPAL program output, such as side effects from
`Print`.

## Development Flags

```bash
./rpal20 -tokens file_name
./rpal20 -ast file_name
```

- `-tokens` prints the lexer token stream.
- `-ast` prints the raw parser AST in dotted format.
- `-st` prints the standardized tree in dotted format.

## Implemented Runtime Features

- Literals: integers, strings, truth values, `nil`, and `dummy`
- Arithmetic: `+`, `-`, `neg`, `*`, `/`, and `**`
- Boolean logic and comparisons
- `let`, `where`, `within`, `and`, `rec`, `lambda`, `gamma`, and conditionals
- Tuples, tuple augmentation, tuple parameter binding, and tuple indexing
- Built-ins: `Print`, `Conc`, `Stem`, `Stern`, `Order`, `Null`, `ItoS`,
  `Isinteger`, `Istruthvalue`, `Isstring`, `Istuple`, `Isfunction`,
  and `Isdummy`

## Clean

```bash
make clean
```

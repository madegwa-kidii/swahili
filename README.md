# Swahili Compiler (Educational)

A small, modular compiler written in C that demonstrates the **classical compiler pipeline** using a tiny Swahili-flavored language.

This project is meant for learning: each compiler phase is separated into its own module so you can study and run each stage end-to-end.

## Why this project exists

- Teach compiler fundamentals with a compact codebase.
- Keep each phase independent and readable.
- Use Swahili-inspired keywords (`namba`, `maneno`, `kama`, `sio`, `wakati`, `andika`) so syntax examples feel approachable and distinctive.

## Implemented pipeline

The compiler currently runs the following phases:

1. **Lexical analysis** (`lexer/`)  
   Converts source text into a stream of tokens.
2. **Parsing / syntax analysis** (`parser/`)  
   Builds an Abstract Syntax Tree (AST) with expression precedence.
3. **Semantic analysis** (`semantic/`)  
   Tracks declarations/types and reports semantic errors.
4. **Intermediate Representation (IR) generation** (`ir/`)  
   Produces simple three-address-code (TAC)-style instructions.
5. **Optimization** (`optimizer/`)  
   Applies basic optimization passes.
6. **Code generation** (`codegen/`)  
   Emits readable pseudo-assembly.

Driver entrypoint: `compiler/main.c`.

## Repository structure

```text
compiler/   # pipeline driver (main)
lexer/      # tokenizer + token stream helpers
parser/     # AST definitions + recursive-descent parser
semantic/   # symbol table + semantic checks
ir/         # TAC-style intermediate code generation
optimizer/  # basic IR optimization
codegen/    # pseudo-assembly emitter
examples/   # sample .swa programs
```

> Note: There is also an older `src/` tree in this repository. The command below builds the modular pipeline used by `compiler/main.c`.

## Language overview (current grammar/features)

### Data types

- `namba` → integer
- `maneno` → string

### Statements

- Variable declaration (optional initialization)
  - `namba x;`
  - `namba x = 10;`
  - `namba x ni 10;` (also accepted)
- Assignment
  - `x = x + 1;`
- Print
  - `andika(x);`
- Conditional
  - `kama (x > 0) { ... } sio { ... }`
- Loop
  - `wakati (x > 0) { ... }`

### Expressions

- Literals: numbers and strings
- Identifiers
- Arithmetic: `+ - * /`
- Comparisons: `< > <= >= == !=`
- Parentheses for grouping

## Example program

See `examples/sample_compiler_demo.swa`:

```swa
namba x ni 5;
namba y ni 3;
namba z = x + y * 2;

kama (z > 10) {
  andika(z);
} sio {
  andika(0);
}

wakati (x > 0) {
  andika(x);
  x = x - 1;
}
```

## Build

From repository root:

```bash
gcc -std=c99 -Wall -Wextra -pedantic \
  compiler/main.c \
  lexer/lexer.c \
  parser/parser.c \
  semantic/semantic.c semantic/symbol_table.c \
  ir/ir.c \
  optimizer/optimizer.c \
  codegen/codegen.c \
  -o swahili_compiler
```

## Run

```bash
./swahili_compiler examples/sample_compiler_demo.swa
```

The compiler prints each stage output (tokens, AST, semantic status, IR, optimized IR, pseudo-assembly).

## Current status and limitations

- Designed as an educational prototype, not a production compiler.
- Emits pseudo-assembly, not machine code.
- The implemented syntax is intentionally small.
- Error handling is present but still minimal in some paths.

## Contributing ideas

Potential next improvements:

- richer type system and operations
- functions/procedures
- better diagnostics and recovery
- more optimization passes
- backend that emits real target code

# mini-scheme

A compact implementation of a scheme-like programming language interpreter.

## Overview

`mini-scheme` is a minimalist (not finished) implementation of a scheme-like
programming language, created with an emphasis on learning and providing a
foundation for experimenting with language implementation concepts. While the
project currently offers a C-based interpreter, it is also intended to expand
to include other languages. The overarching goal is to explore and deepen the
understanding of interpreter design and language implementation.

## Project Structure

```
mini-scheme/
├── c/                  # C implementation
│   ├── src/            # Source code
│   │   ├── tests/      # Test files
│   ├── external/       # External dependencies (munit)
│   └── docs/           # Documentation and code generation resources
```

## Features

- Scheme language parser and interpreter
- REPL (Read-Eval-Print Loop) for interactive use
- Core Scheme types: numbers, strings, symbols, lists, procedures
- Lexical analysis using generated NFA-based lexer (C implementation)

## C Implementation

The C implementation of `mini-scheme` is located in the `c/` directory. It includes
a lexer, parser, and interpreter for a subset of the Scheme language. The
interpreter supports basic operations, including arithmetic, conditionals,
function definitions, and list manipulation.

The lexer is generated using a custom NFA engine written in C, which allows for
efficient lexical analysis of expressions.

The parser is based on a recursive descent approach, which is a common technique
for parsing expressions in programming languages. The parser constructs an
abstract syntax tree (AST) from the input expressions, which is then evaluated
by the interpreter.

The interpreter supports a variety of built-in functions and allows for the
definition of user-defined functions. It also includes a REPL for interactive
evaluation of expressions.

#### Prerequisites

- GCC or compatible C compiler
- GNU Make
- readline library (`libreadline-dev` on Debian-based systems)
- Python 3 (for lexer generation using a custom nfa engine written in C)

Or just use `nix` and use the available `flake.nix` on `c/` with `nix develop`.

### Build and Run

```bash
cd c
make          # Build the interpreter
make repl     # Build and run the REPL
make test     # Build and run tests
make clean    # Clean build artifacts
```

For Nix users, there's also a Nix build configuration:

```bash
cd c
make nix      # Build and run using Nix
```

## Usage Example

After building the project, you can launch the REPL and try some Scheme expressions:

```scheme
> (define (factorial n) (if (< n 2) 1 (* n (factorial (- n 1)))))
> (factorial 5)
120
```

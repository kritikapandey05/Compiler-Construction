# Compiler Front-End Implementation in C

## Overview

This project implements the **lexical analysis** and **syntax analysis** phases of a compiler front-end in C using GCC on Linux/Ubuntu.

The compiler reads a source file written in the specified language and performs:

- Lexical Analysis (Tokenization)
- Syntax Analysis using a Predictive Parser (LL(1))

The lexical analyzer scans the source code once and produces a token stream.  
The syntax analyzer verifies syntactic correctness using a parsing table and generates a parse tree.

No external lexer/parser generators or high-level languages are used.

---

## Features

### Lexical Analyzer

- Single-pass scanning of source code
- Tokenizes lexemes according to language specification
- Recognizes:
  - Keywords
  - Identifiers
  - Numeric literals
  - Operators
  - Delimiters
- Ignores whitespace and comments
- Maintains token attributes (token type, lexeme, line number)
- Reports lexical errors with accurate line numbers
- Efficient in both time and space complexity

### Syntax Analyzer

- LL(1) predictive parsing technique
- Parsing table-driven implementation
- Stack-based parsing mechanism
- Single pass over the token stream
- Constructs parse tree for syntactically correct programs
- Reports syntax errors with meaningful messages and line numbers
- Deterministic parsing without backtracking

---

## Design Overview

### Lexical Analysis Design

- Character-by-character input processing
- Deterministic state-based recognition of tokens
- Line tracking for error reporting
- Token stream generation for parser input

### Syntax Analysis Design

- Grammar assumed to be LL(1)
- Parsing table constructed from grammar rules
- Predictive parsing algorithm implemented manually
- Parse tree dynamically built during parsing
- Basic error detection and reporting implemented


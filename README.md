# SIGNAL Compiler (Lexical and Syntax Analyzer)

This project implements a lexical and syntax analyzer for a subset of the SIGNAL programming language. It was developed as a coursework project for the subject "Fundamentals of Compiler Design" at Kyiv Polytechnic Institute (KPI).

## Overview

The program consists of two main components:
- **Lexical Analyzer** – transforms SIGNAL source code into a stream of lexemes (tokens).
- **Syntax Analyzer** – performs recursive descent parsing based on a simplified SIGNAL grammar.

The system:
- Reads a list of lexemes and symbol tables from the lexical analyzer.
- Parses the token stream and builds a parse tree.
- Reports lexical and syntax errors with line and column references.
- Outputs a listing of the input program along with error messages.
- Supports multiple test cases (both valid and invalid programs).

## Features

- Recursive descent parser in modern C++ with `std::unique_ptr` tree structure.
- Parse tree pretty-printing to console.
- Grammar rules include: variable declarations, IF-THEN-ELSE-ENDIF, expressions with `=`.
- Graceful error recovery and reporting.


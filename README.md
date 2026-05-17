# CVM++ — A Custom Scripting Language

CVM++ is a lightweight custom scripting language with its own **Lexer**, **Parser**, **Bytecode Compiler**, and **Stack-Based Virtual Machine**, all written in modern C++17.

## Features

- **Integer** and **Boolean** data types
- **Variables** with `let` declarations and assignments
- **Arithmetic** operators: `+`, `-`, `*`, `/`
- **Comparison** operators: `==`, `<`
- **Control flow**: `if`/`else`, `while` loops
- **I/O**: `print` and `input` statements
- **REPL** (interactive mode) and file execution
- **Debug mode** with token/AST/bytecode dump

## Architecture

```
Source Code (.cvm)
    │
    ▼
┌──────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐
│  Lexer   │ ──▶ │  Parser  │ ──▶ │ Compiler │ ──▶ │    VM    │
│ (tokens) │     │  (AST)   │     │(bytecode)│     │ (execute)│
└──────────┘     └──────────┘     └──────────┘     └──────────┘
```

## Project Structure

```
CVM++/
├── CMakeLists.txt          Build configuration
├── README.md               This file
├── src/
│   ├── main.cpp            Entry point (REPL + File Runner)
│   ├── lexer.h / lexer.cpp Tokenizer
│   ├── token.h             Token type definitions
│   ├── parser.h / parser.cpp  Recursive descent parser
│   ├── ast.h               AST node definitions
│   ├── compiler.h / compiler.cpp  Bytecode compiler
│   ├── bytecode.h          Opcode definitions
│   ├── vm.h / vm.cpp       Stack-based virtual machine
│   └── value.h             Runtime value type
├── examples/
│   ├── hello.cvm           Simple print
│   ├── variables.cvm       Variable declarations
│   ├── arithmetic.cvm      Arithmetic operations
│   ├── control_flow.cvm    If/else and while loops
│   └── input_output.cvm    User input
```

## Building

### Option 1: Direct Compile

```bash
g++ -std=c++17 -Wall -o cvm src/main.cpp src/lexer.cpp src/parser.cpp src/compiler.cpp src/vm.cpp
```

### Option 2: CMake

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

### Run a Script

```bash
./cvm examples/hello.cvm
```

### Start the REPL

```bash
./cvm
```

```
CVM++ REPL v1.0  (type 'exit' to quit)
>>> print 42;
42
>>> let x = 10;
>>> print x + 5;
15
>>> exit
```

### Debug Mode

```bash
./cvm --debug examples/hello.cvm
```

Output:
```
=== TOKENS ===
PRINT NUMBER(42) SEMICOLON EOF

=== AST ===
ProgramNode
  PrintNode
    NumberLiteral(42)

=== BYTECODE ===
0000: PUSH_INT 42
0009: PRINT
0010: HALT

=== EXECUTION OUTPUT ===
42
```

## Language Guide

### Variables

```
let x = 10;
let y = x + 5;
x = 42;           // reassignment
```

### Arithmetic

```
print 2 + 3;      // 5
print 10 - 4;     // 6
print 6 * 7;      // 42
print 100 / 10;   // 10
```

### Control Flow

```
if (x < 10) {
    print x;
} else {
    print 0;
}

while (x < 100) {
    x = x + 1;
}
```

### Input / Output

```
input x;           // reads integer from stdin
print x;           // prints value to stdout
```
## Opcodes (Instruction Set)

| Opcode | Description |
|---|---|
| `PUSH_INT` | Push 64-bit integer onto stack |
| `PUSH_BOOL` | Push boolean onto stack |
| `LOAD_VAR` | Load variable value onto stack |
| `STORE_VAR` | Store stack top into variable |
| `ADD/SUB/MUL/DIV` | Arithmetic operations |
| `EQUAL` / `LESS_THAN` | Comparison operations |
| `JUMP` | Unconditional jump (loops) |
| `JUMP_IF_FALSE` | Conditional jump (if/while) |
| `PRINT` | Pop and print stack top |
| `INPUT` | Read integer from stdin |
| `HALT` | Stop execution |
## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+ (for CMake builds)

## License

MIT

# Cinder Architecture

Cinder is an educational C-subset compiler written in C. The same compiler core builds as a native command-line program and as a WebAssembly module for the browser.

## Compilation pipeline

```text
C source
  |
  v
Lexer
  |
  v
Tokens
  |
  v
Recursive-descent parser
  |
  v
Abstract syntax tree
  |
  v
Bytecode generator
  |
  v
Bounded virtual machine
  |
  v
Program output or diagnostic
```

The website exposes tokens, syntax-tree nodes, generated instructions, output, return value, and execution statistics.

## Compiler core

The compiler implementation lives in `compiler/cinder.c`. It does not depend on a parser generator, compiler framework, or external runtime library.

The core uses fixed-size storage for source text, tokens, syntax-tree nodes, functions, symbols, bytecode, stack frames, program output, and the JSON report. These limits keep execution predictable in both native and browser builds.

## Lexer

The lexer reads the source one character at a time and produces tokens.

It recognizes:

- Identifiers and keywords
- Decimal integer literals
- Character literals
- String literals used as `printf` formats
- Arithmetic operators
- Comparison operators
- Logical operators
- Bitwise operators
- Assignment operators
- Increment and decrement operators
- Braces, parentheses, commas, and semicolons
- Single-line and block comments

Every token stores its source start and end offsets. The website uses those offsets to select source text when a visitor clicks a token.

## Parser

Cinder uses a recursive-descent parser. Expression precedence is implemented explicitly, from logical OR down to unary and primary expressions.

The parser creates syntax-tree nodes for:

- Values and variables
- Unary and binary expressions
- Assignments
- Conditional expressions
- Function calls
- Blocks and declarations
- Returns
- Conditions
- Loops
- Loop control
- Prefix and postfix updates

The syntax tree uses integer node identifiers instead of pointers. This works consistently in native memory and WebAssembly linear memory.

## Symbols and scopes

Each function owns numbered local-variable slots. Entering a nested block increases the scope depth. Leaving the block removes symbols declared inside it.

Nested blocks may shadow variables from outer scopes, but a variable cannot be declared twice in the same scope.

Function parameters occupy the first local slots of a function frame.

## Bytecode

The syntax tree is compiled into a compact stack-machine instruction stream.

Instruction groups include:

- Constants, loads, stores, and drops
- Arithmetic operations
- Bitwise operations
- Comparisons
- Boolean conversion
- Unary operations
- Conditional and unconditional jumps
- Function calls and returns
- Formatted output

Branches and loops are compiled using jump placeholders. The compiler patches each placeholder after it knows the destination instruction.

`break` and `continue` instructions are associated with the current loop depth and patched to the correct target.

## Virtual machine

The virtual machine executes Cinder bytecode with:

- A value stack
- Function-local storage
- Return addresses
- Stack-base records
- A bounded call stack
- An instruction counter
- A bounded output buffer

Function arguments are evaluated from left to right, placed on the value stack, and copied into parameter slots when the function is called.

The VM reports invalid operations such as division by zero, invalid shifts, arithmetic overflow, stack exhaustion, excessive recursion, excessive output, and excessive execution.

## Browser execution

`compiler/cinder.c` is compiled to `web/cinder.wasm`.

The browser interface communicates with the module through four exports:

```text
input_ptr()
compile()
report_ptr()
report_len()
```

JavaScript writes UTF-8 source text into WebAssembly memory and calls `compile()`. The compiler returns a JSON report containing the compilation and execution results.

## Web Worker

`web/compiler-worker.js` owns the WebAssembly instance.

Compilation runs outside the browser’s main interface thread. If a request exceeds the configured timeout, `web/app.js` terminates the worker and creates a fresh one.

This prevents an expensive program from freezing the page.

## Interface rendering limits

The compiler can produce thousands of tokens, nodes, and instructions. Rendering all of them at once would make the interface unnecessarily slow.

The interface therefore displays at most:

- 1,200 tokens
- 1,500 syntax-tree nodes
- 1,500 instructions

These are display limits only. The compiler’s internal limits are documented in `LANGUAGE.md`.

## Native build

The native build uses GCC in C11 mode with warnings treated as errors.

```powershell
.\scripts\build.ps1
```

The resulting executable is:

```text
build/cinder.exe
```

It accepts a source-file path and prints the JSON compiler report.

## WebAssembly build

The browser build uses Zig’s C compiler with a freestanding WebAssembly target.

It has no operating-system, filesystem, or network access. The output is:

```text
web/cinder.wasm
```

## Security boundaries

Cinder does not execute native machine code supplied by visitors. Programs execute as instructions inside the bounded virtual machine.

The compiler enforces limits for:

- Source size
- Token count
- Syntax-tree size
- Generated instructions
- Local variables
- VM stack size
- Function-call depth
- Execution steps
- Program output

These limits make Cinder suitable for static browser hosting. They do not make it a sandbox for arbitrary native executables.

## Repository structure

```text
compiler/             Compiler and virtual machine
tests/                C programs used by automated tests
scripts/              Build, test, and local-server scripts
web/                  Static browser application
docs/                 Architecture and language documentation
.github/workflows/    Continuous integration and deployment
```
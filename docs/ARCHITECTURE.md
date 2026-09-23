# Cinder Architecture

Cinder is an educational C-subset compiler and bounded virtual machine written from scratch in C.

The same compiler core builds as:

- A native Windows command-line executable
- A freestanding WebAssembly module
- The execution engine behind the browser workbench

No parser generator, compiler framework, JavaScript C interpreter, or external execution service is used.

## System overview

```text
C source
  |
  v
Source buffer
  |
  v
Lexer
  |
  v
Token stream
  |
  v
Recursive-descent parser
  |
  v
Type and symbol analysis
  |
  v
Abstract syntax tree
  |
  v
Bytecode generator
  |
  v
Instruction stream
  |
  v
Bounded virtual machine
  |
  +--> Program output
  |
  +--> Return value
  |
  +--> Execution statistics
  |
  +--> Source-linked diagnostics
```

The compiler returns a structured JSON report containing the observable results of this pipeline.

## Compiler source

The compiler and virtual machine are implemented in:

```text
compiler/cinder.c
```

The implementation intentionally uses a single C translation unit.

This makes the complete pipeline easy to build, distribute, inspect, and compile to WebAssembly. It also makes the relationship between compiler stages visible to students and interviewers.

A production compiler would normally divide these systems into multiple modules.

## Fixed-capacity design

Cinder uses fixed-capacity tables and buffers for major compiler resources.

These include:

- Source text
- Standard input
- Tokens
- Types
- Structures
- Structure members
- Enumerations
- Enumeration values
- Symbols
- Functions
- Syntax-tree nodes
- Generated instructions
- Jump-patching records
- VM stack values
- Function frames
- Virtual memory
- Program output
- Diagnostics
- JSON output

This design has several benefits:

- Predictable memory use
- No dependency on dynamic allocation
- Straightforward WebAssembly compilation
- Consistent native and browser behavior
- Explicit failure when a resource limit is reached
- Easier reasoning about untrusted visitor programs

The tradeoff is that every resource has a documented maximum.

## Lexer

The lexer reads the source buffer one character at a time and produces tokens.

It recognizes:

- Identifiers
- Keywords
- Decimal integer literals
- Integer suffixes supported by Cinder
- Character literals
- String literals
- Arithmetic operators
- Comparison operators
- Logical operators
- Bitwise operators
- Assignment operators
- Increment and decrement operators
- Pointer and member operators
- Braces
- Brackets
- Parentheses
- Commas
- Colons
- Semicolons
- Single-line comments
- Block comments

Every token stores:

- Token kind
- Source start offset
- Source end offset
- Parsed integer value where applicable

Source offsets allow diagnostics and interface views to refer back to the original program.

## Parser

Cinder uses a handwritten recursive-descent parser.

The parser handles:

- External declarations
- Function definitions
- Global variables
- Local variables
- Type specifiers
- Pointer declarators
- Array declarators
- Structure definitions
- Enumeration definitions
- Initializers
- Statements
- Expressions

Expression parsing is divided into precedence levels.

These levels cover:

- Comma expressions
- Assignment
- Conditional expressions
- Logical operations
- Bitwise operations
- Equality
- Relational comparisons
- Shifts
- Addition and subtraction
- Multiplication, division, and modulo
- Casts
- Unary operations
- Postfix expressions
- Primary expressions

The parser builds integer-indexed syntax-tree nodes.

## Why node identifiers are integers

Cinder stores relationships between syntax-tree nodes as integer identifiers rather than native C pointers.

For example, a binary-expression node stores the identifiers of its left and right operands.

This approach provides:

- Stable references inside fixed arrays
- Consistent behavior in native and WebAssembly builds
- Easier JSON serialization
- Easier bounds checking
- Simpler browser visualization
- Independence from host memory addresses

A zero identifier represents the absence of a node where appropriate.

## Syntax-tree nodes

The syntax tree represents constructs including:

- Integer literals
- String literals
- Variables
- Binary expressions
- Unary expressions
- Assignments
- Function calls
- Function arguments
- Blocks
- Declaration statements
- Initializers
- Returns
- Conditions
- Loops
- `break`
- `continue`
- Prefix operations
- Postfix operations
- Conditional expressions
- Array indexing
- Address-of expressions
- Dereference expressions
- Casts
- `sizeof`
- Structure members
- `switch`
- `case`
- `default`
- Comma expressions

Each node records its relevant child identifiers, type, value, and source token.

## Type system

Cinder maintains a compiler-side type table.

Supported type categories include:

- `void`
- Character types
- Short integer types
- Integer types
- Long integer types
- Pointers
- Arrays
- Structures
- Enumerations
- Functions

A type record may contain:

- Type kind
- Base type
- Array length
- Structure identifier
- Function return type
- Parameter information
- Size
- Alignment
- `const` state
- Unsigned state

The compiler uses these records for:

- Declaration validation
- Memory layout
- Pointer scaling
- Array indexing
- Member access
- Assignment checking
- Cast generation
- Integer conversion
- Function argument checking
- `sizeof`
- Typed VM loads and stores

Cinder implements a practical educational type system rather than every ISO C compatibility and conversion rule.

## Symbols and scopes

Cinder stores declared names in a symbol table.

Symbols represent:

- Local variables
- Global variables
- Functions
- Enumeration constants

Each symbol records information such as:

- Name token
- Type
- Storage category
- Scope depth
- Function ownership
- Memory offset
- Active state

Entering a block increases the active scope depth.

Leaving a block marks symbols declared in that scope as inactive.

Inactive symbols remain in the compiler’s history so syntax-tree references and reports remain stable, but they no longer participate in name lookup.

This allows correct nested scopes and shadowing without invalidating earlier symbol identifiers.

## Global storage

Global objects receive offsets in the virtual machine’s linear memory.

The compiler lays out:

- Scalar globals
- Global arrays
- Global strings
- Global structures

Global initializers are converted into the initial virtual-memory image before execution.

Global addresses remain stable for the duration of the program.

## Local storage

Each function receives a frame layout.

Local variables and parameters are assigned offsets within the frame’s memory region.

The frame layout supports:

- Scalar parameters
- Pointer parameters
- Local scalars
- Local arrays
- Local structures
- Compiler-created temporary storage

When a function is called, the virtual machine creates a frame record and reserves the function’s local-memory region.

## Structures

A structure definition owns a linked sequence of member records.

Each member records:

- Name
- Type
- Byte offset
- Link to the next member

The compiler calculates member offsets using size and alignment.

The structure’s final size is rounded to satisfy its alignment requirements.

Member expressions use these computed offsets during address generation.

## Enumerations

Enumeration definitions store:

- Enumeration name
- Ordered enumeration values
- Explicit or automatically assigned integer values

Enumeration constants enter the symbol system as compile-time integer values.

They can be used in expressions and `switch` case labels.

## Initializers

Initializer nodes represent:

- Scalar initializers
- Array initializers
- Nested array initializers
- Structure initializers
- Character-array initialization from string literals

The compiler writes global initializers into the initial VM-memory image.

Local initializers generate runtime bytecode that clears, copies, or stores data in the current frame.

## Bytecode generator

The bytecode generator traverses the syntax tree and emits instructions for a stack-based virtual machine.

Instruction groups include:

- Constants
- Local addresses
- Global addresses
- Typed memory loads
- Typed memory stores
- Memory copying
- Memory clearing
- Stack operations
- Arithmetic
- Bitwise operations
- Comparisons
- Boolean conversion
- Integer conversion
- Jumps
- Conditional jumps
- Function calls
- Returns
- Formatted output
- Standard-input reads
- Program termination

Expressions leave their result on the VM value stack.

Statements either discard expression results or alter control flow.

## Address generation

Cinder separates value compilation from address compilation.

Value generation produces the value of an expression.

Address generation produces the virtual-memory address associated with an assignable expression.

Addressable expressions include:

- Local variables
- Global variables
- Array elements
- Dereferenced pointers
- Structure members
- Members accessed through structure pointers

This separation is required for:

- Assignment
- Compound assignment
- Increment and decrement
- Address-of expressions
- Array indexing
- Structure member access
- Typed loads and stores

## Typed memory operations

The VM supports typed memory access through instructions for:

- Signed 8-bit loads
- Unsigned 8-bit loads
- Signed 16-bit loads
- Unsigned 16-bit loads
- 32-bit loads
- 8-bit stores
- 16-bit stores
- 32-bit stores

The compiler chooses the correct instruction using the expression’s type.

This allows character and short integer objects to use their documented memory sizes.

## Control-flow generation

Branches and loops are generated using jump instructions.

When the final jump destination is not yet known, Cinder:

1. Emits a placeholder instruction.
2. Records the placeholder.
3. Compiles the remaining construct.
4. Patches the placeholder with the final destination.

This process is used for:

- `if`
- `else`
- Conditional expressions
- Logical short-circuit evaluation
- `while`
- `do...while`
- `for`
- `break`
- `continue`
- `switch`
- `case`
- `default`

## `switch` generation

A `switch` expression is evaluated once and stored in compiler-managed temporary frame storage.

Cinder records each case label and its instruction address.

Generated comparisons route control to the matching case body.

If no case matches, execution goes to:

- The `default` label, when present
- The end of the `switch`, otherwise

Case bodies preserve normal C-style fallthrough behavior.

`break` jumps are patched to the end of the closest applicable loop or switch construct.

## Function calls

Function arguments are evaluated and placed on the VM value stack.

A call instruction identifies:

- Target function
- Argument count
- Relevant syntax-tree node

The VM creates a frame containing:

- Return instruction address
- Caller stack base
- Caller memory-frame information
- Callee local-memory base
- Function identifier

Parameters are copied into their assigned local-memory offsets.

When the function returns, the frame is removed and the return value is restored to the caller’s stack.

The VM enforces a maximum call depth.

## Linear virtual memory

Cinder owns a fixed two-megabyte virtual-memory region.

This region stores:

- Global variables
- Global arrays
- Global strings
- Local frames
- Local arrays
- Local structures
- String literals
- Compiler-created temporary objects

Pointers are integer offsets into this region.

They are not native process addresses.

All VM memory operations verify that the requested address and byte range remain inside the virtual-memory region.

This design prevents a Cinder pointer from accessing the compiler process or browser memory outside the VM array.

## Virtual machine

The virtual machine executes Cinder bytecode using:

- Instruction pointer
- Value stack
- Stack pointer
- Function frames
- Frame pointer
- Linear memory
- Standard-input cursor
- Output buffer
- Execution-step counter

Before executing an instruction, the VM validates the instruction address and resource state.

During execution, it checks conditions including:

- Stack underflow
- Stack overflow
- Invalid memory access
- Division by zero
- Modulo by zero
- Invalid shifts
- Detected signed overflow
- Invalid function targets
- Excessive call depth
- Excessive execution steps
- Excessive program output
- Invalid strings
- Unknown opcodes

Runtime failures are converted into structured diagnostics.

## Standard input

Cinder maintains a separate bounded input buffer.

The native executable can populate this buffer from an optional text file:

```powershell
.\build\cinder.exe `
    .\source.c `
    .\input.txt
```

`getchar` reads one byte at a time.

When the buffer is exhausted, it returns `-1`.

For WebAssembly, JavaScript writes input text into the exported standard-input buffer before compilation begins.

## Program output

Program output is appended to a bounded output buffer.

Output can be produced by:

- `print`
- `printf`

The VM implements the supported formatting conversions directly.

No host C standard-library `printf` call is used by visitor code.

For `%s`, the VM reads a null-terminated string from virtual memory while validating every accessed address.

## Diagnostics

Cinder stores structured diagnostics.

Each diagnostic contains:

- Kind
- Message
- Source token
- Start offset
- End offset
- Line
- Column

Diagnostics can be produced during:

- Lexical analysis
- Parsing
- Type validation
- Symbol resolution
- Bytecode generation
- Virtual-machine execution

The browser can select the related source text using the stored offsets.

## JSON report

After compilation and execution, Cinder constructs a JSON report.

The report includes information such as:

- Whether compilation and execution succeeded
- Error summary
- Source range
- Program output
- Program return value
- Execution-step count
- Diagnostics
- Tokens
- Types
- Functions
- Syntax-tree nodes
- Instructions

The native executable writes this report to standard output.

The WebAssembly build exposes the report through a pointer and length.

## Native interface

The native executable accepts:

```text
cinder <source.c> [stdin.txt]
```

The source file is loaded into the compiler’s source buffer.

The optional input file is loaded into the standard-input buffer.

The resulting JSON report is written to the terminal.

## WebAssembly interface

The browser build exports functions including:

```text
input_ptr()
stdin_ptr()
input_capacity()
stdin_capacity()
set_stdin_len()
compile()
report_ptr()
report_len()
```

JavaScript uses these exports to:

1. Locate the source buffer.
2. Check the source capacity.
3. Copy UTF-8 source bytes into WebAssembly memory.
4. Locate the standard-input buffer.
5. Check the input capacity.
6. Copy UTF-8 input bytes into WebAssembly memory.
7. Set the input length.
8. Call `compile()`.
9. Locate the JSON report.
10. Decode and parse the report.

The WebAssembly module has no direct filesystem, network, process, or operating-system access.

## Web Worker

`web/compiler-worker.js` owns the WebAssembly instance.

The main interface sends compilation requests containing:

```js
{
  type: "compile",
  requestId,
  source,
  stdin
}
```

The worker:

1. Validates the request.
2. Checks encoded source and input sizes.
3. Copies data into WebAssembly memory.
4. Runs the compiler.
5. Reads the JSON report.
6. Sends the parsed report back to the interface.

Compilation occurs outside the browser’s main UI thread.

## Worker timeout

The interface applies a timeout to compilation requests.

If a compilation exceeds the limit, `web/app.js`:

1. Terminates the worker.
2. Rejects the pending compilation.
3. Creates a new worker.
4. Restores the compiler engine for the next run.

This prevents expensive compilation or execution from permanently freezing the interface.

The VM’s one-million-instruction limit remains the primary deterministic execution bound.

## Interface rendering limits

A successful compiler report may contain thousands of tokens, syntax-tree nodes, and instructions.

Rendering every item at once can make the browser interface slow even when compilation itself is fast.

The interface therefore applies separate display limits.

These limits affect only visualization.

They do not change what the compiler accepts or executes.

## Native build

The native build uses GCC in C11 mode with strict warnings:

```powershell
.\scripts\build.ps1
```

Warnings are treated as errors.

The output is:

```text
build/cinder.exe
```

## WebAssembly build

The browser build uses Zig’s C compiler with a freestanding WebAssembly target.

The output is:

```text
web/cinder.wasm
```

The build exports the compiler interface and WebAssembly memory while avoiding host operating-system dependencies.

## Automated tests

The test suite performs a clean build and checks:

- Successful compilation
- Expected output
- Expected return value
- Token generation
- Instruction generation
- Execution-step reporting
- Structured diagnostics
- Diagnostic source locations

Language tests cover:

- Arithmetic
- Functions
- Recursion
- Control flow
- Extended expressions
- Arrays
- Multidimensional arrays
- Pointers
- Strings
- Globals
- Structures
- Enumerations
- `switch`
- Integer types
- `sizeof`
- Standard input
- Runtime failures

Run:

```powershell
.\scripts\test.ps1
```

## Security boundary

Cinder does not compile visitor programs into native executable code.

Visitor programs execute only as Cinder bytecode inside the bounded VM.

The VM controls:

- Available instructions
- Addressable memory
- Stack capacity
- Call depth
- Execution count
- Output capacity
- Standard-input capacity

This allows the browser application to remain a static website.

Cinder is still an educational project rather than a formally verified security sandbox.

## Resource limits

| Resource | Maximum |
|---|---:|
| Source text | 64 KB |
| Standard input | 64 KB |
| Tokens | 16,384 |
| Syntax-tree nodes | 24,576 |
| Types | 4,096 |
| Symbols | 8,192 |
| Functions | 256 |
| Structures | 256 |
| Structure members | 4,096 |
| Enumerations | 256 |
| Enumeration values | 2,048 |
| Instructions | 49,152 |
| VM stack | 16,384 values |
| Call depth | 256 |
| Execution | 1,000,000 instructions |
| Output | 16 KB |
| Virtual memory | 2 MB |
| Diagnostics | 128 |

## Repository structure

```text
compiler/
  cinder.c
      Compiler core and virtual machine

tests/
  arithmetic.c
  functions.c
  control-flow.c
  extended.c
  diagnostics.c
  advanced-memory.c
  advanced-types.c
  advanced-stdin.c
  advanced-stdin.txt

scripts/
  build.ps1
  test.ps1
  serve.ps1

web/
  index.html
  globals.css
  app.js
  compiler-worker.js
  cinder.wasm

docs/
  LANGUAGE.md
  ARCHITECTURE.md
  ROADMAP.md
  screenshots/

.github/
  workflows/
    test.yml
```

## Architectural limitations

The current design intentionally prioritizes clarity and bounded execution.

Known architectural limits include:

- Single C source file for the compiler implementation
- Fixed-capacity compiler tables
- No multiple C translation units
- No native machine-code backend
- No optimization pipeline
- No preprocessor
- No complete standard library
- Partial ISO C type compatibility
- Partial undefined-behavior modeling
- A stack-oriented bytecode format
- A single-threaded virtual machine

These constraints keep the compiler understandable while leaving clear directions for future work.
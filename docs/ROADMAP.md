# Cinder Roadmap

Cinder is an educational C-subset compiler, bytecode generator, bounded virtual machine, and interactive browser workbench.

The project focuses on making compilation observable while supporting a meaningful portion of C.

## Current release

The current advanced compiler includes the complete pipeline from source text to bounded bytecode execution.

### Compiler front end

- Handwritten lexer
- Source offsets for every token
- Character literals
- String literals
- Integer literals
- C-style keywords and operators
- Single-line comments
- Block comments
- Recursive-descent parser
- Operator precedence
- Abstract syntax tree
- Structured diagnostics
- Line and column reporting

### Types

- `void`
- `char`
- `signed char`
- `unsigned char`
- `short`
- `unsigned short`
- `int`
- `unsigned int`
- `long`
- `unsigned long`
- Pointers
- Arrays
- Multidimensional arrays
- Structures
- Nested structures
- Structure pointers
- Enumerations
- Function types
- `const`
- Explicit casts
- `sizeof`
- Type size and alignment
- Typed integer conversions

### Storage and memory

- Local variables
- Global variables
- Nested block scopes
- Variable shadowing
- Active-symbol lifetime tracking
- Global initializers
- Local initializers
- Nested initializer lists
- String initialization
- Linear virtual memory
- Typed memory loads
- Typed memory stores
- Memory clearing
- Memory copying
- Pointer arithmetic
- Address validation

### Expressions

- Arithmetic
- Comparisons
- Logical operations
- Short-circuit evaluation
- Bitwise operations
- Shifts
- Assignment
- Compound assignment
- Prefix increment and decrement
- Postfix increment and decrement
- Conditional expressions
- Comma expressions
- Address-of
- Dereferencing
- Array indexing
- Structure member access
- Structure-pointer member access

### Statements and control flow

- Blocks
- Declarations
- Expression statements
- `if`
- `else`
- `while`
- `do...while`
- `for`
- `switch`
- `case`
- `default`
- Fallthrough
- `break`
- `continue`
- `return`

### Functions

- Function definitions
- Parameters
- Pointer parameters
- Array-parameter syntax
- Calls
- Nested calls
- Recursion
- Return values
- Argument-count checking
- Basic argument-type checking
- Bounded call depth

### Input and output

- `print`
- Bounded `printf`
- `%d`
- `%i`
- `%u`
- `%x`
- `%X`
- `%o`
- `%c`
- `%s`
- `%%`
- `getchar`
- Native input files
- WebAssembly input buffer
- Bounded program output

### Bytecode and execution

- Stack-based bytecode
- Value compilation
- Address compilation
- Typed load and store instructions
- Integer conversion instructions
- Jump generation
- Jump patching
- Loop control patching
- Switch dispatch
- Function call frames
- Return handling
- Two-megabyte virtual memory
- One-million-instruction execution limit
- Runtime diagnostics

### Browser workbench

- WebAssembly compiler engine
- Background Web Worker
- Worker recovery after timeout
- Source editor
- Program output
- Diagnostic display
- Source navigation
- Token explorer
- Syntax-tree explorer
- Instruction explorer
- Function information
- Responsive layout
- Keyboard execution shortcut
- Example programs
- Developer contact action
- Static Vercel deployment

### Development workflow

- Native GCC build
- Freestanding WebAssembly build
- Strict compiler warnings
- Automated PowerShell test suite
- GitHub Actions continuous integration
- Language documentation
- Architecture documentation
- Screenshots
- MIT License
- Contribution guide

## Immediate release work

These tasks complete the advanced browser release.

### Browser standard input

- Add a visible standard-input editor
- Send input text with every worker request
- Add an input-clear action
- Show the input-size limit
- Preserve input with the current example
- Add a browser example using `getchar`
- Verify the worker receives UTF-8 input correctly
- Test empty input
- Test input ending without a newline
- Test the 64 KB input boundary

### Workbench examples

Add examples demonstrating:

- Arrays and pointers
- Structures and enums
- `switch`
- Strings and `%s`
- Global variables
- Standard input
- Compiler diagnostics

Each example should run without modification.

### Interface content

Update the website to describe:

- Linear virtual memory
- Advanced type support
- Arrays and pointers
- Structures and enums
- Standard input
- Bounded execution
- Current limitations

The interface should continue to emphasize that Cinder is an educational C subset.

### Documentation

- Keep README limits synchronized with compiler constants
- Keep the language guide synchronized with parser support
- Keep architecture exports synchronized with the worker
- Document browser standard input
- Update screenshots after interface changes
- Add the advanced release to project history

### Deployment

- Rebuild `web/cinder.wasm`
- Run the complete test suite
- Test the production build locally
- Push the advanced branch
- Review the GitHub Actions result
- Merge into `main`
- Verify the Vercel deployment
- Test the deployed worker and WebAssembly paths
- Test on desktop and mobile

## Near-term compiler improvements

These changes would expand correctness without changing Cinder’s educational scope.

### Diagnostic quality

- Add expected-token suggestions
- Show related declaration locations
- Distinguish syntax, type, and runtime errors visually
- Add notes for incompatible pointer types
- Add clearer array-bound context
- Add clearer structure-member suggestions
- Add function-call argument diagnostics
- Include source excerpts in native diagnostic output
- Add warning categories
- Support multiple diagnostics where recovery is possible

### Parser recovery

- Recover after statement-level syntax errors
- Continue after malformed declarations
- Synchronize at semicolons and closing braces
- Report multiple independent errors in one compilation
- Avoid cascaded diagnostics caused by one missing token

### Type checking

- Expand integer-promotion rules
- Expand usual arithmetic conversions
- Improve signed and unsigned comparisons
- Improve pointer compatibility checks
- Improve `const` propagation
- Validate more structure assignments
- Validate more return-type conversions
- Warn about narrowing conversions

### Initializers

- Infer more omitted array dimensions
- Improve excess-initializer diagnostics
- Support more partially initialized aggregates
- Add designated initializers
- Add compound literals

### Switch statements

- Improve duplicate-case reporting
- Warn about unreachable cases
- Improve enumeration switch checking
- Optionally report missing enumeration values

## Medium-term language features

### Type aliases

Implement:

```c
typedef unsigned long Size;
```

This requires:

- Typedef symbols
- Declaration disambiguation
- Type-name lookup
- Typedef-aware parsing
- Updated syntax-tree reporting

### Unions

Implement:

```c
union Value {
    int integer;
    char bytes[4];
};
```

This requires:

- Shared member offsets
- Union size and alignment
- Union initialization
- Member-access validation
- Updated type reporting

### Function prototypes

Implement declarations such as:

```c
int add(int first, int second);
```

This requires:

- Function declaration records
- Definition matching
- Conflicting-declaration diagnostics
- Call validation before definitions
- Support for source-order independence

### Function pointers

Implement declarations such as:

```c
int (*operation)(int, int);
```

This requires:

- More complete declarator parsing
- Callable pointer types
- Indirect-call bytecode
- Runtime target validation
- Function-address representation

### Additional integer support

Potential additions include:

- `long long`
- `unsigned long long`
- Additional literal suffixes
- Hexadecimal literals
- Octal literals
- Binary literals as an extension
- More complete overflow behavior

### Boolean support

Add:

```c
_Bool
```

Potentially provide a small built-in equivalent of `stdbool.h` without implementing a general preprocessor.

## Standard-library direction

Cinder does not aim to reproduce the complete C standard library.

A small educational runtime could provide bounded implementations of:

- `puts`
- `putchar`
- `strlen`
- `strcmp`
- `strcpy` with strict memory validation
- `memset`
- `memcpy`
- Integer parsing

Every routine would operate only on Cinder virtual memory.

Filesystem, networking, processes, and native library loading would remain unavailable in the browser build.

## Preprocessor direction

A future preprocessing stage could support:

- Object-like macros
- Function-like macros
- Conditional compilation
- Local header inclusion
- Macro-expansion diagnostics

A complete production-quality C preprocessor is a large independent project.

Preprocessor support should therefore remain a separate milestone.

## Bytecode improvements

Potential bytecode work includes:

- Instruction validation before execution
- Constant folding
- Dead-code elimination
- Jump simplification
- Stack-depth analysis
- Compact instruction encoding
- Disassembler output
- Per-function instruction ranges
- Bytecode verification
- Execution tracing
- Breakpoints
- Single-step execution

Optimization should preserve source relationships needed by the visual interface.

## Virtual-machine improvements

Potential VM improvements include:

- Read-only memory regions
- Separate global, stack, and literal regions
- More precise pointer-range validation
- Object-aware bounds checking
- Uninitialized-memory diagnostics
- Stack traces
- Function names in runtime failures
- Source locations for every executed instruction
- Optional execution tracing
- Configurable limits for local development
- Deterministic profiling information

## Browser-workbench improvements

Potential interface improvements include:

- Resizable editor and result panels
- Search inside token and instruction views
- Syntax-tree filtering
- Collapsible structure and function sections
- Instruction-to-source highlighting
- Source-to-instruction highlighting
- VM memory viewer
- Call-stack viewer
- Execution-step mode
- Breakpoints
- Shareable examples encoded in URLs
- Download source button
- Import local `.c` files
- Export compiler reports
- Accessible high-contrast theme
- Reduced-motion mode
- More mobile-specific controls

These features should be implemented only when they preserve responsiveness.

## Testing improvements

The test suite should continue expanding with focused programs.

Planned categories include:

- Pointer boundary failures
- Multidimensional array layout
- Structure padding
- Nested structure assignment
- Global aggregate initialization
- String termination
- Invalid format strings
- Input exhaustion
- Recursion limits
- Stack exhaustion
- Instruction-limit termination
- Output-limit termination
- Invalid memory access
- Duplicate case labels
- Invalid member access
- Invalid casts
- `const` assignment failures
- Signed and unsigned conversion boundaries

Future tests should distinguish:

- Lexer tests
- Parser tests
- Type-system tests
- Code-generation tests
- VM tests
- Integration tests
- Browser-worker tests

## Fuzzing

A future native test tool could generate or mutate Cinder programs and verify that:

- The compiler never crashes
- Reports always contain valid JSON
- Invalid input produces bounded diagnostics
- Execution always respects limits
- Native and WebAssembly results agree

Differential testing against GCC or Clang could be used only for programs inside Cinder’s documented subset.

## Multiple-file architecture

The current single-file compiler makes the project easy to inspect and build.

As the implementation grows, it may eventually be divided into modules such as:

```text
compiler/
  lexer.c
  parser.c
  types.c
  symbols.c
  bytecode.c
  vm.c
  diagnostics.c
  report.c
  cinder.c
```

Headers could define the shared compiler structures.

This refactor should happen only when it improves maintainability without hiding the educational pipeline.

## Native backend

Generating native machine code is outside the current release scope.

Possible long-term backends include:

- A simple x86-64 backend
- WebAssembly code generation
- LLVM IR generation
- A register-based custom VM

The existing bytecode VM remains useful because it provides bounded browser execution and inspectable instructions.

## Project principles

Future changes should preserve these principles:

1. The compiler core remains written in C.
2. Compiler stages remain observable.
3. Visitor code remains bounded.
4. The browser remains responsive.
5. Diagnostics refer back to source code.
6. Documented limits match implementation constants.
7. Tests cover every advertised language feature.
8. The website does not claim full ISO C compatibility.
9. The implementation remains understandable in an interview.
10. New features should strengthen the compiler rather than only enlarge it.

## Long-term goal

Cinder’s long-term goal is to become a polished teaching compiler that demonstrates how source code moves through:

```text
text
  -> tokens
  -> syntax
  -> types
  -> memory layout
  -> bytecode
  -> execution
  -> diagnostics
```

It does not need full ISO C compatibility to meet this goal.

Its value comes from combining a real compiler pipeline, a bounded runtime, detailed visualizations, and an interactive browser experience in one inspectable project.
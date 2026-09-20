# Cinder Roadmap

Cinder’s current release focuses on a complete and inspectable compilation pipeline rather than full ISO C compatibility.

## Current release

The current compiler includes:

- Lexer with source locations
- Recursive-descent parser
- Abstract syntax tree
- Local symbols and block scopes
- Signed integer arithmetic
- Basic character declarations and literals
- Comparisons and logical expressions
- Bitwise expressions and shifts
- Simple and compound assignment
- Prefix and postfix updates
- Conditional expressions
- Conditions and loops
- Functions and recursion
- Bounded bytecode virtual machine
- Limited formatted output
- Native and WebAssembly builds
- Web Worker execution
- Tokens, syntax-tree, and instruction explorers
- Source-linked diagnostics
- Responsive interface
- Execution and rendering limits

## Near-term improvements

These features are appropriate future extensions:

- Additional compiler tests
- More precise type tracking
- Better diagnostic context
- Line and column information in reports
- Downloadable compiler reports
- Shareable source-code URLs
- More example programs
- Accessibility audits
- Performance measurements

## Memory-model milestone

A future memory-model release may add:

- Global variables
- Linear VM memory
- Arrays
- Array indexing
- Address-of and dereference operators
- Pointer arithmetic
- String storage
- Character arrays
- `%s` output
- Memory-bound diagnostics

This milestone requires changes across the parser, type system, bytecode, VM, tests, and visual explorer.

## Type-system milestone

A later type-system release may add:

- Accurate `char` conversion
- Signed and unsigned integer types
- `short` and `long`
- Explicit casts
- `sizeof`
- Function return-type validation
- Declaration compatibility checks

## Control-flow milestone

Possible control-flow additions include:

- `switch`
- `case`
- `default`
- Constant expressions
- Improved unreachable-code diagnostics

## Full C mode

Full ISO C support is outside the current from-scratch compiler scope.

A separate full-C mode could submit programs to GCC or Clang inside an isolated backend container. That service would require:

- Process isolation
- CPU and memory limits
- Execution timeouts
- Restricted filesystem access
- Disabled network access
- Request limits
- Output limits
- Monitoring and abuse protection

This mode would remain separate from Cinder’s own compiler so the project continues to demonstrate an original compilation pipeline.

## Project principles

Future work should preserve:

- Clear compiler stages
- Predictable execution
- Honest documentation
- Useful diagnostics
- Browser responsiveness
- Inspectable intermediate representations
- Tests for every advertised feature
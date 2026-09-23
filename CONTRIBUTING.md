# Contributing to Cinder

Cinder is an educational C-subset compiler, bytecode virtual machine, and interactive browser workbench.

Contributions should improve compiler correctness, diagnostics, documentation, tests, performance, accessibility, or the learning experience.

## Project scope

Cinder demonstrates how C source code moves through:

```text
source
  -> tokens
  -> syntax tree
  -> semantic analysis
  -> bytecode
  -> bounded execution
  -> output or diagnostics
```

Cinder is not a complete ISO C implementation.

Do not describe it as a replacement for GCC, Clang, or another production C compiler.

## Before contributing

Read:

- `README.md`
- `docs/LANGUAGE.md`
- `docs/ARCHITECTURE.md`
- `docs/ROADMAP.md`

Review the existing tests before implementing a language feature.

Every advertised feature must be supported by:

- The native compiler
- The WebAssembly compiler
- Automated tests
- Accurate documentation

## Requirements

Local development requires:

- GCC with C11 support
- Zig 0.14.1 or a compatible release
- PowerShell
- Python
- Git
- A modern browser

Verify the required tools:

```powershell
gcc --version
zig version
python --version
git --version
```

## Repository setup

Clone the repository and enter it:

```powershell
git clone <repository-url>
cd Cinder
```

Create a feature branch:

```powershell
git switch -c feature/short-description
```

Use a focused branch name such as:

```text
feature/pointer-diagnostics
feature/standard-input-ui
fix/structure-layout
docs/language-guide
```

## Build

From the repository root, run:

```powershell
.\scripts\build.ps1
```

The build creates:

```text
build/cinder.exe
web/cinder.wasm
```

The native compiler is built using C11 with warnings treated as errors.

The WebAssembly compiler uses the same `compiler/cinder.c` implementation.

A contribution is not complete if only one build target succeeds.

## Test

Run:

```powershell
.\scripts\test.ps1
```

The test script:

1. Builds the native compiler.
2. Builds the WebAssembly module.
3. Runs successful programs.
4. Checks exact program output.
5. Checks return values.
6. Checks token and instruction generation.
7. Checks execution-step reporting.
8. Runs expected-failure programs.
9. Checks structured diagnostic information.
10. Checks diagnostic source locations.

The complete suite should finish with:

```text
8 of 8 tests passed.
```

The number will increase when new tests are added.

## Test expectations

Every new language feature should include meaningful tests.

A test should verify observable behavior such as:

- Program output
- Program return value
- Memory layout
- Pointer behavior
- Integer conversion
- Control flow
- Function behavior
- Standard input
- Compiler diagnostics
- Runtime diagnostics
- Safety limits

Avoid tests that merely repeat the implementation.

Prefer small programs that isolate one behavior.

When fixing a bug:

1. Add a test that fails before the correction.
2. Implement the correction.
3. Confirm the new test passes.
4. Run the complete suite.
5. Update documentation if public behavior changed.

## Adding a successful-program test

Create a C source file under:

```text
tests/
```

Add its expected output and result to:

```text
scripts/test.ps1
```

A successful test entry follows this structure:

```powershell
@{
    Name = "Feature description"
    File = "tests\feature.c"
    ExpectedOutput =
        "expected output`n"
    ExpectedResult = 0
}
```

For a program using standard input:

```powershell
@{
    Name = "Standard input feature"
    File = "tests\feature.c"
    InputFile = "tests\feature-input.txt"
    ExpectedOutput =
        "expected output`n"
    ExpectedResult = 0
}
```

## Adding a diagnostic test

Create the invalid C program under:

```text
tests/
```

Add an entry to `$diagnosticTests` in `scripts/test.ps1`:

```powershell
@{
    Name = "Diagnostic description"
    File = "tests\invalid-feature.c"
    ExpectedError = "Expected diagnostic text"
}
```

Diagnostics should be:

- Precise
- Actionable
- Connected to the relevant source range
- Stable enough for automated tests
- Free from internal implementation terminology where possible

## Run locally

Start the local static server:

```powershell
.\scripts\serve.ps1
```

Open:

```text
http://localhost:4173
```

Do not open `web/index.html` directly. Browsers restrict WebAssembly and worker loading from local files.

## Compiler changes

When adding or changing a language feature, review every affected compiler stage.

A typical compiler change may require updates to:

1. Token definitions
2. Keyword recognition
3. Lexer logic
4. Type representation
5. Declarator parsing
6. Expression parsing
7. Statement parsing
8. Syntax-tree node definitions
9. Symbol lookup
10. Scope management
11. Type validation
12. Memory layout
13. Address generation
14. Value generation
15. Bytecode instructions
16. Virtual-machine execution
17. Diagnostics
18. JSON reporting
19. Native tests
20. Browser examples
21. Documentation

Do not stop after parsing a feature. A parsed feature is incomplete unless it also generates correct bytecode, executes correctly, reports errors, and has tests.

## Type-system changes

When adding a type or conversion, verify:

- Type size
- Type alignment
- Signedness
- Qualifiers
- Assignment compatibility
- Function argument compatibility
- Return compatibility
- Cast behavior
- `sizeof`
- Typed loads
- Typed stores
- JSON type names
- Documentation

Pointer arithmetic must scale using the pointed-to type.

Aggregate layout must remain consistent between native and WebAssembly builds.

## Virtual-memory changes

Cinder pointers are offsets inside the VM’s linear memory.

They are never native process addresses.

Memory changes must preserve:

- Address bounds checks
- Object-size calculations
- Frame bounds
- Global storage bounds
- Stack limits
- Alignment calculations
- Deterministic native and WebAssembly behavior

Visitor code must not receive direct access to host memory.

## Bytecode changes

When adding an opcode:

1. Add it to the opcode enumeration.
2. Add its reporting name.
3. Emit it from code generation.
4. Execute it in the VM.
5. Validate its stack behavior.
6. Connect it to a source node.
7. Add positive tests.
8. Add failure tests where relevant.
9. Update architecture documentation.

Every instruction must preserve the VM’s bounded execution model.

## Resource limits

Cinder intentionally limits:

- Source size
- Standard-input size
- Tokens
- Syntax-tree nodes
- Types
- Symbols
- Functions
- Structures
- Generated instructions
- VM stack
- Call depth
- Virtual memory
- Execution steps
- Program output
- Diagnostics

Do not silently increase a limit in only one layer.

A limit change may require coordinated updates to:

- `compiler/cinder.c`
- `scripts/build.ps1`
- `web/compiler-worker.js`
- `web/app.js`
- `README.md`
- `docs/LANGUAGE.md`
- `docs/ARCHITECTURE.md`
- Tests

## Website changes

When changing the interface:

- Keep compilation inside the Web Worker.
- Keep visitor code inside the browser.
- Avoid blocking the main thread.
- Preserve the worker timeout.
- Preserve keyboard navigation.
- Preserve visible focus states.
- Preserve semantic labels.
- Preserve screen-reader status text.
- Test keyboard-only operation.
- Test reduced-motion mode.
- Avoid unnecessary third-party dependencies.
- Keep rendering limits for large reports.
- Escape compiler-controlled text before inserting HTML.
- Keep mobile controls usable.

Test these viewport widths:

- 1,440 pixels
- 1,024 pixels
- 768 pixels
- 430 pixels
- 360 pixels

## Browser-worker messages

Compilation requests use a structured message containing:

```js
{
  type: "compile",
  requestId,
  source,
  stdin
}
```

Worker responses must retain the matching `requestId`.

This prevents an older compilation response from replacing a newer result.

## WebAssembly interface

If a WebAssembly export changes, update all of these together:

- `compiler/cinder.c`
- `scripts/build.ps1`
- `web/compiler-worker.js`
- `docs/ARCHITECTURE.md`
- Automated tests

Required compiler exports currently include:

```text
input_ptr
stdin_ptr
input_capacity
stdin_capacity
set_stdin_len
compile
report_ptr
report_len
memory
```

## Documentation changes

Update documentation whenever public behavior changes.

Use:

- `README.md` for the project overview
- `docs/LANGUAGE.md` for supported syntax
- `docs/ARCHITECTURE.md` for implementation details
- `docs/ROADMAP.md` for planned work
- `CONTRIBUTING.md` for contributor workflow

Compiler constants and documented limits must agree.

Do not advertise a feature until its implementation and tests pass.

## Code style

### C

- Use C11.
- Treat warnings as errors.
- Use descriptive names.
- Keep functions focused.
- Check every fixed-capacity table before appending.
- Check memory ranges before VM access.
- Keep native and WebAssembly behavior consistent.
- Use comments for algorithms and architectural decisions.
- Remove comments that only repeat the next line.
- Avoid unused functions and variables.
- Keep diagnostics understandable to C programmers.

### JavaScript

- Use `const` by default.
- Use `let` only for changing bindings.
- Keep DOM references near the top of the file.
- Keep state explicit.
- Avoid global browser dependencies.
- Escape untrusted text.
- Keep expensive work outside the main thread.
- Preserve request identifiers and timeout cleanup.

### HTML

- Use semantic elements.
- Connect labels to controls.
- Use buttons for actions.
- Preserve ARIA relationships for tabs and panels.
- Keep the document usable without a mouse.

### CSS

- Reuse existing design variables.
- Preserve the established Cinder visual language.
- Test focus, hover, disabled, loading, and error states.
- Support reduced motion.
- Avoid layout shifts during compilation.
- Check long diagnostics and large outputs.

### PowerShell

- Use `$ErrorActionPreference = "Stop"`.
- Resolve paths relative to the script.
- Use `-LiteralPath` for filesystem paths.
- Throw clear messages when a step fails.
- Keep scripts runnable from the repository root.

## Commit quality

Before committing, run:

```powershell
git diff --check
.\scripts\test.ps1
git status --short
```

`git diff --check` should produce no output.

Review the staged files:

```powershell
git diff --cached --name-only
git diff --cached
```

Use a clear commit message:

```powershell
git commit -m "Add browser standard input"
```

or:

```powershell
git commit -m "Improve pointer diagnostics"
```

Do not commit:

- `build/`
- Executable files
- Temporary backups
- Editor settings
- Environment files
- Local logs
- Debug dumps
- Personal credentials

`web/cinder.wasm` is an intentional tracked deployment artifact.

## Pull requests

A pull request should explain:

- The concrete problem
- The resulting behavior
- Compiler stages affected
- Tests added or updated
- Documentation changed
- User-visible changes
- Known limitations

For a compiler feature, include a small C example.

For an interface change, include updated screenshots when the appearance materially changes.

## Reporting problems

A compiler problem report should include:

- The smallest C program that reproduces the problem
- Expected behavior
- Actual output or diagnostic
- Whether it occurred natively or in the browser
- Browser and operating system when relevant
- Build output when relevant
- Test output when relevant

Do not include:

- Passwords
- API keys
- Private source code
- Personal access tokens
- Environment secrets
- Private customer information

## Security

Cinder executes programs inside its own bounded VM.

Changes must not introduce a path that executes visitor-supplied native code or grants access to:

- The filesystem
- The network
- Browser credentials
- Environment variables
- Host processes
- Native process memory

Report security concerns privately to the repository owner before public disclosure.
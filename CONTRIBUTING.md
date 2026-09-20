# Contributing to Cinder

Cinder is an educational C-subset compiler and interactive compiler explorer.

## Before contributing

Read:

- `README.md`
- `docs/ARCHITECTURE.md`
- `docs/LANGUAGE.md`
- `docs/ROADMAP.md`

Do not describe Cinder as a complete ISO C implementation.

## Requirements

Local development requires:

- GCC with C11 support
- Zig
- PowerShell
- A modern browser

Verify that GCC and Zig are available:

```powershell
gcc --version
zig version
```

## Build

From the repository root:

```powershell
.\scripts\build.ps1
```

This produces:

```text
build/cinder.exe
web/cinder.wasm
```

Warnings are treated as errors.

## Test

```powershell
.\scripts\test.ps1
```

Every advertised language feature should have at least one meaningful test.

A test should verify observable behavior such as:

- Program output
- Return result
- Compiler diagnostic
- Short-circuit behavior
- Loop behavior
- Function behavior
- Runtime safety limit

Avoid tests that only reproduce implementation details.

## Run locally

```powershell
.\scripts\serve.ps1
```

Open the local address printed by the script. Do not open `index.html` directly because browsers restrict WebAssembly loading from local files.

## Compiler changes

When changing the compiler:

1. Update the lexer if new tokens are required.
2. Update parser precedence and syntax-tree nodes.
3. Update bytecode generation.
4. Update the virtual machine.
5. Update JSON names for new nodes or instructions.
6. Add positive and negative tests.
7. Update `docs/LANGUAGE.md`.
8. Verify both native and WebAssembly builds.

Keep execution bounded. New features must not bypass source, stack, recursion, execution, output, or rendering limits.

## Website changes

When changing the interface:

- Keep compilation inside the Web Worker.
- Avoid blocking the main browser thread.
- Preserve keyboard navigation.
- Preserve visible focus states.
- Test reduced-motion mode.
- Test widths of 1,440, 1,024, 768, 430, and 360 pixels.
- Avoid large frameworks or assets without a clear reason.

## Code style

- Use C11 for the compiler core.
- Treat compiler warnings as errors.
- Prefer descriptive names.
- Keep functions focused.
- Comment architectural decisions and non-obvious algorithms.
- Remove comments that only restate the next line.
- Keep error messages precise and actionable.
- Do not add unsupported claims to documentation.

## Pull requests

A pull request should explain:

- The problem being solved
- The resulting behavior
- Compiler stages affected
- Tests added or updated
- Documentation changes
- Known limitations

## Reporting problems

When reporting a compiler problem, include:

- The smallest C program that reproduces it
- Expected output or behavior
- Actual output or diagnostic
- Browser or native build
- Build and test output when relevant

Do not include secrets, private source code, or personal credentials in reports.
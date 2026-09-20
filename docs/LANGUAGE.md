# Cinder Language Guide

Cinder implements a focused subset of C for learning compiler construction. It is not a complete ISO C compiler.

## Program entry point

Every program must define `main` without parameters.

```c
int main(void)
{
    return 0;
}
```

This form also works:

```c
int main()
{
    return 0;
}
```

## Values

Cinder uses signed 32-bit integer values internally.

Decimal integer literals are supported:

```c
0
42
2147483647
```

Integer literals outside the signed 32-bit range are rejected.

## Character literals

Character literals produce integer character values:

```c
'A'
'7'
'\n'
'\r'
'\t'
'\0'
'\\'
'\''
```

`char` variables currently occupy an integer-sized VM slot. Cinder does not yet implement every ISO C integer-conversion rule.

## Variables

Local integer and character declarations are supported:

```c
int answer = 42;
int empty;
char letter = 'A';
```

Variables without an initializer start at zero.

Nested scopes and shadowing are supported:

```c
int value = 10;

{
    int value = 20;
    print(value);
}

print(value);
```

Declaring the same name twice in one scope is an error.

## Arithmetic

```c
a + b
a - b
a * b
a / b
a % b
```

Cinder detects division by zero, modulo by zero, and signed arithmetic overflow.

## Comparisons

```c
a == b
a != b
a < b
a <= b
a > b
a >= b
```

Comparison results are `0` or `1`.

## Logical expressions

```c
a && b
a || b
!a
```

Logical AND and logical OR use short-circuit evaluation.

## Bitwise expressions

```c
a & b
a | b
a ^ b
~a
a << count
a >> count
```

A shift count must be between `0` and `31`.

## Assignment

```c
value = 10;
value += 2;
value -= 2;
value *= 2;
value /= 2;
value %= 2;
```

Assignments are expressions and return the stored value.

## Increment and decrement

```c
value++;
value--;
++value;
--value;
```

Postfix operations return the old value. Prefix operations return the updated value.

## Conditional expressions

```c
int largest =
    first > second
        ? first
        : second;
```

Only the selected expression is evaluated.

## Conditions

```c
if (condition) {
    print(1);
}

if (condition) {
    print(1);
} else {
    print(0);
}
```

Zero is false. Any nonzero value is true.

## Loops

### While

```c
while (condition) {
    statement;
}
```

### Do while

```c
do {
    statement;
} while (condition);
```

### For

```c
for (
    int index = 0;
    index < 10;
    index++
) {
    print(index);
}
```

A missing `for` condition is treated as true.

### Loop control

```c
break;
continue;
```

These statements must appear inside a loop.

## Functions

```c
int add(
    int first,
    int second
)
{
    return first + second;
}
```

Cinder supports:

- `int` and basic `char` declarations
- As many as 32 parameters
- Function calls
- Nested calls
- Recursion
- Return values
- Argument-count checking

Function overloading and function prototypes are not supported.

## Output

### `print`

`print` accepts one integer and appends a newline.

```c
print(42);
```

### `printf`

The first argument must be a literal format string.

```c
printf(
    "answer = %d\n",
    answer
);
```

Supported conversions:

| Conversion | Meaning |
|---|---|
| `%d` | Signed decimal integer |
| `%i` | Signed decimal integer |
| `%u` | Unsigned decimal representation |
| `%x` | Lowercase hexadecimal |
| `%X` | Uppercase hexadecimal |
| `%o` | Octal |
| `%c` | Character value from 0 to 255 |
| `%%` | Literal percent sign |

Cinder reports missing and extra format arguments.

## Comments

```c
// Single-line comment

/*
 * Block comment
 */
```

Nested block comments are not supported.

## Operator precedence

From lowest to highest:

1. Assignment
2. Conditional `?:`
3. Logical OR `||`
4. Logical AND `&&`
5. Bitwise OR `|`
6. Bitwise XOR `^`
7. Bitwise AND `&`
8. Equality `== !=`
9. Relational `< <= > >=`
10. Shifts `<< >>`
11. Addition and subtraction
12. Multiplication, division, and modulo
13. Unary operations
14. Primary expressions and postfix updates

## Compiler limits

| Resource | Maximum |
|---|---:|
| Source text | 32 KB |
| Tokens | 8,192 |
| Syntax-tree nodes | 8,192 |
| Generated instructions | 16,384 |
| Functions | 128 |
| Parameters per function | 32 |
| Local variables per function | 256 |
| VM value stack | 4,096 values |
| Function-call depth | 128 |
| Executed instructions | 200,000 |
| Program output | 8 KB |

The browser adds a three-second worker timeout.

## Unsupported C features

Cinder currently does not support:

- Arrays
- Pointers
- Global variables
- String variables
- `%s`
- `scanf`
- `switch`, `case`, or `default`
- Structures or unions
- Enums
- `typedef`
- Floating-point types
- `short`, `long`, or a complete unsigned type system
- Function prototypes
- Variadic user-defined functions
- Multiple source files
- Header inclusion
- Macros
- The standard C library
- Filesystem, network, or process APIs

Programs requiring those features should be compiled with a complete implementation such as GCC or Clang.
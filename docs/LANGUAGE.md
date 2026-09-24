# Cinder Language Guide

Cinder implements an educational subset of C. It supports enough of the language to demonstrate lexical analysis, parsing, type checking, memory layout, bytecode generation, and bounded execution.

Cinder is not a complete ISO C compiler. Programs that depend on the preprocessor, the complete standard library, floating-point arithmetic, multiple source files, or complete ISO C conversion rules should use GCC or Clang.

## Program entry point

Every executable program must define `main`.

```c
int main()
{
    return 0;
}
```

This form is also accepted:

```c
int main(void)
{
    return 0;
}
```

Cinder begins execution by calling `main`.

## Comments

Single-line comments are supported:

```c
// This is a comment.
int answer = 42;
```

Block comments are supported:

```c
/*
 * This is a block comment.
 */
int answer = 42;
```

Nested block comments are not supported.

## Fundamental types

Cinder supports the following fundamental types:

```c
void
char
signed char
unsigned char
short
unsigned short
int
unsigned int
long
unsigned long
```

The virtual machine currently uses these sizes:

| Type | Size |
|---|---:|
| `char` | 1 byte |
| `signed char` | 1 byte |
| `unsigned char` | 1 byte |
| `short` | 2 bytes |
| `unsigned short` | 2 bytes |
| `int` | 4 bytes |
| `unsigned int` | 4 bytes |
| `long` | 4 bytes |
| `unsigned long` | 4 bytes |
| Pointer | 4 bytes |

Cinder does not currently implement `long long`, floating-point types, or every integer-promotion rule from ISO C.

## Integer literals

Decimal integer literals are supported:

```c
0
42
100000
```

Unsigned suffixes are supported:

```c
15u
25U
```

Values are checked against the ranges supported by Cinder’s integer model.

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

Character values can be stored in `char`, signed character, unsigned character, or integer objects.

```c
char grade = 'A';
unsigned char byte = 250;
```

## String literals

String literals are stored in Cinder’s virtual memory and terminated with a null character.

```c
char message[] = "Hello, Cinder";
```

String literals can be:

- Used to initialize character arrays
- Stored through character pointers
- Passed to functions
- Printed with `%s`
- Indexed like character arrays

Example:

```c
int main()
{
    char message[] = "Hello";

    printf("%s\n", message);
    printf("%c\n", message[1]);

    return 0;
}
```

String literals are not dynamically allocated.

## Variables

Local variables are supported:

```c
int answer = 42;
char letter = 'A';
```

Variables without an explicit initializer are initialized to zero:

```c
int total;
```

Nested scopes are supported:

```c
int main()
{
    int value = 10;

    {
        int inner = 20;
        printf("%d\n", inner);
    }

    return value;
}
```

A nested scope may shadow an outer variable:

```c
int main()
{
    int value = 10;

    {
        int value = 20;
        printf("%d\n", value);
    }

    printf("%d\n", value);

    return 0;
}
```

Declaring the same name twice in one active scope is an error.

## Global variables

Global variables are supported:

```c
int total = 10;
char message[] = "global text";

int main()
{
    printf("%d\n", total);
    printf("%s\n", message);

    return 0;
}
```

Global objects are placed in the virtual machine’s linear memory before program execution begins.

Global arrays and structures are supported.

## Constants

`const` declarations are supported:

```c
const int limit = 10;
```

Assignments to `const` objects are rejected:

```c
const int limit = 10;
limit = 20;
```

Cinder enforces `const` for direct assignments to declared objects. It does not yet implement every qualifier-conversion rule required by ISO C.

## Arrays

One-dimensional arrays are supported:

```c
int numbers[5];
```

Arrays can be initialized:

```c
int numbers[5] = {
    2,
    4,
    6,
    8,
    10
};
```

Array elements can be read and written:

```c
numbers[2] = 20;
printf("%d\n", numbers[2]);
```

Array indexing uses the element type’s size to calculate the memory address.

## Multidimensional arrays

Multidimensional arrays are supported:

```c
int matrix[2][3] = {
    {
        1,
        2,
        3
    },
    {
        4,
        5,
        6
    }
};
```

Elements can be accessed using multiple index expressions:

```c
int value = matrix[1][2];
```

Array dimensions must be known during compilation.

Variable-length arrays are not supported.

## Array initialization

Nested initializer lists are supported:

```c
int matrix[2][2] = {
    {
        1,
        2
    },
    {
        3,
        4
    }
};
```

Character arrays may be initialized using string literals:

```c
char name[] = "Cinder";
```

Cinder zero-initializes unused storage where supported by the declaration.

Designated initializers are not supported.

## Pointers

Pointer declarations are supported:

```c
int *pointer;
char *text;
```

The address-of operator obtains the address of an object:

```c
int value = 42;
int *pointer = &value;
```

The dereference operator reads or writes the pointed-to object:

```c
int value = 42;
int *pointer = &value;

printf("%d\n", *pointer);

*pointer = 100;
```

Cinder pointers store offsets inside the virtual machine’s linear memory.

They are not native operating-system addresses.

## Pointer arithmetic

Pointer addition and subtraction are supported:

```c
int numbers[4] = {
    10,
    20,
    30,
    40
};

int *pointer = &numbers[0];

printf("%d\n", *(pointer + 2));
```

Pointer arithmetic scales the integer offset by the size of the pointed-to type.

Array indexing and pointer arithmetic are related:

```c
numbers[index]
*(numbers + index)
```

Invalid virtual-memory access produces a runtime diagnostic.

Complete ISO C pointer-comparison and pointer-provenance rules are not implemented.

## Pointer parameters

Arrays can be passed to functions using pointer or array parameter syntax:

```c
int sum(
    int values[],
    int count
)
{
    int index = 0;
    int total = 0;

    while (index < count) {
        total += values[index];
        index++;
    }

    return total;
}
```

Explicit pointer syntax is also supported:

```c
int first_value(int *values)
{
    return *values;
}
```

## Structures

Structures are supported:

```c
struct Point {
    int x;
    int y;
};
```

Structure objects can be declared and initialized:

```c
struct Point point = {
    10,
    20
};
```

Members are accessed with the member operator:

```c
point.x = 30;
printf("%d\n", point.y);
```

Nested structures are supported:

```c
struct Position {
    int row;
    int column;
};

struct Record {
    struct Position position;
    unsigned short code;
};
```

Nested member access is supported:

```c
record.position.row
```

Structure layout accounts for member size and alignment.

## Structure pointers

The arrow operator is supported:

```c
int point_total(
    struct Point *point
)
{
    return point->x + point->y;
}
```

The following expressions refer to the same member:

```c
pointer->x
(*pointer).x
```

## Enumerations

Enumerations are supported:

```c
enum Status {
    STATUS_IDLE,
    STATUS_RUNNING,
    STATUS_DONE
};
```

Explicit values are supported:

```c
enum Status {
    STATUS_IDLE = 2,
    STATUS_RUNNING,
    STATUS_DONE = 8
};
```

When a value is omitted, Cinder assigns the preceding value plus one.

Enumeration constants can be used in expressions and `switch` labels.

## `sizeof`

The `sizeof` operator is supported for types and expressions:

```c
sizeof(char)
sizeof(short)
sizeof(int)
sizeof(struct Point)
sizeof(numbers)
```

`sizeof` returns the number of bytes reserved by Cinder’s virtual-memory model.

For an array object, `sizeof` returns the complete array size:

```c
int numbers[5];

printf("%u\n", sizeof(numbers));
```

This prints `20` because each Cinder `int` occupies four bytes.

## Casts

Explicit casts are supported:

```c
int value = 300;
unsigned char byte =
    (unsigned char)value;
```

Cinder implements conversions for its supported integer types, enumerations, and compatible pointer operations.

It does not implement every implicit conversion or qualifier rule from ISO C.

## Arithmetic

The following arithmetic operators are supported:

```c
a + b
a - b
a * b
a / b
a % b
```

Cinder reports runtime errors for:

- Division by zero
- Modulo by zero
- Invalid arithmetic operations
- Supported signed-overflow cases detected by the VM

## Comparisons

The following comparison operators are supported:

```c
a == b
a != b
a < b
a <= b
a > b
a >= b
```

Comparison expressions return `0` or `1`.

## Logical expressions

Logical operators are supported:

```c
a && b
a || b
!a
```

Logical AND and logical OR use short-circuit evaluation.

For example, the right operand is not evaluated when the left operand already determines the result.

## Bitwise expressions

Bitwise operators are supported:

```c
a & b
a | b
a ^ b
~a
a << count
a >> count
```

Invalid shift counts produce a runtime diagnostic.

## Assignment

Simple assignment is supported:

```c
value = 10;
```

Compound assignment operators are supported:

```c
value += 2;
value -= 2;
value *= 2;
value /= 2;
value %= 2;
value &= mask;
value |= mask;
value ^= mask;
value <<= count;
value >>= count;
```

Assignments are expressions and produce the stored value.

The left side of an assignment must identify writable storage.

## Increment and decrement

Prefix operations are supported:

```c
++value;
--value;
```

Postfix operations are supported:

```c
value++;
value--;
```

Prefix operations produce the updated value.

Postfix operations produce the previous value.

## Conditional expressions

Conditional expressions are supported:

```c
int largest =
    first > second
        ? first
        : second;
```

Only the selected expression is evaluated.

## Comma expressions

Comma expressions are supported where allowed by Cinder’s grammar:

```c
value = (first = 10, second = 20);
```

Expressions are evaluated from left to right, and the final expression supplies the result.

## Conditions

`if` statements are supported:

```c
if (condition) {
    print(1);
}
```

`if...else` statements are supported:

```c
if (condition) {
    print(1);
} else {
    print(0);
}
```

Zero is false.

Any nonzero value is true.

## `while`

```c
while (condition) {
    statement;
}
```

The condition is evaluated before every iteration.

## `do...while`

```c
do {
    statement;
} while (condition);
```

The body executes at least once.

## `for`

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

## `break` and `continue`

`break` exits the closest loop or `switch` statement:

```c
while (1) {
    break;
}
```

`continue` begins the next iteration of the closest loop:

```c
for (
    int index = 0;
    index < 10;
    index++
) {
    if (index == 5) {
        continue;
    }

    print(index);
}
```

Using these statements outside their valid context is a compiler error.

## `switch`

`switch`, `case`, and `default` are supported:

```c
int classify(int value)
{
    switch (value) {
        case 0:
            return 100;

        case 1:
        case 2:
            return 200;

        default:
            return -1;
    }
}
```

Cinder supports:

- Integer switch expressions
- Enumeration values
- Multiple labels sharing a statement
- `default`
- Fallthrough
- `break`

Case values must be compile-time integer values supported by Cinder.

Duplicate case labels are rejected.

Only one `default` label is allowed in a `switch`.

## Functions

Function definitions are supported:

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

- Integer parameters
- Character parameters
- Pointer parameters
- Structure pointers
- Array-parameter syntax
- Function calls
- Nested calls
- Recursion
- Return values
- Argument-count checking
- Basic argument-type checking

Example:

```c
int factorial(int number)
{
    if (number < 2) {
        return 1;
    }

    return
        number *
        factorial(number - 1);
}
```

Function overloading is not supported.

Function prototypes without definitions are not currently supported.

Function pointers are not supported.

User-defined variadic functions are not supported.

## `return`

A function returning a value uses:

```c
return expression;
```

A `void` function can use:

```c
return;
```

Returning an incompatible value produces a diagnostic where Cinder can identify the mismatch.

## `print`

`print` is a Cinder convenience function that prints one integer followed by a newline:

```c
print(42);
```

It is provided for small demonstrations and compiler tests.

## `printf`

Cinder implements a bounded built-in form of `printf`.

Example:

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
| `%u` | Unsigned decimal integer |
| `%x` | Lowercase hexadecimal |
| `%X` | Uppercase hexadecimal |
| `%o` | Octal |
| `%c` | Character |
| `%s` | Null-terminated string in VM memory |
| `%%` | Literal percent sign |

Cinder checks the number of format arguments.

Invalid string addresses and unterminated strings are rejected during execution.

Width, precision, flags, and length modifiers are not currently supported.

## `getchar`

`getchar` reads the next byte from Cinder’s standard-input buffer:

```c
int character =
    getchar();
```

When the input has been exhausted, `getchar` returns `-1`.

Example:

```c
int main()
{
    int character;

    character = getchar();

    while (
        character != -1 &&
        character != '\n'
    ) {
        printf("%c", character);
        character = getchar();
    }

    printf("\n");

    return 0;
}
```

The native executable accepts an optional input-file argument:

```powershell
.\build\cinder.exe `
    .\program.c `
    .\input.txt
```

The WebAssembly module also exposes a separate standard-input buffer. The browser workbench provides access to this buffer when its standard-input panel is enabled.

## Operator precedence

From lowest precedence to highest:

1. Comma
2. Assignment
3. Conditional `?:`
4. Logical OR `||`
5. Logical AND `&&`
6. Bitwise OR `|`
7. Bitwise XOR `^`
8. Bitwise AND `&`
9. Equality `== !=`
10. Relational `< <= > >=`
11. Shifts `<< >>`
12. Addition and subtraction
13. Multiplication, division, and modulo
14. Cast and unary operators
15. Postfix expressions
16. Primary expressions

Parentheses may be used to control evaluation:

```c
result =
    (first + second) *
    third;
```

## Evaluation model

Cinder compiles source into stack-based bytecode.

The bytecode runs inside a bounded virtual machine rather than executing as native machine code.

Function arguments are evaluated from left to right by the current implementation.

Programs are stopped if they exceed the instruction limit.

## Compiler limits

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
| Generated instructions | 49,152 |
| VM value stack | 16,384 values |
| Function-call depth | 256 |
| Executed instructions | 1,000,000 |
| Program output | 16 KB |
| Virtual memory | 2 MB |
| Diagnostics | 128 |

The browser additionally applies a time limit to worker requests.

## Unsupported C features

Cinder currently does not support:

- Complete ISO C compatibility
- Preprocessor directives
- `#include`
- Macros
- Conditional compilation
- `typedef`
- Unions
- Bit-fields
- Flexible array members
- Variable-length arrays
- Designated initializers
- Compound literals
- `_Bool`
- `_Complex`
- Floating-point types
- `long long`
- Function prototypes without definitions
- Function pointers
- Variadic user-defined functions
- Complete storage-class semantics
- Complete qualifier-conversion rules
- Complete integer-promotion rules
- Multiple translation units
- Header files
- Dynamic linking
- Dynamic memory allocation
- The complete C standard library
- Filesystem APIs
- Network APIs
- Process APIs
- Threads
- Native machine-code generation

Programs requiring these features should be compiled using a complete C implementation such as GCC or Clang.
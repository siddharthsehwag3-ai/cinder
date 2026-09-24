#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifndef __wasm__
#include <stdio.h>
#endif

#define MAX_SOURCE 65536
#define MAX_STDIN 65536
#define MAX_TOKENS 16384
#define MAX_NODES 24576
#define MAX_TYPES 4096
#define MAX_SYMBOLS 8192
#define MAX_FUNCTIONS 256
#define MAX_STRUCTS 256
#define MAX_MEMBERS 4096
#define MAX_ENUMS 256
#define MAX_ENUM_VALUES 2048
#define MAX_CODE 49152
#define MAX_STACK 16384
#define MAX_CALL_DEPTH 256
#define MAX_STEPS 1000000
#define MAX_OUTPUT 16384
#define MAX_REPORT 6000000
#define MAX_DIAGNOSTICS 128
#define MAX_JUMP_MARKERS 2048
#define MAX_SWITCH_CASES 2048
#define VM_MEMORY_SIZE 2097152

#define BUILTIN_PRINTF (-10)
#define BUILTIN_PRINT (-11)
#define BUILTIN_GETCHAR (-12)

enum TokenKind {
    TOKEN_EOF = 256,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_CHARACTER,
    TOKEN_STRING,

    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,

    TOKEN_SHIFT_LEFT,
    TOKEN_SHIFT_RIGHT,

    TOKEN_LOGICAL_AND,
    TOKEN_LOGICAL_OR,

    TOKEN_INCREMENT,
    TOKEN_DECREMENT,

    TOKEN_ADD_ASSIGN,
    TOKEN_SUBTRACT_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,

    TOKEN_AND_ASSIGN,
    TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN,
    TOKEN_SHIFT_LEFT_ASSIGN,
    TOKEN_SHIFT_RIGHT_ASSIGN,

    TOKEN_ARROW
};

enum TypeKind {
    TYPE_INVALID,
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_SIGNED_CHAR,
    TYPE_UNSIGNED_CHAR,
    TYPE_SHORT,
    TYPE_UNSIGNED_SHORT,
    TYPE_INT,
    TYPE_UNSIGNED_INT,
    TYPE_LONG,
    TYPE_UNSIGNED_LONG,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_FUNCTION
};

enum StorageKind {
    STORAGE_NONE,
    STORAGE_LOCAL,
    STORAGE_GLOBAL,
    STORAGE_FUNCTION,
    STORAGE_ENUM_CONSTANT
};

enum NodeKind {
    NODE_NONE,
    NODE_INTEGER,
    NODE_STRING,
    NODE_VARIABLE,
    NODE_BINARY,
    NODE_UNARY,
    NODE_ASSIGNMENT,
    NODE_CALL,
    NODE_ARGUMENT,
    NODE_BLOCK,
    NODE_LINK,
    NODE_DECLARATION,
    NODE_RETURN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_EXPRESSION,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_POSTFIX,
    NODE_PREFIX,
    NODE_CONDITIONAL,
    NODE_DO_WHILE,
    NODE_INDEX,
    NODE_ADDRESS,
    NODE_DEREFERENCE,
    NODE_CAST,
    NODE_SIZEOF,
    NODE_MEMBER,
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT,
    NODE_INITIALIZER,
    NODE_COMMA
};

enum Opcode {
    OP_PUSH,

    OP_ADDRESS_LOCAL,
    OP_ADDRESS_GLOBAL,

    OP_LOAD8,
    OP_LOAD16,
    OP_LOAD32,

    OP_STORE8,
    OP_STORE16,
    OP_STORE32,

    OP_COPY_MEMORY,
    OP_CLEAR_MEMORY,

    OP_DROP,
    OP_DUP,
    OP_SWAP,

    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,

    OP_BITWISE_AND,
    OP_BITWISE_OR,
    OP_BITWISE_XOR,
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,

    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,

    OP_NOT,
    OP_NEGATE,
    OP_BITWISE_NOT,
    OP_BOOLEAN,

    OP_CONVERT_SIGNED8,
    OP_CONVERT_UNSIGNED8,
    OP_CONVERT_SIGNED16,
    OP_CONVERT_UNSIGNED16,

    OP_JUMP,
    OP_JUMP_IF_ZERO,
    OP_JUMP_IF_NOT_ZERO,

    OP_CALL,
    OP_RETURN,

    OP_PRINT,
    OP_READ_STDIN,
    OP_HALT
};

enum DiagnosticKind {
    DIAGNOSTIC_ERROR,
    DIAGNOSTIC_WARNING
};

typedef struct {
    int kind;
    int start;
    int end;
    int64_t value;
} Token;

typedef struct {
    int kind;
    int base;
    int array_length;
    int structure;
    int return_type;
    int first_parameter;
    int parameter_count;
    int size;
    int alignment;
    int is_const;
    int is_unsigned;
} Type;

typedef struct {
    int name_token;
    int type;
    int offset;
    int next;
} Member;

typedef struct {
    int name_token;
    int first_member;
    int last_member;
    int member_count;
    int size;
    int alignment;
    int is_complete;
} Structure;

typedef struct {
    int name_token;
    int value;
} EnumValue;

typedef struct {
    int name_token;
    int first_value;
    int value_count;
    int is_complete;
} Enumeration;

typedef struct {
    int kind;
    int type;
    int a;
    int b;
    int c;
    int d;
    int64_t value;
    int token;
    int symbol;
} Node;

typedef struct {
    int name_token;
    int type;
    int storage;
    int address;
    int depth;
    int is_parameter;
    int is_defined;
    int is_active;
} Symbol;

typedef struct {
    int name_token;
    int type;
    int body;
    int first_parameter;
    int parameter_count;
    int local_count;
    int frame_size;
    int entry;
    int is_defined;
} Function;

typedef struct {
    int opcode;
    int argument;
    int extra;
    int node;
} Instruction;

typedef struct {
    int kind;
    int token;
    int start;
    int end;
    int line;
    int column;
    char message[256];
} Diagnostic;

typedef struct {
    int instruction;
    int depth;
} JumpMarker;

typedef struct {
    int value;
    int target;
    int token;
    int is_default;
} SwitchCase;

typedef struct {
    int name_token;
    int type;
} ParsedDeclarator;

static char source[MAX_SOURCE];
static char stdin_buffer[MAX_STDIN];

static Token tokens[MAX_TOKENS];
static Node nodes[MAX_NODES];
static Type types[MAX_TYPES];
static Symbol symbols[MAX_SYMBOLS];
static Function functions[MAX_FUNCTIONS];
static Structure structures[MAX_STRUCTS];
static Member members[MAX_MEMBERS];
static Enumeration enumerations[MAX_ENUMS];
static EnumValue enum_values[MAX_ENUM_VALUES];

static Instruction instructions[MAX_CODE];
static Diagnostic diagnostics[MAX_DIAGNOSTICS];

static JumpMarker break_markers[MAX_JUMP_MARKERS];
static JumpMarker continue_markers[MAX_JUMP_MARKERS];
static SwitchCase switch_cases[MAX_SWITCH_CASES];

static unsigned char initial_memory[VM_MEMORY_SIZE];
static unsigned char vm_memory[VM_MEMORY_SIZE];

static int64_t value_stack[MAX_STACK];
static int return_addresses[MAX_CALL_DEPTH];
static int frame_bases[MAX_CALL_DEPTH];
static int stack_bases[MAX_CALL_DEPTH];

static char program_output[MAX_OUTPUT];
static char report[MAX_REPORT];

static int token_count;
static int node_count;
static int type_count;
static int symbol_count;
static int global_symbol_count;
static int function_count;
static int structure_count;
static int member_count;
static int enumeration_count;
static int enum_value_count;
static int instruction_count;
static int diagnostic_count;

static int parser_position;
static int scope_depth;
static int current_function;
static int nesting_depth;
static int loop_depth;
static int switch_depth;

static int global_memory_top;
static int stack_memory_top;

static int stack_pointer;
static int frame_pointer;
static int output_position;
static int report_position;
static int stdin_length;
static int stdin_position;
static int execution_steps;
static int program_result;

static int compilation_failed;

static int global_declaration_first;
static int global_declaration_last;

static int break_marker_count;
static int continue_marker_count;
static int switch_case_count;
static int current_switch_case_start;

static int type_void;
static int type_char;
static int type_signed_char;
static int type_unsigned_char;
static int type_short;
static int type_unsigned_short;
static int type_int;
static int type_unsigned_int;
static int type_long;
static int type_unsigned_long;
static int text_length(
    const char *text
)
{
    int length = 0;

    while (text[length]) {
        length++;
    }

    return length;
}

static void copy_text(
    char *destination,
    int capacity,
    const char *text
)
{
    int index = 0;

    if (capacity <= 0) {
        return;
    }

    while (
        text[index] &&
        index < capacity - 1
    ) {
        destination[index] =
            text[index];

        index++;
    }

    destination[index] = '\0';
}

static void copy_bytes(
    unsigned char *destination,
    const unsigned char *input,
    int count
)
{
    int index;

    for (
        index = 0;
        index < count;
        index++
    ) {
        destination[index] =
            input[index];
    }
}

static void clear_bytes(
    unsigned char *destination,
    int count
)
{
    int index;

    for (
        index = 0;
        index < count;
        index++
    ) {
        destination[index] = 0;
    }
}

static int is_digit(
    char character
)
{
    return
        character >= '0' &&
        character <= '9';
}

static int is_hex_digit(
    char character
)
{
    return
        is_digit(character) ||
        (
            character >= 'a' &&
            character <= 'f'
        ) ||
        (
            character >= 'A' &&
            character <= 'F'
        );
}

static int hex_value(
    char character
)
{
    if (is_digit(character)) {
        return character - '0';
    }

    if (
        character >= 'a' &&
        character <= 'f'
    ) {
        return
            character - 'a' + 10;
    }

    return
        character - 'A' + 10;
}

static int is_alpha(
    char character
)
{
    return
        (
            character >= 'a' &&
            character <= 'z'
        ) ||
        (
            character >= 'A' &&
            character <= 'Z'
        ) ||
        character == '_';
}

static int is_alphanumeric(
    char character
)
{
    return
        is_alpha(character) ||
        is_digit(character);
}

static int align_to(
    int value,
    int alignment
)
{
    int remainder;

    if (alignment <= 1) {
        return value;
    }

    remainder =
        value % alignment;

    if (!remainder) {
        return value;
    }

    return
        value +
        alignment -
        remainder;
}

static int token_length(
    int token_index
)
{
    if (
        token_index < 0 ||
        token_index >= token_count
    ) {
        return 0;
    }

    return
        tokens[token_index].end -
        tokens[token_index].start;
}

static int token_has_text(
    int token_index,
    const char *text
)
{
    int index = 0;
    int length;

    if (
        token_index < 0 ||
        token_index >= token_count
    ) {
        return 0;
    }

    length =
        token_length(token_index);

    while (
        index < length &&
        text[index] &&
        source[
            tokens[token_index].start +
            index
        ] == text[index]
    ) {
        index++;
    }

    return
        index == length &&
        text[index] == '\0';
}

static int tokens_have_same_text(
    int left_token,
    int right_token
)
{
    int left_length;
    int right_length;
    int index;

    if (
        left_token < 0 ||
        left_token >= token_count ||
        right_token < 0 ||
        right_token >= token_count
    ) {
        return 0;
    }

    left_length =
        token_length(left_token);

    right_length =
        token_length(right_token);

    if (left_length != right_length) {
        return 0;
    }

    for (
        index = 0;
        index < left_length;
        index++
    ) {
        if (
            source[
                tokens[left_token].start +
                index
            ] !=
            source[
                tokens[right_token].start +
                index
            ]
        ) {
            return 0;
        }
    }

    return 1;
}

static int line_for_offset(
    int offset
)
{
    int line = 1;
    int index;

    if (offset < 0) {
        return 1;
    }

    for (
        index = 0;
        index < offset &&
        source[index];
        index++
    ) {
        if (source[index] == '\n') {
            line++;
        }
    }

    return line;
}

static int column_for_offset(
    int offset
)
{
    int column = 1;
    int index = offset - 1;

    while (
        index >= 0 &&
        source[index] != '\n'
    ) {
        column++;
        index--;
    }

    return column;
}

static void add_diagnostic(
    int kind,
    int token_index,
    const char *message
)
{
    Diagnostic *diagnostic;
    int safe_token =
        token_index;

    if (
        safe_token < 0 ||
        safe_token >= token_count
    ) {
        safe_token =
            token_count > 0
                ? token_count - 1
                : 0;
    }

    if (
        diagnostic_count >=
        MAX_DIAGNOSTICS
    ) {
        compilation_failed = 1;
        return;
    }

    diagnostic =
        &diagnostics[
            diagnostic_count
        ];

    diagnostic_count++;

    diagnostic->kind = kind;
    diagnostic->token = safe_token;

    if (token_count > 0) {
        diagnostic->start =
            tokens[safe_token].start;

        diagnostic->end =
            tokens[safe_token].end;
    } else {
        diagnostic->start = 0;
        diagnostic->end = 0;
    }

    diagnostic->line =
        line_for_offset(
            diagnostic->start
        );

    diagnostic->column =
        column_for_offset(
            diagnostic->start
        );

    copy_text(
        diagnostic->message,
        (int)sizeof(
            diagnostic->message
        ),
        message
    );

    if (
        kind ==
        DIAGNOSTIC_ERROR
    ) {
        compilation_failed = 1;
    }
}

static void fail(
    const char *message,
    int token_index
)
{
    add_diagnostic(
        DIAGNOSTIC_ERROR,
        token_index,
        message
    );
}

static void warn(
    const char *message,
    int token_index
)
{
    add_diagnostic(
        DIAGNOSTIC_WARNING,
        token_index,
        message
    );
}

static void add_token(
    int kind,
    int start,
    int end,
    int64_t value
)
{
    if (
        token_count >=
        MAX_TOKENS
    ) {
        if (!compilation_failed) {
            fail(
                "The source contains too many tokens.",
                token_count > 0
                    ? token_count - 1
                    : 0
            );
        }

        return;
    }

    tokens[token_count].kind =
        kind;

    tokens[token_count].start =
        start;

    tokens[token_count].end =
        end;

    tokens[token_count].value =
        value;

    token_count++;
}

static int is_keyword_token(
    int token_index
)
{
    return
        token_has_text(
            token_index,
            "void"
        ) ||
        token_has_text(
            token_index,
            "char"
        ) ||
        token_has_text(
            token_index,
            "short"
        ) ||
        token_has_text(
            token_index,
            "int"
        ) ||
        token_has_text(
            token_index,
            "long"
        ) ||
        token_has_text(
            token_index,
            "signed"
        ) ||
        token_has_text(
            token_index,
            "unsigned"
        ) ||
        token_has_text(
            token_index,
            "const"
        ) ||
        token_has_text(
            token_index,
            "struct"
        ) ||
        token_has_text(
            token_index,
            "enum"
        ) ||
        token_has_text(
            token_index,
            "sizeof"
        ) ||
        token_has_text(
            token_index,
            "return"
        ) ||
        token_has_text(
            token_index,
            "if"
        ) ||
        token_has_text(
            token_index,
            "else"
        ) ||
        token_has_text(
            token_index,
            "while"
        ) ||
        token_has_text(
            token_index,
            "do"
        ) ||
        token_has_text(
            token_index,
            "for"
        ) ||
        token_has_text(
            token_index,
            "switch"
        ) ||
        token_has_text(
            token_index,
            "case"
        ) ||
        token_has_text(
            token_index,
            "default"
        ) ||
        token_has_text(
            token_index,
            "break"
        ) ||
        token_has_text(
            token_index,
            "continue"
        );
}

static int escaped_character(
    char character,
    int token_index
)
{
    switch (character) {
        case '0':
            return 0;

        case 'a':
            return 7;

        case 'b':
            return 8;

        case 'f':
            return 12;

        case 'n':
            return '\n';

        case 'r':
            return '\r';

        case 't':
            return '\t';

        case 'v':
            return 11;

        case '\\':
            return '\\';

        case '\'':
            return '\'';

        case '"':
            return '"';

        case '?':
            return '?';

        default:
            fail(
                "Unknown escape sequence.",
                token_index
            );

            return 0;
    }
}
static void lex_source(void)
{
    int position = 0;

    while (
        source[position] &&
        !compilation_failed
    ) {
        int start = position;
        char character =
            source[position++];

        if (
            character == ' ' ||
            character == '\t' ||
            character == '\r' ||
            character == '\n'
        ) {
            continue;
        }

        if (
            character == '/' &&
            source[position] == '/'
        ) {
            while (
                source[position] &&
                source[position] != '\n'
            ) {
                position++;
            }

            continue;
        }

        if (
            character == '/' &&
            source[position] == '*'
        ) {
            int closed = 0;

            position++;

            while (source[position]) {
                if (
                    source[position] == '*' &&
                    source[position + 1] == '/'
                ) {
                    position += 2;
                    closed = 1;
                    break;
                }

                position++;
            }

            if (!closed) {
                add_token(
                    '/',
                    start,
                    position,
                    0
                );

                fail(
                    "Unterminated block comment.",
                    token_count - 1
                );
            }

            continue;
        }

        if (is_alpha(character)) {
            while (
                is_alphanumeric(
                    source[position]
                )
            ) {
                position++;
            }

            add_token(
                TOKEN_IDENTIFIER,
                start,
                position,
                0
            );

            continue;
        }

        if (is_digit(character)) {
            uint64_t value = 0;
            int base = 10;
            int digit_count = 0;
            int invalid = 0;
            int saw_unsigned = 0;
            int saw_long = 0;

            position = start;

            if (
                source[position] == '0' &&
                (
                    source[position + 1] == 'x' ||
                    source[position + 1] == 'X'
                )
            ) {
                base = 16;
                position += 2;

                while (
                    is_hex_digit(
                        source[position]
                    )
                ) {
                    value =
                        value * 16u +
                        (uint64_t)hex_value(
                            source[position]
                        );

                    position++;
                    digit_count++;

                    if (value > UINT32_MAX) {
                        invalid = 1;
                    }
                }

                if (!digit_count) {
                    add_token(
                        TOKEN_NUMBER,
                        start,
                        position,
                        0
                    );

                    fail(
                        "Expected hexadecimal digits after 0x.",
                        token_count - 1
                    );

                    continue;
                }
            } else if (
                source[position] == '0'
            ) {
                base = 8;
                value = 0;
                position++;
                digit_count = 1;

                while (
                    source[position] >= '0' &&
                    source[position] <= '9'
                ) {
                    if (
                        source[position] > '7'
                    ) {
                        invalid = 1;
                    } else {
                        value =
                            value * 8u +
                            (uint64_t)(
                                source[position] -
                                '0'
                            );
                    }

                    position++;
                    digit_count++;

                    if (value > UINT32_MAX) {
                        invalid = 1;
                    }
                }
            } else {
                while (
                    is_digit(
                        source[position]
                    )
                ) {
                    value =
                        value * 10u +
                        (uint64_t)(
                            source[position] -
                            '0'
                        );

                    position++;
                    digit_count++;

                    if (value > UINT32_MAX) {
                        invalid = 1;
                    }
                }
            }

            while (
                source[position] == 'u' ||
                source[position] == 'U' ||
                source[position] == 'l' ||
                source[position] == 'L'
            ) {
                if (
                    source[position] == 'u' ||
                    source[position] == 'U'
                ) {
                    if (saw_unsigned) {
                        invalid = 1;
                    }

                    saw_unsigned = 1;
                } else {
                    if (saw_long) {
                        invalid = 1;
                    }

                    saw_long = 1;
                }

                position++;
            }

            if (
                is_alpha(
                    source[position]
                )
            ) {
                while (
                    is_alphanumeric(
                        source[position]
                    )
                ) {
                    position++;
                }

                invalid = 1;
            }

            add_token(
                TOKEN_NUMBER,
                start,
                position,
                (int64_t)value
            );

            if (invalid) {
                if (base == 8) {
                    fail(
                        "Invalid octal integer literal or integer suffix.",
                        token_count - 1
                    );
                } else {
                    fail(
                        "Invalid integer literal or value outside the supported 32-bit range.",
                        token_count - 1
                    );
                }
            }

            continue;
        }

        if (character == '\'') {
            int value = 0;
            int valid = 1;

            if (
                !source[position] ||
                source[position] == '\n' ||
                source[position] == '\''
            ) {
                valid = 0;
            } else if (
                source[position] == '\\'
            ) {
                position++;

                if (!source[position]) {
                    valid = 0;
                } else if (
                    source[position] == 'x'
                ) {
                    int digits = 0;

                    position++;

                    while (
                        digits < 2 &&
                        is_hex_digit(
                            source[position]
                        )
                    ) {
                        value =
                            value * 16 +
                            hex_value(
                                source[position]
                            );

                        position++;
                        digits++;
                    }

                    if (!digits) {
                        valid = 0;
                    }
                } else {
                    value =
                        escaped_character(
                            source[position],
                            token_count
                        );

                    position++;
                }
            } else {
                value =
                    (unsigned char)
                        source[position];

                position++;
            }

            if (
                !valid ||
                source[position] != '\''
            ) {
                while (
                    source[position] &&
                    source[position] != '\n' &&
                    source[position] != '\''
                ) {
                    position++;
                }

                if (
                    source[position] == '\''
                ) {
                    position++;
                }

                add_token(
                    TOKEN_CHARACTER,
                    start,
                    position,
                    value
                );

                fail(
                    valid
                        ? "Character literal must contain exactly one character."
                        : "Invalid or unterminated character literal.",
                    token_count - 1
                );

                continue;
            }

            position++;

            add_token(
                TOKEN_CHARACTER,
                start,
                position,
                value
            );

            continue;
        }

        if (character == '"') {
            int closed = 0;

            while (source[position]) {
                if (
                    source[position] == '"'
                ) {
                    position++;
                    closed = 1;
                    break;
                }

                if (
                    source[position] == '\n'
                ) {
                    break;
                }

                if (
                    source[position] == '\\'
                ) {
                    position++;

                    if (!source[position]) {
                        break;
                    }

                    if (
                        source[position] == 'x'
                    ) {
                        int digits = 0;

                        position++;

                        while (
                            digits < 2 &&
                            is_hex_digit(
                                source[position]
                            )
                        ) {
                            position++;
                            digits++;
                        }

                        if (!digits) {
                            add_token(
                                TOKEN_STRING,
                                start,
                                position,
                                0
                            );

                            fail(
                                "Expected hexadecimal digits after \\x in the string.",
                                token_count - 1
                            );

                            break;
                        }

                        continue;
                    }

                    escaped_character(
                        source[position],
                        token_count
                    );

                    position++;
                    continue;
                }

                position++;
            }

            add_token(
                TOKEN_STRING,
                start,
                position,
                0
            );

            if (!closed) {
                fail(
                    "Unterminated string literal.",
                    token_count - 1
                );
            }

            continue;
        }

        {
            int kind = character;

            if (
                character == '=' &&
                source[position] == '='
            ) {
                kind = TOKEN_EQUAL;
                position++;
            } else if (
                character == '!' &&
                source[position] == '='
            ) {
                kind = TOKEN_NOT_EQUAL;
                position++;
            } else if (
                character == '<' &&
                source[position] == '='
            ) {
                kind = TOKEN_LESS_EQUAL;
                position++;
            } else if (
                character == '>' &&
                source[position] == '='
            ) {
                kind =
                    TOKEN_GREATER_EQUAL;

                position++;
            } else if (
                character == '<' &&
                source[position] == '<' &&
                source[position + 1] == '='
            ) {
                kind =
                    TOKEN_SHIFT_LEFT_ASSIGN;

                position += 2;
            } else if (
                character == '>' &&
                source[position] == '>' &&
                source[position + 1] == '='
            ) {
                kind =
                    TOKEN_SHIFT_RIGHT_ASSIGN;

                position += 2;
            } else if (
                character == '<' &&
                source[position] == '<'
            ) {
                kind =
                    TOKEN_SHIFT_LEFT;

                position++;
            } else if (
                character == '>' &&
                source[position] == '>'
            ) {
                kind =
                    TOKEN_SHIFT_RIGHT;

                position++;
            } else if (
                character == '&' &&
                source[position] == '&'
            ) {
                kind =
                    TOKEN_LOGICAL_AND;

                position++;
            } else if (
                character == '|' &&
                source[position] == '|'
            ) {
                kind =
                    TOKEN_LOGICAL_OR;

                position++;
            } else if (
                character == '+' &&
                source[position] == '+'
            ) {
                kind = TOKEN_INCREMENT;
                position++;
            } else if (
                character == '-' &&
                source[position] == '-'
            ) {
                kind = TOKEN_DECREMENT;
                position++;
            } else if (
                character == '-' &&
                source[position] == '>'
            ) {
                kind = TOKEN_ARROW;
                position++;
            } else if (
                character == '+' &&
                source[position] == '='
            ) {
                kind = TOKEN_ADD_ASSIGN;
                position++;
            } else if (
                character == '-' &&
                source[position] == '='
            ) {
                kind =
                    TOKEN_SUBTRACT_ASSIGN;

                position++;
            } else if (
                character == '*' &&
                source[position] == '='
            ) {
                kind =
                    TOKEN_MULTIPLY_ASSIGN;

                position++;
            } else if (
                character == '/' &&
                source[position] == '='
            ) {
                kind =
                    TOKEN_DIVIDE_ASSIGN;

                position++;
            } else if (
                character == '%' &&
                source[position] == '='
            ) {
                kind =
                    TOKEN_MODULO_ASSIGN;

                position++;
            } else if (
                character == '&' &&
                source[position] == '='
            ) {
                kind = TOKEN_AND_ASSIGN;
                position++;
            } else if (
                character == '|' &&
                source[position] == '='
            ) {
                kind = TOKEN_OR_ASSIGN;
                position++;
            } else if (
                character == '^' &&
                source[position] == '='
            ) {
                kind = TOKEN_XOR_ASSIGN;
                position++;
            }

            if (
                kind < 256 &&
                !(
                    character == '+' ||
                    character == '-' ||
                    character == '*' ||
                    character == '/' ||
                    character == '%' ||
                    character == '=' ||
                    character == '!' ||
                    character == '<' ||
                    character == '>' ||
                    character == '&' ||
                    character == '|' ||
                    character == '^' ||
                    character == '~' ||
                    character == '?' ||
                    character == ':' ||
                    character == '(' ||
                    character == ')' ||
                    character == '{' ||
                    character == '}' ||
                    character == '[' ||
                    character == ']' ||
                    character == ';' ||
                    character == ',' ||
                    character == '.'
                )
            ) {
                add_token(
                    kind,
                    start,
                    position,
                    0
                );

                fail(
                    "Unsupported character.",
                    token_count - 1
                );

                continue;
            }

            add_token(
                kind,
                start,
                position,
                0
            );
        }
    }

    add_token(
        TOKEN_EOF,
        position,
        position,
        0
    );
}

static int is_at(
    int kind
)
{
    return
        tokens[parser_position].kind ==
        kind;
}

static int take(
    int kind
)
{
    if (!is_at(kind)) {
        return 0;
    }

    parser_position++;
    return 1;
}

static int is_keyword(
    const char *text
)
{
    return
        is_at(TOKEN_IDENTIFIER) &&
        token_has_text(
            parser_position,
            text
        );
}

static int take_keyword(
    const char *text
)
{
    if (!is_keyword(text)) {
        return 0;
    }

    parser_position++;
    return 1;
}

static void expect(
    int kind,
    const char *message
)
{
    if (!take(kind)) {
        fail(
            message,
            parser_position
        );
    }
}

static void expect_keyword(
    const char *text,
    const char *message
)
{
    if (!take_keyword(text)) {
        fail(
            message,
            parser_position
        );
    }
}

static int token_is_type_keyword(
    int token_index
)
{
    return
        token_has_text(
            token_index,
            "void"
        ) ||
        token_has_text(
            token_index,
            "char"
        ) ||
        token_has_text(
            token_index,
            "short"
        ) ||
        token_has_text(
            token_index,
            "int"
        ) ||
        token_has_text(
            token_index,
            "long"
        ) ||
        token_has_text(
            token_index,
            "signed"
        ) ||
        token_has_text(
            token_index,
            "unsigned"
        ) ||
        token_has_text(
            token_index,
            "const"
        ) ||
        token_has_text(
            token_index,
            "struct"
        ) ||
        token_has_text(
            token_index,
            "enum"
        );
}

static int begins_type_name(void)
{
    return
        is_at(TOKEN_IDENTIFIER) &&
        token_is_type_keyword(
            parser_position
        );
}

static int create_node(
    int kind,
    int type,
    int a,
    int b,
    int c,
    int d,
    int64_t value,
    int token_index
)
{
    Node *node;

    if (
        node_count >=
        MAX_NODES - 1
    ) {
        fail(
            "The syntax tree contains too many nodes.",
            token_index
        );

        return 0;
    }

    node_count++;

    node =
        &nodes[node_count];

    node->kind = kind;
    node->type = type;
    node->a = a;
    node->b = b;
    node->c = c;
    node->d = d;
    node->value = value;
    node->token = token_index;
    node->symbol = -1;

    return node_count;
}

static int create_link(
    int item,
    int token_index
)
{
    return create_node(
        NODE_LINK,
        item
            ? nodes[item].type
            : type_void,
        item,
        0,
        0,
        0,
        0,
        token_index
    );
}

static void append_link(
    int *first,
    int *last,
    int item,
    int token_index
)
{
    int link =
        create_link(
            item,
            token_index
        );

    if (*last) {
        nodes[*last].b = link;
    } else {
        *first = link;
    }

    *last = link;
}
static int create_type(
    int kind,
    int size,
    int alignment,
    int is_unsigned
)
{
    Type *type;

    if (
        type_count >=
        MAX_TYPES
    ) {
        fail(
            "The program contains too many distinct types.",
            parser_position
        );

        return type_int;
    }

    type =
        &types[type_count];

    type->kind = kind;
    type->base = 0;
    type->array_length = 0;
    type->structure = -1;
    type->return_type = 0;
    type->first_parameter = -1;
    type->parameter_count = 0;
    type->size = size;
    type->alignment = alignment;
    type->is_const = 0;
    type->is_unsigned =
        is_unsigned;

    type_count++;

    return type_count - 1;
}

static void initialize_types(void)
{
    type_count = 0;

    type_void =
        create_type(
            TYPE_VOID,
            0,
            1,
            0
        );

    type_char =
        create_type(
            TYPE_CHAR,
            1,
            1,
            0
        );

    type_signed_char =
        create_type(
            TYPE_SIGNED_CHAR,
            1,
            1,
            0
        );

    type_unsigned_char =
        create_type(
            TYPE_UNSIGNED_CHAR,
            1,
            1,
            1
        );

    type_short =
        create_type(
            TYPE_SHORT,
            2,
            2,
            0
        );

    type_unsigned_short =
        create_type(
            TYPE_UNSIGNED_SHORT,
            2,
            2,
            1
        );

    type_int =
        create_type(
            TYPE_INT,
            4,
            4,
            0
        );

    type_unsigned_int =
        create_type(
            TYPE_UNSIGNED_INT,
            4,
            4,
            1
        );

    type_long =
        create_type(
            TYPE_LONG,
            4,
            4,
            0
        );

    type_unsigned_long =
        create_type(
            TYPE_UNSIGNED_LONG,
            4,
            4,
            1
        );
}

static int clone_type(
    int type_index
)
{
    int result;

    if (
        type_index < 0 ||
        type_index >= type_count
    ) {
        fail(
            "Internal invalid type reference.",
            parser_position
        );

        return type_int;
    }

    if (
        type_count >=
        MAX_TYPES
    ) {
        fail(
            "The program contains too many distinct types.",
            parser_position
        );

        return type_int;
    }

    result = type_count;

    types[result] =
        types[type_index];

    type_count++;

    return result;
}

static int const_type(
    int base_type
)
{
    int result;

    if (
        types[base_type].is_const
    ) {
        return base_type;
    }

    result =
        clone_type(base_type);

    types[result].is_const = 1;

    return result;
}

static int unqualified_type(
    int type_index
)
{
    int result;

    if (
        !types[type_index].is_const
    ) {
        return type_index;
    }

    result =
        clone_type(type_index);

    types[result].is_const = 0;

    return result;
}

static int pointer_to(
    int base_type
)
{
    int index;

    for (
        index = 0;
        index < type_count;
        index++
    ) {
        if (
            types[index].kind ==
                TYPE_POINTER &&
            types[index].base ==
                base_type &&
            !types[index].is_const
        ) {
            return index;
        }
    }

    index =
        create_type(
            TYPE_POINTER,
            4,
            4,
            1
        );

    types[index].base =
        base_type;

    return index;
}

static int array_of(
    int base_type,
    int length,
    int token_index
)
{
    int index;
    int64_t size = 0;

    if (length < -1) {
        fail(
            "Invalid array length.",
            token_index
        );

        length = 1;
    }

    if (
        types[base_type].kind ==
        TYPE_VOID
    ) {
        fail(
            "An array cannot contain void elements.",
            token_index
        );

        base_type = type_char;
    }

    if (
        types[base_type].kind ==
            TYPE_FUNCTION
    ) {
        fail(
            "An array cannot contain function elements.",
            token_index
        );

        base_type = pointer_to(
            base_type
        );
    }

    if (length >= 0) {
        size =
            (int64_t)
                types[base_type].size *
            length;

        if (
            size > VM_MEMORY_SIZE ||
            size > INT32_MAX
        ) {
            fail(
                "Array is too large for Cinder virtual memory.",
                token_index
            );

            size = 0;
        }
    }

    index =
        create_type(
            TYPE_ARRAY,
            (int)size,
            types[base_type].alignment,
            0
        );

    types[index].base =
        base_type;

    types[index].array_length =
        length;

    return index;
}

static int complete_array_type(
    int array_type,
    int length,
    int token_index
)
{
    Type *type;
    int64_t size;

    if (
        array_type < 0 ||
        array_type >= type_count ||
        types[array_type].kind !=
            TYPE_ARRAY
    ) {
        fail(
            "Internal incomplete-array error.",
            token_index
        );

        return array_type;
    }

    if (length <= 0) {
        fail(
            "Completed array length must be greater than zero.",
            token_index
        );

        length = 1;
    }

    type = &types[array_type];

    size =
        (int64_t)
            types[type->base].size *
        length;

    if (
        size > VM_MEMORY_SIZE ||
        size > INT32_MAX
    ) {
        fail(
            "Completed array is too large.",
            token_index
        );

        size =
            types[type->base].size;
        length = 1;
    }

    type->array_length = length;
    type->size = (int)size;

    return array_type;
}

static int struct_type(
    int structure_index
)
{
    int index;

    for (
        index = 0;
        index < type_count;
        index++
    ) {
        if (
            types[index].kind ==
                TYPE_STRUCT &&
            types[index].structure ==
                structure_index &&
            !types[index].is_const
        ) {
            types[index].size =
                structures[
                    structure_index
                ].size;

            types[index].alignment =
                structures[
                    structure_index
                ].alignment;

            return index;
        }
    }

    index =
        create_type(
            TYPE_STRUCT,
            structures[
                structure_index
            ].size,
            structures[
                structure_index
            ].alignment,
            0
        );

    types[index].structure =
        structure_index;

    return index;
}

static int enum_type(
    int enumeration_index
)
{
    int index;

    for (
        index = 0;
        index < type_count;
        index++
    ) {
        if (
            types[index].kind ==
                TYPE_ENUM &&
            types[index].structure ==
                enumeration_index &&
            !types[index].is_const
        ) {
            return index;
        }
    }

    index =
        create_type(
            TYPE_ENUM,
            4,
            4,
            0
        );

    types[index].structure =
        enumeration_index;

    return index;
}

static int function_type(
    int return_type,
    int first_parameter,
    int parameter_count
)
{
    int index =
        create_type(
            TYPE_FUNCTION,
            0,
            1,
            0
        );

    types[index].return_type =
        return_type;

    types[index].first_parameter =
        first_parameter;

    types[index].parameter_count =
        parameter_count;

    return index;
}

static int type_size(
    int type_index,
    int token_index
)
{
    Type type;

    if (
        type_index < 0 ||
        type_index >= type_count
    ) {
        fail(
            "Internal invalid type reference.",
            token_index
        );

        return 0;
    }

    type =
        types[type_index];

    if (
        type.kind == TYPE_VOID ||
        type.kind == TYPE_FUNCTION
    ) {
        fail(
            "This type has no object size.",
            token_index
        );

        return 0;
    }

    if (
        type.kind == TYPE_ARRAY &&
        type.array_length < 0
    ) {
        fail(
            "Cannot determine the size of an incomplete array.",
            token_index
        );

        return 0;
    }

    if (
        type.kind == TYPE_STRUCT &&
        !structures[
            type.structure
        ].is_complete
    ) {
        fail(
            "Cannot determine the size of an incomplete structure.",
            token_index
        );

        return 0;
    }

    return type.size;
}

static int is_integer_type(
    int type_index
)
{
    int kind =
        types[type_index].kind;

    return
        kind == TYPE_CHAR ||
        kind == TYPE_SIGNED_CHAR ||
        kind ==
            TYPE_UNSIGNED_CHAR ||
        kind == TYPE_SHORT ||
        kind ==
            TYPE_UNSIGNED_SHORT ||
        kind == TYPE_INT ||
        kind ==
            TYPE_UNSIGNED_INT ||
        kind == TYPE_LONG ||
        kind ==
            TYPE_UNSIGNED_LONG ||
        kind == TYPE_ENUM;
}

static int is_pointer_type(
    int type_index
)
{
    return
        types[type_index].kind ==
        TYPE_POINTER;
}

static int is_array_type(
    int type_index
)
{
    return
        types[type_index].kind ==
        TYPE_ARRAY;
}

static int is_struct_type(
    int type_index
)
{
    return
        types[type_index].kind ==
        TYPE_STRUCT;
}

static int is_function_type(
    int type_index
)
{
    return
        types[type_index].kind ==
        TYPE_FUNCTION;
}

static int is_scalar_type(
    int type_index
)
{
    return
        is_integer_type(
            type_index
        ) ||
        is_pointer_type(
            type_index
        );
}

static int integer_rank(
    int type_index
)
{
    switch (
        types[type_index].kind
    ) {
        case TYPE_CHAR:
        case TYPE_SIGNED_CHAR:
        case TYPE_UNSIGNED_CHAR:
            return 1;

        case TYPE_SHORT:
        case TYPE_UNSIGNED_SHORT:
            return 2;

        case TYPE_INT:
        case TYPE_UNSIGNED_INT:
        case TYPE_ENUM:
            return 3;

        case TYPE_LONG:
        case TYPE_UNSIGNED_LONG:
            return 4;

        default:
            return 0;
    }
}

static int integer_promotion(
    int type_index
)
{
    if (
        !is_integer_type(
            type_index
        )
    ) {
        return type_index;
    }

    if (
        integer_rank(type_index) <
        integer_rank(type_int)
    ) {
        return type_int;
    }

    return type_index;
}

static int usual_arithmetic_type(
    int left_type,
    int right_type
)
{
    int left =
        integer_promotion(
            left_type
        );

    int right =
        integer_promotion(
            right_type
        );

    int left_rank;
    int right_rank;

    if (
        !is_integer_type(left) ||
        !is_integer_type(right)
    ) {
        return type_int;
    }

    left_rank =
        integer_rank(left);

    right_rank =
        integer_rank(right);

    if (left_rank > right_rank) {
        return left;
    }

    if (right_rank > left_rank) {
        return right;
    }

    if (
        types[left].is_unsigned ||
        types[right].is_unsigned
    ) {
        if (left_rank >= 4) {
            return
                type_unsigned_long;
        }

        return
            type_unsigned_int;
    }

    if (left_rank >= 4) {
        return type_long;
    }

    return type_int;
}

static int decay_type(
    int type_index
)
{
    if (
        is_array_type(type_index)
    ) {
        return pointer_to(
            types[type_index].base
        );
    }

    if (
        is_function_type(type_index)
    ) {
        return
            pointer_to(type_index);
    }

    return type_index;
}

static int same_type(
    int left_type,
    int right_type
)
{
    Type left;
    Type right;

    if (left_type == right_type) {
        return 1;
    }

    if (
        left_type < 0 ||
        left_type >= type_count ||
        right_type < 0 ||
        right_type >= type_count
    ) {
        return 0;
    }

    left =
        types[left_type];

    right =
        types[right_type];

    if (
        left.kind != right.kind
    ) {
        return 0;
    }

    switch (left.kind) {
        case TYPE_POINTER:
            return same_type(
                left.base,
                right.base
            );

        case TYPE_ARRAY:
            return
                left.array_length ==
                    right.array_length &&
                same_type(
                    left.base,
                    right.base
                );

        case TYPE_STRUCT:
        case TYPE_ENUM:
            return
                left.structure ==
                right.structure;

        case TYPE_FUNCTION:
            return
                same_type(
                    left.return_type,
                    right.return_type
                ) &&
                left.parameter_count ==
                    right.parameter_count;

        default:
            return
                left.size ==
                    right.size &&
                left.is_unsigned ==
                    right.is_unsigned;
    }
}

static int compatible_assignment(
    int destination_type,
    int source_type
)
{
    int destination =
        decay_type(
            destination_type
        );

    int value =
        decay_type(
            source_type
        );

    if (
        same_type(
            destination,
            value
        )
    ) {
        return 1;
    }

    if (
        is_integer_type(
            destination
        ) &&
        is_integer_type(value)
    ) {
        return 1;
    }

    if (
        is_pointer_type(
            destination
        ) &&
        is_pointer_type(value)
    ) {
        int destination_base =
            types[destination].base;

        int value_base =
            types[value].base;

        return
            types[destination_base]
                .kind ==
                TYPE_VOID ||
            types[value_base].kind ==
                TYPE_VOID ||
            same_type(
                destination_base,
                value_base
            );
    }

    return 0;
}

static int pointer_element_size(
    int pointer_type,
    int token_index
)
{
    int base_type;

    pointer_type =
        decay_type(pointer_type);

    if (
        !is_pointer_type(
            pointer_type
        )
    ) {
        fail(
            "Pointer operation requires a pointer operand.",
            token_index
        );

        return 1;
    }

    base_type =
        types[pointer_type].base;

    if (
        types[base_type].kind ==
        TYPE_VOID
    ) {
        fail(
            "Pointer arithmetic on void pointers is not supported.",
            token_index
        );

        return 1;
    }

    return
        type_size(
            base_type,
            token_index
        );
}

static int integer_literal_type(
    int token_index
)
{
    uint64_t value =
        (uint64_t)
            tokens[token_index].value;

    int start =
        tokens[token_index].start;

    int end =
        tokens[token_index].end;

    int saw_unsigned = 0;
    int saw_long = 0;
    int index;

    for (
        index = start;
        index < end;
        index++
    ) {
        if (
            source[index] == 'u' ||
            source[index] == 'U'
        ) {
            saw_unsigned = 1;
        } else if (
            source[index] == 'l' ||
            source[index] == 'L'
        ) {
            saw_long = 1;
        }
    }

    if (
        saw_unsigned &&
        saw_long
    ) {
        return
            type_unsigned_long;
    }

    if (saw_long) {
        return
            value <= INT32_MAX
                ? type_long
                : type_unsigned_long;
    }

    if (saw_unsigned) {
        return
            type_unsigned_int;
    }

    return
        value <= INT32_MAX
            ? type_int
            : type_unsigned_int;
}

static int expression_is_lvalue(
    int node_index
)
{
    if (
        node_index <= 0 ||
        node_index > node_count
    ) {
        return 0;
    }

    return
        nodes[node_index].kind ==
            NODE_VARIABLE ||
        nodes[node_index].kind ==
            NODE_DEREFERENCE ||
        nodes[node_index].kind ==
            NODE_INDEX ||
        nodes[node_index].kind ==
            NODE_MEMBER;
}

static int expression_is_modifiable_lvalue(
    int node_index
)
{
    int type_index;

    if (
        !expression_is_lvalue(
            node_index
        )
    ) {
        return 0;
    }

    type_index =
        nodes[node_index].type;

    if (
        types[type_index].is_const ||
        is_array_type(type_index) ||
        is_function_type(type_index)
    ) {
        return 0;
    }

    return 1;
}

static int require_scalar(
    int node_index,
    const char *message
)
{
    if (
        node_index > 0 &&
        is_scalar_type(
            decay_type(
                nodes[node_index].type
            )
        )
    ) {
        return 1;
    }

    fail(
        message,
        node_index > 0
            ? nodes[node_index].token
            : parser_position
    );

    return 0;
}

static int require_integer(
    int node_index,
    const char *message
)
{
    if (
        node_index > 0 &&
        is_integer_type(
            nodes[node_index].type
        )
    ) {
        return 1;
    }

    fail(
        message,
        node_index > 0
            ? nodes[node_index].token
            : parser_position
    );

    return 0;
}

static int require_modifiable_lvalue(
    int node_index,
    const char *message
)
{
    if (
        expression_is_modifiable_lvalue(
            node_index
        )
    ) {
        return 1;
    }

    fail(
        message,
        node_index > 0
            ? nodes[node_index].token
            : parser_position
    );

    return 0;
}

static int memory_range_is_valid(
    int address,
    int size
)
{
    if (
        address < 0 ||
        size < 0
    ) {
        return 0;
    }

    if (
        address >
        VM_MEMORY_SIZE - size
    ) {
        return 0;
    }

    return 1;
}

static int check_memory_range(
    int address,
    int size,
    int token_index
)
{
    if (
        memory_range_is_valid(
            address,
            size
        )
    ) {
        return 1;
    }

    fail(
        "Invalid virtual-memory access.",
        token_index
    );

    return 0;
}

static uint32_t read_memory_unsigned(
    int address,
    int size,
    int token_index
)
{
    uint32_t value = 0;
    int index;

    if (
        !check_memory_range(
            address,
            size,
            token_index
        )
    ) {
        return 0;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        value |=
            (uint32_t)
                vm_memory[
                    address + index
                ]
            << (index * 8);
    }

    return value;
}

static int32_t read_memory_signed(
    int address,
    int size,
    int token_index
)
{
    uint32_t value =
        read_memory_unsigned(
            address,
            size,
            token_index
        );

    if (size == 1) {
        return (int8_t)value;
    }

    if (size == 2) {
        return (int16_t)value;
    }

    return (int32_t)value;
}

static void write_memory_value(
    int address,
    int size,
    uint32_t value,
    int token_index
)
{
    int index;

    if (
        !check_memory_range(
            address,
            size,
            token_index
        )
    ) {
        return;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        vm_memory[
            address + index
        ] =
            (unsigned char)(
                value & 255u
            );

        value >>= 8;
    }
}

static void copy_memory_range(
    int destination,
    int source_address,
    int size,
    int token_index
)
{
    int index;

    if (
        !check_memory_range(
            destination,
            size,
            token_index
        ) ||
        !check_memory_range(
            source_address,
            size,
            token_index
        )
    ) {
        return;
    }

    if (
        destination <
        source_address
    ) {
        for (
            index = 0;
            index < size;
            index++
        ) {
            vm_memory[
                destination + index
            ] =
                vm_memory[
                    source_address +
                    index
                ];
        }
    } else if (
        destination >
        source_address
    ) {
        for (
            index = size - 1;
            index >= 0;
            index--
        ) {
            vm_memory[
                destination + index
            ] =
                vm_memory[
                    source_address +
                    index
                ];
        }
    }
}

static void clear_memory_range(
    int address,
    int size,
    int token_index
)
{
    int index;

    if (
        !check_memory_range(
            address,
            size,
            token_index
        )
    ) {
        return;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        vm_memory[
            address + index
        ] = 0;
    }
}
static int allocate_global_memory(
    int size,
    int alignment,
    int token_index
)
{
    int address =
        align_to(
            global_memory_top,
            alignment
        );

    int64_t end =
        (int64_t)address +
        size;

    if (
        size < 0 ||
        end > VM_MEMORY_SIZE
    ) {
        fail(
            "Global data exceeds Cinder virtual-memory capacity.",
            token_index
        );

        return 0;
    }

    global_memory_top =
        (int)end;

    return address;
}

static int allocate_local_memory(
    int type_index,
    int token_index
)
{
    int size;
    int alignment;
    int offset;

    if (
        current_function < 0 ||
        current_function >=
            function_count
    ) {
        fail(
            "Internal local-allocation error.",
            token_index
        );

        return 0;
    }

    size =
        type_size(
            type_index,
            token_index
        );

    alignment =
        types[type_index].alignment;

    offset =
        align_to(
            functions[
                current_function
            ].frame_size,
            alignment
        );

    if (
        (int64_t)offset + size >
        VM_MEMORY_SIZE
    ) {
        fail(
            "Function stack frame is too large.",
            token_index
        );

        return 0;
    }

    functions[
        current_function
    ].frame_size =
        offset + size;

    functions[
        current_function
    ].local_count++;

    return offset;
}

static int find_symbol_in_scope(
    int name_token,
    int depth
)
{
    int index;

    for (
        index = symbol_count - 1;
        index >= 0;
        index--
    ) {
        if (
            symbols[index].is_active &&
            symbols[index].depth ==
                depth &&
            tokens_have_same_text(
                symbols[index]
                    .name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int find_symbol(
    int name_token
)
{
    int index;

    for (
        index = symbol_count - 1;
        index >= 0;
        index--
    ) {
        if (
            symbols[index].is_active &&
            tokens_have_same_text(
                symbols[index]
                    .name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int add_symbol(
    int name_token,
    int type_index,
    int storage,
    int depth,
    int is_parameter
)
{
    int existing;
    int address = 0;
    int index;

    existing =
        find_symbol_in_scope(
            name_token,
            depth
        );

    if (existing >= 0) {
        fail(
            "This name is already declared in the current scope.",
            name_token
        );

        return existing;
    }

    if (
        symbol_count >=
        MAX_SYMBOLS
    ) {
        fail(
            "The program contains too many symbols.",
            name_token
        );

        return -1;
    }

    if (
        storage ==
        STORAGE_GLOBAL
    ) {
        address =
            allocate_global_memory(
                type_size(
                    type_index,
                    name_token
                ),
                types[type_index]
                    .alignment,
                name_token
            );
    } else if (
        storage ==
        STORAGE_LOCAL
    ) {
        address =
            allocate_local_memory(
                type_index,
                name_token
            );
    }

    index = symbol_count;
    symbol_count++;

    symbols[index].name_token =
        name_token;

    symbols[index].type =
        type_index;

    symbols[index].storage =
        storage;

    symbols[index].address =
        address;

    symbols[index].depth =
        depth;

    symbols[index].is_parameter =
        is_parameter;

    symbols[index].is_defined =
        storage !=
        STORAGE_FUNCTION;

    symbols[index].is_active = 1;

    return index;
}

static int add_enum_constant(
    int name_token,
    int value
)
{
    int symbol_index =
        add_symbol(
            name_token,
            type_int,
            STORAGE_ENUM_CONSTANT,
            scope_depth,
            0
        );

    if (symbol_index >= 0) {
        symbols[symbol_index].address =
            value;

        symbols[symbol_index].is_defined =
            1;
    }

    return symbol_index;
}

static void leave_scope(
    int previous_symbol_count
)
{
    int index;

    if (
        previous_symbol_count < 0 ||
        previous_symbol_count >
            symbol_count
    ) {
        return;
    }

    for (
        index = previous_symbol_count;
        index < symbol_count;
        index++
    ) {
        if (
            symbols[index].storage !=
                STORAGE_GLOBAL &&
            symbols[index].storage !=
                STORAGE_FUNCTION &&
            symbols[index].storage !=
                STORAGE_ENUM_CONSTANT
        ) {
            symbols[index].is_active = 0;
        }
    }
}

static int find_function(
    int name_token
)
{
    int index;

    for (
        index = 0;
        index < function_count;
        index++
    ) {
        if (
            tokens_have_same_text(
                functions[index]
                    .name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int add_function(
    int name_token,
    int return_type
)
{
    int existing =
        find_function(
            name_token
        );

    int function_index;
    int symbol_index;

    if (existing >= 0) {
        return existing;
    }

    if (
        function_count >=
        MAX_FUNCTIONS
    ) {
        fail(
            "The program contains too many functions.",
            name_token
        );

        return 0;
    }

    function_index =
        function_count;

    function_count++;

    functions[function_index]
        .name_token = name_token;

    functions[function_index].type =
        function_type(
            return_type,
            -1,
            0
        );

    functions[function_index].body =
        0;

    functions[function_index]
        .first_parameter = -1;

    functions[function_index]
        .parameter_count = 0;

    functions[function_index]
        .local_count = 0;

    functions[function_index]
        .frame_size = 0;

    functions[function_index].entry =
        0;

    functions[function_index]
        .is_defined = 0;

    symbol_index =
        add_symbol(
            name_token,
            functions[
                function_index
            ].type,
            STORAGE_FUNCTION,
            0,
            0
        );

    if (symbol_index >= 0) {
        symbols[symbol_index].address =
            function_index;
    }

    return function_index;
}

static int function_for_symbol(
    int symbol_index
)
{
    if (
        symbol_index < 0 ||
        symbol_index >=
            symbol_count
    ) {
        return -1;
    }

    if (
        symbols[symbol_index]
            .storage !=
        STORAGE_FUNCTION
    ) {
        return -1;
    }

    return
        symbols[symbol_index]
            .address;
}

static int find_structure(
    int name_token
)
{
    int index;

    for (
        index = structure_count - 1;
        index >= 0;
        index--
    ) {
        if (
            structures[index]
                .name_token >= 0 &&
            tokens_have_same_text(
                structures[index]
                    .name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int add_structure(
    int name_token
)
{
    int existing;

    if (name_token >= 0) {
        existing =
            find_structure(
                name_token
            );

        if (existing >= 0) {
            return existing;
        }
    }

    if (
        structure_count >=
        MAX_STRUCTS
    ) {
        fail(
            "The program contains too many structure definitions.",
            name_token >= 0
                ? name_token
                : parser_position
        );

        return 0;
    }

    structures[
        structure_count
    ].name_token = name_token;

    structures[
        structure_count
    ].first_member = -1;

    structures[
        structure_count
    ].last_member = -1;

    structures[
        structure_count
    ].member_count = 0;

    structures[
        structure_count
    ].size = 0;

    structures[
        structure_count
    ].alignment = 1;

    structures[
        structure_count
    ].is_complete = 0;

    structure_count++;

    return structure_count - 1;
}

static int find_member(
    int structure_index,
    int name_token
)
{
    int member_index;
    int remaining;

    if (
        structure_index < 0 ||
        structure_index >=
            structure_count
    ) {
        return -1;
    }

    member_index =
        structures[
            structure_index
        ].first_member;

    remaining =
        structures[
            structure_index
        ].member_count;

    while (
        member_index >= 0 &&
        remaining > 0
    ) {
        if (
            tokens_have_same_text(
                members[member_index]
                    .name_token,
                name_token
            )
        ) {
            return member_index;
        }

        member_index =
            members[member_index]
                .next;

        remaining--;
    }

    return -1;
}

static int add_member(
    int structure_index,
    int name_token,
    int type_index
)
{
    Structure *structure;
    int member_index;
    int offset;

    if (
        structure_index < 0 ||
        structure_index >=
            structure_count
    ) {
        fail(
            "Internal invalid structure reference.",
            name_token
        );

        return -1;
    }

    structure =
        &structures[
            structure_index
        ];

    if (
        find_member(
            structure_index,
            name_token
        ) >= 0
    ) {
        fail(
            "This structure already has a member with that name.",
            name_token
        );

        return -1;
    }

    if (
        member_count >=
        MAX_MEMBERS
    ) {
        fail(
            "The program contains too many structure members.",
            name_token
        );

        return -1;
    }

    if (
        types[type_index].kind ==
        TYPE_VOID
    ) {
        fail(
            "A structure member cannot have type void.",
            name_token
        );

        return -1;
    }

    if (
        types[type_index].kind ==
            TYPE_STRUCT &&
        !structures[
            types[type_index]
                .structure
        ].is_complete
    ) {
        fail(
            "A structure cannot contain an incomplete structure by value.",
            name_token
        );

        return -1;
    }

    if (
        types[type_index].kind ==
            TYPE_ARRAY &&
        types[type_index]
            .array_length < 0
    ) {
        fail(
            "A structure member cannot use an incomplete array type.",
            name_token
        );

        return -1;
    }

    offset =
        align_to(
            structure->size,
            types[type_index]
                .alignment
        );

    if (
        (int64_t)offset +
        types[type_index].size >
        VM_MEMORY_SIZE
    ) {
        fail(
            "The structure is too large.",
            name_token
        );

        return -1;
    }

    member_index =
        member_count;

    member_count++;

    members[member_index]
        .name_token = name_token;

    members[member_index].type =
        type_index;

    members[member_index].offset =
        offset;

    members[member_index].next =
        -1;

    if (
        structure->last_member >= 0
    ) {
        members[
            structure->last_member
        ].next = member_index;
    } else {
        structure->first_member =
            member_index;
    }

    structure->last_member =
        member_index;

    structure->member_count++;

    structure->size =
        offset +
        types[type_index].size;

    if (
        types[type_index]
            .alignment >
        structure->alignment
    ) {
        structure->alignment =
            types[type_index]
                .alignment;
    }

    return member_index;
}

static void complete_structure(
    int structure_index,
    int token_index
)
{
    Structure *structure;

    if (
        structure_index < 0 ||
        structure_index >=
            structure_count
    ) {
        fail(
            "Internal invalid structure definition.",
            token_index
        );

        return;
    }

    structure =
        &structures[
            structure_index
        ];

    if (structure->is_complete) {
        fail(
            "This structure is already defined.",
            token_index
        );

        return;
    }

    structure->size =
        align_to(
            structure->size,
            structure->alignment
        );

    structure->is_complete = 1;
}

static int find_enumeration(
    int name_token
)
{
    int index;

    for (
        index = enumeration_count - 1;
        index >= 0;
        index--
    ) {
        if (
            enumerations[index]
                .name_token >= 0 &&
            tokens_have_same_text(
                enumerations[index]
                    .name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int add_enumeration(
    int name_token
)
{
    int existing;

    if (name_token >= 0) {
        existing =
            find_enumeration(
                name_token
            );

        if (existing >= 0) {
            return existing;
        }
    }

    if (
        enumeration_count >=
        MAX_ENUMS
    ) {
        fail(
            "The program contains too many enum definitions.",
            name_token >= 0
                ? name_token
                : parser_position
        );

        return 0;
    }

    enumerations[
        enumeration_count
    ].name_token = name_token;

    enumerations[
        enumeration_count
    ].first_value =
        enum_value_count;

    enumerations[
        enumeration_count
    ].value_count = 0;

    enumerations[
        enumeration_count
    ].is_complete = 0;

    enumeration_count++;

    return enumeration_count - 1;
}

static int add_enumeration_value(
    int enumeration_index,
    int name_token,
    int value
)
{
    Enumeration *enumeration;
    int index;

    if (
        enumeration_index < 0 ||
        enumeration_index >=
            enumeration_count
    ) {
        fail(
            "Internal invalid enum reference.",
            name_token
        );

        return -1;
    }

    enumeration =
        &enumerations[
            enumeration_index
        ];

    for (
        index = 0;
        index <
            enumeration->value_count;
        index++
    ) {
        int value_index =
            enumeration->first_value +
            index;

        if (
            tokens_have_same_text(
                enum_values[
                    value_index
                ].name_token,
                name_token
            )
        ) {
            fail(
                "This enumerator is already defined.",
                name_token
            );

            return value_index;
        }
    }

    if (
        enum_value_count >=
        MAX_ENUM_VALUES
    ) {
        fail(
            "The program contains too many enum constants.",
            name_token
        );

        return -1;
    }

    enum_values[
        enum_value_count
    ].name_token = name_token;

    enum_values[
        enum_value_count
    ].value = value;

    add_enum_constant(
        name_token,
        value
    );

    enumeration->value_count++;

    enum_value_count++;

    return enum_value_count - 1;
}

static int find_member_for_type(
    int type_index,
    int member_token
)
{
    int structure_index;
    int member_index;

    if (
        !is_struct_type(
            type_index
        )
    ) {
        fail(
            "Member access requires a structure object.",
            member_token
        );

        return -1;
    }

    structure_index =
        types[type_index]
            .structure;

    member_index =
        find_member(
            structure_index,
            member_token
        );

    if (member_index < 0) {
        fail(
            "The structure has no member with this name.",
            member_token
        );
    }

    return member_index;
}

static int decoded_string_length(
    int token_index
)
{
    Token token =
        tokens[token_index];

    int position =
        token.start + 1;

    int end =
        token.end - 1;

    int length = 0;

    while (position < end) {
        if (
            source[position] == '\\'
        ) {
            position++;

            if (
                position < end &&
                source[position] == 'x'
            ) {
                int digits = 0;

                position++;

                while (
                    position < end &&
                    digits < 2 &&
                    is_hex_digit(
                        source[position]
                    )
                ) {
                    position++;
                    digits++;
                }

                if (!digits) {
                    fail(
                        "Expected hexadecimal digits after \\x.",
                        token_index
                    );

                    break;
                }
            } else {
                if (position >= end) {
                    fail(
                        "Incomplete string escape.",
                        token_index
                    );

                    break;
                }

                escaped_character(
                    source[position],
                    token_index
                );

                position++;
            }
        } else {
            position++;
        }

        length++;
    }

    return length;
}

static int decode_string_into_memory(
    int token_index,
    int address,
    int capacity,
    unsigned char *memory
)
{
    Token token =
        tokens[token_index];

    int position =
        token.start + 1;

    int end =
        token.end - 1;

    int written = 0;

    if (
        capacity <= 0 ||
        !memory_range_is_valid(
            address,
            capacity
        )
    ) {
        fail(
            "String destination is outside virtual memory.",
            token_index
        );

        return 0;
    }

    while (
        position < end &&
        !compilation_failed
    ) {
        int value;

        if (
            written >=
            capacity - 1
        ) {
            fail(
                "String literal does not fit in its destination.",
                token_index
            );

            break;
        }

        if (
            source[position] == '\\'
        ) {
            position++;

            if (
                source[position] == 'x'
            ) {
                int digits = 0;

                value = 0;
                position++;

                while (
                    position < end &&
                    digits < 2 &&
                    is_hex_digit(
                        source[position]
                    )
                ) {
                    value =
                        value * 16 +
                        hex_value(
                            source[position]
                        );

                    position++;
                    digits++;
                }

                if (!digits) {
                    fail(
                        "Expected hexadecimal digits after \\x.",
                        token_index
                    );

                    break;
                }
            } else {
                value =
                    escaped_character(
                        source[position],
                        token_index
                    );

                position++;
            }
        } else {
            value =
                (unsigned char)
                    source[position];

            position++;
        }

        memory[address + written] =
            (unsigned char)value;

        written++;
    }

    if (written < capacity) {
        memory[address + written] = 0;
    }

    return written + 1;
}

static int store_string_literal(
    int token_index
)
{
    int length =
        decoded_string_length(
            token_index
        ) + 1;

    int address =
        allocate_global_memory(
            length,
            1,
            token_index
        );

    if (!compilation_failed) {
        decode_string_into_memory(
            token_index,
            address,
            length,
            initial_memory
        );
    }

    return address;
}

static int string_length_in_memory(
    int address,
    int token_index
)
{
    int current = address;

    if (
        !memory_range_is_valid(
            address,
            1
        )
    ) {
        fail(
            "String pointer is outside virtual memory.",
            token_index
        );

        return 0;
    }

    while (
        current < VM_MEMORY_SIZE
    ) {
        if (
            vm_memory[current] == 0
        ) {
            return
                current - address;
        }

        current++;
    }

    fail(
        "String is not null terminated inside virtual memory.",
        token_index
    );

    return 0;
}

static void push_value(
    int64_t value,
    int token_index
)
{
    if (
        stack_pointer >=
        MAX_STACK
    ) {
        fail(
            "Virtual-machine value stack exceeded its limit.",
            token_index
        );

        return;
    }

    value_stack[
        stack_pointer
    ] = value;

    stack_pointer++;
}

static int64_t pop_value(
    int token_index
)
{
    if (
        stack_pointer <= 0
    ) {
        fail(
            "Internal virtual-machine stack underflow.",
            token_index
        );

        return 0;
    }

    stack_pointer--;

    return
        value_stack[
            stack_pointer
        ];
}

static int64_t peek_value(
    int token_index
)
{
    if (
        stack_pointer <= 0
    ) {
        fail(
            "Internal virtual-machine stack underflow.",
            token_index
        );

        return 0;
    }

    return
        value_stack[
            stack_pointer - 1
        ];
}

static int parse_expression(void);
static int parse_assignment(void);
static int parse_statement(void);
static int parse_declaration_statement(void);
static int parse_type_name(void);
static int parse_constant_expression(void);
static int parse_declaration_specifiers(void);

static ParsedDeclarator parse_declarator(
    int base_type,
    int allow_abstract,
    int parameter_context
);

static int parse_struct_specifier(void)
{
    int struct_token =
        parser_position - 1;

    int name_token = -1;
    int structure_index;
    int type_index;

    if (
        is_at(TOKEN_IDENTIFIER) &&
        !is_keyword_token(
            parser_position
        )
    ) {
        name_token =
            parser_position;

        parser_position++;
    }

    if (take('{')) {
        if (name_token >= 0) {
            structure_index =
                find_structure(
                    name_token
                );

            if (structure_index < 0) {
                structure_index =
                    add_structure(
                        name_token
                    );
            } else if (
                structures[
                    structure_index
                ].is_complete
            ) {
                fail(
                    "This structure tag is already defined.",
                    name_token
                );
            }
        } else {
            structure_index =
                add_structure(-1);
        }

        while (
            !is_at('}') &&
            !is_at(TOKEN_EOF) &&
            !compilation_failed
        ) {
            int base_type =
                parse_declaration_specifiers();

            if (take(';')) {
                if (
                    types[base_type].kind !=
                        TYPE_STRUCT &&
                    types[base_type].kind !=
                        TYPE_ENUM
                ) {
                    fail(
                        "Structure member declaration does not declare a member.",
                        parser_position - 1
                    );
                }

                continue;
            }

            do {
                ParsedDeclarator declarator =
                    parse_declarator(
                        base_type,
                        0,
                        0
                    );

                if (
                    declarator.name_token >= 0
                ) {
                    add_member(
                        structure_index,
                        declarator.name_token,
                        declarator.type
                    );
                }
            } while (
                take(',') &&
                !compilation_failed
            );

            expect(
                ';',
                "Expected ';' after the structure member declaration."
            );
        }

        expect(
            '}',
            "Expected '}' after the structure definition."
        );

        complete_structure(
            structure_index,
            struct_token
        );
    } else {
        if (name_token < 0) {
            fail(
                "Expected a structure tag or structure definition.",
                struct_token
            );

            structure_index =
                add_structure(-1);
        } else {
            structure_index =
                find_structure(
                    name_token
                );

            if (structure_index < 0) {
                structure_index =
                    add_structure(
                        name_token
                    );
            }
        }
    }

    type_index =
        struct_type(
            structure_index
        );

    types[type_index].size =
        structures[
            structure_index
        ].size;

    types[type_index].alignment =
        structures[
            structure_index
        ].alignment;

    return type_index;
}

static int parse_enum_specifier(void)
{
    int enum_token =
        parser_position - 1;

    int name_token = -1;
    int enumeration_index;
    int next_value = 0;

    if (
        is_at(TOKEN_IDENTIFIER) &&
        !is_keyword_token(
            parser_position
        )
    ) {
        name_token =
            parser_position;

        parser_position++;
    }

    if (take('{')) {
        if (name_token >= 0) {
            enumeration_index =
                find_enumeration(
                    name_token
                );

            if (
                enumeration_index < 0
            ) {
                enumeration_index =
                    add_enumeration(
                        name_token
                    );
            } else if (
                enumerations[
                    enumeration_index
                ].is_complete
            ) {
                fail(
                    "This enum tag is already defined.",
                    name_token
                );
            }
        } else {
            enumeration_index =
                add_enumeration(-1);
        }

        while (
            !is_at('}') &&
            !is_at(TOKEN_EOF) &&
            !compilation_failed
        ) {
            int enumerator_token;
            int value =
                next_value;

            if (
                !is_at(TOKEN_IDENTIFIER) ||
                is_keyword_token(
                    parser_position
                )
            ) {
                fail(
                    "Expected an enumerator name.",
                    parser_position
                );

                break;
            }

            enumerator_token =
                parser_position;

            parser_position++;

            if (take('=')) {
                value =
                    parse_constant_expression();
            }

            add_enumeration_value(
                enumeration_index,
                enumerator_token,
                value
            );

            if (value == INT32_MAX) {
                if (!is_at('}')) {
                    fail(
                        "The next enum value exceeds the signed 32-bit range.",
                        enumerator_token
                    );
                }
            } else {
                next_value =
                    value + 1;
            }

            if (!take(',')) {
                break;
            }

            if (is_at('}')) {
                break;
            }
        }

        expect(
            '}',
            "Expected '}' after the enum definition."
        );

        enumerations[
            enumeration_index
        ].is_complete = 1;
    } else {
        if (name_token < 0) {
            fail(
                "Expected an enum tag or enum definition.",
                enum_token
            );

            enumeration_index =
                add_enumeration(-1);
        } else {
            enumeration_index =
                find_enumeration(
                    name_token
                );

            if (
                enumeration_index < 0
            ) {
                enumeration_index =
                    add_enumeration(
                        name_token
                    );
            }
        }
    }

    return
        enum_type(
            enumeration_index
        );
}

static int parse_declaration_specifiers(void)
{
    int saw_type = 0;
    int saw_const = 0;
    int saw_signed = 0;
    int saw_unsigned = 0;
    int saw_short = 0;
    int saw_long = 0;
    int saw_char = 0;
    int saw_int = 0;
    int saw_void = 0;

    int result = type_int;
    int first_token =
        parser_position;

    while (
        is_at(TOKEN_IDENTIFIER)
    ) {
        if (take_keyword("const")) {
            if (saw_const) {
                warn(
                    "Duplicate const qualifier.",
                    parser_position - 1
                );
            }

            saw_const = 1;
            continue;
        }

        if (take_keyword("signed")) {
            if (saw_signed) {
                warn(
                    "Duplicate signed specifier.",
                    parser_position - 1
                );
            }

            saw_signed = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("unsigned")) {
            if (saw_unsigned) {
                warn(
                    "Duplicate unsigned specifier.",
                    parser_position - 1
                );
            }

            saw_unsigned = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("short")) {
            if (saw_short) {
                warn(
                    "Duplicate short specifier.",
                    parser_position - 1
                );
            }

            saw_short = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("long")) {
            if (saw_long) {
                warn(
                    "Duplicate long specifier.",
                    parser_position - 1
                );
            }

            saw_long = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("char")) {
            if (saw_char) {
                warn(
                    "Duplicate char specifier.",
                    parser_position - 1
                );
            }

            saw_char = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("int")) {
            if (saw_int) {
                warn(
                    "Duplicate int specifier.",
                    parser_position - 1
                );
            }

            saw_int = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("void")) {
            if (saw_void) {
                warn(
                    "Duplicate void specifier.",
                    parser_position - 1
                );
            }

            saw_void = 1;
            saw_type = 1;
            continue;
        }

        if (take_keyword("struct")) {
            if (saw_type) {
                fail(
                    "A struct specifier cannot be combined with another base type.",
                    parser_position - 1
                );
            }

            result =
                parse_struct_specifier();

            saw_type = 1;

            if (saw_const) {
                result =
                    const_type(result);
            }

            return result;
        }

        if (take_keyword("enum")) {
            if (saw_type) {
                fail(
                    "An enum specifier cannot be combined with another base type.",
                    parser_position - 1
                );
            }

            result =
                parse_enum_specifier();

            saw_type = 1;

            if (saw_const) {
                result =
                    const_type(result);
            }

            return result;
        }

        break;
    }

    if (!saw_type) {
        fail(
            "Expected a type specifier.",
            first_token
        );

        return type_int;
    }

    if (
        saw_signed &&
        saw_unsigned
    ) {
        fail(
            "A type cannot be both signed and unsigned.",
            first_token
        );
    }

    if (
        saw_short &&
        saw_long
    ) {
        fail(
            "A type cannot be both short and long.",
            first_token
        );
    }

    if (
        saw_void &&
        (
            saw_signed ||
            saw_unsigned ||
            saw_short ||
            saw_long ||
            saw_char ||
            saw_int
        )
    ) {
        fail(
            "void cannot be combined with integer type specifiers.",
            first_token
        );
    }

    if (
        saw_char &&
        (
            saw_short ||
            saw_long ||
            saw_int
        )
    ) {
        fail(
            "char cannot be combined with short, int, or long.",
            first_token
        );
    }

    if (saw_void) {
        result = type_void;
    } else if (saw_char) {
        if (saw_unsigned) {
            result =
                type_unsigned_char;
        } else if (saw_signed) {
            result =
                type_signed_char;
        } else {
            result = type_char;
        }
    } else if (saw_short) {
        result =
            saw_unsigned
                ? type_unsigned_short
                : type_short;
    } else if (saw_long) {
        result =
            saw_unsigned
                ? type_unsigned_long
                : type_long;
    } else {
        result =
            saw_unsigned
                ? type_unsigned_int
                : type_int;
    }

    if (saw_const) {
        result =
            const_type(result);
    }

    return result;
}

static ParsedDeclarator parse_declarator(
    int base_type,
    int allow_abstract,
    int parameter_context
)
{
    ParsedDeclarator result;

    int pointer_const[64];
    int pointer_count = 0;

    int array_lengths[64];
    int array_tokens[64];
    int array_count = 0;

    int index;

    result.name_token = -1;
    result.type = base_type;

    while (take('*')) {
        if (pointer_count >= 64) {
            fail(
                "Pointer nesting exceeds 64 levels.",
                parser_position - 1
            );

            break;
        }

        pointer_const[
            pointer_count
        ] =
            take_keyword("const");

        pointer_count++;
    }

    if (
        is_at(TOKEN_IDENTIFIER) &&
        !is_keyword_token(
            parser_position
        )
    ) {
        result.name_token =
            parser_position;

        parser_position++;
    } else if (!allow_abstract) {
        fail(
            "Expected a declarator name.",
            parser_position
        );
    }

    while (take('[')) {
        int length = -1;
        int bracket_token =
            parser_position - 1;

        if (array_count >= 64) {
            fail(
                "Array nesting exceeds 64 dimensions.",
                bracket_token
            );

            break;
        }

        if (!is_at(']')) {
            if (
                !is_at(TOKEN_NUMBER)
            ) {
                fail(
                    "Array length must be an integer constant.",
                    parser_position
                );

                length = 1;
            } else {
                uint64_t raw_length =
                    (uint64_t)
                        tokens[
                            parser_position
                        ].value;

                if (
                    raw_length >
                    INT32_MAX
                ) {
                    fail(
                        "Array length is too large.",
                        parser_position
                    );

                    length = 1;
                } else {
                    length =
                        (int)raw_length;
                }

                parser_position++;
            }
        }

        expect(
            ']',
            "Expected ']' after the array declarator."
        );

        if (length == 0) {
            fail(
                "Array length must be greater than zero.",
                bracket_token
            );

            length = 1;
        }

        array_lengths[
            array_count
        ] = length;

        array_tokens[
            array_count
        ] = bracket_token;

        array_count++;
    }

    for (
        index = 0;
        index < pointer_count;
        index++
    ) {
        result.type =
            pointer_to(
                result.type
            );

        if (
            pointer_const[index]
        ) {
            result.type =
                const_type(
                    result.type
                );
        }
    }

    for (
        index = array_count - 1;
        index >= 0;
        index--
    ) {
        int length =
            array_lengths[index];

        if (
            parameter_context &&
            index == 0
        ) {
            result.type =
                pointer_to(
                    result.type
                );

            continue;
        }

        result.type =
            array_of(
                result.type,
                length,
                array_tokens[index]
            );
    }

    return result;
}

static int parse_type_name(void)
{
    int base_type =
        parse_declaration_specifiers();

    ParsedDeclarator declarator =
        parse_declarator(
            base_type,
            1,
            0
        );

    if (
        declarator.name_token >= 0
    ) {
        fail(
            "A type name cannot contain an identifier.",
            declarator.name_token
        );
    }

    return declarator.type;
}

static int constant_from_symbol(
    int symbol_index,
    int token_index
)
{
    if (
        symbol_index < 0 ||
        symbol_index >=
            symbol_count
    ) {
        fail(
            "Unknown constant name.",
            token_index
        );

        return 0;
    }

    if (
        symbols[symbol_index]
            .storage !=
        STORAGE_ENUM_CONSTANT
    ) {
        fail(
            "This name is not an integer constant.",
            token_index
        );

        return 0;
    }

    return
        symbols[symbol_index]
            .address;
}

static int variable_node(
    int symbol_index,
    int token_index
)
{
    int node_index;

    if (
        symbol_index < 0 ||
        symbol_index >=
            symbol_count
    ) {
        fail(
            "Internal invalid symbol reference.",
            token_index
        );

        return create_node(
            NODE_INTEGER,
            type_int,
            0,
            0,
            0,
            0,
            0,
            token_index
        );
    }

    node_index =
        create_node(
            NODE_VARIABLE,
            symbols[symbol_index]
                .type,
            0,
            0,
            0,
            0,
            0,
            token_index
        );

    nodes[node_index].symbol =
        symbol_index;

    return node_index;
}

static int parse_primary(void)
{
    int token_index =
        parser_position;

    if (
        take(TOKEN_NUMBER) ||
        take(TOKEN_CHARACTER)
    ) {
        int literal_type =
            tokens[token_index].kind ==
                TOKEN_CHARACTER
                ? type_int
                : integer_literal_type(
                    token_index
                );

        return create_node(
            NODE_INTEGER,
            literal_type,
            0,
            0,
            0,
            0,
            tokens[token_index].value,
            token_index
        );
    }

    if (take(TOKEN_STRING)) {
        int length =
            decoded_string_length(
                token_index
            ) + 1;

        int address =
            store_string_literal(
                token_index
            );

        int string_type =
            array_of(
                type_char,
                length,
                token_index
            );

        return create_node(
            NODE_STRING,
            string_type,
            0,
            0,
            0,
            0,
            address,
            token_index
        );
    }

    if (take('(')) {
        int expression =
            parse_expression();

        expect(
            ')',
            "Expected ')' after the expression."
        );

        return expression;
    }

    if (
        is_at(TOKEN_IDENTIFIER) &&
        !is_keyword_token(
            parser_position
        )
    ) {
        int identifier_token =
            parser_position;

        int symbol_index;

        parser_position++;

        symbol_index =
            find_symbol(
                identifier_token
            );

        if (symbol_index < 0) {
            fail(
                "Use of an undeclared identifier.",
                identifier_token
            );

            return create_node(
                NODE_INTEGER,
                type_int,
                0,
                0,
                0,
                0,
                0,
                identifier_token
            );
        }

        if (
            symbols[symbol_index]
                .storage ==
            STORAGE_ENUM_CONSTANT
        ) {
            return create_node(
                NODE_INTEGER,
                type_int,
                0,
                0,
                0,
                0,
                constant_from_symbol(
                    symbol_index,
                    identifier_token
                ),
                identifier_token
            );
        }

        return variable_node(
            symbol_index,
            identifier_token
        );
    }

    fail(
        "Expected a literal, identifier, or parenthesized expression.",
        parser_position
    );

    return create_node(
        NODE_INTEGER,
        type_int,
        0,
        0,
        0,
        0,
        0,
        parser_position
    );
}
static int append_argument(
    int *first_argument,
    int *last_argument,
    int expression,
    int token_index
)
{
    int link =
        create_node(
            NODE_ARGUMENT,
            nodes[expression].type,
            expression,
            0,
            0,
            0,
            0,
            token_index
        );

    if (*last_argument) {
        nodes[*last_argument].b =
            link;
    } else {
        *first_argument = link;
    }

    *last_argument = link;

    return link;
}

static int argument_count_from_links(
    int first_argument
)
{
    int count = 0;
    int link =
        first_argument;

    while (link) {
        count++;
        link = nodes[link].b;
    }

    return count;
}

static int parse_call_suffix(
    int callee,
    int call_token
)
{
    int first_argument = 0;
    int last_argument = 0;
    int argument_count = 0;
    int function_index = -1;
    int return_type = type_int;

    if (
        nodes[callee].kind ==
            NODE_VARIABLE &&
        nodes[callee].symbol >= 0 &&
        symbols[
            nodes[callee].symbol
        ].storage ==
            STORAGE_FUNCTION
    ) {
        function_index =
            function_for_symbol(
                nodes[callee].symbol
            );

        if (
            function_index >= 0 &&
            function_index <
                function_count
        ) {
            return_type =
                types[
                    functions[
                        function_index
                    ].type
                ].return_type;
        }
    } else {
        int callee_type =
            decay_type(
                nodes[callee].type
            );

        if (
            is_pointer_type(
                callee_type
            ) &&
            is_function_type(
                types[callee_type].base
            )
        ) {
            return_type =
                types[
                    types[callee_type]
                        .base
                ].return_type;
        } else {
            fail(
                "Called expression is not a function.",
                call_token
            );
        }
    }

    if (!is_at(')')) {
        do {
            int argument =
                parse_assignment();

            append_argument(
                &first_argument,
                &last_argument,
                argument,
                call_token
            );

            argument_count++;

            if (
                argument_count > 64
            ) {
                fail(
                    "A function call supports at most 64 arguments.",
                    call_token
                );

                break;
            }
        } while (
            take(',') &&
            !compilation_failed
        );
    }

    expect(
        ')',
        "Expected ')' after the function arguments."
    );

    if (
        function_index >= 0 &&
        function_index <
            function_count &&
        functions[function_index]
            .is_defined &&
        argument_count !=
            functions[function_index]
                .parameter_count
    ) {
        fail(
            "Function argument count does not match its declaration.",
            call_token
        );
    }

    {
        int call =
            create_node(
                NODE_CALL,
                return_type,
                callee,
                first_argument,
                0,
                0,
                argument_count,
                call_token
            );

        nodes[call].symbol =
            function_index;

        return call;
    }
}

static int parse_postfix(void)
{
    int expression =
        parse_primary();

    while (!compilation_failed) {
        if (take('[')) {
            int bracket_token =
                parser_position - 1;

            int index =
                parse_expression();

            int pointer_type =
                decay_type(
                    nodes[expression].type
                );

            int element_type =
                type_int;

            expect(
                ']',
                "Expected ']' after the array index."
            );

            if (
                !is_pointer_type(
                    pointer_type
                )
            ) {
                fail(
                    "Subscripted expression is not an array or pointer.",
                    bracket_token
                );
            } else {
                element_type =
                    types[pointer_type]
                        .base;
            }

            require_integer(
                index,
                "Array index must have an integer type."
            );

            expression =
                create_node(
                    NODE_INDEX,
                    element_type,
                    expression,
                    index,
                    0,
                    0,
                    0,
                    bracket_token
                );

            continue;
        }

        if (take('(')) {
            expression =
                parse_call_suffix(
                    expression,
                    parser_position - 1
                );

            continue;
        }

        if (
            take('.') ||
            take(TOKEN_ARROW)
        ) {
            int operator_token =
                parser_position - 1;

            int is_arrow =
                tokens[operator_token]
                    .kind ==
                TOKEN_ARROW;

            int object_type =
                decay_type(
                    nodes[expression].type
                );

            int structure_type =
                type_int;

            int member_token;
            int member_index;

            if (is_arrow) {
                if (
                    !is_pointer_type(
                        object_type
                    )
                ) {
                    fail(
                        "The -> operator requires a pointer to a structure.",
                        operator_token
                    );
                } else {
                    structure_type =
                        types[object_type]
                            .base;
                }
            } else {
                structure_type =
                    nodes[expression].type;
            }

            if (
                !is_at(TOKEN_IDENTIFIER) ||
                is_keyword_token(
                    parser_position
                )
            ) {
                fail(
                    "Expected a member name after the member-access operator.",
                    parser_position
                );

                member_token =
                    parser_position;
            } else {
                member_token =
                    parser_position;

                parser_position++;
            }

            member_index =
                find_member_for_type(
                    structure_type,
                    member_token
                );

            expression =
                create_node(
                    NODE_MEMBER,
                    member_index >= 0
                        ? members[
                            member_index
                        ].type
                        : type_int,
                    expression,
                    0,
                    0,
                    0,
                    is_arrow,
                    operator_token
                );

            nodes[expression].symbol =
                member_index;

            continue;
        }

        if (
            take(TOKEN_INCREMENT) ||
            take(TOKEN_DECREMENT)
        ) {
            int operator_token =
                parser_position - 1;

            require_modifiable_lvalue(
                expression,
                "Postfix increment and decrement require a modifiable lvalue."
            );

            if (
                !is_scalar_type(
                    decay_type(
                        nodes[expression]
                            .type
                    )
                )
            ) {
                fail(
                    "Postfix increment and decrement require an integer or pointer.",
                    operator_token
                );
            }

            expression =
                create_node(
                    NODE_POSTFIX,
                    nodes[expression].type,
                    expression,
                    0,
                    0,
                    0,
                    tokens[operator_token]
                        .kind,
                    operator_token
                );

            continue;
        }

        break;
    }

    return expression;
}

static int token_after_matching_parenthesis(
    int opening_token
)
{
    int depth = 0;
    int index;

    if (
        opening_token < 0 ||
        opening_token >= token_count ||
        tokens[opening_token].kind !=
            '('
    ) {
        return -1;
    }

    for (
        index = opening_token;
        index < token_count;
        index++
    ) {
        if (
            tokens[index].kind == '('
        ) {
            depth++;
        } else if (
            tokens[index].kind == ')'
        ) {
            depth--;

            if (!depth) {
                return index + 1;
            }
        }
    }

    return -1;
}

static int parenthesis_begins_type_name(void)
{
    if (!is_at('(')) {
        return 0;
    }

    if (
        parser_position + 1 >=
        token_count
    ) {
        return 0;
    }

    return
        tokens[
            parser_position + 1
        ].kind ==
            TOKEN_IDENTIFIER &&
        token_is_type_keyword(
            parser_position + 1
        );
}

static int parse_unary(void)
{
    int operator_token =
        parser_position;

    if (
        take(TOKEN_INCREMENT) ||
        take(TOKEN_DECREMENT)
    ) {
        int operand =
            parse_unary();

        require_modifiable_lvalue(
            operand,
            "Prefix increment and decrement require a modifiable lvalue."
        );

        if (
            !is_scalar_type(
                decay_type(
                    nodes[operand].type
                )
            )
        ) {
            fail(
                "Prefix increment and decrement require an integer or pointer.",
                operator_token
            );
        }

        return create_node(
            NODE_PREFIX,
            nodes[operand].type,
            operand,
            0,
            0,
            0,
            tokens[operator_token].kind,
            operator_token
        );
    }

    if (take('&')) {
        int operand =
            parse_unary();

        int addressable =
            expression_is_lvalue(
                operand
            );

        if (
            !addressable &&
            nodes[operand].kind ==
                NODE_VARIABLE &&
            nodes[operand].symbol >= 0 &&
            symbols[
                nodes[operand].symbol
            ].storage ==
                STORAGE_FUNCTION
        ) {
            addressable = 1;
        }

        if (!addressable) {
            fail(
                "The address-of operator requires an object or function.",
                operator_token
            );
        }

        return create_node(
            NODE_ADDRESS,
            pointer_to(
                nodes[operand].type
            ),
            operand,
            0,
            0,
            0,
            0,
            operator_token
        );
    }

    if (take('*')) {
        int operand =
            parse_unary();

        int pointer_type =
            decay_type(
                nodes[operand].type
            );

        int result_type =
            type_int;

        if (
            !is_pointer_type(
                pointer_type
            )
        ) {
            fail(
                "The dereference operator requires a pointer.",
                operator_token
            );
        } else {
            result_type =
                types[pointer_type]
                    .base;

            if (
                types[result_type].kind ==
                TYPE_VOID
            ) {
                fail(
                    "A void pointer cannot be dereferenced.",
                    operator_token
                );
            }
        }

        return create_node(
            NODE_DEREFERENCE,
            result_type,
            operand,
            0,
            0,
            0,
            0,
            operator_token
        );
    }

    if (
        take('+') ||
        take('-') ||
        take('!') ||
        take('~')
    ) {
        int operand =
            parse_unary();

        int operator_kind =
            tokens[operator_token]
                .kind;

        if (operator_kind == '!') {
            require_scalar(
                operand,
                "Logical negation requires a scalar operand."
            );

            return create_node(
                NODE_UNARY,
                type_int,
                operand,
                0,
                0,
                0,
                operator_kind,
                operator_token
            );
        }

        require_integer(
            operand,
            "Unary arithmetic operators require an integer operand."
        );

        return create_node(
            NODE_UNARY,
            integer_promotion(
                nodes[operand].type
            ),
            operand,
            0,
            0,
            0,
            operator_kind,
            operator_token
        );
    }

    if (take_keyword("sizeof")) {
        int measured_type;
        int size;

        if (
            parenthesis_begins_type_name()
        ) {
            take('(');

            measured_type =
                parse_type_name();

            expect(
                ')',
                "Expected ')' after the sizeof type."
            );
        } else {
            int operand =
                parse_unary();

            measured_type =
                nodes[operand].type;
        }

        size =
            type_size(
                measured_type,
                operator_token
            );

        return create_node(
            NODE_SIZEOF,
            type_unsigned_int,
            0,
            0,
            0,
            0,
            size,
            operator_token
        );
    }

    if (
        parenthesis_begins_type_name()
    ) {
        int cast_token =
            parser_position;

        int destination_type;
        int operand;

        take('(');

        destination_type =
            parse_type_name();

        expect(
            ')',
            "Expected ')' after the cast type."
        );

        operand =
            parse_unary();

        if (
            !is_scalar_type(
                decay_type(
                    destination_type
                )
            ) ||
            !is_scalar_type(
                decay_type(
                    nodes[operand].type
                )
            )
        ) {
            fail(
                "Cinder supports casts between integer and pointer types.",
                cast_token
            );
        }

        return create_node(
            NODE_CAST,
            destination_type,
            operand,
            0,
            0,
            0,
            0,
            cast_token
        );
    }

    nesting_depth++;

    if (
        nesting_depth > 256
    ) {
        fail(
            "Expression nesting exceeds 256 levels.",
            parser_position
        );

        nesting_depth--;
        return 0;
    }

    {
        int result =
            parse_postfix();

        nesting_depth--;

        return result;
    }
}

static int create_binary_expression(
    int operator_kind,
    int operator_token,
    int left,
    int right
)
{
    int left_type =
        decay_type(
            nodes[left].type
        );

    int right_type =
        decay_type(
            nodes[right].type
        );

    int result_type =
        type_int;

    if (
        operator_kind == '+' ||
        operator_kind == '-'
    ) {
        if (
            is_pointer_type(
                left_type
            ) &&
            is_integer_type(
                right_type
            )
        ) {
            result_type =
                left_type;
        } else if (
            operator_kind == '+' &&
            is_integer_type(
                left_type
            ) &&
            is_pointer_type(
                right_type
            )
        ) {
            result_type =
                right_type;
        } else if (
            operator_kind == '-' &&
            is_pointer_type(
                left_type
            ) &&
            is_pointer_type(
                right_type
            )
        ) {
            if (
                !same_type(
                    types[left_type].base,
                    types[right_type].base
                )
            ) {
                fail(
                    "Pointer subtraction requires compatible pointer types.",
                    operator_token
                );
            }

            result_type =
                type_int;
        } else {
            require_integer(
                left,
                "Arithmetic operator requires integer operands."
            );

            require_integer(
                right,
                "Arithmetic operator requires integer operands."
            );

            result_type =
                usual_arithmetic_type(
                    left_type,
                    right_type
                );
        }
    } else if (
        operator_kind == '*' ||
        operator_kind == '/' ||
        operator_kind == '%'
    ) {
        require_integer(
            left,
            "Arithmetic operator requires integer operands."
        );

        require_integer(
            right,
            "Arithmetic operator requires integer operands."
        );

        result_type =
            usual_arithmetic_type(
                left_type,
                right_type
            );
    } else if (
        operator_kind == '&' ||
        operator_kind == '|' ||
        operator_kind == '^' ||
        operator_kind ==
            TOKEN_SHIFT_LEFT ||
        operator_kind ==
            TOKEN_SHIFT_RIGHT
    ) {
        require_integer(
            left,
            "Bitwise operator requires integer operands."
        );

        require_integer(
            right,
            "Bitwise operator requires integer operands."
        );

        result_type =
            usual_arithmetic_type(
                left_type,
                right_type
            );
    } else if (
        operator_kind ==
            TOKEN_LOGICAL_AND ||
        operator_kind ==
            TOKEN_LOGICAL_OR
    ) {
        require_scalar(
            left,
            "Logical operator requires scalar operands."
        );

        require_scalar(
            right,
            "Logical operator requires scalar operands."
        );

        result_type =
            type_int;
    } else {
        if (
            !is_scalar_type(
                left_type
            ) ||
            !is_scalar_type(
                right_type
            )
        ) {
            fail(
                "Comparison requires integer or pointer operands.",
                operator_token
            );
        }

        result_type =
            type_int;
    }

    return create_node(
        NODE_BINARY,
        result_type,
        left,
        right,
        0,
        0,
        operator_kind,
        operator_token
    );
}

static int binary_precedence(
    int kind
)
{
    switch (kind) {
        case TOKEN_LOGICAL_OR:
            return 1;

        case TOKEN_LOGICAL_AND:
            return 2;

        case '|':
            return 3;

        case '^':
            return 4;

        case '&':
            return 5;

        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            return 6;

        case '<':
        case '>':
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER_EQUAL:
            return 7;

        case TOKEN_SHIFT_LEFT:
        case TOKEN_SHIFT_RIGHT:
            return 8;

        case '+':
        case '-':
            return 9;

        case '*':
        case '/':
        case '%':
            return 10;

        default:
            return 0;
    }
}

static int parse_binary_expression(
    int minimum_precedence
)
{
    int left =
        parse_unary();

    while (!compilation_failed) {
        int operator_kind =
            tokens[
                parser_position
            ].kind;

        int precedence =
            binary_precedence(
                operator_kind
            );

        int operator_token;
        int right;

        if (
            precedence <
            minimum_precedence
        ) {
            break;
        }

        operator_token =
            parser_position;

        parser_position++;

        right =
            parse_binary_expression(
                precedence + 1
            );

        left =
            create_binary_expression(
                operator_kind,
                operator_token,
                left,
                right
            );
    }

    return left;
}
static int parse_conditional_expression(void)
{
    int condition =
        parse_binary_expression(1);

    if (!take('?')) {
        return condition;
    }

    {
        int question_token =
            parser_position - 1;

        int when_true =
            parse_expression();

        int when_false;
        int result_type;

        expect(
            ':',
            "Expected ':' in the conditional expression."
        );

        when_false =
            parse_conditional_expression();

        require_scalar(
            condition,
            "Conditional expression requires a scalar condition."
        );

        if (
            compatible_assignment(
                nodes[when_true].type,
                nodes[when_false].type
            )
        ) {
            result_type =
                decay_type(
                    nodes[when_true].type
                );
        } else if (
            compatible_assignment(
                nodes[when_false].type,
                nodes[when_true].type
            )
        ) {
            result_type =
                decay_type(
                    nodes[when_false].type
                );
        } else {
            fail(
                "Conditional-expression results have incompatible types.",
                question_token
            );

            result_type =
                type_int;
        }

        return create_node(
            NODE_CONDITIONAL,
            result_type,
            condition,
            when_true,
            when_false,
            0,
            0,
            question_token
        );
    }
}

static int is_assignment_operator(
    int kind
)
{
    return
        kind == '=' ||
        kind ==
            TOKEN_ADD_ASSIGN ||
        kind ==
            TOKEN_SUBTRACT_ASSIGN ||
        kind ==
            TOKEN_MULTIPLY_ASSIGN ||
        kind ==
            TOKEN_DIVIDE_ASSIGN ||
        kind ==
            TOKEN_MODULO_ASSIGN ||
        kind ==
            TOKEN_AND_ASSIGN ||
        kind ==
            TOKEN_OR_ASSIGN ||
        kind ==
            TOKEN_XOR_ASSIGN ||
        kind ==
            TOKEN_SHIFT_LEFT_ASSIGN ||
        kind ==
            TOKEN_SHIFT_RIGHT_ASSIGN;
}

static int assignment_binary_operator(
    int assignment_kind
)
{
    switch (assignment_kind) {
        case TOKEN_ADD_ASSIGN:
            return '+';

        case TOKEN_SUBTRACT_ASSIGN:
            return '-';

        case TOKEN_MULTIPLY_ASSIGN:
            return '*';

        case TOKEN_DIVIDE_ASSIGN:
            return '/';

        case TOKEN_MODULO_ASSIGN:
            return '%';

        case TOKEN_AND_ASSIGN:
            return '&';

        case TOKEN_OR_ASSIGN:
            return '|';

        case TOKEN_XOR_ASSIGN:
            return '^';

        case TOKEN_SHIFT_LEFT_ASSIGN:
            return
                TOKEN_SHIFT_LEFT;

        case TOKEN_SHIFT_RIGHT_ASSIGN:
            return
                TOKEN_SHIFT_RIGHT;

        default:
            return '=';
    }
}

static int parse_assignment(void)
{
    int left =
        parse_conditional_expression();

    int operator_kind =
        tokens[
            parser_position
        ].kind;

    if (
        !is_assignment_operator(
            operator_kind
        )
    ) {
        return left;
    }

    {
        int operator_token =
            parser_position;

        int right;

        parser_position++;

        require_modifiable_lvalue(
            left,
            "Assignment requires a modifiable lvalue."
        );

        right =
            parse_assignment();

        if (operator_kind == '=') {
            if (
                !compatible_assignment(
                    nodes[left].type,
                    nodes[right].type
                )
            ) {
                fail(
                    "Assigned value has an incompatible type.",
                    operator_token
                );
            }
        } else {
            int binary_operator =
                assignment_binary_operator(
                    operator_kind
                );

            if (
                (
                    binary_operator == '+' ||
                    binary_operator == '-'
                ) &&
                is_pointer_type(
                    decay_type(
                        nodes[left].type
                    )
                )
            ) {
                require_integer(
                    right,
                    "Pointer compound assignment requires an integer offset."
                );
            } else {
                require_integer(
                    left,
                    "Compound assignment requires an integer left operand."
                );

                require_integer(
                    right,
                    "Compound assignment requires an integer right operand."
                );
            }
        }

        return create_node(
            NODE_ASSIGNMENT,
            nodes[left].type,
            left,
            right,
            0,
            0,
            operator_kind,
            operator_token
        );
    }
}

static int parse_expression(void)
{
    int expression =
        parse_assignment();

    while (take(',')) {
        int comma_token =
            parser_position - 1;

        int right =
            parse_assignment();

        expression =
            create_node(
                NODE_COMMA,
                nodes[right].type,
                expression,
                right,
                0,
                0,
                0,
                comma_token
            );
    }

    return expression;
}

static int evaluate_constant_node(
    int node_index
)
{
    Node node;

    if (!node_index) {
        return 0;
    }

    node =
        nodes[node_index];

    switch (node.kind) {
        case NODE_INTEGER:
            return (int)node.value;

        case NODE_VARIABLE:
            if (
                node.symbol >= 0 &&
                node.symbol <
                    symbol_count &&
                symbols[node.symbol]
                    .storage ==
                    STORAGE_ENUM_CONSTANT
            ) {
                return
                    symbols[node.symbol]
                        .address;
            }

            fail(
                "Constant expression cannot read a variable.",
                node.token
            );

            return 0;

        case NODE_UNARY:
        {
            int operand =
                evaluate_constant_node(
                    node.a
                );

            if (node.value == '+') {
                return operand;
            }

            if (node.value == '-') {
                if (
                    operand ==
                    INT32_MIN
                ) {
                    fail(
                        "Constant expression overflows signed int.",
                        node.token
                    );

                    return 0;
                }

                return -operand;
            }

            if (node.value == '!') {
                return !operand;
            }

            if (node.value == '~') {
                return ~operand;
            }

            fail(
                "Unsupported unary operator in constant expression.",
                node.token
            );

            return 0;
        }

        case NODE_BINARY:
        {
            int operator_kind =
                (int)node.value;

            int left;
            int right;
            int64_t result = 0;

            if (
                operator_kind ==
                TOKEN_LOGICAL_AND
            ) {
                left =
                    evaluate_constant_node(
                        node.a
                    );

                if (!left) {
                    return 0;
                }

                right =
                    evaluate_constant_node(
                        node.b
                    );

                return !!right;
            }

            if (
                operator_kind ==
                TOKEN_LOGICAL_OR
            ) {
                left =
                    evaluate_constant_node(
                        node.a
                    );

                if (left) {
                    return 1;
                }

                right =
                    evaluate_constant_node(
                        node.b
                    );

                return !!right;
            }

            left =
                evaluate_constant_node(
                    node.a
                );

            right =
                evaluate_constant_node(
                    node.b
                );

            switch (operator_kind) {
                case '+':
                    result =
                        (int64_t)left +
                        right;
                    break;

                case '-':
                    result =
                        (int64_t)left -
                        right;
                    break;

                case '*':
                    result =
                        (int64_t)left *
                        right;
                    break;

                case '/':
                    if (!right) {
                        fail(
                            "Division by zero in constant expression.",
                            node.token
                        );

                        return 0;
                    }

                    if (
                        left == INT32_MIN &&
                        right == -1
                    ) {
                        fail(
                            "Constant expression overflows signed int.",
                            node.token
                        );

                        return 0;
                    }

                    result =
                        left / right;
                    break;

                case '%':
                    if (!right) {
                        fail(
                            "Division by zero in constant expression.",
                            node.token
                        );

                        return 0;
                    }

                    if (
                        left == INT32_MIN &&
                        right == -1
                    ) {
                        result = 0;
                    } else {
                        result =
                            left % right;
                    }

                    break;

                case '&':
                    result =
                        left & right;
                    break;

                case '|':
                    result =
                        left | right;
                    break;

                case '^':
                    result =
                        left ^ right;
                    break;

                case TOKEN_SHIFT_LEFT:
                    if (
                        right < 0 ||
                        right > 31
                    ) {
                        fail(
                            "Shift count must be between 0 and 31.",
                            node.token
                        );

                        return 0;
                    }

                    result =
                        (int32_t)(
                            (uint32_t)left
                            << right
                        );

                    break;

                case TOKEN_SHIFT_RIGHT:
                    if (
                        right < 0 ||
                        right > 31
                    ) {
                        fail(
                            "Shift count must be between 0 and 31.",
                            node.token
                        );

                        return 0;
                    }

                    result =
                        left >> right;
                    break;

                case TOKEN_EQUAL:
                    result =
                        left == right;
                    break;

                case TOKEN_NOT_EQUAL:
                    result =
                        left != right;
                    break;

                case '<':
                    result =
                        left < right;
                    break;

                case TOKEN_LESS_EQUAL:
                    result =
                        left <= right;
                    break;

                case '>':
                    result =
                        left > right;
                    break;

                case TOKEN_GREATER_EQUAL:
                    result =
                        left >= right;
                    break;

                default:
                    fail(
                        "Unsupported operator in constant expression.",
                        node.token
                    );

                    return 0;
            }

            if (
                result < INT32_MIN ||
                result > INT32_MAX
            ) {
                fail(
                    "Constant expression exceeds the signed 32-bit range.",
                    node.token
                );

                return 0;
            }

            return (int)result;
        }

        case NODE_CONDITIONAL:
            if (
                evaluate_constant_node(
                    node.a
                )
            ) {
                return
                    evaluate_constant_node(
                        node.b
                    );
            }

            return
                evaluate_constant_node(
                    node.c
                );

        case NODE_SIZEOF:
            return (int)node.value;

        case NODE_CAST:
        {
            int value =
                evaluate_constant_node(
                    node.a
                );

            switch (
                types[node.type].kind
            ) {
                case TYPE_CHAR:
                case TYPE_SIGNED_CHAR:
                    return (int8_t)value;

                case TYPE_UNSIGNED_CHAR:
                    return (uint8_t)value;

                case TYPE_SHORT:
                    return (int16_t)value;

                case TYPE_UNSIGNED_SHORT:
                    return (uint16_t)value;

                default:
                    return value;
            }
        }

        case NODE_COMMA:
            evaluate_constant_node(
                node.a
            );

            return
                evaluate_constant_node(
                    node.b
                );

        default:
            fail(
                "Expression is not an integer constant expression.",
                node.token
            );

            return 0;
    }
}

static int parse_constant_expression(void)
{
    int expression =
        parse_conditional_expression();

    return
        evaluate_constant_node(
            expression
        );
}

static int parse_initializer(void)
{
    int initializer_token =
        parser_position;

    if (take('{')) {
        int first = 0;
        int last = 0;
        int count = 0;

        if (!is_at('}')) {
            do {
                int item =
                    parse_initializer();

                append_link(
                    &first,
                    &last,
                    item,
                    initializer_token
                );

                count++;

                if (
                    count >
                    MAX_NODES / 2
                ) {
                    fail(
                        "Initializer contains too many elements.",
                        initializer_token
                    );

                    break;
                }
            } while (
                take(',') &&
                !is_at('}') &&
                !compilation_failed
            );
        }

        expect(
            '}',
            "Expected '}' after the initializer list."
        );

        return create_node(
            NODE_INITIALIZER,
            type_void,
            first,
            0,
            0,
            0,
            count,
            initializer_token
        );
    }

    return
        parse_assignment();
}

static int initializer_element_count(
    int initializer
)
{
    if (!initializer) {
        return 0;
    }

    if (
        nodes[initializer].kind !=
        NODE_INITIALIZER
    ) {
        return 1;
    }

    return
        (int)nodes[initializer]
            .value;
}

static int scalar_initializer_expression(
    int initializer
)
{
    if (!initializer) {
        return 0;
    }

    if (
        nodes[initializer].kind !=
        NODE_INITIALIZER
    ) {
        return initializer;
    }

    {
        int first =
            nodes[initializer].a;

        if (!first) {
            return 0;
        }

        if (nodes[first].b) {
            fail(
                "Scalar initializer contains more than one value.",
                nodes[initializer].token
            );
        }

        return
            nodes[first].a;
    }
}

static int complete_inferred_array(
    int type_index,
    int initializer,
    int token_index
)
{
    Type *type =
        &types[type_index];

    int length = 0;

    if (
        type->kind != TYPE_ARRAY ||
        type->array_length >= 0
    ) {
        return type_index;
    }

    if (!initializer) {
        fail(
            "An array without a specified length requires an initializer.",
            token_index
        );

        return
            complete_array_type(
                type_index,
                1,
                token_index
            );
    }

    if (
        nodes[initializer].kind ==
        NODE_STRING
    ) {
        if (
            types[type->base].kind !=
                TYPE_CHAR &&
            types[type->base].kind !=
                TYPE_SIGNED_CHAR &&
            types[type->base].kind !=
                TYPE_UNSIGNED_CHAR
        ) {
            fail(
                "Only a character array may infer its length from a string literal.",
                token_index
            );

            length = 1;
        } else {
            length =
                types[
                    nodes[initializer].type
                ].array_length;
        }
    } else if (
        nodes[initializer].kind ==
        NODE_INITIALIZER
    ) {
        length =
            initializer_element_count(
                initializer
            );
    } else {
        fail(
            "Incomplete array requires a brace initializer or string literal.",
            token_index
        );

        length = 1;
    }

    if (length <= 0) {
        fail(
            "Cannot infer a positive array length from an empty initializer.",
            token_index
        );

        length = 1;
    }

    return
        complete_array_type(
            type_index,
            length,
            token_index
        );
}

static int validate_initializer(
    int destination_type,
    int initializer,
    int token_index
)
{
    Type destination =
        types[destination_type];

    if (!initializer) {
        return 1;
    }

    if (
        destination.kind ==
        TYPE_ARRAY
    ) {
        int element_type =
            destination.base;

        if (
            destination.array_length < 0
        ) {
            fail(
                "Internal incomplete array was not completed before validation.",
                token_index
            );

            return 0;
        }

        if (
            nodes[initializer].kind ==
                NODE_STRING &&
            (
                types[element_type].kind ==
                    TYPE_CHAR ||
                types[element_type].kind ==
                    TYPE_SIGNED_CHAR ||
                types[element_type].kind ==
                    TYPE_UNSIGNED_CHAR
            )
        ) {
            int required =
                types[
                    nodes[initializer].type
                ].array_length;

            if (
                required >
                destination.array_length
            ) {
                fail(
                    "String literal is too long for the character array.",
                    token_index
                );

                return 0;
            }

            return 1;
        }

        if (
            nodes[initializer].kind !=
            NODE_INITIALIZER
        ) {
            fail(
                "Array initialization requires braces or a string literal.",
                token_index
            );

            return 0;
        }

        if (
            initializer_element_count(
                initializer
            ) >
            destination.array_length
        ) {
            fail(
                "Array initializer contains too many elements.",
                token_index
            );

            return 0;
        }

        {
            int link =
                nodes[initializer].a;

            while (link) {
                validate_initializer(
                    element_type,
                    nodes[link].a,
                    nodes[link].token
                );

                link =
                    nodes[link].b;
            }
        }

        return
            !compilation_failed;
    }

    if (
        destination.kind ==
        TYPE_STRUCT
    ) {
        Structure structure =
            structures[
                destination.structure
            ];

        int link;
        int member_index;
        int member_number = 0;

        if (
            nodes[initializer].kind !=
            NODE_INITIALIZER
        ) {
            if (
                same_type(
                    destination_type,
                    nodes[initializer].type
                )
            ) {
                return 1;
            }

            fail(
                "Structure initialization requires a compatible structure or brace list.",
                token_index
            );

            return 0;
        }

        if (
            initializer_element_count(
                initializer
            ) >
            structure.member_count
        ) {
            fail(
                "Structure initializer contains too many elements.",
                token_index
            );

            return 0;
        }

        link =
            nodes[initializer].a;

        member_index =
            structure.first_member;

        while (
            link &&
            member_index >= 0 &&
            member_number <
                structure.member_count
        ) {
            validate_initializer(
                members[member_index].type,
                nodes[link].a,
                nodes[link].token
            );

            link =
                nodes[link].b;

            member_index =
                members[member_index].next;

            member_number++;
        }

        return
            !compilation_failed;
    }

    {
        int expression =
            scalar_initializer_expression(
                initializer
            );

        if (!expression) {
            return 1;
        }

        if (
            !compatible_assignment(
                destination_type,
                nodes[expression].type
            )
        ) {
            fail(
                "Initializer has an incompatible type.",
                token_index
            );

            return 0;
        }
    }

    return 1;
}

static int finish_object_declaration(
    ParsedDeclarator declarator,
    int storage,
    int initializer,
    int declaration_token,
    int is_parameter
)
{
    int symbol_index;
    int node_index;

    if (
        declarator.name_token < 0
    ) {
        fail(
            "Expected an object name.",
            declaration_token
        );

        return 0;
    }

    if (
        types[declarator.type].kind ==
        TYPE_ARRAY &&
        types[declarator.type]
            .array_length < 0
    ) {
        declarator.type =
            complete_inferred_array(
                declarator.type,
                initializer,
                declarator.name_token
            );
    }

    if (
        types[declarator.type].kind ==
        TYPE_VOID
    ) {
        fail(
            "An object cannot have type void.",
            declarator.name_token
        );
    }

    if (
        types[declarator.type].kind ==
        TYPE_FUNCTION
    ) {
        fail(
            "A function type cannot be used as an object.",
            declarator.name_token
        );
    }

    if (
        types[declarator.type].kind ==
            TYPE_STRUCT &&
        !structures[
            types[declarator.type]
                .structure
        ].is_complete
    ) {
        fail(
            "An object cannot have an incomplete structure type.",
            declarator.name_token
        );
    }

    if (initializer) {
        validate_initializer(
            declarator.type,
            initializer,
            declarator.name_token
        );
    }

    symbol_index =
        add_symbol(
            declarator.name_token,
            declarator.type,
            storage,
            scope_depth,
            is_parameter
        );

    node_index =
        create_node(
            NODE_DECLARATION,
            declarator.type,
            initializer,
            0,
            0,
            0,
            symbol_index,
            declaration_token
        );

    nodes[node_index].symbol =
        symbol_index;

    return node_index;
}
static int parse_declaration_statement(void)
{
    int declaration_token =
        parser_position;

    int base_type =
        parse_declaration_specifiers();

    int first = 0;
    int last = 0;

    if (take(';')) {
        if (
            types[base_type].kind !=
                TYPE_STRUCT &&
            types[base_type].kind !=
                TYPE_ENUM
        ) {
            fail(
                "Declaration does not declare an object.",
                declaration_token
            );
        }

        return create_node(
            NODE_BLOCK,
            type_void,
            0,
            0,
            0,
            0,
            0,
            declaration_token
        );
    }

    do {
        ParsedDeclarator declarator =
            parse_declarator(
                base_type,
                0,
                0
            );

        int initializer = 0;
        int declaration;

        if (take('=')) {
            initializer =
                parse_initializer();
        }

        declaration =
            finish_object_declaration(
                declarator,
                STORAGE_LOCAL,
                initializer,
                declaration_token,
                0
            );

        append_link(
            &first,
            &last,
            declaration,
            declaration_token
        );
    } while (
        take(',') &&
        !compilation_failed
    );

    expect(
        ';',
        "Expected ';' after the declaration."
    );

    return create_node(
        NODE_BLOCK,
        type_void,
        first,
        0,
        0,
        0,
        0,
        declaration_token
    );
}

static int parse_block(
    int create_scope
)
{
    int block_token =
        parser_position;

    int first = 0;
    int last = 0;

    int saved_symbol_count =
        symbol_count;

    if (create_scope) {
        scope_depth++;
    }

    expect(
        '{',
        "Expected '{' to begin the block."
    );

    while (
        !is_at('}') &&
        !is_at(TOKEN_EOF) &&
        !compilation_failed
    ) {
        int statement =
            parse_statement();

        append_link(
            &first,
            &last,
            statement,
            block_token
        );
    }

    expect(
        '}',
        "Expected '}' to close the block."
    );

    if (create_scope) {
        scope_depth--;

        leave_scope(
            saved_symbol_count
        );
    }

    return create_node(
        NODE_BLOCK,
        type_void,
        first,
        0,
        0,
        0,
        0,
        block_token
    );
}

static int parse_return_statement(void)
{
    int return_token =
        parser_position;

    int expression = 0;
    int return_type =
        type_int;

    parser_position++;

    if (
        current_function < 0 ||
        current_function >=
            function_count
    ) {
        fail(
            "return may appear only inside a function.",
            return_token
        );
    } else {
        return_type =
            types[
                functions[
                    current_function
                ].type
            ].return_type;
    }

    if (!is_at(';')) {
        expression =
            parse_expression();
    }

    expect(
        ';',
        "Expected ';' after return."
    );

    if (
        types[return_type].kind ==
        TYPE_VOID
    ) {
        if (expression) {
            fail(
                "A void function cannot return a value.",
                return_token
            );
        }
    } else if (!expression) {
        fail(
            "A non-void function must return a value.",
            return_token
        );
    } else if (
        !compatible_assignment(
            return_type,
            nodes[expression].type
        )
    ) {
        fail(
            "Returned expression has an incompatible type.",
            return_token
        );
    }

    return create_node(
        NODE_RETURN,
        return_type,
        expression,
        0,
        0,
        0,
        0,
        return_token
    );
}

static int parse_if_statement(void)
{
    int if_token =
        parser_position;

    int condition;
    int then_branch;
    int else_branch = 0;

    parser_position++;

    expect(
        '(',
        "Expected '(' after if."
    );

    condition =
        parse_expression();

    expect(
        ')',
        "Expected ')' after the if condition."
    );

    require_scalar(
        condition,
        "The if condition must have a scalar type."
    );

    then_branch =
        parse_statement();

    if (take_keyword("else")) {
        else_branch =
            parse_statement();
    }

    return create_node(
        NODE_IF,
        type_void,
        condition,
        then_branch,
        else_branch,
        0,
        0,
        if_token
    );
}

static int parse_while_statement(void)
{
    int while_token =
        parser_position;

    int condition;
    int body;

    parser_position++;

    expect(
        '(',
        "Expected '(' after while."
    );

    condition =
        parse_expression();

    expect(
        ')',
        "Expected ')' after the while condition."
    );

    require_scalar(
        condition,
        "The while condition must have a scalar type."
    );

    loop_depth++;

    body =
        parse_statement();

    loop_depth--;

    return create_node(
        NODE_WHILE,
        type_void,
        condition,
        body,
        0,
        0,
        0,
        while_token
    );
}

static int parse_do_while_statement(void)
{
    int do_token =
        parser_position;

    int body;
    int condition;

    parser_position++;

    loop_depth++;

    body =
        parse_statement();

    loop_depth--;

    expect_keyword(
        "while",
        "Expected while after the do-loop body."
    );

    expect(
        '(',
        "Expected '(' after while."
    );

    condition =
        parse_expression();

    expect(
        ')',
        "Expected ')' after the do-while condition."
    );

    expect(
        ';',
        "Expected ';' after the do-while statement."
    );

    require_scalar(
        condition,
        "The do-while condition must have a scalar type."
    );

    return create_node(
        NODE_DO_WHILE,
        type_void,
        body,
        condition,
        0,
        0,
        0,
        do_token
    );
}

static int parse_for_statement(void)
{
    int for_token =
        parser_position;

    int initializer = 0;
    int condition = 0;
    int increment = 0;
    int body;

    int saved_symbol_count =
        symbol_count;

    parser_position++;
    scope_depth++;

    expect(
        '(',
        "Expected '(' after for."
    );

    if (begins_type_name()) {
        initializer =
            parse_declaration_statement();
    } else {
        if (!is_at(';')) {
            int expression =
                parse_expression();

            initializer =
                create_node(
                    NODE_EXPRESSION,
                    type_void,
                    expression,
                    0,
                    0,
                    0,
                    0,
                    for_token
                );
        }

        expect(
            ';',
            "Expected ';' after the for initializer."
        );
    }

    if (!is_at(';')) {
        condition =
            parse_expression();

        require_scalar(
            condition,
            "The for condition must have a scalar type."
        );
    }

    expect(
        ';',
        "Expected the second ';' in the for statement."
    );

    if (!is_at(')')) {
        increment =
            parse_expression();
    }

    expect(
        ')',
        "Expected ')' after the for clauses."
    );

    loop_depth++;

    body =
        parse_statement();

    loop_depth--;

    scope_depth--;

    leave_scope(
        saved_symbol_count
    );

    return create_node(
        NODE_FOR,
        type_void,
        initializer,
        condition,
        increment,
        body,
        0,
        for_token
    );
}

static int find_case_value(
    int first_case,
    int case_count,
    int value
)
{
    int index;

    for (
        index = 0;
        index < case_count;
        index++
    ) {
        int case_index =
            first_case + index;

        if (
            !switch_cases[
                case_index
            ].is_default &&
            switch_cases[
                case_index
            ].value == value
        ) {
            return case_index;
        }
    }

    return -1;
}

static int find_default_case(
    int first_case,
    int case_count
)
{
    int index;

    for (
        index = 0;
        index < case_count;
        index++
    ) {
        int case_index =
            first_case + index;

        if (
            switch_cases[
                case_index
            ].is_default
        ) {
            return case_index;
        }
    }

    return -1;
}

static int parse_switch_statement(void)
{
    int switch_token =
        parser_position;

    int expression;
    int body;

    int previous_case_start =
        current_switch_case_start;

    int first_case =
        switch_case_count;

    parser_position++;

    expect(
        '(',
        "Expected '(' after switch."
    );

    expression =
        parse_expression();

    expect(
        ')',
        "Expected ')' after the switch expression."
    );

    require_integer(
        expression,
        "The switch expression must have an integer type."
    );

    current_switch_case_start =
        first_case;

    switch_depth++;

    body =
        parse_statement();

    switch_depth--;

    {
        int case_count =
            switch_case_count -
            first_case;

        int switch_node =
            create_node(
                NODE_SWITCH,
                type_void,
                expression,
                body,
                0,
                0,
                case_count,
                switch_token
            );

        nodes[switch_node].symbol =
            first_case;

        current_switch_case_start =
            previous_case_start;

        return switch_node;
    }
}

static int parse_case_statement(void)
{
    int case_token =
        parser_position;

    int value;
    int body;
    int case_index;

    parser_position++;

    if (!switch_depth) {
        fail(
            "case may appear only inside a switch statement.",
            case_token
        );
    }

    value =
        parse_constant_expression();

    expect(
        ':',
        "Expected ':' after the case value."
    );

    if (
        find_case_value(
            current_switch_case_start,
            switch_case_count -
                current_switch_case_start,
            value
        ) >= 0
    ) {
        fail(
            "Duplicate case value in this switch statement.",
            case_token
        );
    }

    if (
        switch_case_count >=
        MAX_SWITCH_CASES
    ) {
        fail(
            "The program contains too many switch cases.",
            case_token
        );

        return 0;
    }

    case_index =
        switch_case_count;

    switch_case_count++;

    switch_cases[case_index].value =
        value;

    switch_cases[case_index].target =
        0;

    switch_cases[case_index].token =
        case_token;

    switch_cases[case_index]
        .is_default = 0;

    body =
        parse_statement();

    {
        int node =
            create_node(
                NODE_CASE,
                type_void,
                body,
                0,
                0,
                0,
                value,
                case_token
            );

        nodes[node].symbol =
            case_index;

        return node;
    }
}

static int parse_default_statement(void)
{
    int default_token =
        parser_position;

    int body;
    int case_index;

    parser_position++;

    if (!switch_depth) {
        fail(
            "default may appear only inside a switch statement.",
            default_token
        );
    }

    expect(
        ':',
        "Expected ':' after default."
    );

    if (
        find_default_case(
            current_switch_case_start,
            switch_case_count -
                current_switch_case_start
        ) >= 0
    ) {
        fail(
            "A switch statement may contain only one default label.",
            default_token
        );
    }

    if (
        switch_case_count >=
        MAX_SWITCH_CASES
    ) {
        fail(
            "The program contains too many switch cases.",
            default_token
        );

        return 0;
    }

    case_index =
        switch_case_count;

    switch_case_count++;

    switch_cases[case_index].value =
        0;

    switch_cases[case_index].target =
        0;

    switch_cases[case_index].token =
        default_token;

    switch_cases[case_index]
        .is_default = 1;

    body =
        parse_statement();

    {
        int node =
            create_node(
                NODE_DEFAULT,
                type_void,
                body,
                0,
                0,
                0,
                0,
                default_token
            );

        nodes[node].symbol =
            case_index;

        return node;
    }
}

static int parse_loop_control_statement(
    int is_break
)
{
    int token_index =
        parser_position;

    parser_position++;

    if (is_break) {
        if (
            !loop_depth &&
            !switch_depth
        ) {
            fail(
                "break may appear only inside a loop or switch.",
                token_index
            );
        }
    } else if (!loop_depth) {
        fail(
            "continue may appear only inside a loop.",
            token_index
        );
    }

    expect(
        ';',
        is_break
            ? "Expected ';' after break."
            : "Expected ';' after continue."
    );

    return create_node(
        is_break
            ? NODE_BREAK
            : NODE_CONTINUE,
        type_void,
        0,
        0,
        0,
        0,
        0,
        token_index
    );
}

static int parse_statement_body(void)
{
    int token_index =
        parser_position;

    if (is_at('{')) {
        return parse_block(1);
    }

    if (begins_type_name()) {
        return
            parse_declaration_statement();
    }

    if (is_keyword("return")) {
        return
            parse_return_statement();
    }

    if (is_keyword("if")) {
        return
            parse_if_statement();
    }

    if (is_keyword("while")) {
        return
            parse_while_statement();
    }

    if (is_keyword("do")) {
        return
            parse_do_while_statement();
    }

    if (is_keyword("for")) {
        return
            parse_for_statement();
    }

    if (is_keyword("switch")) {
        return
            parse_switch_statement();
    }

    if (is_keyword("case")) {
        return
            parse_case_statement();
    }

    if (is_keyword("default")) {
        return
            parse_default_statement();
    }

    if (is_keyword("break")) {
        return
            parse_loop_control_statement(
                1
            );
    }

    if (is_keyword("continue")) {
        return
            parse_loop_control_statement(
                0
            );
    }

    if (take(';')) {
        return create_node(
            NODE_BLOCK,
            type_void,
            0,
            0,
            0,
            0,
            0,
            token_index
        );
    }

    {
        int expression =
            parse_expression();

        expect(
            ';',
            "Expected ';' after the expression."
        );

        return create_node(
            NODE_EXPRESSION,
            type_void,
            expression,
            0,
            0,
            0,
            0,
            token_index
        );
    }
}

static int parse_statement(void)
{
    int result;

    nesting_depth++;

    if (
        nesting_depth > 256
    ) {
        fail(
            "Statement nesting exceeds 256 levels.",
            parser_position
        );

        nesting_depth--;

        return 0;
    }

    result =
        parse_statement_body();

    nesting_depth--;

    return result;
}

static int parse_parameter_list(
    int function_index
)
{
    int first_parameter = -1;
    int parameter_count = 0;

    if (
        is_keyword("void") &&
        tokens[
            parser_position + 1
        ].kind == ')'
    ) {
        parser_position++;
        return 0;
    }

    if (is_at(')')) {
        return 0;
    }

    do {
        int base_type =
            parse_declaration_specifiers();

        ParsedDeclarator parameter =
            parse_declarator(
                base_type,
                0,
                1
            );

        int symbol_index;

        if (
            parameter.name_token < 0
        ) {
            fail(
                "Expected a parameter name.",
                parser_position
            );

            break;
        }

        if (
            types[parameter.type].kind ==
            TYPE_VOID
        ) {
            fail(
                "A named parameter cannot have type void.",
                parameter.name_token
            );
        }

        if (
            is_array_type(
                parameter.type
            )
        ) {
            parameter.type =
                pointer_to(
                    types[
                        parameter.type
                    ].base
                );
        }

        symbol_index =
            add_symbol(
                parameter.name_token,
                parameter.type,
                STORAGE_LOCAL,
                1,
                1
            );

        if (
            first_parameter < 0
        ) {
            first_parameter =
                symbol_index;
        }

        parameter_count++;

        if (
            parameter_count > 64
        ) {
            fail(
                "Functions support at most 64 parameters.",
                parameter.name_token
            );

            break;
        }
    } while (
        take(',') &&
        !compilation_failed
    );

    functions[function_index]
        .first_parameter =
            first_parameter;

    functions[function_index]
        .parameter_count =
            parameter_count;

    types[
        functions[function_index]
            .type
    ].first_parameter =
        first_parameter;

    types[
        functions[function_index]
            .type
    ].parameter_count =
        parameter_count;

    return parameter_count;
}
static int find_first_identifier(
    const char *name
)
{
    int index;

    for (
        index = 0;
        index < token_count;
        index++
    ) {
        if (
            tokens[index].kind ==
                TOKEN_IDENTIFIER &&
            token_has_text(
                index,
                name
            )
        ) {
            return index;
        }
    }

    return -1;
}

static void add_builtin_symbol(
    const char *name,
    int builtin_id
)
{
    int name_token =
        find_first_identifier(
            name
        );

    int builtin_type;
    int symbol_index;

    if (name_token < 0) {
        return;
    }

    if (
        find_symbol(
            name_token
        ) >= 0
    ) {
        return;
    }

    builtin_type =
        function_type(
            type_int,
            -1,
            0
        );

    symbol_index =
        add_symbol(
            name_token,
            builtin_type,
            STORAGE_FUNCTION,
            0,
            0
        );

    if (symbol_index >= 0) {
        symbols[symbol_index]
            .address =
                builtin_id;

        symbols[symbol_index]
            .is_defined = 1;
    }
}

static void initialize_builtin_symbols(void)
{
    add_builtin_symbol(
        "printf",
        BUILTIN_PRINTF
    );

    add_builtin_symbol(
        "print",
        BUILTIN_PRINT
    );

    add_builtin_symbol(
        "getchar",
        BUILTIN_GETCHAR
    );

    global_symbol_count =
        symbol_count;
}

static void parse_function_definition(
    ParsedDeclarator declarator,
    int declaration_token
)
{
    int existing =
        find_function(
            declarator.name_token
        );

    int function_index;
    int saved_symbol_count =
        symbol_count;

    int return_type =
        declarator.type;

    if (
        is_array_type(
            return_type
        ) ||
        is_function_type(
            return_type
        )
    ) {
        fail(
            "A function cannot return an array or function type.",
            declarator.name_token
        );

        return_type =
            type_int;
    }

    if (existing >= 0) {
        function_index =
            existing;

        if (
            !same_type(
                types[
                    functions[
                        function_index
                    ].type
                ].return_type,
                return_type
            )
        ) {
            fail(
                "Function return type conflicts with its earlier declaration.",
                declarator.name_token
            );
        }
    } else {
        function_index =
            add_function(
                declarator.name_token,
                return_type
            );
    }

    current_function =
        function_index;

    functions[function_index]
        .local_count = 0;

    functions[function_index]
        .frame_size = 0;

    functions[function_index]
        .first_parameter = -1;

    functions[function_index]
        .parameter_count = 0;

    scope_depth = 1;

    expect(
        '(',
        "Expected '(' after the function name."
    );

    parse_parameter_list(
        function_index
    );

    expect(
        ')',
        "Expected ')' after the function parameters."
    );

    if (take(';')) {
        leave_scope(
            saved_symbol_count
        );

        current_function = -1;
        scope_depth = 0;
        return;
    }

    if (
        functions[function_index]
            .is_defined
    ) {
        fail(
            "Function is already defined.",
            declarator.name_token
        );
    }

    functions[function_index]
        .is_defined = 1;

    functions[function_index].body =
        parse_block(0);

    functions[function_index]
        .frame_size =
            align_to(
                functions[
                    function_index
                ].frame_size,
                4
            );

    leave_scope(
        saved_symbol_count
    );

    current_function = -1;
    scope_depth = 0;

    (void)declaration_token;
}

static void append_global_declaration(
    int declaration,
    int token_index
)
{
    append_link(
        &global_declaration_first,
        &global_declaration_last,
        declaration,
        token_index
    );
}

static void parse_global_object_list(
    int base_type,
    ParsedDeclarator first,
    int declaration_token
)
{
    ParsedDeclarator declarator =
        first;

    for (;;) {
        int initializer = 0;
        int declaration;

        if (take('=')) {
            initializer =
                parse_initializer();
        }

        declaration =
            finish_object_declaration(
                declarator,
                STORAGE_GLOBAL,
                initializer,
                declaration_token,
                0
            );

        append_global_declaration(
            declaration,
            declaration_token
        );

        global_symbol_count =
            symbol_count;

        if (!take(',')) {
            break;
        }

        declarator =
            parse_declarator(
                base_type,
                0,
                0
            );
    }

    expect(
        ';',
        "Expected ';' after the global declaration."
    );
}

static void parse_external_declaration(void)
{
    int declaration_token =
        parser_position;

    int base_type =
        parse_declaration_specifiers();

    ParsedDeclarator declarator;

    if (take(';')) {
        if (
            types[base_type].kind !=
                TYPE_STRUCT &&
            types[base_type].kind !=
                TYPE_ENUM
        ) {
            fail(
                "Top-level declaration does not declare an object or function.",
                declaration_token
            );
        }

        return;
    }

    declarator =
        parse_declarator(
            base_type,
            0,
            0
        );

    if (is_at('(')) {
        parse_function_definition(
            declarator,
            declaration_token
        );

        return;
    }

    parse_global_object_list(
        base_type,
        declarator,
        declaration_token
    );
}

static void parse_program(void)
{
    global_declaration_first = 0;
    global_declaration_last = 0;

    current_function = -1;
    scope_depth = 0;

    initialize_builtin_symbols();

    while (
        !is_at(TOKEN_EOF) &&
        !compilation_failed
    ) {
        if (!begins_type_name()) {
            fail(
                "Expected a global declaration or function definition.",
                parser_position
            );

            break;
        }

        parse_external_declaration();
    }

    global_symbol_count =
        symbol_count;
}

static void write_initial_value(
    int address,
    int size,
    uint32_t value,
    int token_index
)
{
    int index;

    if (
        !memory_range_is_valid(
            address,
            size
        )
    ) {
        fail(
            "Global initializer writes outside virtual memory.",
            token_index
        );

        return;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        initial_memory[
            address + index
        ] =
            (unsigned char)(
                value & 255u
            );

        value >>= 8;
    }
}

static void clear_initial_range(
    int address,
    int size,
    int token_index
)
{
    int index;

    if (
        !memory_range_is_valid(
            address,
            size
        )
    ) {
        fail(
            "Global initializer clears outside virtual memory.",
            token_index
        );

        return;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        initial_memory[
            address + index
        ] = 0;
    }
}

static void copy_initial_range(
    int destination,
    int source_address,
    int size,
    int token_index
)
{
    int index;

    if (
        !memory_range_is_valid(
            destination,
            size
        ) ||
        !memory_range_is_valid(
            source_address,
            size
        )
    ) {
        fail(
            "Global initializer copies outside virtual memory.",
            token_index
        );

        return;
    }

    for (
        index = 0;
        index < size;
        index++
    ) {
        initial_memory[
            destination + index
        ] =
            initial_memory[
                source_address +
                index
            ];
    }
}

static int evaluate_static_scalar(
    int expression,
    int destination_type,
    int token_index
)
{
    if (!expression) {
        return 0;
    }

    if (
        nodes[expression].kind ==
        NODE_STRING
    ) {
        if (
            !is_pointer_type(
                decay_type(
                    destination_type
                )
            )
        ) {
            fail(
                "String address requires a pointer destination.",
                token_index
            );

            return 0;
        }

        return
            (int)nodes[expression]
                .value;
    }

    if (
        nodes[expression].kind ==
        NODE_ADDRESS
    ) {
        int operand =
            nodes[expression].a;

        if (
            nodes[operand].kind ==
                NODE_VARIABLE &&
            nodes[operand].symbol >= 0 &&
            symbols[
                nodes[operand].symbol
            ].storage ==
                STORAGE_GLOBAL
        ) {
            return
                symbols[
                    nodes[operand].symbol
                ].address;
        }

        fail(
            "Global pointer initializer must use the address of a global object.",
            token_index
        );

        return 0;
    }

    return
        evaluate_constant_node(
            expression
        );
}

static void apply_static_initializer(
    int address,
    int type_index,
    int initializer,
    int token_index
)
{
    Type type =
        types[type_index];

    if (!initializer) {
        clear_initial_range(
            address,
            type.size,
            token_index
        );

        return;
    }

    if (
        type.kind ==
        TYPE_ARRAY
    ) {
        int element_type =
            type.base;

        int element_size =
            types[element_type]
                .size;

        int element_index = 0;

        clear_initial_range(
            address,
            type.size,
            token_index
        );

        if (
            nodes[initializer].kind ==
                NODE_STRING &&
            (
                types[element_type].kind ==
                    TYPE_CHAR ||
                types[element_type].kind ==
                    TYPE_SIGNED_CHAR ||
                types[element_type].kind ==
                    TYPE_UNSIGNED_CHAR
            )
        ) {
            int source_address =
                (int)nodes[initializer]
                    .value;

            int literal_size =
                types[
                    nodes[initializer]
                        .type
                ].size;

            int copy_size =
                literal_size <
                    type.size
                    ? literal_size
                    : type.size;

            copy_initial_range(
                address,
                source_address,
                copy_size,
                token_index
            );

            return;
        }

        if (
            nodes[initializer].kind ==
            NODE_INITIALIZER
        ) {
            int link =
                nodes[initializer].a;

            while (
                link &&
                element_index <
                    type.array_length
            ) {
                apply_static_initializer(
                    address +
                        element_index *
                        element_size,
                    element_type,
                    nodes[link].a,
                    nodes[link].token
                );

                link =
                    nodes[link].b;

                element_index++;
            }
        }

        return;
    }

    if (
        type.kind ==
        TYPE_STRUCT
    ) {
        Structure structure =
            structures[
                type.structure
            ];

        int link;
        int member_index;
        int member_number = 0;

        clear_initial_range(
            address,
            type.size,
            token_index
        );

        if (
            nodes[initializer].kind !=
            NODE_INITIALIZER
        ) {
            fail(
                "Global structure initializer must use braces.",
                token_index
            );

            return;
        }

        link =
            nodes[initializer].a;

        member_index =
            structure.first_member;

        while (
            link &&
            member_index >= 0 &&
            member_number <
                structure.member_count
        ) {
            apply_static_initializer(
                address +
                    members[
                        member_index
                    ].offset,
                members[
                    member_index
                ].type,
                nodes[link].a,
                nodes[link].token
            );

            link =
                nodes[link].b;

            member_index =
                members[
                    member_index
                ].next;

            member_number++;
        }

        return;
    }

    {
        int expression =
            scalar_initializer_expression(
                initializer
            );

        int value =
            evaluate_static_scalar(
                expression,
                type_index,
                token_index
            );

        write_initial_value(
            address,
            type.size,
            (uint32_t)value,
            token_index
        );
    }
}

static void initialize_global_objects(void)
{
    int link =
        global_declaration_first;

    while (
        link &&
        !compilation_failed
    ) {
        int declaration =
            nodes[link].a;

        int symbol_index =
            nodes[declaration]
                .symbol;

        if (
            symbol_index >= 0 &&
            symbol_index <
                symbol_count &&
            symbols[symbol_index]
                .storage ==
                STORAGE_GLOBAL
        ) {
            clear_initial_range(
                symbols[symbol_index]
                    .address,
                types[
                    symbols[symbol_index]
                        .type
                ].size,
                nodes[declaration]
                    .token
            );

            if (
                nodes[declaration].a
            ) {
                apply_static_initializer(
                    symbols[symbol_index]
                        .address,
                    symbols[symbol_index]
                        .type,
                    nodes[declaration].a,
                    nodes[declaration]
                        .token
                );
            }
        }

        link =
            nodes[link].b;
    }
}

static int emit(
    int opcode,
    int argument,
    int extra,
    int node_index
)
{
    if (
        instruction_count >=
        MAX_CODE
    ) {
        fail(
            "Generated program exceeds the bytecode instruction limit.",
            node_index > 0
                ? nodes[node_index]
                    .token
                : 0
        );

        return 0;
    }

    instructions[
        instruction_count
    ].opcode = opcode;

    instructions[
        instruction_count
    ].argument = argument;

    instructions[
        instruction_count
    ].extra = extra;

    instructions[
        instruction_count
    ].node = node_index;

    instruction_count++;

    return
        instruction_count - 1;
}

static int load_opcode_for_type(
    int type_index,
    int token_index
)
{
    Type type =
        types[type_index];

    if (
        type.kind ==
            TYPE_ARRAY ||
        type.kind ==
            TYPE_FUNCTION
    ) {
        return OP_HALT;
    }

    if (
        type.kind ==
        TYPE_STRUCT
    ) {
        fail(
            "A structure value cannot be loaded as a scalar.",
            token_index
        );

        return OP_HALT;
    }

    if (type.size == 1) {
        return OP_LOAD8;
    }

    if (type.size == 2) {
        return OP_LOAD16;
    }

    if (type.size == 4) {
        return OP_LOAD32;
    }

    fail(
        "Unsupported object size for a load operation.",
        token_index
    );

    return OP_HALT;
}

static int store_opcode_for_type(
    int type_index,
    int token_index
)
{
    Type type =
        types[type_index];

    if (
        type.kind ==
        TYPE_ARRAY
    ) {
        fail(
            "An array is not assignable.",
            token_index
        );

        return OP_HALT;
    }

    if (
        type.kind ==
        TYPE_STRUCT
    ) {
        fail(
            "Structure assignment requires an object copy.",
            token_index
        );

        return OP_HALT;
    }

    if (type.size == 1) {
        return OP_STORE8;
    }

    if (type.size == 2) {
        return OP_STORE16;
    }

    if (type.size == 4) {
        return OP_STORE32;
    }

    fail(
        "Unsupported object size for a store operation.",
        token_index
    );

    return OP_HALT;
}

static void emit_typed_load(
    int type_index,
    int node_index
)
{
    Type type =
        types[type_index];

    if (
        type.kind ==
            TYPE_ARRAY ||
        type.kind ==
            TYPE_FUNCTION
    ) {
        return;
    }

    emit(
        load_opcode_for_type(
            type_index,
            nodes[node_index].token
        ),
        type.is_unsigned ||
            type.kind ==
                TYPE_POINTER,
        type_index,
        node_index
    );
}

static void emit_typed_store(
    int type_index,
    int node_index
)
{
    emit(
        store_opcode_for_type(
            type_index,
            nodes[node_index].token
        ),
        0,
        type_index,
        node_index
    );
}
static int arithmetic_opcode(
    int operator_kind
)
{
    switch (operator_kind) {
        case '+':
        case TOKEN_ADD_ASSIGN:
            return OP_ADD;

        case '-':
        case TOKEN_SUBTRACT_ASSIGN:
            return OP_SUBTRACT;

        case '*':
        case TOKEN_MULTIPLY_ASSIGN:
            return OP_MULTIPLY;

        case '/':
        case TOKEN_DIVIDE_ASSIGN:
            return OP_DIVIDE;

        case '%':
        case TOKEN_MODULO_ASSIGN:
            return OP_MODULO;

        case '&':
        case TOKEN_AND_ASSIGN:
            return OP_BITWISE_AND;

        case '|':
        case TOKEN_OR_ASSIGN:
            return OP_BITWISE_OR;

        case '^':
        case TOKEN_XOR_ASSIGN:
            return OP_BITWISE_XOR;

        case TOKEN_SHIFT_LEFT:
        case TOKEN_SHIFT_LEFT_ASSIGN:
            return OP_SHIFT_LEFT;

        case TOKEN_SHIFT_RIGHT:
        case TOKEN_SHIFT_RIGHT_ASSIGN:
            return OP_SHIFT_RIGHT;

        case TOKEN_EQUAL:
            return OP_EQUAL;

        case TOKEN_NOT_EQUAL:
            return OP_NOT_EQUAL;

        case '<':
            return OP_LESS;

        case TOKEN_LESS_EQUAL:
            return OP_LESS_EQUAL;

        case '>':
            return OP_GREATER;

        case TOKEN_GREATER_EQUAL:
            return OP_GREATER_EQUAL;

        default:
            return OP_HALT;
    }
}

static void emit_integer_conversion(
    int destination_type,
    int node_index
)
{
    Type destination =
        types[destination_type];

    switch (destination.kind) {
        case TYPE_CHAR:
        case TYPE_SIGNED_CHAR:
            emit(
                OP_CONVERT_SIGNED8,
                0,
                0,
                node_index
            );
            break;

        case TYPE_UNSIGNED_CHAR:
            emit(
                OP_CONVERT_UNSIGNED8,
                0,
                0,
                node_index
            );
            break;

        case TYPE_SHORT:
            emit(
                OP_CONVERT_SIGNED16,
                0,
                0,
                node_index
            );
            break;

        case TYPE_UNSIGNED_SHORT:
            emit(
                OP_CONVERT_UNSIGNED16,
                0,
                0,
                node_index
            );
            break;

        default:
            break;
    }
}

static void compile_value(
    int node_index
);

static void compile_statement(
    int node_index
);

static void compile_address(
    int node_index
)
{
    Node node;

    if (
        !node_index ||
        compilation_failed
    ) {
        return;
    }

    node =
        nodes[node_index];

    switch (node.kind) {
        case NODE_VARIABLE:
        {
            int symbol_index =
                node.symbol;

            if (
                symbol_index < 0 ||
                symbol_index >=
                    symbol_count
            ) {
                fail(
                    "Internal invalid variable symbol.",
                    node.token
                );

                return;
            }

            if (
                symbols[symbol_index]
                    .storage ==
                STORAGE_GLOBAL
            ) {
                emit(
                    OP_ADDRESS_GLOBAL,
                    symbols[symbol_index]
                        .address,
                    0,
                    node_index
                );
            } else if (
                symbols[symbol_index]
                    .storage ==
                STORAGE_LOCAL
            ) {
                emit(
                    OP_ADDRESS_LOCAL,
                    symbols[symbol_index]
                        .address,
                    0,
                    node_index
                );
            } else {
                fail(
                    "Expression does not refer to an addressable object.",
                    node.token
                );
            }

            break;
        }

        case NODE_DEREFERENCE:
            compile_value(
                node.a
            );
            break;

        case NODE_INDEX:
        {
            int pointer_type =
                decay_type(
                    nodes[node.a].type
                );

            int element_size =
                pointer_element_size(
                    pointer_type,
                    node.token
                );

            compile_value(
                node.a
            );

            compile_value(
                node.b
            );

            if (element_size != 1) {
                emit(
                    OP_PUSH,
                    element_size,
                    0,
                    node_index
                );

                emit(
                    OP_MULTIPLY,
                    0,
                    0,
                    node_index
                );
            }

            emit(
                OP_ADD,
                0,
                0,
                node_index
            );

            break;
        }

        case NODE_MEMBER:
        {
            int member_index =
                node.symbol;

            if (
                member_index < 0 ||
                member_index >=
                    member_count
            ) {
                fail(
                    "Internal invalid structure member.",
                    node.token
                );

                return;
            }

            if (node.value) {
                compile_value(
                    node.a
                );
            } else {
                compile_address(
                    node.a
                );
            }

            if (
                members[member_index]
                    .offset
            ) {
                emit(
                    OP_PUSH,
                    members[member_index]
                        .offset,
                    0,
                    node_index
                );

                emit(
                    OP_ADD,
                    0,
                    0,
                    node_index
                );
            }

            break;
        }

        default:
            fail(
                "Expression does not have an address.",
                node.token
            );
            break;
    }
}

static void compile_binary_value(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int operator_kind =
        (int)node.value;

    int left_type =
        decay_type(
            nodes[node.a].type
        );

    int right_type =
        decay_type(
            nodes[node.b].type
        );

    if (
        operator_kind ==
            TOKEN_LOGICAL_AND ||
        operator_kind ==
            TOKEN_LOGICAL_OR
    ) {
        int condition_jump;
        int finish_jump;

        compile_value(
            node.a
        );

        condition_jump =
            emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );

        if (
            operator_kind ==
            TOKEN_LOGICAL_AND
        ) {
            compile_value(
                node.b
            );

            emit(
                OP_BOOLEAN,
                0,
                0,
                node_index
            );

            finish_jump =
                emit(
                    OP_JUMP,
                    0,
                    0,
                    node_index
                );

            instructions[
                condition_jump
            ].argument =
                instruction_count;

            emit(
                OP_PUSH,
                0,
                0,
                node_index
            );
        } else {
            emit(
                OP_PUSH,
                1,
                0,
                node_index
            );

            finish_jump =
                emit(
                    OP_JUMP,
                    0,
                    0,
                    node_index
                );

            instructions[
                condition_jump
            ].argument =
                instruction_count;

            compile_value(
                node.b
            );

            emit(
                OP_BOOLEAN,
                0,
                0,
                node_index
            );
        }

        instructions[
            finish_jump
        ].argument =
            instruction_count;

        return;
    }

    if (
        (
            operator_kind == '+' ||
            operator_kind == '-'
        ) &&
        is_pointer_type(
            left_type
        ) &&
        is_integer_type(
            right_type
        )
    ) {
        int element_size =
            pointer_element_size(
                left_type,
                node.token
            );

        compile_value(
            node.a
        );

        compile_value(
            node.b
        );

        if (element_size != 1) {
            emit(
                OP_PUSH,
                element_size,
                0,
                node_index
            );

            emit(
                OP_MULTIPLY,
                0,
                0,
                node_index
            );
        }

        emit(
            operator_kind == '+'
                ? OP_ADD
                : OP_SUBTRACT,
            0,
            0,
            node_index
        );

        return;
    }

    if (
        operator_kind == '+' &&
        is_integer_type(
            left_type
        ) &&
        is_pointer_type(
            right_type
        )
    ) {
        int element_size =
            pointer_element_size(
                right_type,
                node.token
            );

        compile_value(
            node.a
        );

        if (element_size != 1) {
            emit(
                OP_PUSH,
                element_size,
                0,
                node_index
            );

            emit(
                OP_MULTIPLY,
                0,
                0,
                node_index
            );
        }

        compile_value(
            node.b
        );

        emit(
            OP_ADD,
            0,
            0,
            node_index
        );

        return;
    }

    if (
        operator_kind == '-' &&
        is_pointer_type(
            left_type
        ) &&
        is_pointer_type(
            right_type
        )
    ) {
        int element_size =
            pointer_element_size(
                left_type,
                node.token
            );

        compile_value(
            node.a
        );

        compile_value(
            node.b
        );

        emit(
            OP_SUBTRACT,
            0,
            0,
            node_index
        );

        if (element_size != 1) {
            emit(
                OP_PUSH,
                element_size,
                0,
                node_index
            );

            emit(
                OP_DIVIDE,
                0,
                0,
                node_index
            );
        }

        return;
    }

    compile_value(
        node.a
    );

    compile_value(
        node.b
    );

    emit(
        arithmetic_opcode(
            operator_kind
        ),
        (
            is_integer_type(
                left_type
            ) &&
            is_integer_type(
                right_type
            )
        )
            ? types[
                usual_arithmetic_type(
                    left_type,
                    right_type
                )
            ].is_unsigned
            : 0,
        0,
        node_index
    );
}

static void compile_assignment_value(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int assignment_kind =
        (int)node.value;

    int destination_type =
        nodes[node.a].type;

    if (
        is_struct_type(
            destination_type
        ) &&
        assignment_kind == '='
    ) {
        compile_address(
            node.a
        );

        compile_address(
            node.b
        );

        emit(
            OP_COPY_MEMORY,
            types[destination_type]
                .size,
            0,
            node_index
        );

        return;
    }

    compile_address(
        node.a
    );

    if (assignment_kind == '=') {
        compile_value(
            node.b
        );

        emit_integer_conversion(
            destination_type,
            node_index
        );

        emit_typed_store(
            destination_type,
            node_index
        );

        return;
    }

    emit(
        OP_DUP,
        0,
        0,
        node_index
    );

    emit_typed_load(
        destination_type,
        node_index
    );

    if (
        (
            assignment_kind ==
                TOKEN_ADD_ASSIGN ||
            assignment_kind ==
                TOKEN_SUBTRACT_ASSIGN
        ) &&
        is_pointer_type(
            decay_type(
                destination_type
            )
        )
    ) {
        int element_size =
            pointer_element_size(
                decay_type(
                    destination_type
                ),
                node.token
            );

        compile_value(
            node.b
        );

        if (element_size != 1) {
            emit(
                OP_PUSH,
                element_size,
                0,
                node_index
            );

            emit(
                OP_MULTIPLY,
                0,
                0,
                node_index
            );
        }
    } else {
        compile_value(
            node.b
        );
    }

    emit(
        arithmetic_opcode(
            assignment_kind
        ),
        types[
            decay_type(
                destination_type
            )
        ].is_unsigned,
        0,
        node_index
    );

    emit_integer_conversion(
        destination_type,
        node_index
    );

    emit_typed_store(
        destination_type,
        node_index
    );
}

static void compile_increment_value(
    int node_index,
    int is_postfix
)
{
    Node node =
        nodes[node_index];

    int operand_type =
        nodes[node.a].type;

    int amount = 1;

    if (
        is_pointer_type(
            decay_type(
                operand_type
            )
        )
    ) {
        amount =
            pointer_element_size(
                decay_type(
                    operand_type
                ),
                node.token
            );
    }

    compile_address(
        node.a
    );

    if (is_postfix) {
        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit_typed_load(
            operand_type,
            node_index
        );

        emit(
            OP_SWAP,
            0,
            0,
            node_index
        );

        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit_typed_load(
            operand_type,
            node_index
        );
    } else {
        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit_typed_load(
            operand_type,
            node_index
        );
    }

    emit(
        OP_PUSH,
        amount,
        0,
        node_index
    );

    emit(
        node.value ==
            TOKEN_INCREMENT
                ? OP_ADD
                : OP_SUBTRACT,
        0,
        0,
        node_index
    );

    emit_integer_conversion(
        operand_type,
        node_index
    );

    emit_typed_store(
        operand_type,
        node_index
    );

    if (is_postfix) {
        emit(
            OP_DROP,
            0,
            0,
            node_index
        );
    }
}

static int call_is_builtin(
    int node_index,
    int builtin_id
)
{
    Node call =
        nodes[node_index];

    Node callee;

    if (
        call.kind != NODE_CALL ||
        !call.a
    ) {
        return 0;
    }

    callee =
        nodes[call.a];

    if (
        callee.kind !=
            NODE_VARIABLE ||
        callee.symbol < 0 ||
        callee.symbol >=
            symbol_count
    ) {
        return 0;
    }

    return
        symbols[callee.symbol]
            .storage ==
            STORAGE_FUNCTION &&
        symbols[callee.symbol]
            .address ==
            builtin_id;
}

static int call_argument_at(
    int first_argument,
    int requested_index
)
{
    int link =
        first_argument;

    int index = 0;

    while (link) {
        if (
            index ==
            requested_index
        ) {
            return
                nodes[link].a;
        }

        link =
            nodes[link].b;

        index++;
    }

    return 0;
}

static void compile_printf_call(
    int node_index
)
{
    Node call =
        nodes[node_index];

    int argument_count =
        argument_count_from_links(
            call.b
        );

    int format_argument;
    int format_address;
    int link;
    int value_count = 0;

    if (
        argument_count < 1
    ) {
        fail(
            "printf requires a format string.",
            call.token
        );

        return;
    }

    format_argument =
        call_argument_at(
            call.b,
            0
        );

    if (
        !format_argument ||
        nodes[format_argument].kind !=
            NODE_STRING
    ) {
        fail(
            "Cinder printf requires a literal format string as its first argument.",
            call.token
        );

        return;
    }

    format_address =
        (int)nodes[format_argument]
            .value;

    link =
        nodes[call.b].b;

    while (
        link &&
        !compilation_failed
    ) {
        compile_value(
            nodes[link].a
        );

        value_count++;

        link =
            nodes[link].b;
    }

    emit(
        OP_PRINT,
        format_address,
        value_count,
        node_index
    );
}

static void compile_print_call(
    int node_index
)
{
    Node call =
        nodes[node_index];

    int argument_count =
        argument_count_from_links(
            call.b
        );

    int argument;

    if (
        argument_count != 1
    ) {
        fail(
            "print requires exactly one integer argument.",
            call.token
        );

        return;
    }

    argument =
        call_argument_at(
            call.b,
            0
        );

    require_integer(
        argument,
        "print requires an integer argument."
    );

    compile_value(
        argument
    );

    emit(
        OP_PRINT,
        -1,
        1,
        node_index
    );
}

static void compile_getchar_call(
    int node_index
)
{
    Node call =
        nodes[node_index];

    if (
        argument_count_from_links(
            call.b
        ) != 0
    ) {
        fail(
            "getchar does not accept arguments.",
            call.token
        );

        return;
    }

    emit(
        OP_READ_STDIN,
        0,
        0,
        node_index
    );
}

static void compile_normal_call(
    int node_index
)
{
    Node call =
        nodes[node_index];

    Node callee =
        nodes[call.a];

    int function_index;
    int link;
    int argument_count = 0;

    if (
        callee.kind !=
            NODE_VARIABLE ||
        callee.symbol < 0 ||
        callee.symbol >=
            symbol_count ||
        symbols[callee.symbol]
            .storage !=
            STORAGE_FUNCTION
    ) {
        fail(
            "Indirect function calls are not supported.",
            call.token
        );

        return;
    }

    function_index =
        symbols[callee.symbol]
            .address;

    if (
        function_index < 0 ||
        function_index >=
            function_count
    ) {
        fail(
            "Invalid function reference.",
            call.token
        );

        return;
    }

    if (
        !functions[function_index]
            .is_defined
    ) {
        fail(
            "Called function has no definition.",
            call.token
        );

        return;
    }

    link =
        call.b;

    while (
        link &&
        !compilation_failed
    ) {
        compile_value(
            nodes[link].a
        );

        argument_count++;

        link =
            nodes[link].b;
    }

    if (
        argument_count !=
        functions[function_index]
            .parameter_count
    ) {
        fail(
            "Function argument count does not match its definition.",
            call.token
        );

        return;
    }

    emit(
        OP_CALL,
        function_index,
        argument_count,
        node_index
    );
}

static void compile_call_value(
    int node_index
)
{
    if (
        call_is_builtin(
            node_index,
            BUILTIN_PRINTF
        )
    ) {
        compile_printf_call(
            node_index
        );

        return;
    }

    if (
        call_is_builtin(
            node_index,
            BUILTIN_PRINT
        )
    ) {
        compile_print_call(
            node_index
        );

        return;
    }

    if (
        call_is_builtin(
            node_index,
            BUILTIN_GETCHAR
        )
    ) {
        compile_getchar_call(
            node_index
        );

        return;
    }

    compile_normal_call(
        node_index
    );
}
static void compile_value(
    int node_index
)
{
    Node node;

    if (
        !node_index ||
        compilation_failed
    ) {
        return;
    }

    node =
        nodes[node_index];

    switch (node.kind) {
        case NODE_INTEGER:
            emit(
                OP_PUSH,
                (int)node.value,
                0,
                node_index
            );
            break;

        case NODE_STRING:
            emit(
                OP_PUSH,
                (int)node.value,
                0,
                node_index
            );
            break;

        case NODE_VARIABLE:
            if (
                node.symbol >= 0 &&
                node.symbol <
                    symbol_count &&
                symbols[node.symbol]
                    .storage ==
                    STORAGE_FUNCTION
            ) {
                emit(
                    OP_PUSH,
                    symbols[node.symbol]
                        .address,
                    0,
                    node_index
                );
            } else {
                compile_address(
                    node_index
                );

                emit_typed_load(
                    node.type,
                    node_index
                );
            }

            break;

        case NODE_INDEX:
        case NODE_MEMBER:
        case NODE_DEREFERENCE:
            compile_address(
                node_index
            );

            emit_typed_load(
                node.type,
                node_index
            );
            break;

        case NODE_ADDRESS:
            compile_address(
                node.a
            );
            break;

        case NODE_UNARY:
            compile_value(
                node.a
            );

            if (node.value == '!') {
                emit(
                    OP_NOT,
                    0,
                    0,
                    node_index
                );
            } else if (
                node.value == '-'
            ) {
                emit(
                    OP_NEGATE,
                    0,
                    0,
                    node_index
                );
            } else if (
                node.value == '~'
            ) {
                emit(
                    OP_BITWISE_NOT,
                    0,
                    0,
                    node_index
                );
            }

            break;

        case NODE_BINARY:
            compile_binary_value(
                node_index
            );
            break;

        case NODE_ASSIGNMENT:
            compile_assignment_value(
                node_index
            );
            break;

        case NODE_PREFIX:
            compile_increment_value(
                node_index,
                0
            );
            break;

        case NODE_POSTFIX:
            compile_increment_value(
                node_index,
                1
            );
            break;

        case NODE_CAST:
            compile_value(
                node.a
            );

            emit_integer_conversion(
                node.type,
                node_index
            );

            break;

        case NODE_SIZEOF:
            emit(
                OP_PUSH,
                (int)node.value,
                0,
                node_index
            );
            break;

        case NODE_CONDITIONAL:
        {
            int false_jump;
            int finish_jump;

            compile_value(
                node.a
            );

            false_jump =
                emit(
                    OP_JUMP_IF_ZERO,
                    0,
                    0,
                    node_index
                );

            compile_value(
                node.b
            );

            finish_jump =
                emit(
                    OP_JUMP,
                    0,
                    0,
                    node_index
                );

            instructions[
                false_jump
            ].argument =
                instruction_count;

            compile_value(
                node.c
            );

            instructions[
                finish_jump
            ].argument =
                instruction_count;

            break;
        }

        case NODE_COMMA:
            compile_value(
                node.a
            );

            emit(
                OP_DROP,
                0,
                0,
                node_index
            );

            compile_value(
                node.b
            );

            break;

        case NODE_CALL:
            compile_call_value(
                node_index
            );
            break;

        default:
            fail(
                "Expression cannot be compiled as a value.",
                node.token
            );
            break;
    }
}

static void compile_initializer_at_address(
    int type_index,
    int initializer,
    int node_index
)
{
    Type type =
        types[type_index];

    if (!initializer) {
        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit(
            OP_CLEAR_MEMORY,
            type.size,
            0,
            node_index
        );

        return;
    }

    if (
        type.kind ==
        TYPE_ARRAY
    ) {
        int element_type =
            type.base;

        int element_size =
            types[element_type]
                .size;

        int element_index = 0;

        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit(
            OP_CLEAR_MEMORY,
            type.size,
            0,
            node_index
        );

        if (
            nodes[initializer].kind ==
                NODE_STRING &&
            (
                types[element_type].kind ==
                    TYPE_CHAR ||
                types[element_type].kind ==
                    TYPE_SIGNED_CHAR ||
                types[element_type].kind ==
                    TYPE_UNSIGNED_CHAR
            )
        ) {
            int source_address =
                (int)nodes[initializer]
                    .value;

            int literal_size =
                types[
                    nodes[initializer]
                        .type
                ].size;

            int copy_size =
                literal_size <
                    type.size
                    ? literal_size
                    : type.size;

            emit(
                OP_DUP,
                0,
                0,
                node_index
            );

            emit(
                OP_PUSH,
                source_address,
                0,
                node_index
            );

            emit(
                OP_COPY_MEMORY,
                copy_size,
                0,
                node_index
            );

            emit(
                OP_DROP,
                0,
                0,
                node_index
            );

            return;
        }

        if (
            nodes[initializer].kind ==
            NODE_INITIALIZER
        ) {
            int link =
                nodes[initializer].a;

            while (
                link &&
                element_index <
                    type.array_length
            ) {
                emit(
                    OP_DUP,
                    0,
                    0,
                    node_index
                );

                if (element_index) {
                    emit(
                        OP_PUSH,
                        element_index *
                            element_size,
                        0,
                        node_index
                    );

                    emit(
                        OP_ADD,
                        0,
                        0,
                        node_index
                    );
                }

                compile_initializer_at_address(
                    element_type,
                    nodes[link].a,
                    node_index
                );

                emit(
                    OP_DROP,
                    0,
                    0,
                    node_index
                );

                element_index++;

                link =
                    nodes[link].b;
            }
        }

        return;
    }

    if (
        type.kind ==
        TYPE_STRUCT
    ) {
        Structure structure =
            structures[
                type.structure
            ];

        emit(
            OP_DUP,
            0,
            0,
            node_index
        );

        emit(
            OP_CLEAR_MEMORY,
            type.size,
            0,
            node_index
        );

        if (
            nodes[initializer].kind ==
            NODE_INITIALIZER
        ) {
            int link =
                nodes[initializer].a;

            int member_index =
                structure.first_member;

            int member_number = 0;

            while (
                link &&
                member_index >= 0 &&
                member_number <
                    structure.member_count
            ) {
                emit(
                    OP_DUP,
                    0,
                    0,
                    node_index
                );

                if (
                    members[member_index]
                        .offset
                ) {
                    emit(
                        OP_PUSH,
                        members[
                            member_index
                        ].offset,
                        0,
                        node_index
                    );

                    emit(
                        OP_ADD,
                        0,
                        0,
                        node_index
                    );
                }

                compile_initializer_at_address(
                    members[
                        member_index
                    ].type,
                    nodes[link].a,
                    node_index
                );

                emit(
                    OP_DROP,
                    0,
                    0,
                    node_index
                );

                link =
                    nodes[link].b;

                member_index =
                    members[
                        member_index
                    ].next;

                member_number++;
            }

            return;
        }

        if (
            same_type(
                type_index,
                nodes[initializer].type
            )
        ) {
            compile_address(
                initializer
            );

            emit(
                OP_COPY_MEMORY,
                type.size,
                0,
                node_index
            );

            return;
        }

        fail(
            "Runtime structure initializer requires braces or a compatible structure.",
            nodes[initializer].token
        );

        return;
    }

    {
        int expression =
            scalar_initializer_expression(
                initializer
            );

        if (expression) {
            compile_value(
                expression
            );
        } else {
            emit(
                OP_PUSH,
                0,
                0,
                node_index
            );
        }

        emit_integer_conversion(
            type_index,
            node_index
        );

        emit_typed_store(
            type_index,
            node_index
        );
    }
}

static void compile_local_declaration(
    int node_index
)
{
    Node declaration =
        nodes[node_index];

    int symbol_index =
        declaration.symbol;

    if (
        symbol_index < 0 ||
        symbol_index >=
            symbol_count
    ) {
        fail(
            "Internal invalid declaration symbol.",
            declaration.token
        );

        return;
    }

    if (
        symbols[symbol_index]
            .storage !=
        STORAGE_LOCAL
    ) {
        return;
    }

    emit(
        OP_ADDRESS_LOCAL,
        symbols[symbol_index]
            .address,
        0,
        node_index
    );

    compile_initializer_at_address(
        symbols[symbol_index].type,
        declaration.a,
        node_index
    );

    emit(
        OP_DROP,
        0,
        0,
        node_index
    );
}

static int add_break_marker(
    int instruction,
    int depth,
    int token_index
)
{
    if (
        break_marker_count >=
        MAX_JUMP_MARKERS
    ) {
        fail(
            "The program contains too many unresolved break statements.",
            token_index
        );

        return -1;
    }

    break_markers[
        break_marker_count
    ].instruction =
        instruction;

    break_markers[
        break_marker_count
    ].depth =
        depth;

    break_marker_count++;

    return
        break_marker_count - 1;
}

static int add_continue_marker(
    int instruction,
    int depth,
    int token_index
)
{
    if (
        continue_marker_count >=
        MAX_JUMP_MARKERS
    ) {
        fail(
            "The program contains too many unresolved continue statements.",
            token_index
        );

        return -1;
    }

    continue_markers[
        continue_marker_count
    ].instruction =
        instruction;

    continue_markers[
        continue_marker_count
    ].depth =
        depth;

    continue_marker_count++;

    return
        continue_marker_count - 1;
}

static void patch_break_markers(
    int marker_start,
    int depth,
    int target
)
{
    int index;

    for (
        index = marker_start;
        index <
            break_marker_count;
        index++
    ) {
        if (
            break_markers[index]
                .depth ==
            depth
        ) {
            instructions[
                break_markers[index]
                    .instruction
            ].argument =
                target;

            break_markers[index]
                .depth = 0;
        }
    }
}

static void patch_continue_markers(
    int marker_start,
    int depth,
    int target
)
{
    int index;

    for (
        index = marker_start;
        index <
            continue_marker_count;
        index++
    ) {
        if (
            continue_markers[index]
                .depth ==
            depth
        ) {
            instructions[
                continue_markers[index]
                    .instruction
            ].argument =
                target;

            continue_markers[index]
                .depth = 0;
        }
    }
}

static void compile_block(
    int node_index
)
{
    int link =
        nodes[node_index].a;

    while (
        link &&
        !compilation_failed
    ) {
        compile_statement(
            nodes[link].a
        );

        link =
            nodes[link].b;
    }
}
static void compile_if_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int false_jump;
    int finish_jump = -1;

    compile_value(
        node.a
    );

    false_jump =
        emit(
            OP_JUMP_IF_ZERO,
            0,
            0,
            node_index
        );

    compile_statement(
        node.b
    );

    if (node.c) {
        finish_jump =
            emit(
                OP_JUMP,
                0,
                0,
                node_index
            );
    }

    instructions[
        false_jump
    ].argument =
        instruction_count;

    if (node.c) {
        compile_statement(
            node.c
        );

        instructions[
            finish_jump
        ].argument =
            instruction_count;
    }
}

static void compile_while_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int loop_start;
    int exit_jump;

    int saved_depth =
        ++loop_depth;

    int break_start =
        break_marker_count;

    int continue_start =
        continue_marker_count;

    loop_start =
        instruction_count;

    compile_value(
        node.a
    );

    exit_jump =
        emit(
            OP_JUMP_IF_ZERO,
            0,
            0,
            node_index
        );

    compile_statement(
        node.b
    );

    emit(
        OP_JUMP,
        loop_start,
        0,
        node_index
    );

    instructions[
        exit_jump
    ].argument =
        instruction_count;

    patch_break_markers(
        break_start,
        saved_depth,
        instruction_count
    );

    patch_continue_markers(
        continue_start,
        saved_depth,
        loop_start
    );

    loop_depth--;
}

static void compile_do_while_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int body_start;
    int condition_start;

    int saved_depth =
        ++loop_depth;

    int break_start =
        break_marker_count;

    int continue_start =
        continue_marker_count;

    body_start =
        instruction_count;

    compile_statement(
        node.a
    );

    condition_start =
        instruction_count;

    compile_value(
        node.b
    );

    emit(
        OP_JUMP_IF_NOT_ZERO,
        body_start,
        0,
        node_index
    );

    patch_break_markers(
        break_start,
        saved_depth,
        instruction_count
    );

    patch_continue_markers(
        continue_start,
        saved_depth,
        condition_start
    );

    loop_depth--;
}

static void compile_for_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int condition_start;
    int increment_start;
    int exit_jump = -1;

    int saved_depth =
        ++loop_depth;

    int break_start =
        break_marker_count;

    int continue_start =
        continue_marker_count;

    if (node.a) {
        compile_statement(
            node.a
        );
    }

    condition_start =
        instruction_count;

    if (node.b) {
        compile_value(
            node.b
        );

        exit_jump =
            emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );
    }

    compile_statement(
        node.d
    );

    increment_start =
        instruction_count;

    if (node.c) {
        compile_value(
            node.c
        );

        emit(
            OP_DROP,
            0,
            0,
            node_index
        );
    }

    emit(
        OP_JUMP,
        condition_start,
        0,
        node_index
    );

    if (exit_jump >= 0) {
        instructions[
            exit_jump
        ].argument =
            instruction_count;
    }

    patch_break_markers(
        break_start,
        saved_depth,
        instruction_count
    );

    patch_continue_markers(
        continue_start,
        saved_depth,
        increment_start
    );

    loop_depth--;
}

static int allocate_switch_temporary(
    int node_index
)
{
    int offset;

    if (
        current_function < 0 ||
        current_function >=
            function_count
    ) {
        fail(
            "Internal switch compilation outside a function.",
            nodes[node_index].token
        );

        return 0;
    }

    offset =
        align_to(
            functions[
                current_function
            ].frame_size,
            4
        );

    if (
        offset >
        VM_MEMORY_SIZE - 4
    ) {
        fail(
            "Function frame is too large for switch storage.",
            nodes[node_index].token
        );

        return 0;
    }

    functions[
        current_function
    ].frame_size =
        offset + 4;

    return offset;
}

static void compile_switch_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int first_case =
        node.symbol;

    int case_count =
        (int)node.value;

    int temporary_offset =
        allocate_switch_temporary(
            node_index
        );

    int dispatch_jump_start =
        instruction_count;

    int default_jump = -1;
    int finish_dispatch_jump;

    int break_start =
        break_marker_count;

    int saved_depth =
        ++loop_depth;

    int index;

    emit(
        OP_ADDRESS_LOCAL,
        temporary_offset,
        0,
        node_index
    );

    compile_value(
        node.a
    );

    emit(
        OP_STORE32,
        0,
        type_int,
        node_index
    );

    emit(
        OP_DROP,
        0,
        0,
        node_index
    );

    for (
        index = 0;
        index < case_count;
        index++
    ) {
        int case_index =
            first_case + index;

        if (
            switch_cases[
                case_index
            ].is_default
        ) {
            continue;
        }

        emit(
            OP_ADDRESS_LOCAL,
            temporary_offset,
            0,
            node_index
        );

        emit(
            OP_LOAD32,
            0,
            type_int,
            node_index
        );

        emit(
            OP_PUSH,
            switch_cases[
                case_index
            ].value,
            0,
            node_index
        );

        emit(
            OP_EQUAL,
            0,
            0,
            node_index
        );

        switch_cases[
            case_index
        ].target =
            emit(
                OP_JUMP_IF_NOT_ZERO,
                0,
                0,
                node_index
            );
    }

    default_jump =
        emit(
            OP_JUMP,
            0,
            0,
            node_index
        );

    finish_dispatch_jump =
        instruction_count;

    compile_statement(
        node.b
    );

    for (
        index = 0;
        index < case_count;
        index++
    ) {
        int case_index =
            first_case + index;

        if (
            switch_cases[
                case_index
            ].is_default
        ) {
            instructions[
                default_jump
            ].argument =
                switch_cases[
                    case_index
                ].target;
        } else {
            int jump_instruction =
                switch_cases[
                    case_index
                ].target;

            instructions[
                jump_instruction
            ].argument =
                switch_cases[
                    case_index
                ].token;
        }
    }

    if (
        instructions[
            default_jump
        ].argument == 0
    ) {
        instructions[
            default_jump
        ].argument =
            instruction_count;
    }

    patch_break_markers(
        break_start,
        saved_depth,
        instruction_count
    );

    loop_depth--;

    (void)dispatch_jump_start;
    (void)finish_dispatch_jump;
}

static void compile_case_statement(
    int node_index
)
{
    Node node =
        nodes[node_index];

    int case_index =
        node.symbol;

    if (
        case_index < 0 ||
        case_index >=
            switch_case_count
    ) {
        fail(
            "Internal invalid case label.",
            node.token
        );

        return;
    }

    switch_cases[
        case_index
    ].token =
        instruction_count;

    if (
        switch_cases[
            case_index
        ].is_default
    ) {
        switch_cases[
            case_index
        ].target =
            instruction_count;
    }

    compile_statement(
        node.a
    );
}

static void compile_statement(
    int node_index
)
{
    Node node;
    int jump;

    if (
        !node_index ||
        compilation_failed
    ) {
        return;
    }

    node =
        nodes[node_index];

    switch (node.kind) {
        case NODE_BLOCK:
            compile_block(
                node_index
            );
            break;

        case NODE_DECLARATION:
            compile_local_declaration(
                node_index
            );
            break;

        case NODE_EXPRESSION:
            if (node.a) {
                compile_value(
                    node.a
                );

                emit(
                    OP_DROP,
                    0,
                    0,
                    node_index
                );
            }

            break;

        case NODE_RETURN:
            if (node.a) {
                compile_value(
                    node.a
                );
            } else {
                emit(
                    OP_PUSH,
                    0,
                    0,
                    node_index
                );
            }

            emit(
                OP_RETURN,
                0,
                0,
                node_index
            );

            break;

        case NODE_IF:
            compile_if_statement(
                node_index
            );
            break;

        case NODE_WHILE:
            compile_while_statement(
                node_index
            );
            break;

        case NODE_DO_WHILE:
            compile_do_while_statement(
                node_index
            );
            break;

        case NODE_FOR:
            compile_for_statement(
                node_index
            );
            break;

        case NODE_SWITCH:
            compile_switch_statement(
                node_index
            );
            break;

        case NODE_CASE:
        case NODE_DEFAULT:
            compile_case_statement(
                node_index
            );
            break;

        case NODE_BREAK:
            jump =
                emit(
                    OP_JUMP,
                    0,
                    0,
                    node_index
                );

            add_break_marker(
                jump,
                loop_depth,
                node.token
            );

            break;

        case NODE_CONTINUE:
            jump =
                emit(
                    OP_JUMP,
                    0,
                    0,
                    node_index
                );

            add_continue_marker(
                jump,
                loop_depth,
                node.token
            );

            break;

        default:
            fail(
                "Syntax-tree node cannot be compiled as a statement.",
                node.token
            );

            break;
    }
}

static void compile_functions(void)
{
    int index;

    for (
        index = 0;
        index < function_count &&
        !compilation_failed;
        index++
    ) {
        current_function =
            index;

        functions[index].entry =
            instruction_count;

        compile_statement(
            functions[index].body
        );

        emit(
            OP_PUSH,
            0,
            0,
            functions[index].body
        );

        emit(
            OP_RETURN,
            0,
            0,
            functions[index].body
        );

        functions[index].frame_size =
            align_to(
                functions[index]
                    .frame_size,
                4
            );
    }

    current_function = -1;
}

static int find_entry_function(void)
{
    int index;
    int entry = -1;

    for (
        index = 0;
        index < function_count;
        index++
    ) {
        if (
            token_has_text(
                functions[index]
                    .name_token,
                "main"
            )
        ) {
            if (entry >= 0) {
                fail(
                    "The program defines main more than once.",
                    functions[index]
                        .name_token
                );

                return entry;
            }

            entry = index;
        }
    }

    if (entry < 0) {
        fail(
            "Define int main() as the program entry point.",
            0
        );

        return -1;
    }

    if (
        !functions[entry]
            .is_defined
    ) {
        fail(
            "main is declared but not defined.",
            functions[entry]
                .name_token
        );
    }

    if (
        functions[entry]
            .parameter_count != 0
    ) {
        fail(
            "Cinder main must not have parameters.",
            functions[entry]
                .name_token
        );
    }

    if (
        types[
            functions[entry].type
        ].return_type !=
            type_int
    ) {
        warn(
            "Cinder expects main to return int.",
            functions[entry]
                .name_token
        );
    }

    return entry;
}
static void output_character(
    char character,
    int token_index
)
{
    if (
        output_position >=
        MAX_OUTPUT - 1
    ) {
        fail(
            "Program output exceeded the 16 KB limit.",
            token_index
        );

        return;
    }

    program_output[
        output_position
    ] = character;

    output_position++;

    program_output[
        output_position
    ] = '\0';
}

static void output_text_from_memory(
    int address,
    int token_index
)
{
    int length =
        string_length_in_memory(
            address,
            token_index
        );

    int index;

    for (
        index = 0;
        index < length &&
        !compilation_failed;
        index++
    ) {
        output_character(
            (char)vm_memory[
                address + index
            ],
            token_index
        );
    }
}

static void output_signed_integer(
    int32_t value,
    int token_index
)
{
    char digits[16];
    int count = 0;
    int64_t absolute =
        value;

    if (absolute < 0) {
        output_character(
            '-',
            token_index
        );

        absolute = -absolute;
    }

    do {
        digits[count] =
            (char)(
                '0' +
                absolute % 10
            );

        count++;
        absolute /= 10;
    } while (absolute);

    while (count) {
        count--;

        output_character(
            digits[count],
            token_index
        );
    }
}

static void output_unsigned_integer(
    uint32_t value,
    uint32_t base,
    int uppercase,
    int token_index
)
{
    char digits[32];
    int count = 0;

    const char *alphabet =
        uppercase
            ? "0123456789ABCDEF"
            : "0123456789abcdef";

    do {
        digits[count] =
            alphabet[
                value % base
            ];

        count++;
        value /= base;
    } while (value);

    while (count) {
        count--;

        output_character(
            digits[count],
            token_index
        );
    }
}

static int instruction_token(
    Instruction instruction
)
{
    if (
        instruction.node > 0 &&
        instruction.node <=
            node_count
    ) {
        return
            nodes[
                instruction.node
            ].token;
    }

    return 0;
}

static int allocate_runtime_frame(
    int function_index,
    int token_index
)
{
    int frame_size;
    int frame_base;
    int64_t frame_end;

    if (
        function_index < 0 ||
        function_index >=
            function_count
    ) {
        fail(
            "Invalid function frame request.",
            token_index
        );

        return 0;
    }

    frame_size =
        functions[function_index]
            .frame_size;

    frame_base =
        align_to(
            stack_memory_top,
            4
        );

    frame_end =
        (int64_t)frame_base +
        frame_size;

    if (
        frame_end >
        VM_MEMORY_SIZE
    ) {
        fail(
            "Virtual-machine call stack exhausted its memory.",
            token_index
        );

        return 0;
    }

    clear_memory_range(
        frame_base,
        frame_size,
        token_index
    );

    stack_memory_top =
        (int)frame_end;

    return frame_base;
}

static void store_function_arguments(
    int function_index,
    int frame_base,
    int argument_count,
    int argument_base,
    int token_index
)
{
    int first_parameter =
        functions[function_index]
            .first_parameter;

    int index;

    if (
        argument_count !=
        functions[function_index]
            .parameter_count
    ) {
        fail(
            "Runtime function argument count mismatch.",
            token_index
        );

        return;
    }

    for (
        index = 0;
        index < argument_count;
        index++
    ) {
        int symbol_index =
            first_parameter +
            index;

        int value_index =
            argument_base +
            index;

        int address;
        int type_index;

        if (
            symbol_index < 0 ||
            symbol_index >=
                symbol_count ||
            !symbols[symbol_index]
                .is_parameter
        ) {
            fail(
                "Runtime parameter metadata is invalid.",
                token_index
            );

            return;
        }

        if (
            value_index < 0 ||
            value_index >=
                stack_pointer
        ) {
            fail(
                "Runtime argument stack is invalid.",
                token_index
            );

            return;
        }

        type_index =
            symbols[symbol_index]
                .type;

        address =
            frame_base +
            symbols[symbol_index]
                .address;

        if (
            is_struct_type(
                type_index
            )
        ) {
            int source_address =
                (int)value_stack[
                    value_index
                ];

            copy_memory_range(
                address,
                source_address,
                types[type_index].size,
                token_index
            );
        } else {
            write_memory_value(
                address,
                types[type_index].size,
                (uint32_t)
                    value_stack[
                        value_index
                    ],
                token_index
            );
        }
    }
}

static void execute_program(
    int entry_function
)
{
    int program_counter;
    int index;

    stack_pointer = 0;
    frame_pointer = 0;
    execution_steps = 0;
    program_result = 0;

    copy_bytes(
        vm_memory,
        initial_memory,
        VM_MEMORY_SIZE
    );

    stack_memory_top =
        align_to(
            global_memory_top,
            4
        );

    frame_bases[0] =
        allocate_runtime_frame(
            entry_function,
            functions[
                entry_function
            ].name_token
        );

    stack_bases[0] = 0;
    return_addresses[0] = -1;

    program_counter =
        functions[
            entry_function
        ].entry;

    while (!compilation_failed) {
        Instruction instruction;
        int token_index;
        int64_t left;
        int64_t right;
        int64_t result;

        if (
            program_counter < 0 ||
            program_counter >=
                instruction_count
        ) {
            fail(
                "Invalid virtual-machine instruction address.",
                0
            );

            break;
        }

        if (
            ++execution_steps >
            MAX_STEPS
        ) {
            int current_node =
                instructions[
                    program_counter
                ].node;

            fail(
                "Execution stopped after 1,000,000 instructions. The program may contain an infinite loop.",
                current_node > 0
                    ? nodes[current_node]
                        .token
                    : 0
            );

            break;
        }

        instruction =
            instructions[
                program_counter
            ];

        program_counter++;

        token_index =
            instruction_token(
                instruction
            );

        switch (
            instruction.opcode
        ) {
            case OP_PUSH:
                push_value(
                    instruction.argument,
                    token_index
                );
                break;

            case OP_ADDRESS_LOCAL:
                push_value(
                    frame_bases[
                        frame_pointer
                    ] +
                    instruction.argument,
                    token_index
                );
                break;

            case OP_ADDRESS_GLOBAL:
                push_value(
                    instruction.argument,
                    token_index
                );
                break;

            case OP_LOAD8:
            case OP_LOAD16:
            case OP_LOAD32:
            {
                int address =
                    (int)pop_value(
                        token_index
                    );

                int size =
                    instruction.opcode ==
                        OP_LOAD8
                        ? 1
                        : (
                            instruction.opcode ==
                                OP_LOAD16
                                ? 2
                                : 4
                        );

                int32_t value;

                if (instruction.argument) {
                    value =
                        (int32_t)
                            read_memory_unsigned(
                                address,
                                size,
                                token_index
                            );
                } else {
                    value =
                        read_memory_signed(
                            address,
                            size,
                            token_index
                        );
                }

                push_value(
                    value,
                    token_index
                );

                break;
            }

            case OP_STORE8:
            case OP_STORE16:
            case OP_STORE32:
            {
                int32_t value =
                    (int32_t)pop_value(
                        token_index
                    );

                int address =
                    (int)pop_value(
                        token_index
                    );

                int size =
                    instruction.opcode ==
                        OP_STORE8
                        ? 1
                        : (
                            instruction.opcode ==
                                OP_STORE16
                                ? 2
                                : 4
                        );

                write_memory_value(
                    address,
                    size,
                    (uint32_t)value,
                    token_index
                );

                if (size == 1) {
                    if (
                        instruction.extra >= 0 &&
                        instruction.extra <
                            type_count &&
                        types[
                            instruction.extra
                        ].is_unsigned
                    ) {
                        value =
                            (uint8_t)value;
                    } else {
                        value =
                            (int8_t)value;
                    }
                } else if (
                    size == 2
                ) {
                    if (
                        instruction.extra >= 0 &&
                        instruction.extra <
                            type_count &&
                        types[
                            instruction.extra
                        ].is_unsigned
                    ) {
                        value =
                            (uint16_t)value;
                    } else {
                        value =
                            (int16_t)value;
                    }
                }

                push_value(
                    value,
                    token_index
                );

                break;
            }

            case OP_COPY_MEMORY:
            {
                int source_address =
                    (int)pop_value(
                        token_index
                    );

                int destination =
                    (int)pop_value(
                        token_index
                    );

                copy_memory_range(
                    destination,
                    source_address,
                    instruction.argument,
                    token_index
                );

                push_value(
                    destination,
                    token_index
                );

                break;
            }

            case OP_CLEAR_MEMORY:
            {
                int address =
                    (int)pop_value(
                        token_index
                    );

                clear_memory_range(
                    address,
                    instruction.argument,
                    token_index
                );

                push_value(
                    address,
                    token_index
                );

                break;
            }

            case OP_DROP:
                pop_value(
                    token_index
                );
                break;

            case OP_DUP:
                push_value(
                    peek_value(
                        token_index
                    ),
                    token_index
                );
                break;

            case OP_SWAP:
                if (
                    stack_pointer < 2
                ) {
                    fail(
                        "Internal swap requires two stack values.",
                        token_index
                    );
                } else {
                    int64_t temporary =
                        value_stack[
                            stack_pointer - 1
                        ];

                    value_stack[
                        stack_pointer - 1
                    ] =
                        value_stack[
                            stack_pointer - 2
                        ];

                    value_stack[
                        stack_pointer - 2
                    ] =
                        temporary;
                }

                break;

            case OP_ADD:
            case OP_SUBTRACT:
            case OP_MULTIPLY:
            case OP_DIVIDE:
            case OP_MODULO:
            case OP_BITWISE_AND:
            case OP_BITWISE_OR:
            case OP_BITWISE_XOR:
            case OP_SHIFT_LEFT:
            case OP_SHIFT_RIGHT:
            case OP_EQUAL:
            case OP_NOT_EQUAL:
            case OP_LESS:
            case OP_LESS_EQUAL:
            case OP_GREATER:
            case OP_GREATER_EQUAL:
                right =
                    pop_value(
                        token_index
                    );

                left =
                    pop_value(
                        token_index
                    );

                result = 0;

                if (
                    (
                        instruction.opcode ==
                            OP_DIVIDE ||
                        instruction.opcode ==
                            OP_MODULO
                    ) &&
                    right == 0
                ) {
                    fail(
                        "Division by zero.",
                        token_index
                    );

                    break;
                }

                if (
                    (
                        instruction.opcode ==
                            OP_DIVIDE ||
                        instruction.opcode ==
                            OP_MODULO
                    ) &&
                    (int32_t)left ==
                        INT32_MIN &&
                    (int32_t)right == -1
                ) {
                    fail(
                        "Signed integer overflow.",
                        token_index
                    );

                    break;
                }

                if (
                    (
                        instruction.opcode ==
                            OP_SHIFT_LEFT ||
                        instruction.opcode ==
                            OP_SHIFT_RIGHT
                    ) &&
                    (
                        right < 0 ||
                        right > 31
                    )
                ) {
                    fail(
                        "Shift count must be between 0 and 31.",
                        token_index
                    );

                    break;
                }

                switch (
                    instruction.opcode
                ) {
                    case OP_ADD:
                        result =
                            (int64_t)
                                (int32_t)left +
                            (int32_t)right;
                        break;

                    case OP_SUBTRACT:
                        result =
                            (int64_t)
                                (int32_t)left -
                            (int32_t)right;
                        break;

                    case OP_MULTIPLY:
                        result =
                            (int64_t)
                                (int32_t)left *
                            (int32_t)right;
                        break;

                    case OP_DIVIDE:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left /
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left /
                                (int32_t)right;
                        }

                        break;

                    case OP_MODULO:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left %
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left %
                                (int32_t)right;
                        }

                        break;

                    case OP_BITWISE_AND:
                        result =
                            (uint32_t)left &
                            (uint32_t)right;
                        break;

                    case OP_BITWISE_OR:
                        result =
                            (uint32_t)left |
                            (uint32_t)right;
                        break;

                    case OP_BITWISE_XOR:
                        result =
                            (uint32_t)left ^
                            (uint32_t)right;
                        break;

                    case OP_SHIFT_LEFT:
                        result =
                            (uint32_t)left
                            << right;
                        break;

                    case OP_SHIFT_RIGHT:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left
                                >> right;
                        } else {
                            result =
                                (int32_t)left
                                >> right;
                        }

                        break;

                    case OP_EQUAL:
                        result =
                            (uint32_t)left ==
                            (uint32_t)right;
                        break;

                    case OP_NOT_EQUAL:
                        result =
                            (uint32_t)left !=
                            (uint32_t)right;
                        break;

                    case OP_LESS:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left <
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left <
                                (int32_t)right;
                        }

                        break;

                    case OP_LESS_EQUAL:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left <=
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left <=
                                (int32_t)right;
                        }

                        break;

                    case OP_GREATER:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left >
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left >
                                (int32_t)right;
                        }

                        break;

                    case OP_GREATER_EQUAL:
                        if (
                            instruction.argument
                        ) {
                            result =
                                (uint32_t)left >=
                                (uint32_t)right;
                        } else {
                            result =
                                (int32_t)left >=
                                (int32_t)right;
                        }

                        break;

                    default:
                        break;
                }

                if (
                    (
                        instruction.opcode ==
                            OP_ADD ||
                        instruction.opcode ==
                            OP_SUBTRACT ||
                        instruction.opcode ==
                            OP_MULTIPLY
                    ) &&
                    (
                        result < INT32_MIN ||
                        result > UINT32_MAX
                    )
                ) {
                    fail(
                        "Integer arithmetic exceeded the supported 32-bit range.",
                        token_index
                    );

                    break;
                }

                push_value(
                    (int32_t)result,
                    token_index
                );

                break;
                            case OP_NOT:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    !left,
                    token_index
                );

                break;

            case OP_BOOLEAN:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    !!left,
                    token_index
                );

                break;

            case OP_NEGATE:
                left =
                    pop_value(
                        token_index
                    );

                if (
                    (int32_t)left ==
                    INT32_MIN
                ) {
                    fail(
                        "Signed integer overflow.",
                        token_index
                    );
                } else {
                    push_value(
                        -(int32_t)left,
                        token_index
                    );
                }

                break;

            case OP_BITWISE_NOT:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    ~(int32_t)left,
                    token_index
                );

                break;

            case OP_CONVERT_SIGNED8:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    (int8_t)left,
                    token_index
                );

                break;

            case OP_CONVERT_UNSIGNED8:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    (uint8_t)left,
                    token_index
                );

                break;

            case OP_CONVERT_SIGNED16:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    (int16_t)left,
                    token_index
                );

                break;

            case OP_CONVERT_UNSIGNED16:
                left =
                    pop_value(
                        token_index
                    );

                push_value(
                    (uint16_t)left,
                    token_index
                );

                break;

            case OP_JUMP:
                program_counter =
                    instruction.argument;
                break;

            case OP_JUMP_IF_ZERO:
                left =
                    pop_value(
                        token_index
                    );

                if (!left) {
                    program_counter =
                        instruction.argument;
                }

                break;

            case OP_JUMP_IF_NOT_ZERO:
                left =
                    pop_value(
                        token_index
                    );

                if (left) {
                    program_counter =
                        instruction.argument;
                }

                break;

            case OP_CALL:
            {
                int function_index =
                    instruction.argument;

                int argument_count =
                    instruction.extra;

                int argument_base =
                    stack_pointer -
                    argument_count;

                int new_frame_base;

                if (
                    function_index < 0 ||
                    function_index >=
                        function_count
                ) {
                    fail(
                        "Invalid function call target.",
                        token_index
                    );

                    break;
                }

                if (
                    argument_base < 0
                ) {
                    fail(
                        "Function call does not have enough stack arguments.",
                        token_index
                    );

                    break;
                }

                if (
                    frame_pointer + 1 >=
                    MAX_CALL_DEPTH
                ) {
                    fail(
                        "Recursion limit reached.",
                        token_index
                    );

                    break;
                }

                new_frame_base =
                    allocate_runtime_frame(
                        function_index,
                        token_index
                    );

                store_function_arguments(
                    function_index,
                    new_frame_base,
                    argument_count,
                    argument_base,
                    token_index
                );

                if (compilation_failed) {
                    break;
                }

                frame_pointer++;

                frame_bases[
                    frame_pointer
                ] =
                    new_frame_base;

                return_addresses[
                    frame_pointer
                ] =
                    program_counter;

                stack_bases[
                    frame_pointer
                ] =
                    argument_base;

                stack_pointer =
                    argument_base;

                program_counter =
                    functions[
                        function_index
                    ].entry;

                break;
            }

            case OP_RETURN:
            {
                int32_t return_value =
                    (int32_t)pop_value(
                        token_index
                    );

                if (
                    frame_pointer == 0
                ) {
                    program_result =
                        return_value;

                    return;
                }

                stack_memory_top =
                    frame_bases[
                        frame_pointer
                    ];

                stack_pointer =
                    stack_bases[
                        frame_pointer
                    ];

                program_counter =
                    return_addresses[
                        frame_pointer
                    ];

                frame_pointer--;

                push_value(
                    return_value,
                    token_index
                );

                break;
            }

            case OP_READ_STDIN:
                if (
                    stdin_position >=
                    stdin_length
                ) {
                    push_value(
                        -1,
                        token_index
                    );
                } else {
                    push_value(
                        (unsigned char)
                            stdin_buffer[
                                stdin_position
                            ],
                        token_index
                    );

                    stdin_position++;
                }

                break;

            case OP_PRINT:
            {
                int argument_base =
                    stack_pointer -
                    instruction.extra;

                int output_before =
                    output_position;

                int used_arguments = 0;

                if (
                    argument_base < 0
                ) {
                    fail(
                        "Internal printf argument stack error.",
                        token_index
                    );

                    break;
                }

                if (
                    instruction.argument < 0
                ) {
                    if (
                        instruction.extra != 1
                    ) {
                        fail(
                            "Internal print argument count error.",
                            token_index
                        );

                        break;
                    }

                    output_signed_integer(
                        (int32_t)
                            value_stack[
                                argument_base
                            ],
                        token_index
                    );

                    output_character(
                        '\n',
                        token_index
                    );

                    used_arguments = 1;
                } else {
                    int format_address =
                        instruction.argument;

                    int format_position =
                        format_address;

                    if (
                        !memory_range_is_valid(
                            format_address,
                            1
                        )
                    ) {
                        fail(
                            "printf format pointer is outside virtual memory.",
                            token_index
                        );

                        break;
                    }

                    while (
                        format_position <
                            VM_MEMORY_SIZE &&
                        vm_memory[
                            format_position
                        ] &&
                        !compilation_failed
                    ) {
                        char character =
                            (char)vm_memory[
                                format_position
                            ];

                        format_position++;

                        if (
                            character == '%'
                        ) {
                            int32_t argument_value;

                            char format_character;

                            if (
                                format_position >=
                                    VM_MEMORY_SIZE
                            ) {
                                fail(
                                    "Incomplete printf format at the end of memory.",
                                    token_index
                                );

                                break;
                            }

                            format_character =
                                (char)vm_memory[
                                    format_position
                                ];

                            format_position++;

                            if (
                                format_character ==
                                '%'
                            ) {
                                output_character(
                                    '%',
                                    token_index
                                );

                                continue;
                            }

                            if (
                                format_character !=
                                    'd' &&
                                format_character !=
                                    'i' &&
                                format_character !=
                                    'u' &&
                                format_character !=
                                    'x' &&
                                format_character !=
                                    'X' &&
                                format_character !=
                                    'o' &&
                                format_character !=
                                    'c' &&
                                format_character !=
                                    's'
                            ) {
                                fail(
                                    "Supported printf formats are %d, %i, %u, %x, %X, %o, %c, %s and %%.",
                                    token_index
                                );

                                break;
                            }

                            if (
                                used_arguments >=
                                instruction.extra
                            ) {
                                fail(
                                    "Not enough printf arguments.",
                                    token_index
                                );

                                break;
                            }

                            argument_value =
                                (int32_t)
                                    value_stack[
                                        argument_base +
                                        used_arguments
                                    ];

                            used_arguments++;

                            if (
                                format_character ==
                                    'd' ||
                                format_character ==
                                    'i'
                            ) {
                                output_signed_integer(
                                    argument_value,
                                    token_index
                                );
                            } else if (
                                format_character ==
                                'u'
                            ) {
                                output_unsigned_integer(
                                    (uint32_t)
                                        argument_value,
                                    10,
                                    0,
                                    token_index
                                );
                            } else if (
                                format_character ==
                                'x'
                            ) {
                                output_unsigned_integer(
                                    (uint32_t)
                                        argument_value,
                                    16,
                                    0,
                                    token_index
                                );
                            } else if (
                                format_character ==
                                'X'
                            ) {
                                output_unsigned_integer(
                                    (uint32_t)
                                        argument_value,
                                    16,
                                    1,
                                    token_index
                                );
                            } else if (
                                format_character ==
                                'o'
                            ) {
                                output_unsigned_integer(
                                    (uint32_t)
                                        argument_value,
                                    8,
                                    0,
                                    token_index
                                );
                            } else if (
                                format_character ==
                                'c'
                            ) {
                                if (
                                    argument_value < 0 ||
                                    argument_value > 255
                                ) {
                                    fail(
                                        "%c requires a value from 0 to 255.",
                                        token_index
                                    );
                                } else {
                                    output_character(
                                        (char)
                                            argument_value,
                                        token_index
                                    );
                                }
                            } else {
                                output_text_from_memory(
                                    argument_value,
                                    token_index
                                );
                            }
                        } else {
                            output_character(
                                character,
                                token_index
                            );
                        }
                    }

                    if (
                        format_position >=
                            VM_MEMORY_SIZE &&
                        !compilation_failed
                    ) {
                        fail(
                            "printf format string is not null terminated.",
                            token_index
                        );
                    }

                    if (
                        !compilation_failed &&
                        used_arguments !=
                            instruction.extra
                    ) {
                        fail(
                            "Too many printf arguments for the format string.",
                            token_index
                        );
                    }
                }

                stack_pointer =
                    argument_base;

                if (!compilation_failed) {
                    push_value(
                        output_position -
                        output_before,
                        token_index
                    );
                }

                break;
            }

            case OP_HALT:
                return;

            default:
                fail(
                    "Unknown bytecode instruction.",
                    token_index
                );

                break;
        }
    }

    for (
        index = 0;
        index < 1;
        index++
    ) {
    }
}
static void report_character(
    char character
)
{
    if (
        report_position <
        MAX_REPORT - 1
    ) {
        report[
            report_position
        ] = character;

        report_position++;
    }
}

static void report_text(
    const char *text
)
{
    while (*text) {
        report_character(
            *text
        );

        text++;
    }
}

static void report_integer(
    int64_t value
)
{
    char digits[32];
    int count = 0;
    uint64_t absolute;

    if (value < 0) {
        report_character('-');

        absolute =
            (uint64_t)(
                -(value + 1)
            ) + 1u;
    } else {
        absolute =
            (uint64_t)value;
    }

    do {
        digits[count] =
            (char)(
                '0' +
                absolute % 10u
            );

        count++;
        absolute /= 10u;
    } while (absolute);

    while (count) {
        count--;

        report_character(
            digits[count]
        );
    }
}

static void report_quoted(
    const char *text,
    int length
)
{
    int index;

    report_character('"');

    for (
        index = 0;
        index < length;
        index++
    ) {
        unsigned char character =
            (unsigned char)
                text[index];

        if (
            character == '"' ||
            character == '\\'
        ) {
            report_character('\\');

            report_character(
                (char)character
            );
        } else if (
            character == '\n'
        ) {
            report_text("\\n");
        } else if (
            character == '\r'
        ) {
            report_text("\\r");
        } else if (
            character == '\t'
        ) {
            report_text("\\t");
        } else if (
            character == '\b'
        ) {
            report_text("\\b");
        } else if (
            character == '\f'
        ) {
            report_text("\\f");
        } else if (
            character < 32
        ) {
            const char *hex =
                "0123456789abcdef";

            report_text("\\u00");

            report_character(
                hex[
                    character >> 4
                ]
            );

            report_character(
                hex[
                    character & 15
                ]
            );
        } else {
            report_character(
                (char)character
            );
        }
    }

    report_character('"');
}

static const char *node_names[] = {
    "None",
    "Integer",
    "String",
    "Variable",
    "Binary",
    "Unary",
    "Assignment",
    "Call",
    "Argument",
    "Block",
    "Link",
    "Declaration",
    "Return",
    "If",
    "While",
    "For",
    "Expression",
    "Break",
    "Continue",
    "Postfix",
    "Prefix",
    "Conditional",
    "Do while",
    "Index",
    "Address",
    "Dereference",
    "Cast",
    "Sizeof",
    "Member",
    "Switch",
    "Case",
    "Default",
    "Initializer",
    "Comma"
};

static const char *opcode_names[] = {
    "PUSH",

    "ADDRESS_LOCAL",
    "ADDRESS_GLOBAL",

    "LOAD8",
    "LOAD16",
    "LOAD32",

    "STORE8",
    "STORE16",
    "STORE32",

    "COPY_MEMORY",
    "CLEAR_MEMORY",

    "DROP",
    "DUP",
    "SWAP",

    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",

    "BIT_AND",
    "BIT_OR",
    "BIT_XOR",
    "SHIFT_LEFT",
    "SHIFT_RIGHT",

    "EQ",
    "NE",
    "LT",
    "LE",
    "GT",
    "GE",

    "NOT",
    "NEG",
    "BIT_NOT",
    "BOOL",

    "CONVERT_SIGNED8",
    "CONVERT_UNSIGNED8",
    "CONVERT_SIGNED16",
    "CONVERT_UNSIGNED16",

    "JUMP",
    "JUMP_IF_ZERO",
    "JUMP_IF_NOT_ZERO",

    "CALL",
    "RETURN",

    "PRINT",
    "READ_STDIN",
    "HALT"
};

static const char *type_names[] = {
    "invalid",
    "void",
    "char",
    "signed char",
    "unsigned char",
    "short",
    "unsigned short",
    "int",
    "unsigned int",
    "long",
    "unsigned long",
    "pointer",
    "array",
    "struct",
    "enum",
    "function"
};

static const char *token_category(
    int token_index
)
{
    int kind =
        tokens[token_index].kind;

    if (kind == TOKEN_NUMBER) {
        return "number";
    }

    if (
        kind ==
        TOKEN_CHARACTER
    ) {
        return "character";
    }

    if (kind == TOKEN_STRING) {
        return "string";
    }

    if (
        kind ==
        TOKEN_IDENTIFIER
    ) {
        return
            is_keyword_token(
                token_index
            )
                ? "keyword"
                : "identifier";
    }

    return "symbol";
}

static void report_diagnostics(void)
{
    int index;

    report_text(
        "\"diagnostics\":["
    );

    for (
        index = 0;
        index < diagnostic_count;
        index++
    ) {
        Diagnostic diagnostic =
            diagnostics[index];

        if (index) {
            report_character(',');
        }

        report_text("{\"kind\":");

        report_quoted(
            diagnostic.kind ==
                DIAGNOSTIC_ERROR
                ? "error"
                : "warning",
            diagnostic.kind ==
                DIAGNOSTIC_ERROR
                ? 5
                : 7
        );

        report_text(
            ",\"message\":"
        );

        report_quoted(
            diagnostic.message,
            text_length(
                diagnostic.message
            )
        );

        report_text(
            ",\"token\":"
        );

        report_integer(
            diagnostic.token
        );

        report_text(
            ",\"start\":"
        );

        report_integer(
            diagnostic.start
        );

        report_text(
            ",\"end\":"
        );

        report_integer(
            diagnostic.end
        );

        report_text(
            ",\"line\":"
        );

        report_integer(
            diagnostic.line
        );

        report_text(
            ",\"column\":"
        );

        report_integer(
            diagnostic.column
        );

        report_character('}');
    }

    report_character(']');
}

static void report_tokens(void)
{
    int index;

    report_text(
        "\"tokens\":["
    );

    for (
        index = 0;
        index < token_count;
        index++
    ) {
        const char *category;

        if (
            tokens[index].kind ==
            TOKEN_EOF
        ) {
            continue;
        }

        if (index) {
            report_character(',');
        }

        category =
            token_category(index);

        report_text("{\"kind\":");

        report_quoted(
            category,
            text_length(category)
        );

        report_text(",\"start\":");

        report_integer(
            tokens[index].start
        );

        report_text(",\"end\":");

        report_integer(
            tokens[index].end
        );

        report_text(",\"text\":");

        report_quoted(
            source +
                tokens[index].start,
            tokens[index].end -
                tokens[index].start
        );

        report_text(",\"value\":");

        report_integer(
            tokens[index].value
        );

        report_character('}');
    }

    report_character(']');
}

static void report_nodes(void)
{
    int index;

    report_text(
        "\"nodes\":["
    );

    for (
        index = 1;
        index <= node_count;
        index++
    ) {
        Node node =
            nodes[index];

        int children[4];
        int child_index;
        int has_child = 0;
        const char *name;

        if (index > 1) {
            report_character(',');
        }

        children[0] = node.a;
        children[1] = node.b;
        children[2] = node.c;
        children[3] = node.d;

        if (
            node.kind >= 0 &&
            node.kind <=
                NODE_COMMA
        ) {
            name =
                node_names[
                    node.kind
                ];
        } else {
            name = "Unknown";
        }

        report_text("{\"id\":");

        report_integer(index);

        report_text(",\"kind\":");

        report_quoted(
            name,
            text_length(name)
        );

        report_text(",\"type\":");

        report_integer(
            node.type
        );

        report_text(",\"token\":");

        report_integer(
            node.token
        );

        report_text(",\"value\":");

        report_integer(
            node.value
        );

        report_text(",\"symbol\":");

        report_integer(
            node.symbol
        );

        report_text(
            ",\"children\":["
        );

        for (
            child_index = 0;
            child_index < 4;
            child_index++
        ) {
            if (
                children[
                    child_index
                ]
            ) {
                if (has_child) {
                    report_character(',');
                }

                report_integer(
                    children[
                        child_index
                    ]
                );

                has_child = 1;
            }
        }

        report_text("]}");
    }

    report_character(']');
}

static void report_types(void)
{
    int index;

    report_text(
        "\"types\":["
    );

    for (
        index = 0;
        index < type_count;
        index++
    ) {
        Type type =
            types[index];

        const char *name;

        if (index) {
            report_character(',');
        }

        if (
            type.kind >=
                TYPE_INVALID &&
            type.kind <=
                TYPE_FUNCTION
        ) {
            name =
                type_names[
                    type.kind
                ];
        } else {
            name = "unknown";
        }

        report_text("{\"id\":");

        report_integer(index);

        report_text(",\"kind\":");

        report_quoted(
            name,
            text_length(name)
        );

        report_text(",\"size\":");

        report_integer(
            type.size
        );

        report_text(
            ",\"alignment\":"
        );

        report_integer(
            type.alignment
        );

        report_text(",\"base\":");

        report_integer(
            type.base
        );

        report_text(
            ",\"arrayLength\":"
        );

        report_integer(
            type.array_length
        );

        report_text(
            ",\"structure\":"
        );

        report_integer(
            type.structure
        );

        report_text(
            ",\"isConst\":"
        );

        report_text(
            type.is_const
                ? "true"
                : "false"
        );

        report_text(
            ",\"isUnsigned\":"
        );

        report_text(
            type.is_unsigned
                ? "true"
                : "false"
        );

        report_character('}');
    }

    report_character(']');
}

static void report_functions(void)
{
    int index;

    report_text(
        "\"functions\":["
    );

    for (
        index = 0;
        index < function_count;
        index++
    ) {
        Token name =
            tokens[
                functions[index]
                    .name_token
            ];

        if (index) {
            report_character(',');
        }

        report_text("{\"name\":");

        report_quoted(
            source + name.start,
            name.end - name.start
        );

        report_text(",\"root\":");

        report_integer(
            functions[index].body
        );

        report_text(",\"entry\":");

        report_integer(
            functions[index].entry
        );

        report_text(
            ",\"frameSize\":"
        );

        report_integer(
            functions[index]
                .frame_size
        );

        report_text(
            ",\"locals\":"
        );

        report_integer(
            functions[index]
                .local_count
        );

        report_text(
            ",\"parameters\":"
        );

        report_integer(
            functions[index]
                .parameter_count
        );

        report_text(
            ",\"defined\":"
        );

        report_text(
            functions[index]
                .is_defined
                ? "true"
                : "false"
        );

        report_character('}');
    }

    report_character(']');
}

static void report_instructions(void)
{
    int index;

    report_text(
        "\"instructions\":["
    );

    for (
        index = 0;
        index < instruction_count;
        index++
    ) {
        Instruction instruction =
            instructions[index];

        const char *name;

        if (index) {
            report_character(',');
        }

        if (
            instruction.opcode >= 0 &&
            instruction.opcode <=
                OP_HALT
        ) {
            name =
                opcode_names[
                    instruction.opcode
                ];
        } else {
            name = "UNKNOWN";
        }

        report_text(
            "{\"index\":"
        );

        report_integer(index);

        report_text(",\"op\":");

        report_quoted(
            name,
            text_length(name)
        );

        report_text(",\"arg\":");

        report_integer(
            instruction.argument
        );

        report_text(
            ",\"extra\":"
        );

        report_integer(
            instruction.extra
        );

        report_text(",\"node\":");

        report_integer(
            instruction.node
        );

        report_character('}');
    }

    report_character(']');
}

static void build_report(void)
{
    report_position = 0;

    report_text("{\"ok\":");

    report_text(
        compilation_failed
            ? "false"
            : "true"
    );

    report_text(",\"error\":");

    if (diagnostic_count > 0) {
        report_quoted(
            diagnostics[0].message,
            text_length(
                diagnostics[0].message
            )
        );
    } else {
        report_quoted("", 0);
    }

    report_text(
        ",\"errorStart\":"
    );

    report_integer(
        diagnostic_count > 0
            ? diagnostics[0].start
            : 0
    );

    report_text(
        ",\"errorEnd\":"
    );

    report_integer(
        diagnostic_count > 0
            ? diagnostics[0].end
            : 0
    );

    report_text(",\"result\":");

    report_integer(
        program_result
    );

    report_text(",\"steps\":");

    report_integer(
        execution_steps
    );

    report_text(",\"stdout\":");

    report_quoted(
        program_output,
        output_position
    );

    report_character(',');

    report_diagnostics();

    report_character(',');

    report_tokens();

    report_character(',');

    report_nodes();

    report_character(',');

    report_types();

    report_character(',');

    report_functions();

    report_character(',');

    report_instructions();

    report_character('}');

    report[
        report_position
    ] = '\0';
}
static void reset_compiler_state(void)
{
    int index;

    token_count = 0;
    node_count = 0;
    type_count = 0;
    symbol_count = 0;
    global_symbol_count = 0;
    function_count = 0;
    structure_count = 0;
    member_count = 0;
    enumeration_count = 0;
    enum_value_count = 0;
    instruction_count = 0;
    diagnostic_count = 0;

    parser_position = 0;
    scope_depth = 0;
    current_function = -1;
    nesting_depth = 0;
    loop_depth = 0;
    switch_depth = 0;

    global_memory_top = 4;
    stack_memory_top = 4;

    stack_pointer = 0;
    frame_pointer = 0;
    output_position = 0;
    report_position = 0;
    stdin_position = 0;
    execution_steps = 0;
    program_result = 0;

    compilation_failed = 0;

    global_declaration_first = 0;
    global_declaration_last = 0;

    break_marker_count = 0;
    continue_marker_count = 0;
    switch_case_count = 0;
    current_switch_case_start = 0;

    program_output[0] = '\0';
    report[0] = '\0';

    clear_bytes(
        initial_memory,
        VM_MEMORY_SIZE
    );

    clear_bytes(
        vm_memory,
        VM_MEMORY_SIZE
    );

    for (
        index = 0;
        index < MAX_SYMBOLS;
        index++
    ) {
        symbols[index].is_active =
            0;
    }

    for (
        index = 0;
        index <
            MAX_JUMP_MARKERS;
        index++
    ) {
        break_markers[index]
            .instruction = 0;

        break_markers[index]
            .depth = 0;

        continue_markers[index]
            .instruction = 0;

        continue_markers[index]
            .depth = 0;
    }

    initialize_types();
}

static void reference_internal_helpers(void)
{
    (void)unqualified_type;
    (void)token_after_matching_parenthesis;
    (void)global_symbol_count;
}

char *input_ptr(void)
{
    return source;
}

char *stdin_ptr(void)
{
    return stdin_buffer;
}

int input_capacity(void)
{
    return MAX_SOURCE;
}

int stdin_capacity(void)
{
    return MAX_STDIN;
}

void set_stdin_len(
    int length
)
{
    if (length < 0) {
        stdin_length = 0;
        return;
    }

    if (
        length >= MAX_STDIN
    ) {
        stdin_length =
            MAX_STDIN - 1;
    } else {
        stdin_length =
            length;
    }

    stdin_buffer[
        stdin_length
    ] = '\0';

    stdin_position = 0;
}

char *report_ptr(void)
{
    return report;
}

int report_len(void)
{
    return report_position;
}

int compile(void)
{
    int entry_function = -1;

    reference_internal_helpers();

    reset_compiler_state();

    lex_source();

    if (!compilation_failed) {
        parse_program();
    }

    if (!compilation_failed) {
        initialize_global_objects();
    }

    if (!compilation_failed) {
        compile_functions();
    }

    if (!compilation_failed) {
        entry_function =
            find_entry_function();
    }

    if (
        !compilation_failed &&
        entry_function >= 0
    ) {
        execute_program(
            entry_function
        );
    }

    build_report();

    return
        compilation_failed
            ? 1
            : 0;
}

#ifndef __wasm__

static int read_source_file(
    FILE *input
)
{
    int bytes_read;

    bytes_read =
        (int)fread(
            source,
            1,
            MAX_SOURCE - 1,
            input
        );

    if (fgetc(input) != EOF) {
        fprintf(
            stderr,
            "Cinder: source exceeds the 64 KB limit.\n"
        );

        return -1;
    }

    source[bytes_read] = '\0';

    return bytes_read;
}

static int read_stdin_file(
    const char *path
)
{
    FILE *input;
    int bytes_read;

    input =
        fopen(
            path,
            "rb"
        );

    if (!input) {
        fprintf(
            stderr,
            "Cinder: could not open the stdin input file.\n"
        );

        return -1;
    }

    bytes_read =
        (int)fread(
            stdin_buffer,
            1,
            MAX_STDIN - 1,
            input
        );

    if (fgetc(input) != EOF) {
        fprintf(
            stderr,
            "Cinder: stdin input exceeds the 64 KB limit.\n"
        );

        fclose(input);
        return -1;
    }

    fclose(input);

    stdin_buffer[
        bytes_read
    ] = '\0';

    set_stdin_len(
        bytes_read
    );

    return bytes_read;
}

int main(
    int argument_count,
    char **arguments
)
{
    FILE *source_file;
    int status;

    if (
        argument_count < 2
    ) {
        fprintf(
            stderr,
            "Usage: cinder <source.c> [stdin.txt]\n"
        );

        return 2;
    }

    source_file =
        fopen(
            arguments[1],
            "rb"
        );

    if (!source_file) {
        fprintf(
            stderr,
            "Cinder: could not open the source file.\n"
        );

        return 2;
    }

    if (
        read_source_file(
            source_file
        ) < 0
    ) {
        fclose(
            source_file
        );

        return 2;
    }

    fclose(
        source_file
    );

    stdin_length = 0;
    stdin_position = 0;
    stdin_buffer[0] = '\0';

    if (
        argument_count > 2 &&
        read_stdin_file(
            arguments[2]
        ) < 0
    ) {
        return 2;
    }

    status = compile();

    puts(report);

    return status;
}

#endif

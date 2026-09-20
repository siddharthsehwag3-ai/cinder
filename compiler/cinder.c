/*
 * Cinder
 * A bounded C compiler and virtual machine.
 *
 * Pipeline:
 * source -> tokens -> syntax tree -> bytecode -> virtual machine
 */

#ifndef __wasm__
#include <stdio.h>
#endif

#define MAX_SOURCE      32768
#define MAX_TOKENS      8192
#define MAX_NODES       8192
#define MAX_CODE        16384
#define MAX_FUNCTIONS   128
#define MAX_SYMBOLS     2048
#define MAX_LOCALS      256
#define MAX_STACK       4096
#define MAX_CALL_DEPTH  128
#define MAX_STEPS       200000
#define MAX_OUTPUT      8192
#define MAX_REPORT      3000000

typedef struct {
    int kind;
    int start;
    int end;
    int value;
} Token;

typedef struct {
    int kind;
    int a;
    int b;
    int c;
    int d;
    int value;
    int token;
} Node;

typedef struct {
    int opcode;
    int argument;
    int extra;
    int node;
} Instruction;

typedef struct {
    int name_token;
    int body;
    int parameter_count;
    int local_count;
    int entry;
} Function;

typedef struct {
    int name_token;
    int slot;
    int depth;
} Symbol;

enum TokenKind {
    TOKEN_EOF = 256,
    TOKEN_NUMBER,
    TOKEN_CHARACTER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,

    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_LOGICAL_AND,
    TOKEN_LOGICAL_OR,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_SHIFT_LEFT,
    TOKEN_SHIFT_RIGHT,

    TOKEN_ADD_ASSIGN,
    TOKEN_SUBTRACT_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN
};

enum NodeKind {
    NODE_NONE = 0,
    NODE_NUMBER,
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
    NODE_DO_WHILE
};

enum Opcode {
    OP_PUSH,
    OP_LOAD,
    OP_STORE,
    OP_DROP,

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

    OP_JUMP,
    OP_JUMP_IF_ZERO,

    OP_CALL,
    OP_RETURN,
    OP_PRINT,
    OP_HALT
};

static char source[MAX_SOURCE];
static char report[MAX_REPORT];
static char program_output[MAX_OUTPUT];
static char error_message[256];

static Token tokens[MAX_TOKENS];
static Node nodes[MAX_NODES];
static Instruction instructions[MAX_CODE];
static Function functions[MAX_FUNCTIONS];
static Symbol symbols[MAX_SYMBOLS];

static int token_count;
static int node_count;
static int instruction_count;
static int function_count;
static int symbol_count;

static int parser_position;
static int scope_depth;
static int current_function;
static int nesting_depth;
static int loop_depth;

static int compilation_failed;
static int error_token;

static int report_position;
static int output_position;
static int execution_steps;
static int program_result;

static int value_stack[MAX_STACK];
static int stack_pointer;

static int frames[MAX_CALL_DEPTH][MAX_LOCALS];
static int return_addresses[MAX_CALL_DEPTH];
static int stack_bases[MAX_CALL_DEPTH];
static int frame_pointer;

static int break_markers[MAX_CODE];
static int continue_markers[MAX_CODE];

static int text_length(const char *text)
{
    int length = 0;

    while (text[length]) {
        length++;
    }

    return length;
}

static void text_copy(
    char *destination,
    const char *input
)
{
    int index = 0;

    while (input[index]) {
        destination[index] = input[index];
        index++;
    }

    destination[index] = '\0';
}

static int is_alpha(char character)
{
    return
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        character == '_';
}

static int is_digit(char character)
{
    return
        character >= '0' &&
        character <= '9';
}

static void fail(
    const char *message,
    int token_index
)
{
    if (compilation_failed) {
        return;
    }

    compilation_failed = 1;
    text_copy(error_message, message);

    if (token_index < 0) {
        token_index = 0;
    }

    if (token_index >= token_count) {
        token_index = token_count - 1;
    }

    error_token = token_index;
}

static void add_token(
    int kind,
    int start,
    int end,
    int value
)
{
    if (token_count >= MAX_TOKENS - 1) {
        fail(
            "The program contains too many tokens.",
            token_count - 1
        );

        return;
    }

    tokens[token_count].kind = kind;
    tokens[token_count].start = start;
    tokens[token_count].end = end;
    tokens[token_count].value = value;

    token_count++;
}

static int token_has_text(
    int token_index,
    const char *text
)
{
    int index = 0;

    while (text[index]) {
        if (
            tokens[token_index].start + index >=
            tokens[token_index].end
        ) {
            return 0;
        }

        if (
            source[
                tokens[token_index].start + index
            ] != text[index]
        ) {
            return 0;
        }

        index++;
    }

    return
        tokens[token_index].start + index ==
        tokens[token_index].end;
}

static int tokens_have_same_text(
    int first,
    int second
)
{
    int first_length =
        tokens[first].end -
        tokens[first].start;

    int second_length =
        tokens[second].end -
        tokens[second].start;

    int index;

    if (first_length != second_length) {
        return 0;
    }

    for (
        index = 0;
        index < first_length;
        index++
    ) {
        if (
            source[
                tokens[first].start + index
            ] !=
            source[
                tokens[second].start + index
            ]
        ) {
            return 0;
        }
    }

    return 1;
}

static int is_keyword_token(int token_index)
{
    return
        token_has_text(token_index, "int") ||
        token_has_text(token_index, "char") ||
        token_has_text(token_index, "void") ||
        token_has_text(token_index, "return") ||
        token_has_text(token_index, "if") ||
        token_has_text(token_index, "else") ||
        token_has_text(token_index, "while") ||
        token_has_text(token_index, "do") ||
        token_has_text(token_index, "for") ||
        token_has_text(token_index, "break") ||
        token_has_text(token_index, "continue");
}

static int escaped_character(
    char character,
    int token_index
)
{
    switch (character) {
        case '0':
            return 0;

        case 'n':
            return '\n';

        case 'r':
            return '\r';

        case 't':
            return '\t';

        case '\\':
            return '\\';

        case '\'':
            return '\'';

        case '"':
            return '"';

        default:
            fail(
                "Unsupported escape sequence.",
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
        char character = source[position++];

        if (
            character == ' ' ||
            character == '\r' ||
            character == '\n' ||
            character == '\t'
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
            position++;

            while (
                source[position] &&
                !(
                    source[position] == '*' &&
                    source[position + 1] == '/'
                )
            ) {
                position++;
            }

            if (!source[position]) {
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

                break;
            }

            position += 2;
            continue;
        }

        if (is_alpha(character)) {
            while (
                is_alpha(source[position]) ||
                is_digit(source[position])
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
            long long value =
                character - '0';

            while (is_digit(source[position])) {
                value =
                    value * 10 +
                    source[position] -
                    '0';

                position++;

                if (value > 2147483647LL) {
                    add_token(
                        TOKEN_NUMBER,
                        start,
                        position,
                        0
                    );

                    fail(
                        "Integer literal exceeds the signed 32-bit range.",
                        token_count - 1
                    );

                    break;
                }
            }

            if (!compilation_failed) {
                add_token(
                    TOKEN_NUMBER,
                    start,
                    position,
                    (int)value
                );
            }

            continue;
        }
                if (character == '\'') {
            int value;

            if (
                !source[position] ||
                source[position] == '\n' ||
                source[position] == '\''
            ) {
                add_token(
                    TOKEN_CHARACTER,
                    start,
                    position,
                    0
                );

                fail(
                    "Expected one character inside the character literal.",
                    token_count - 1
                );

                break;
            }

            if (source[position] == '\\') {
                position++;

                if (!source[position]) {
                    add_token(
                        TOKEN_CHARACTER,
                        start,
                        position,
                        0
                    );

                    fail(
                        "Unterminated character literal.",
                        token_count - 1
                    );

                    break;
                }

                value = escaped_character(
                    source[position],
                    token_count
                );

                position++;
            } else {
                value =
                    (unsigned char)source[position];

                position++;
            }

            if (source[position] != '\'') {
                add_token(
                    TOKEN_CHARACTER,
                    start,
                    position,
                    value
                );

                fail(
                    "Character literals must contain exactly one character.",
                    token_count - 1
                );

                break;
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
            while (
                source[position] &&
                source[position] != '"' &&
                source[position] != '\n'
            ) {
                if (
                    source[position] == '\\' &&
                    source[position + 1]
                ) {
                    position++;
                }

                position++;
            }

            if (source[position] != '"') {
                add_token(
                    TOKEN_STRING,
                    start,
                    position,
                    0
                );

                fail(
                    "Unterminated string literal.",
                    token_count - 1
                );

                break;
            }

            position++;

            add_token(
                TOKEN_STRING,
                start,
                position,
                0
            );

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
                kind = TOKEN_GREATER_EQUAL;
                position++;
            } else if (
                character == '<' &&
                source[position] == '<'
            ) {
                kind = TOKEN_SHIFT_LEFT;
                position++;
            } else if (
                character == '>' &&
                source[position] == '>'
            ) {
                kind = TOKEN_SHIFT_RIGHT;
                position++;
            } else if (
                character == '&' &&
                source[position] == '&'
            ) {
                kind = TOKEN_LOGICAL_AND;
                position++;
            } else if (
                character == '|' &&
                source[position] == '|'
            ) {
                kind = TOKEN_LOGICAL_OR;
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
                character == '+' &&
                source[position] == '='
            ) {
                kind = TOKEN_ADD_ASSIGN;
                position++;
            } else if (
                character == '-' &&
                source[position] == '='
            ) {
                kind = TOKEN_SUBTRACT_ASSIGN;
                position++;
            } else if (
                character == '*' &&
                source[position] == '='
            ) {
                kind = TOKEN_MULTIPLY_ASSIGN;
                position++;
            } else if (
                character == '/' &&
                source[position] == '='
            ) {
                kind = TOKEN_DIVIDE_ASSIGN;
                position++;
            } else if (
                character == '%' &&
                source[position] == '='
            ) {
                kind = TOKEN_MODULO_ASSIGN;
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
                    character == ';' ||
                    character == ','
                )
            ) {
                add_token(
                    kind,
                    start,
                    position,
                    0
                );

                fail(
                    "Unsupported character or language feature.",
                    token_count - 1
                );

                break;
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

static int is_at(int kind)
{
    return
        tokens[parser_position].kind ==
        kind;
}

static int take(int kind)
{
    if (!is_at(kind)) {
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

static int is_keyword(const char *text)
{
    return
        is_at(TOKEN_IDENTIFIER) &&
        token_has_text(
            parser_position,
            text
        );
}

static int is_type_keyword(void)
{
    return
        is_keyword("int") ||
        is_keyword("char");
}

static int create_node(
    int kind,
    int a,
    int b,
    int c,
    int d,
    int value,
    int token_index
)
{
    if (node_count >= MAX_NODES - 1) {
        fail(
            "The syntax tree is too large.",
            token_index
        );

        return 0;
    }

    node_count++;

    nodes[node_count].kind = kind;
    nodes[node_count].a = a;
    nodes[node_count].b = b;
    nodes[node_count].c = c;
    nodes[node_count].d = d;
    nodes[node_count].value = value;
    nodes[node_count].token = token_index;

    return node_count;
}

static int find_variable(int name_token)
{
    int index;

    for (
        index = symbol_count - 1;
        index >= 0;
        index--
    ) {
        if (
            tokens_have_same_text(
                symbols[index].name_token,
                name_token
            )
        ) {
            return symbols[index].slot;
        }
    }

    fail(
        "Variable has not been declared in this scope.",
        name_token
    );

    return 0;
}

static int add_variable(int name_token)
{
    int index;
    int slot;

    for (
        index = symbol_count - 1;
        index >= 0 &&
        symbols[index].depth == scope_depth;
        index--
    ) {
        if (
            tokens_have_same_text(
                symbols[index].name_token,
                name_token
            )
        ) {
            fail(
                "Variable is already declared in this scope.",
                name_token
            );

            return 0;
        }
    }

    if (
        symbol_count >= MAX_SYMBOLS ||
        functions[current_function].local_count >=
        MAX_LOCALS
    ) {
        fail(
            "The function contains too many local variables.",
            name_token
        );

        return 0;
    }

    slot =
        functions[current_function].local_count;

    functions[current_function].local_count++;

    symbols[symbol_count].name_token =
        name_token;

    symbols[symbol_count].slot = slot;
    symbols[symbol_count].depth =
        scope_depth;

    symbol_count++;

    return slot;
}

static int parse_expression(void);
static int parse_statement(void);
static int parse_primary(void)
{
    int starting_token =
        parser_position;

    int result = 0;

    nesting_depth++;

    if (nesting_depth > 128) {
        fail(
            "Expression nesting limit exceeded.",
            parser_position
        );

        nesting_depth--;
        return 0;
    }

    if (
        take(TOKEN_NUMBER) ||
        take(TOKEN_CHARACTER)
    ) {
        result = create_node(
            NODE_NUMBER,
            0,
            0,
            0,
            0,
            tokens[starting_token].value,
            starting_token
        );
    } else if (take(TOKEN_STRING)) {
        result = create_node(
            NODE_STRING,
            0,
            0,
            0,
            0,
            starting_token,
            starting_token
        );
    } else if (take('(')) {
        result = parse_expression();

        expect(
            ')',
            "Expected ')' after the expression."
        );
    } else if (
        is_at(TOKEN_IDENTIFIER) &&
        !is_keyword_token(parser_position)
    ) {
        parser_position++;

        if (take('(')) {
            int first_argument = 0;
            int last_argument = 0;
            int argument_count = 0;

            if (!is_at(')')) {
                do {
                    int expression =
                        parse_expression();

                    int argument =
                        create_node(
                            NODE_ARGUMENT,
                            expression,
                            0,
                            0,
                            0,
                            0,
                            starting_token
                        );

                    if (last_argument) {
                        nodes[last_argument].b =
                            argument;
                    } else {
                        first_argument =
                            argument;
                    }

                    last_argument = argument;
                    argument_count++;

                    if (argument_count > 32) {
                        fail(
                            "Functions support at most 32 arguments.",
                            starting_token
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

            result = create_node(
                NODE_CALL,
                first_argument,
                0,
                0,
                0,
                argument_count,
                starting_token
            );
        } else {
            result = create_node(
                NODE_VARIABLE,
                0,
                0,
                0,
                0,
                find_variable(starting_token),
                starting_token
            );
        }
    } else {
        fail(
            "Expected a value, variable, function call, or parenthesized expression.",
            parser_position
        );
    }

    if (
        !compilation_failed &&
        (
            is_at(TOKEN_INCREMENT) ||
            is_at(TOKEN_DECREMENT)
        )
    ) {
        int operator_kind =
            tokens[parser_position].kind;

        parser_position++;

        if (
            !result ||
            nodes[result].kind != NODE_VARIABLE
        ) {
            fail(
                "Postfix increment and decrement require a variable.",
                starting_token
            );
        }

        result = create_node(
            NODE_POSTFIX,
            result,
            0,
            0,
            0,
            operator_kind,
            starting_token
        );
    }

    nesting_depth--;
    return result;
}

static int parse_unary(void)
{
    if (
        is_at(TOKEN_INCREMENT) ||
        is_at(TOKEN_DECREMENT)
    ) {
        int operator_token =
            parser_position;

        int operator_kind;
        int operand;

        parser_position++;

        operator_kind =
            tokens[operator_token].kind;

        operand = parse_unary();

        if (
            !operand ||
            nodes[operand].kind != NODE_VARIABLE
        ) {
            fail(
                "Prefix increment and decrement require a variable.",
                operator_token
            );
        }

        return create_node(
            NODE_PREFIX,
            operand,
            0,
            0,
            0,
            operator_kind,
            operator_token
        );
    }

    if (
        is_at('!') ||
        is_at('-') ||
        is_at('+') ||
        is_at('~')
    ) {
        int operator_token =
            parser_position;

        int operator_kind;
        int operand;

        parser_position++;

        operator_kind =
            tokens[operator_token].kind;

        nesting_depth++;

        if (nesting_depth > 128) {
            fail(
                "Expression nesting limit exceeded.",
                operator_token
            );

            nesting_depth--;
            return 0;
        }

        operand = parse_unary();
        nesting_depth--;

        return create_node(
            NODE_UNARY,
            operand,
            0,
            0,
            0,
            operator_kind,
            operator_token
        );
    }

    return parse_primary();
}

static int operator_precedence(int kind)
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

static int parse_binary(
    int minimum_precedence
)
{
    int left = parse_unary();

    while (
        !compilation_failed &&
        operator_precedence(
            tokens[parser_position].kind
        ) >= minimum_precedence
    ) {
        int operator_token =
            parser_position;

        int operator_kind;
        int right;
        int precedence;

        parser_position++;

        operator_kind =
            tokens[operator_token].kind;

        precedence =
            operator_precedence(
                operator_kind
            );

        right = parse_binary(
            precedence + 1
        );

        left = create_node(
            NODE_BINARY,
            left,
            right,
            0,
            0,
            operator_kind,
            operator_token
        );
    }

    return left;
}

static int parse_conditional(void)
{
    int condition =
        parse_binary(1);

    if (
        !compilation_failed &&
        take('?')
    ) {
        int question_token =
            parser_position - 1;

        int when_true;
        int when_false;

        when_true =
            parse_expression();

        expect(
            ':',
            "Expected ':' in the conditional expression."
        );

        when_false =
            parse_conditional();

        return create_node(
            NODE_CONDITIONAL,
            condition,
            when_true,
            when_false,
            0,
            0,
            question_token
        );
    }

    return condition;
}

static int is_assignment_operator(
    int kind
)
{
    return
        kind == '=' ||
        kind == TOKEN_ADD_ASSIGN ||
        kind == TOKEN_SUBTRACT_ASSIGN ||
        kind == TOKEN_MULTIPLY_ASSIGN ||
        kind == TOKEN_DIVIDE_ASSIGN ||
        kind == TOKEN_MODULO_ASSIGN;
}

static int parse_expression(void)
{
    int left =
        parse_conditional();

    int assignment_kind =
        tokens[parser_position].kind;

    if (
        !compilation_failed &&
        is_assignment_operator(
            assignment_kind
        )
    ) {
        int assignment_token =
            parser_position;

        int right;

        parser_position++;

        if (
            !left ||
            nodes[left].kind != NODE_VARIABLE
        ) {
            fail(
                "Assignment requires a variable on the left.",
                assignment_token
            );
        }

        nesting_depth++;

        if (nesting_depth > 128) {
            fail(
                "Assignment nesting limit exceeded.",
                assignment_token
            );

            nesting_depth--;
            return left;
        }

        right = parse_expression();
        nesting_depth--;

        return create_node(
            NODE_ASSIGNMENT,
            left,
            right,
            0,
            0,
            assignment_kind,
            assignment_token
        );
    }

    return left;
}

static int parse_declaration(void)
{
    int declaration_token =
        parser_position;

    int name_token;
    int slot;
    int initializer = 0;

    parser_position++;

    if (
        !is_at(TOKEN_IDENTIFIER) ||
        is_keyword_token(parser_position)
    ) {
        fail(
            "Expected a variable name after the type.",
            parser_position
        );

        return 0;
    }

    name_token = parser_position;
    parser_position++;

    slot = add_variable(name_token);

    if (take('=')) {
        initializer =
            parse_expression();
    }

    expect(
        ';',
        "Expected ';' after the variable declaration."
    );

    return create_node(
        NODE_DECLARATION,
        initializer,
        0,
        0,
        0,
        slot,
        declaration_token
    );
}

static int parse_block(int nested)
{
    int block_token =
        parser_position;

    int first_link = 0;
    int last_link = 0;

    int saved_symbol_count =
        symbol_count;

    if (nested) {
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

        int link =
            create_node(
                NODE_LINK,
                statement,
                0,
                0,
                0,
                0,
                block_token
            );

        if (last_link) {
            nodes[last_link].b = link;
        } else {
            first_link = link;
        }

        last_link = link;
    }

    expect(
        '}',
        "Expected '}' to close the block."
    );

    if (nested) {
        scope_depth--;

        symbol_count =
            saved_symbol_count;
    }

    return create_node(
        NODE_BLOCK,
        first_link,
        0,
        0,
        0,
        0,
        block_token
    );
}
static int parse_statement_body(void)
{
    int statement_token =
        parser_position;

    if (is_at('{')) {
        return parse_block(1);
    }

    if (is_type_keyword()) {
        return parse_declaration();
    }

    if (is_keyword("return")) {
        int expression;

        parser_position++;

        expression =
            parse_expression();

        expect(
            ';',
            "Expected ';' after return."
        );

        return create_node(
            NODE_RETURN,
            expression,
            0,
            0,
            0,
            0,
            statement_token
        );
    }

    if (is_keyword("if")) {
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

        then_branch =
            parse_statement();

        if (is_keyword("else")) {
            parser_position++;

            else_branch =
                parse_statement();
        }

        return create_node(
            NODE_IF,
            condition,
            then_branch,
            else_branch,
            0,
            0,
            statement_token
        );
    }

    if (is_keyword("while")) {
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

        loop_depth++;
        body = parse_statement();
        loop_depth--;

        return create_node(
            NODE_WHILE,
            condition,
            body,
            0,
            0,
            0,
            statement_token
        );
    }

    if (is_keyword("do")) {
        int body;
        int condition;

        parser_position++;

        loop_depth++;
        body = parse_statement();
        loop_depth--;

        if (!is_keyword("while")) {
            fail(
                "Expected 'while' after the do-loop body.",
                parser_position
            );

            return 0;
        }

        parser_position++;

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
            "Expected ';' after the do-while loop."
        );

        return create_node(
            NODE_DO_WHILE,
            body,
            condition,
            0,
            0,
            0,
            statement_token
        );
    }

    if (is_keyword("for")) {
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

        if (is_type_keyword()) {
            initializer =
                parse_declaration();
        } else {
            if (!is_at(';')) {
                initializer =
                    create_node(
                        NODE_EXPRESSION,
                        parse_expression(),
                        0,
                        0,
                        0,
                        0,
                        statement_token
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
        }

        expect(
            ';',
            "Expected the second ';' in the for loop."
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
        body = parse_statement();
        loop_depth--;

        scope_depth--;
        symbol_count =
            saved_symbol_count;

        return create_node(
            NODE_FOR,
            initializer,
            condition,
            increment,
            body,
            0,
            statement_token
        );
    }

    if (
        is_keyword("break") ||
        is_keyword("continue")
    ) {
        int kind =
            is_keyword("break")
                ? NODE_BREAK
                : NODE_CONTINUE;

        parser_position++;

        if (!loop_depth) {
            fail(
                "This statement must be inside a loop.",
                statement_token
            );
        }

        expect(
            ';',
            "Expected ';' after the loop-control statement."
        );

        return create_node(
            kind,
            0,
            0,
            0,
            0,
            0,
            statement_token
        );
    }

    if (take(';')) {
        return create_node(
            NODE_BLOCK,
            0,
            0,
            0,
            0,
            0,
            statement_token
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
            expression,
            0,
            0,
            0,
            0,
            statement_token
        );
    }
}

static int parse_statement(void)
{
    int statement;

    nesting_depth++;

    if (nesting_depth > 128) {
        fail(
            "Statement nesting limit exceeded.",
            parser_position
        );

        nesting_depth--;
        return 0;
    }

    statement =
        parse_statement_body();

    nesting_depth--;
    return statement;
}

static void parse_program(void)
{
    while (
        !is_at(TOKEN_EOF) &&
        !compilation_failed
    ) {
        int function_name;
        int index;

        if (!is_type_keyword()) {
            fail(
                "Expected an int or char function definition.",
                parser_position
            );

            break;
        }

        parser_position++;

        if (
            !is_at(TOKEN_IDENTIFIER) ||
            is_keyword_token(parser_position)
        ) {
            fail(
                "Expected a function name.",
                parser_position
            );

            break;
        }

        function_name =
            parser_position;

        parser_position++;

        if (
            token_has_text(
                function_name,
                "printf"
            ) ||
            token_has_text(
                function_name,
                "print"
            )
        ) {
            fail(
                "This function name is reserved for output.",
                function_name
            );

            break;
        }

        for (
            index = 0;
            index < function_count;
            index++
        ) {
            if (
                tokens_have_same_text(
                    functions[index].name_token,
                    function_name
                )
            ) {
                fail(
                    "Function is already defined.",
                    function_name
                );
            }
        }

        if (function_count >= MAX_FUNCTIONS) {
            fail(
                "The program contains too many functions.",
                function_name
            );

            break;
        }

        current_function =
            function_count;

        function_count++;

        functions[current_function].name_token =
            function_name;

        functions[current_function].body = 0;
        functions[current_function].parameter_count = 0;
        functions[current_function].local_count = 0;
        functions[current_function].entry = 0;

        symbol_count = 0;
        scope_depth = 0;

        expect(
            '(',
            "Expected '(' after the function name."
        );

        if (is_keyword("void")) {
            parser_position++;
        } else if (!is_at(')')) {
            do {
                if (!is_type_keyword()) {
                    fail(
                        "Function parameters must have type int or char.",
                        parser_position
                    );

                    break;
                }

                parser_position++;

                if (
                    !is_at(TOKEN_IDENTIFIER) ||
                    is_keyword_token(parser_position)
                ) {
                    fail(
                        "Expected a parameter name.",
                        parser_position
                    );

                    break;
                }

                add_variable(
                    parser_position
                );

                parser_position++;

                functions[current_function]
                    .parameter_count++;

                if (
                    functions[current_function]
                        .parameter_count > 32
                ) {
                    fail(
                        "Functions support at most 32 parameters.",
                        parser_position
                    );
                }
            } while (
                take(',') &&
                !compilation_failed
            );
        }

        expect(
            ')',
            "Expected ')' after the function parameters."
        );

        functions[current_function].body =
            parse_block(0);
    }
}
static int emit(
    int opcode,
    int argument,
    int extra,
    int node_index
)
{
    if (instruction_count >= MAX_CODE) {
        fail(
            "Program instruction limit exceeded.",
            nodes[node_index].token
        );

        return 0;
    }

    instructions[instruction_count].opcode =
        opcode;

    instructions[instruction_count].argument =
        argument;

    instructions[instruction_count].extra =
        extra;

    instructions[instruction_count].node =
        node_index;

    instruction_count++;

    return instruction_count - 1;
}

static int find_function(int name_token)
{
    int index;

    for (
        index = 0;
        index < function_count;
        index++
    ) {
        if (
            tokens_have_same_text(
                functions[index].name_token,
                name_token
            )
        ) {
            return index;
        }
    }

    return -1;
}

static int binary_opcode(
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
            return OP_BITWISE_AND;

        case '|':
            return OP_BITWISE_OR;

        case '^':
            return OP_BITWISE_XOR;

        case TOKEN_SHIFT_LEFT:
            return OP_SHIFT_LEFT;

        case TOKEN_SHIFT_RIGHT:
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

static void compile_node(int node_index)
{
    Node node;
    int patch;
    int second_patch;

    if (
        !node_index ||
        compilation_failed
    ) {
        return;
    }

    node = nodes[node_index];

    switch (node.kind) {
        case NODE_NUMBER:
            emit(
                OP_PUSH,
                node.value,
                0,
                node_index
            );
            break;

        case NODE_STRING:
            fail(
                "String literals are only supported as printf formats.",
                node.token
            );
            break;

        case NODE_VARIABLE:
            emit(
                OP_LOAD,
                node.value,
                0,
                node_index
            );
            break;

        case NODE_UNARY:
            compile_node(node.a);

            if (node.value == '!') {
                emit(
                    OP_NOT,
                    0,
                    0,
                    node_index
                );
            } else if (node.value == '-') {
                emit(
                    OP_NEGATE,
                    0,
                    0,
                    node_index
                );
            } else if (node.value == '~') {
                emit(
                    OP_BITWISE_NOT,
                    0,
                    0,
                    node_index
                );
            }

            break;

        case NODE_BINARY:
            if (
                node.value == TOKEN_LOGICAL_AND ||
                node.value == TOKEN_LOGICAL_OR
            ) {
                compile_node(node.a);

                patch = emit(
                    OP_JUMP_IF_ZERO,
                    0,
                    0,
                    node_index
                );

                if (
                    node.value ==
                    TOKEN_LOGICAL_OR
                ) {
                    emit(
                        OP_PUSH,
                        1,
                        0,
                        node_index
                    );

                    second_patch = emit(
                        OP_JUMP,
                        0,
                        0,
                        node_index
                    );

                    instructions[patch].argument =
                        instruction_count;

                    compile_node(node.b);

                    emit(
                        OP_BOOLEAN,
                        0,
                        0,
                        node_index
                    );

                    instructions[
                        second_patch
                    ].argument =
                        instruction_count;
                } else {
                    compile_node(node.b);

                    emit(
                        OP_BOOLEAN,
                        0,
                        0,
                        node_index
                    );

                    second_patch = emit(
                        OP_JUMP,
                        0,
                        0,
                        node_index
                    );

                    instructions[patch].argument =
                        instruction_count;

                    emit(
                        OP_PUSH,
                        0,
                        0,
                        node_index
                    );

                    instructions[
                        second_patch
                    ].argument =
                        instruction_count;
                }
            } else {
                compile_node(node.a);
                compile_node(node.b);

                emit(
                    binary_opcode(node.value),
                    0,
                    0,
                    node_index
                );
            }

            break;

        case NODE_CONDITIONAL:
            compile_node(node.a);

            patch = emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );

            compile_node(node.b);

            second_patch = emit(
                OP_JUMP,
                0,
                0,
                node_index
            );

            instructions[patch].argument =
                instruction_count;

            compile_node(node.c);

            instructions[
                second_patch
            ].argument =
                instruction_count;
            break;

        case NODE_ASSIGNMENT:
            if (node.value == '=') {
                compile_node(node.b);
            } else {
                emit(
                    OP_LOAD,
                    nodes[node.a].value,
                    0,
                    node_index
                );

                compile_node(node.b);

                emit(
                    binary_opcode(node.value),
                    0,
                    0,
                    node_index
                );
            }

            emit(
                OP_STORE,
                nodes[node.a].value,
                0,
                node_index
            );
            break;

        case NODE_PREFIX:
            emit(
                OP_LOAD,
                nodes[node.a].value,
                0,
                node_index
            );

            emit(
                OP_PUSH,
                1,
                0,
                node_index
            );

            emit(
                node.value == TOKEN_INCREMENT
                    ? OP_ADD
                    : OP_SUBTRACT,
                0,
                0,
                node_index
            );

            emit(
                OP_STORE,
                nodes[node.a].value,
                0,
                node_index
            );
            break;

        case NODE_POSTFIX:
            emit(
                OP_LOAD,
                nodes[node.a].value,
                0,
                node_index
            );

            emit(
                OP_LOAD,
                nodes[node.a].value,
                0,
                node_index
            );

            emit(
                OP_PUSH,
                1,
                0,
                node_index
            );

            emit(
                node.value == TOKEN_INCREMENT
                    ? OP_ADD
                    : OP_SUBTRACT,
                0,
                0,
                node_index
            );

            emit(
                OP_STORE,
                nodes[node.a].value,
                0,
                node_index
            );

            emit(
                OP_DROP,
                0,
                0,
                node_index
            );
            break;
                    case NODE_CALL:
        {
            int function_index =
                find_function(node.token);

            int argument = node.a;

            if (
                token_has_text(
                    node.token,
                    "printf"
                )
            ) {
                int format_token;

                if (
                    !argument ||
                    nodes[
                        nodes[argument].a
                    ].kind != NODE_STRING
                ) {
                    fail(
                        "printf requires a literal format string first.",
                        node.token
                    );

                    break;
                }

                format_token =
                    nodes[
                        nodes[argument].a
                    ].token;

                argument =
                    nodes[argument].b;

                while (argument) {
                    compile_node(
                        nodes[argument].a
                    );

                    argument =
                        nodes[argument].b;
                }

                emit(
                    OP_PRINT,
                    format_token,
                    node.value - 1,
                    node_index
                );
            } else if (
                token_has_text(
                    node.token,
                    "print"
                )
            ) {
                if (node.value != 1) {
                    fail(
                        "print accepts exactly one integer.",
                        node.token
                    );

                    break;
                }

                compile_node(
                    nodes[argument].a
                );

                emit(
                    OP_PRINT,
                    -1,
                    1,
                    node_index
                );
            } else {
                if (function_index < 0) {
                    fail(
                        "Unknown function.",
                        node.token
                    );

                    break;
                }

                if (
                    functions[function_index]
                        .parameter_count !=
                    node.value
                ) {
                    fail(
                        "Function argument count does not match its definition.",
                        node.token
                    );

                    break;
                }

                while (argument) {
                    compile_node(
                        nodes[argument].a
                    );

                    argument =
                        nodes[argument].b;
                }

                emit(
                    OP_CALL,
                    function_index,
                    node.value,
                    node_index
                );
            }

            break;
        }

        case NODE_BLOCK:
        {
            int link = node.a;

            while (
                link &&
                !compilation_failed
            ) {
                compile_node(
                    nodes[link].a
                );

                link =
                    nodes[link].b;
            }

            break;
        }

        case NODE_DECLARATION:
            if (node.a) {
                compile_node(node.a);
            } else {
                emit(
                    OP_PUSH,
                    0,
                    0,
                    node_index
                );
            }

            emit(
                OP_STORE,
                node.value,
                0,
                node_index
            );

            emit(
                OP_DROP,
                0,
                0,
                node_index
            );
            break;

        case NODE_RETURN:
            compile_node(node.a);

            emit(
                OP_RETURN,
                0,
                0,
                node_index
            );
            break;

        case NODE_EXPRESSION:
            compile_node(node.a);

            emit(
                OP_DROP,
                0,
                0,
                node_index
            );
            break;

        case NODE_IF:
            compile_node(node.a);

            patch = emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );

            compile_node(node.b);

            second_patch = emit(
                OP_JUMP,
                0,
                0,
                node_index
            );

            instructions[patch].argument =
                instruction_count;

            compile_node(node.c);

            instructions[
                second_patch
            ].argument =
                instruction_count;
            break;

        case NODE_DO_WHILE:
        {
            int saved_loop_depth =
                ++loop_depth;

            int loop_start =
                instruction_count;

            int condition_start;
            int marker_start =
                loop_start;

            int index;

            compile_node(node.a);

            condition_start =
                instruction_count;

            compile_node(node.b);

            patch = emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );

            emit(
                OP_JUMP,
                loop_start,
                0,
                node_index
            );

            instructions[patch].argument =
                instruction_count;

            for (
                index = marker_start;
                index < instruction_count;
                index++
            ) {
                if (
                    break_markers[index] ==
                    saved_loop_depth
                ) {
                    instructions[index].argument =
                        instruction_count;

                    break_markers[index] = 0;
                }

                if (
                    continue_markers[index] ==
                    saved_loop_depth
                ) {
                    instructions[index].argument =
                        condition_start;

                    continue_markers[index] = 0;
                }
            }

            loop_depth--;
            break;
        }

        case NODE_WHILE:
        case NODE_FOR:
        {
            int saved_loop_depth;
            int loop_start;
            int increment_start;
            int marker_start;
            int index;

            saved_loop_depth =
                ++loop_depth;

            if (node.kind == NODE_FOR) {
                compile_node(node.a);
            }

            loop_start =
                instruction_count;

            if (node.kind == NODE_WHILE) {
                compile_node(node.a);
            } else if (node.b) {
                compile_node(node.b);
            } else {
                emit(
                    OP_PUSH,
                    1,
                    0,
                    node_index
                );
            }

            patch = emit(
                OP_JUMP_IF_ZERO,
                0,
                0,
                node_index
            );

            marker_start =
                loop_start;

            if (node.kind == NODE_WHILE) {
                compile_node(node.b);
            } else {
                compile_node(node.d);
            }

            increment_start =
                instruction_count;

            if (
                node.kind == NODE_FOR &&
                node.c
            ) {
                compile_node(node.c);

                emit(
                    OP_DROP,
                    0,
                    0,
                    node_index
                );
            }

            emit(
                OP_JUMP,
                loop_start,
                0,
                node_index
            );

            instructions[patch].argument =
                instruction_count;

            for (
                index = marker_start;
                index < instruction_count;
                index++
            ) {
                if (
                    break_markers[index] ==
                    saved_loop_depth
                ) {
                    instructions[index].argument =
                        instruction_count;

                    break_markers[index] = 0;
                }

                if (
                    continue_markers[index] ==
                    saved_loop_depth
                ) {
                    instructions[index].argument =
                        node.kind == NODE_FOR
                            ? increment_start
                            : loop_start;

                    continue_markers[index] = 0;
                }
            }

            loop_depth--;
            break;
        }

        case NODE_BREAK:
            patch = emit(
                OP_JUMP,
                0,
                0,
                node_index
            );

            break_markers[patch] =
                loop_depth;
            break;

        case NODE_CONTINUE:
            patch = emit(
                OP_JUMP,
                0,
                0,
                node_index
            );

            continue_markers[patch] =
                loop_depth;
            break;

        default:
            break;
    }
}
static void output_character(
    char character,
    int token_index
)
{
    if (output_position >= MAX_OUTPUT - 1) {
        fail(
            "Program output exceeded the 8 KB limit.",
            token_index
        );

        return;
    }

    program_output[output_position] =
        character;

    output_position++;

    program_output[output_position] =
        '\0';
}

static void output_integer(
    int value,
    int token_index
)
{
    char digits[16];
    int count = 0;
    long long absolute = value;

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

static void output_unsigned(
    unsigned int value,
    unsigned int base,
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
            alphabet[value % base];

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

static void push_value(
    int value,
    int token_index
)
{
    if (stack_pointer >= MAX_STACK) {
        fail(
            "Virtual-machine stack limit exceeded.",
            token_index
        );

        return;
    }

    value_stack[stack_pointer] =
        value;

    stack_pointer++;
}

static int pop_value(int token_index)
{
    if (!stack_pointer) {
        fail(
            "Internal virtual-machine stack underflow.",
            token_index
        );

        return 0;
    }

    stack_pointer--;

    return value_stack[stack_pointer];
}

static void execute_program(
    int entry_function
)
{
    int program_counter =
        functions[entry_function].entry;

    int index;

    stack_pointer = 0;
    frame_pointer = 0;

    for (
        index = 0;
        index < MAX_LOCALS;
        index++
    ) {
        frames[0][index] = 0;
    }

    while (!compilation_failed) {
        Instruction instruction;
        int token_index;
        int left;
        int right;
        long long value;

        if (
            program_counter < 0 ||
            program_counter >= instruction_count
        ) {
            fail(
                "Invalid virtual-machine instruction address.",
                0
            );

            break;
        }

        if (++execution_steps > MAX_STEPS) {
            int current_node =
                instructions[
                    program_counter
                ].node;

            fail(
                "Execution stopped after 200,000 instructions. "
                "The program may contain an infinite loop.",
                current_node
                    ? nodes[current_node].token
                    : 0
            );

            break;
        }

        instruction =
            instructions[program_counter];

        program_counter++;

        token_index =
            nodes[instruction.node].token;

        switch (instruction.opcode) {
            case OP_PUSH:
                push_value(
                    instruction.argument,
                    token_index
                );
                break;

            case OP_LOAD:
                push_value(
                    frames[frame_pointer]
                          [instruction.argument],
                    token_index
                );
                break;

            case OP_STORE:
                if (!stack_pointer) {
                    fail(
                        "Internal empty store operation.",
                        token_index
                    );
                } else {
                    frames[frame_pointer]
                          [instruction.argument] =
                        value_stack[
                            stack_pointer - 1
                        ];
                }
                break;

            case OP_DROP:
                pop_value(token_index);
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
                    pop_value(token_index);

                left =
                    pop_value(token_index);

                value = 0;

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
                    left == (-2147483647 - 1) &&
                    right == -1
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

                switch (instruction.opcode) {
                    case OP_ADD:
                        value =
                            (long long)left +
                            right;
                        break;

                    case OP_SUBTRACT:
                        value =
                            (long long)left -
                            right;
                        break;

                    case OP_MULTIPLY:
                        value =
                            (long long)left *
                            right;
                        break;

                    case OP_DIVIDE:
                        value =
                            left / right;
                        break;

                    case OP_MODULO:
                        value =
                            left % right;
                        break;

                    case OP_BITWISE_AND:
                        value =
                            left & right;
                        break;

                    case OP_BITWISE_OR:
                        value =
                            left | right;
                        break;

                    case OP_BITWISE_XOR:
                        value =
                            left ^ right;
                        break;

                    case OP_SHIFT_LEFT:
                        value =
                            (int)(
                                (unsigned int)left
                                << right
                            );
                        break;

                    case OP_SHIFT_RIGHT:
                        value =
                            left >> right;
                        break;

                    case OP_EQUAL:
                        value =
                            left == right;
                        break;

                    case OP_NOT_EQUAL:
                        value =
                            left != right;
                        break;

                    case OP_LESS:
                        value =
                            left < right;
                        break;

                    case OP_LESS_EQUAL:
                        value =
                            left <= right;
                        break;

                    case OP_GREATER:
                        value =
                            left > right;
                        break;

                    case OP_GREATER_EQUAL:
                        value =
                            left >= right;
                        break;

                    default:
                        break;
                }

                if (
                    value > 2147483647LL ||
                    value < -2147483648LL
                ) {
                    fail(
                        "Signed integer overflow.",
                        token_index
                    );

                    break;
                }

                push_value(
                    (int)value,
                    token_index
                );
                break;

            case OP_NOT:
                left =
                    pop_value(token_index);

                push_value(
                    !left,
                    token_index
                );
                break;

            case OP_BOOLEAN:
                left =
                    pop_value(token_index);

                push_value(
                    !!left,
                    token_index
                );
                break;

            case OP_NEGATE:
                left =
                    pop_value(token_index);

                if (
                    left ==
                    (-2147483647 - 1)
                ) {
                    fail(
                        "Signed integer overflow.",
                        token_index
                    );
                } else {
                    push_value(
                        -left,
                        token_index
                    );
                }
                break;

            case OP_BITWISE_NOT:
                left =
                    pop_value(token_index);

                push_value(
                    ~left,
                    token_index
                );
                break;
                            case OP_JUMP:
                program_counter =
                    instruction.argument;
                break;

            case OP_JUMP_IF_ZERO:
                left =
                    pop_value(token_index);

                if (!left) {
                    program_counter =
                        instruction.argument;
                }
                break;

            case OP_CALL:
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

                frame_pointer++;

                return_addresses[frame_pointer] =
                    program_counter;

                stack_bases[frame_pointer] =
                    stack_pointer -
                    instruction.extra;

                for (
                    index = 0;
                    index < MAX_LOCALS;
                    index++
                ) {
                    frames[frame_pointer][index] =
                        0;
                }

                for (
                    index =
                        instruction.extra - 1;
                    index >= 0;
                    index--
                ) {
                    frames[frame_pointer][index] =
                        pop_value(token_index);
                }

                program_counter =
                    functions[
                        instruction.argument
                    ].entry;
                break;

            case OP_RETURN:
                left =
                    pop_value(token_index);

                if (frame_pointer == 0) {
                    program_result = left;
                    return;
                }

                stack_pointer =
                    stack_bases[frame_pointer];

                program_counter =
                    return_addresses[
                        frame_pointer
                    ];

                frame_pointer--;

                push_value(
                    left,
                    token_index
                );
                break;

            case OP_PRINT:
            {
                int argument_base =
                    stack_pointer -
                    instruction.extra;

                int output_before =
                    output_position;

                int used_arguments = 0;

                if (argument_base < 0) {
                    fail(
                        "Internal output stack error.",
                        token_index
                    );

                    break;
                }

                if (instruction.argument < 0) {
                    output_integer(
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
                    Token format =
                        tokens[
                            instruction.argument
                        ];

                    for (
                        index = format.start + 1;
                        index < format.end - 1 &&
                        !compilation_failed;
                        index++
                    ) {
                        char character =
                            source[index];

                        if (character == '\\') {
                            character =
                                source[++index];

                            if (character == 'n') {
                                character = '\n';
                            } else if (
                                character == 't'
                            ) {
                                character = '\t';
                            } else if (
                                character == 'r'
                            ) {
                                character = '\r';
                            } else if (
                                character == '0'
                            ) {
                                character = '\0';
                            } else if (
                                character != '\\' &&
                                character != '"'
                            ) {
                                fail(
                                    "Unsupported string escape.",
                                    instruction.argument
                                );

                                break;
                            }

                            output_character(
                                character,
                                token_index
                            );
                        } else if (
                            character == '%'
                        ) {
                            int argument_value;

                            character =
                                source[++index];

                            if (character == '%') {
                                output_character(
                                    '%',
                                    token_index
                                );

                                continue;
                            }

                            if (
                                character != 'd' &&
                                character != 'i' &&
                                character != 'u' &&
                                character != 'x' &&
                                character != 'X' &&
                                character != 'o' &&
                                character != 'c'
                            ) {
                                fail(
                                    "Supported printf formats are %d, %i, %u, %x, %X, %o, %c and %%.",
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
                                value_stack[
                                    argument_base +
                                    used_arguments
                                ];

                            used_arguments++;

                            if (
                                character == 'd' ||
                                character == 'i'
                            ) {
                                output_integer(
                                    argument_value,
                                    token_index
                                );
                            } else if (
                                character == 'u'
                            ) {
                                output_unsigned(
                                    (unsigned int)
                                        argument_value,
                                    10,
                                    0,
                                    token_index
                                );
                            } else if (
                                character == 'x'
                            ) {
                                output_unsigned(
                                    (unsigned int)
                                        argument_value,
                                    16,
                                    0,
                                    token_index
                                );
                            } else if (
                                character == 'X'
                            ) {
                                output_unsigned(
                                    (unsigned int)
                                        argument_value,
                                    16,
                                    1,
                                    token_index
                                );
                            } else if (
                                character == 'o'
                            ) {
                                output_unsigned(
                                    (unsigned int)
                                        argument_value,
                                    8,
                                    0,
                                    token_index
                                );
                            } else if (
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
                            output_character(
                                character,
                                token_index
                            );
                        }
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

            default:
                fail(
                    "Unknown bytecode instruction.",
                    token_index
                );
                break;
        }
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
        report[report_position] =
            character;

        report_position++;
    }
}

static void report_text(
    const char *text
)
{
    while (*text) {
        report_character(*text);
        text++;
    }
}

static void report_integer(int value)
{
    char digits[20];
    int count = 0;
    long long absolute = value;

    if (absolute < 0) {
        report_character('-');
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
            (unsigned char)text[index];

        if (
            character == '"' ||
            character == '\\'
        ) {
            report_character('\\');

            report_character(
                (char)character
            );
        } else if (character == '\n') {
            report_text("\\n");
        } else if (character == '\r') {
            report_text("\\r");
        } else if (character == '\t') {
            report_text("\\t");
        } else if (character < 32) {
            const char *hex =
                "0123456789abcdef";

            report_text("\\u00");

            report_character(
                hex[character >> 4]
            );

            report_character(
                hex[character & 15]
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
    "Statement",
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
    "Do while"
};

static const char *opcode_names[] = {
    "PUSH",
    "LOAD",
    "STORE",
    "DROP",

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

    "JUMP",
    "JUMP_IF_ZERO",

    "CALL",
    "RETURN",
    "PRINT",
    "HALT"
};
static void build_report(void)
{
    int index;

    report_position = 0;

    report_text("{\"ok\":");

    report_text(
        compilation_failed
            ? "false"
            : "true"
    );

    report_text(",\"error\":");

    report_quoted(
        error_message,
        text_length(error_message)
    );

    report_text(",\"errorStart\":");

    report_integer(
        compilation_failed
            ? tokens[error_token].start
            : 0
    );

    report_text(",\"errorEnd\":");

    report_integer(
        compilation_failed
            ? tokens[error_token].end
            : 0
    );

    report_text(",\"result\":");
    report_integer(program_result);

    report_text(",\"steps\":");
    report_integer(execution_steps);

    report_text(",\"stdout\":");

    report_quoted(
        program_output,
        output_position
    );

    report_text(",\"tokens\":[");

    for (
        index = 0;
        index < token_count - 1;
        index++
    ) {
        const char *kind;

        if (index) {
            report_character(',');
        }

        if (
            tokens[index].kind ==
            TOKEN_NUMBER
        ) {
            kind = "number";
        } else if (
            tokens[index].kind ==
            TOKEN_CHARACTER
        ) {
            kind = "character";
        } else if (
            tokens[index].kind ==
            TOKEN_STRING
        ) {
            kind = "string";
        } else if (
            tokens[index].kind ==
            TOKEN_IDENTIFIER
        ) {
            kind =
                is_keyword_token(index)
                    ? "keyword"
                    : "identifier";
        } else {
            kind = "symbol";
        }

        report_text("{\"kind\":");

        report_quoted(
            kind,
            text_length(kind)
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

        report_character('}');
    }

    report_text("],\"nodes\":[");

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

        if (index > 1) {
            report_character(',');
        }

        children[0] = node.a;
        children[1] = node.b;
        children[2] = node.c;
        children[3] = node.d;

        report_text("{\"id\":");
        report_integer(index);

        report_text(",\"kind\":");

        report_quoted(
            node_names[node.kind],
            text_length(
                node_names[node.kind]
            )
        );

        report_text(",\"token\":");
        report_integer(node.token);

        report_text(",\"value\":");
        report_integer(node.value);

        report_text(",\"children\":[");

        for (
            child_index = 0;
            child_index < 4;
            child_index++
        ) {
            if (children[child_index]) {
                if (has_child) {
                    report_character(',');
                }

                report_integer(
                    children[child_index]
                );

                has_child = 1;
            }
        }

        report_text("]}");
    }

    report_text("],\"functions\":[");

    for (
        index = 0;
        index < function_count;
        index++
    ) {
        Token name =
            tokens[
                functions[index].name_token
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

        report_text(",\"locals\":");

        report_integer(
            functions[index].local_count
        );

        report_text(",\"parameters\":");

        report_integer(
            functions[index]
                .parameter_count
        );

        report_character('}');
    }

    report_text("],\"instructions\":[");

    for (
        index = 0;
        index < instruction_count;
        index++
    ) {
        if (index) {
            report_character(',');
        }

        report_text("{\"index\":");
        report_integer(index);

        report_text(",\"op\":");

        report_quoted(
            opcode_names[
                instructions[index].opcode
            ],
            text_length(
                opcode_names[
                    instructions[index].opcode
                ]
            )
        );

        report_text(",\"arg\":");

        report_integer(
            instructions[index].argument
        );

        report_text(",\"extra\":");

        report_integer(
            instructions[index].extra
        );

        report_text(",\"node\":");

        report_integer(
            instructions[index].node
        );

        report_character('}');
    }

    report_text("]}");

    report[report_position] = '\0';
}

char *input_ptr(void)
{
    return source;
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
    int index;
    int entry_function = -1;

    token_count = 0;
    node_count = 0;
    instruction_count = 0;
    function_count = 0;
    symbol_count = 0;

    parser_position = 0;
    scope_depth = 0;
    current_function = 0;
    nesting_depth = 0;
    loop_depth = 0;

    compilation_failed = 0;
    error_token = 0;

    report_position = 0;
    output_position = 0;
    execution_steps = 0;
    program_result = 0;

    stack_pointer = 0;
    frame_pointer = 0;

    error_message[0] = '\0';
    program_output[0] = '\0';

    for (
        index = 0;
        index < MAX_CODE;
        index++
    ) {
        break_markers[index] = 0;
        continue_markers[index] = 0;
    }

    lex_source();

    if (!compilation_failed) {
        parse_program();
    }

    for (
        index = 0;
        index < function_count &&
        !compilation_failed;
        index++
    ) {
        if (
            token_has_text(
                functions[index].name_token,
                "main"
            )
        ) {
            entry_function = index;
        }

        functions[index].entry =
            instruction_count;

        compile_node(
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
    }

    if (
        !compilation_failed &&
        entry_function < 0
    ) {
        fail(
            "Define int main() as the program entry point.",
            0
        );
    }

    if (
        !compilation_failed &&
        functions[entry_function]
            .parameter_count
    ) {
        fail(
            "main must not have parameters.",
            functions[entry_function]
                .name_token
        );
    }

    if (!compilation_failed) {
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

int main(
    int argument_count,
    char **arguments
)
{
    FILE *input = stdin;
    int bytes_read;
    int status;

    if (argument_count > 1) {
        input = fopen(
            arguments[1],
            "rb"
        );

        if (!input) {
            fprintf(
                stderr,
                "Cinder: could not open the source file.\n"
            );

            return 2;
        }
    }

    bytes_read = (int)fread(
        source,
        1,
        MAX_SOURCE - 1,
        input
    );

    if (fgetc(input) != EOF) {
        fprintf(
            stderr,
            "Cinder: source exceeds the 32 KB limit.\n"
        );

        if (input != stdin) {
            fclose(input);
        }

        return 2;
    }

    source[bytes_read] = '\0';

    if (input != stdin) {
        fclose(input);
    }

    status = compile();

    puts(report);

    return status;
}

#endif
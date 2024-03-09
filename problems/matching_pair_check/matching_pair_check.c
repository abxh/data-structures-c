#include <stdio.h>

typedef enum { DEFAULT_SYMBOL_ENUM, LBRACKET, LCURLY, LPARAN, RBRACKET, RCURLY, RPARAN } SYMBOL_ENUM;

SYMBOL_ENUM encode_symbol(char c) {
    switch (c) {
    case '[':
        return LBRACKET;
    case '{':
        return LCURLY;
    case '(':
        return LPARAN;
    case ']':
        return RBRACKET;
    case '}':
        return RCURLY;
    case ')':
        return RPARAN;
    default:
        break;
    }
    return DEFAULT_SYMBOL_ENUM;
}

char decode_symbol(SYMBOL_ENUM symb) {
    switch (symb) {
    case LBRACKET:
        return '[';
    case LCURLY:
        return '{';
    case LPARAN:
        return '(';
    case RBRACKET:
        return ']';
    case RCURLY:
        return '}';
    case RPARAN:
        return ')';
    default:
        break;
    }
    return ' ';
}

SYMBOL_ENUM matching_symbol(SYMBOL_ENUM symb) {
    switch (symb) {
    case LBRACKET:
        return RBRACKET;
    case LCURLY:
        return RCURLY;
    case LPARAN:
        return RPARAN;
    case RBRACKET:
        return LBRACKET;
    case RCURLY:
        return LCURLY;
    case RPARAN:
        return LPARAN;
    default:
        return DEFAULT_SYMBOL_ENUM;
    }
}

#define VALUE_NAME symb
#define VALUE_TYPE SYMBOL_ENUM
#include "../../src/stack.h"

int main(void) {
    puts("Input line:");

    char* str = NULL;
    size_t str_buffersize = 0;
    ssize_t n = getline(&str, &str_buffersize, stdin);
    if (n < 0) {
        return 1;
    }

    Stack* stack_p;
    if (!stack_init_symb(&stack_p)) {
        free(str);
        return 1;
    }

    bool no_errors = true;
    ssize_t i = 0;

    for (i = 0; i < n; i++) {
        switch (str[i]) {
        case '(':
        case '{':
        case '[': {
            SYMBOL_ENUM a = encode_symbol(str[i]);
            stack_push_symb(stack_p, encode_symbol(a));
        } break;
        case ')':
        case '}':
        case ']':
            if (stack_isempty(stack_p)) {
                no_errors = false;
                break;
            }
            SYMBOL_ENUM a = encode_symbol(str[i]);
            SYMBOL_ENUM b = matching_symbol(stack_pop_symb(stack_p));
            no_errors = a == b;
        }
        if (!no_errors) {
            break;
        }
    }

    if (no_errors) {
        printf("\nno errors : )\n");
    } else {
        if (i <= 8) {
            printf("%*c error at\n", (int)i + 1, '^');
        } else {
            printf("%*s ^\n", (int)i - 1, "error at");
        }
    }

    stack_deinit(&stack_p);
    free(str);

    return 0;
}

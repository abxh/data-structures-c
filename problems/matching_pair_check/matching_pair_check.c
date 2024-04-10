#include <stdio.h> // getline, stdin, puts, printf, stdin
#include <stdbool.h> // true, false, bool
#include <stdlib.h> // free, size_t, ssize_t, NULL

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

typedef SYMBOL_ENUM symb;
#define VALUE_TYPE symb
#include "T_stack.h" // int_stack_*, T_stack_for_each

int main(void) {
    puts("Input line:");

    char* str = NULL;
    size_t str_buffersize = 0;
    ssize_t n = getline(&str, &str_buffersize, stdin);
    if (n < 0) {
        return 1;
    }

    symb_stack_type* stack_p = NULL;
    if (!symb_stack_init(&stack_p, n)) {
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
            symb_stack_push(stack_p, encode_symbol(str[i]));
        } break;
        case ')':
        case '}':
        case ']':
            if (symb_stack_is_empty(stack_p)) {
                no_errors = false;
                break;
            }
            no_errors = encode_symbol(str[i]) == matching_symbol(symb_stack_pop(stack_p));
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

    symb_stack_deinit(&stack_p);
    free(str);

    return 0;
}

#include <stdio.h>

#include "stack.h"

CREATE_STACK_INLINE_FUNCTIONS(int, int) // name, type

typedef enum {
    LBRACKET = 0,
    LCURLY,
    LPARAN,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    BACK_QUOTE,
} SIGN_ENUM;

char get_sign(SIGN_ENUM se) {
    switch (se) {
    case LBRACKET:
        return '[';
    case LCURLY:
        return '{';
    case LPARAN:
        return '(';
    case SINGLE_QUOTE:
        return '\'';
    case DOUBLE_QUOTE:
        return '"';
    case BACK_QUOTE:
        return '`';
    }
}

int main(void) {
    Stack stack = stack_empty;

    puts("Input line:");

    char* str = NULL;
    size_t str_buffersize = 0;
    ssize_t n = getline(&str, &str_buffersize, stdin);
    if (n < 0) {
        return 1;
    }

#ifdef DEBUG
    putchar('\n');
#endif

    bool no_errors = true;
    bool single_quote_used = false;
    bool double_quote_used = false;
    bool back_quote_used = false;
    bool comment_sign_used = false;
    ssize_t i = 0;
    int left_side;
    for (i = 0; i < n; i++) {
        if (str[i] == '\\' && i + 1 < n) {
            switch (str[i + 1]) {
            case '/':
            case '*':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case '"':
            case '\'':
            case '`':
                i++;
                continue;
            }
        }
        switch (str[i]) {
        case '/':
            if (i + 1 < n && str[i + 1] == '*') {
                if (!comment_sign_used) {
                    comment_sign_used = true;
                    i++; // skip next char
                }
            } break;
        case '*':
            if (i + 1 < n && str[i + 1] == '/') {
                if (comment_sign_used) {
                    comment_sign_used = false;
                    i++; // skip next char
                } else {
                    no_errors = false;
                }
            }
            break;
        default:
            if (no_errors && comment_sign_used) {
                continue; // continue loop without checking
            }
        }
        switch (str[i]) {
        case '(':
            stack_push_int(&stack, LPARAN);
#ifdef DEBUG
            printf("stack push: %c\n", str[i]);
#endif
            break;
        case '{':
            stack_push_int(&stack, LCURLY);
#ifdef DEBUG
            printf("stack push: %c\n", str[i]);
#endif
            break;
        case '[':
            stack_push_int(&stack, LBRACKET);
#ifdef DEBUG
            printf("stack push: %c\n", str[i]);
#endif
            break;
        case ')':
        case '}':
        case ']':
            if (stack_isempty(stack)) {
                no_errors = false;
                break;
            }
            left_side = stack_pop_int(&stack);
#ifdef DEBUG
            printf("stack pop: %c\n", get_sign(left_side));
            printf("comparing %c with %c\n", get_sign(left_side), str[i]);
#endif
            switch (left_side) {
            case LPARAN:
                no_errors = str[i] == ')';
                break;
            case LCURLY:
                no_errors = str[i] == '}';
                break;
            case LBRACKET:
                no_errors = str[i] == ']';
                break;
            case SINGLE_QUOTE:
            case DOUBLE_QUOTE:
            case BACK_QUOTE:
                no_errors = false;
                break;
            }
            break;
        case '\'':
            if (!single_quote_used) {
                single_quote_used = true;
#ifdef DEBUG
                printf("stack push: %c\n", '\'');
#endif
                stack_push_int(&stack, SINGLE_QUOTE);
            } else {
                left_side = stack_pop_int(&stack);
#ifdef DEBUG
                printf("stack pop: %c\n", get_sign(left_side));
                printf("comparing %c with %c\n", get_sign(left_side), str[i]);
#endif
                no_errors = left_side == SINGLE_QUOTE;
                single_quote_used = false;
            }
            break;
        case '"':
            if (!double_quote_used) {
                double_quote_used = true;
#ifdef DEBUG
                printf("stack push: %c\n", '"');
#endif
                stack_push_int(&stack, DOUBLE_QUOTE);
            } else {
                left_side = stack_pop_int(&stack);
#ifdef DEBUG
                printf("stack pop: %c\n", get_sign(left_side));
                printf("comparing %c with %c\n", get_sign(left_side), str[i]);
#endif
                no_errors = left_side == DOUBLE_QUOTE;
                double_quote_used = false;
            }
            break;
        case '`':
            if (!back_quote_used) {
                back_quote_used = true;
#ifdef DEBUG
                printf("stack push: %c\n", '`');
#endif
                stack_push_int(&stack, BACK_QUOTE);
            } else {
                left_side = stack_pop_int(&stack);
#ifdef DEBUG
                printf("stack pop: %c\n", get_sign(left_side));
                printf("comparing %c with %c\n", get_sign(left_side), str[i]);
#endif
                no_errors = left_side == BACK_QUOTE;
                back_quote_used = false;
            }
            break;
        }
        if (!no_errors) {
            break;
        }
    }
#ifdef DEBUG
    printf("[result] no errors: %s, index stopped at: %lu\n",
           no_errors ? "true" : "false", i);
#else
    if (no_errors) {
        printf("\nno errors : )\n");
    } else {
        // these numbers are hand picked from testing and counting. just for
        // pretty printing.
        if (i <= 8) {
            printf("%*c error at\n", (int)i + 1, '^');
        } else {
            printf("%*s ^\n", (int)i - 1, "error at");
        }
    }
#endif
    stack_clear(&stack);
    free(str);
    return 0;
}

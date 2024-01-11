#include <stdio.h>
#include "stack.h"

#define OFFSET sizeof(char) + 1

typedef enum {
    SINGLE_QUOTE = OFFSET,
    DOUBLE_QUOTE,
    BACK_QUOTE,
    COMMENT
} QUOTE_ENUM;

int main(void) {
    stack *stack_p = st_new();
    if (stack_p == NULL) {
        return 1;
    }
    puts("Input line:");

    char* str = NULL;
    size_t str_buffersize = 0;
    ssize_t n = getline(&str, &str_buffersize, stdin);
    if (n < 0) {
        return 1;
    }
    putchar('\n');

    bool no_errors = true;
    ssize_t i = 0;
    for (i = 0; i < n; i++) {
        if (!no_errors) {
            break;
        }
        switch (str[i]) {
            case '(':
            case '{':
            case '[':
                if (st_push(stack_p, str[i]) == false) {
                    return 1;
                }
                printf("stack push: %c\n", str[i]);
                break;
            case ')':
            case '}':
            case ']':
                if (st_empty(stack_p) == true) {
                    no_errors = false;
                    break;
                }
                value_t left_side = st_pop(stack_p);
                printf("stack pop: %c\n", left_side);

                printf("comparing %c with %c\n", left_side, str[i]);
                switch (left_side) {
                    case '(':
                        no_errors = str[i] == ')';
                        break;
                    case '{':
                        no_errors = str[i] == '}';
                        break;
                    case '[':
                        no_errors = str[i] == ']';
                        break;
                }
                break;
        }
    }
    printf("[result] no errors: %s, index stopped at: %lu\n", no_errors ? "true" : "false", i);
    st_free(stack_p);
    return 0;
}


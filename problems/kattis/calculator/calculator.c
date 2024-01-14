#include "queue.h"
#include "stack.h"
#include <limits.h>
#include <stdio.h>

typedef enum {
    ADD = -INT_MAX, // note: enum values can only be of type int.
    SUB,            // -INT_MAX + 1
    MUL,            // -INT_MAX + 2
    DIV,            // ...
    LPARAN
} OP_ENUM;

char *get_queue_value_repr(q_value value) {}

int main() {
    char *str = NULL;
    size_t str_buffersize = 0;
    ssize_t n = getline(&str, &str_buffersize, stdin);
    if (n < 0) {
        return 1;
    }

    // preprocess and make the minuses cancel each other:
    // > `--1` -> ` +1`
    // > `---1` -> ` +-1` -> `  -1`
    // > `- -1` -> ` --1` -> `  +1`
    for (ssize_t i = 0; i < n - 1; i++) { // n-1, since the last character is a newline.
        switch (str[i]) {
        case '-':
            switch (str[i + 1]) {
            case '-':
                str[i] = ' ';
                str[i + 1] = '+';
                break;
            case '+':
            case ' ':
                str[i] = ' ';
                str[i + 1] = '-';
                break;
            }
        case '+':
            switch (str[i + 1]) {
            case '-':
            case '+':
                str[i] = ' ';
                break;
            case ' ':
                str[i] = ' ';
                str[i + 1] = '+';
                break;
            }
        }
    }

    // Using the shunting yard algorithm:
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm
    queue *q_p = q_new();
    stack *st_p = st_new();
    bool parsing_number = false;

    // for the case '-number':
    q_enqueue(q_p, 0);
    st_push(st_p, ADD);

    for (ssize_t i = 0; i < n; i++) {
        switch (str[i]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (!parsing_number) {
                q_enqueue(q_p, str[i] - '0');
            } else {
                q_p->front->value = 10 * q_p->front->value + (str[i] - '0');
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            parsing_number = false;
            while (!st_empty(st_p) && st_peek(st_p) != LPARAN) {
                q_enqueue(q_p, st_pop(st_p));
            }
            switch (str[i]) {
            case '+':
                st_push(st_p, ADD);
                break;
            case '-':
                st_push(st_p, SUB);
                break;
            case '*':
                st_push(st_p, MUL);
                break;
            case '/':
                st_push(st_p, DIV);
                break;
            }
            break;
        case '(':
            parsing_number = false;
            st_push(st_p, LPARAN);
            break;
        case ')':
            parsing_number = false;
            while (!st_empty(st_p) && st_peek(st_p) != LPARAN) {
                q_enqueue(q_p, st_pop(st_p));
            }
            st_pop(st_p); // now there should be a left paranthesis.
        }
    }
    while (!st_empty(st_p) && st_peek(st_p) != LPARAN) {
        q_enqueue(q_p, st_pop(st_p));
    }
    free(str);

#ifndef DEBUG // is ifndef for convienience. will be ifdef in the future.
    printf("Input queue:\n");
    q_element *elm_p = q_p->front;

    putchar('0'); // because 0 is set to be the first in queue.
    while ((elm_p = elm_p->next)) {
        putchar(' ');
        switch (elm_p->value) {
        case ADD:
            putchar('+');
            break;
        case SUB:
            putchar('-');
            break;
        case MUL:
            putchar('*');
            break;
        case DIV:
            putchar('/');
            break;
        default:
            printf("%d", elm_p->value);
            break;
        }
    }
    putchar('\n');
#endif

    q_free(q_p);
    return 0;
}

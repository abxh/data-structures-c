#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "stack.h"
#undef ST_VALUE
#define ST_VALUE long

#include "queue.h"
#undef Q_VALUE
#define Q_VALUE long

typedef enum {
    ADD = INT_MIN,
    SUB,
    MUL,
    DIV,
    LPARAN
} OP_ENUM;

char* preprocess_exp(const char* lineptr, ssize_t* n_p) {
    ssize_t n = *n_p;
    // append "0+" in front of the string. This is for the case where there are
    // a single term.
    n = n + 3;
    char *str = malloc(n * sizeof(char));
    strncpy(str, (char[]){'0', '+', '\0'}, 3);
    strncat(str, lineptr, n);

    // preprocess and make the minuses cancel each other:
    // > `--1` -> ` +1`
    // > `---1` -> ` +-1` -> `  -1`
    // > `- -1` -> ` --1` -> `  +1`
    for (ssize_t i = 0; i < n - 1;
         i++) { // n-1, since the last character is a newline.
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
            break;
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
            break;
        }
    }
    *n_p = n;
    return str;
}

queue *get_postfix_exp(const char* str, const ssize_t n) {
    // Using the shunting yard algorithm:
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm
    queue *output_queue_p = q_new();
    stack *operator_stack_p = st_new();
    bool parsing_number = false;

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
                q_enqueue(output_queue_p, (str[i] - '0'));
                parsing_number = true;
            } else {
                output_queue_p->back->value = output_queue_p->back->value * 10 + (str[i] - '0');
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            parsing_number = false;
            while (!st_empty(operator_stack_p) && st_peek(operator_stack_p) != LPARAN) {
                q_enqueue(output_queue_p, st_pop(operator_stack_p));
            }
            switch (str[i]) {
            case '+':
                st_push(operator_stack_p, ADD);
                break;
            case '-':
                st_push(operator_stack_p, SUB);
                break;
            case '*':
                st_push(operator_stack_p, MUL);
                break;
            case '/':
                st_push(operator_stack_p, DIV);
                break;
            }
            break;
        case '(':
            parsing_number = false;
            st_push(operator_stack_p, LPARAN);
            break;
        case ')':
            while (!st_empty(operator_stack_p) && st_peek(operator_stack_p) != LPARAN) {
                q_enqueue(output_queue_p, st_pop(operator_stack_p));
            }
            st_pop(operator_stack_p); // now there should be a left paranthesis.
        }
    }
    while (!st_empty(operator_stack_p) && st_peek(operator_stack_p) != LPARAN) {
        q_enqueue(output_queue_p, st_pop(operator_stack_p));
    }
    st_free(operator_stack_p);
    return output_queue_p;
}

int main() {
    char *lineptr = NULL;
    size_t line_buffersize = 0;
    ssize_t n = getline(&lineptr, &line_buffersize, stdin);
    if (n < 0) {
        return 1;
    }

    char *str = preprocess_exp(lineptr, &n);
    free(lineptr);
    queue *q_p = get_postfix_exp(str, n);
    free(str);

#ifndef DEBUG // is ifndef for convienience. will be ifdef in the future.
    printf("Input queue:\n");
    q_element *elm_p = q_p->front;

    putchar('0'); // because 0 is set to be the first in queue.
    while ((elm_p = elm_p->next)) {
        putchar(' ');
        switch ((elm_p->value)) {
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
            printf("%li", elm_p->value);
            break;
        }
    }
    putchar('\n');
#endif

    return 0;
}

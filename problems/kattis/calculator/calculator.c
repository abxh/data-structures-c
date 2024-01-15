#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "stack.h"

typedef enum { ADD = INT_MIN, SUB, MUL, DIV, LPARAN } OPT;

#define PRECISION 100

queue *get_postfix_exp(queue *queue_p, stack *stack_p, const char *str, const ssize_t n) {
    // Using the shunting yard algorithm:
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm
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
                q_enqueue(queue_p, str[i] - '0');
                parsing_number = true;
            } else {
                queue_p->back->value =
                    10 * queue_p->back->value + (str[i] - '0');
            }
            if (!isdigit(str[i + 1])) {
                queue_p->back->value *= PRECISION;
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            parsing_number = false;
            while (!st_empty(stack_p) && st_peek(stack_p) != LPARAN) {
                q_enqueue(queue_p, st_pop(stack_p));
            }
            switch (str[i]) {
            case '+':
                st_push(stack_p, ADD);
                break;
            case '-':
                st_push(stack_p, SUB);
                break;
            case '*':
                st_push(stack_p, MUL);
                break;
            case '/':
                st_push(stack_p, DIV);
                break;
            }
            break;
        case '(':
            parsing_number = false;
            st_push(stack_p, LPARAN);
            break;
        case ')':
            while (!st_empty(stack_p) && st_peek(stack_p) != LPARAN) {
                q_enqueue(queue_p, st_pop(stack_p));
            }
            st_pop(stack_p); // now there should be a left paranthesis.
            break;
        }
    }
    while (!st_empty(stack_p) && st_peek(stack_p) != LPARAN) {
        double value = st_pop(stack_p);
        q_enqueue(queue_p, value);
    }
    return queue_p;
}

/*
Q_VALUE eval_postfix_exp(queue *queue_p, stack *stack_p) {
    // Using the algorithm described in
    // https://www.youtube.com/watch?v=7ha78yWRDlE
    Q_VALUE value, fst, snd;
    while (!q_empty(queue_p)) {
        value = q_dequeue(queue_p);
        switch (value) {
        case ADD:
            snd = st_pop(stack_p);
            fst = st_pop(stack_p);
            st_push(stack_p, fst + snd);
            break;
        case SUB:
            snd = st_pop(stack_p);
            fst = st_pop(stack_p);
            st_push(stack_p, fst - snd);
            break;
        case MUL:
            snd = st_pop(stack_p);
            fst = st_pop(stack_p);
            st_push(stack_p, fst * snd);
        case DIV:
            snd = st_pop(stack_p);
            fst = st_pop(stack_p);
            st_push(stack_p, fst / snd);
            break;
        default:
            st_push(stack_p, value);
            break;
        }
    }
    return st_pop(stack_p);
}
*/

/*
void print_queue(queue *q_p) {
    printf("Queue:\n");
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
            printf("%d.%d", elm_p->value / PRECISION, elm_p->value % PRECISION);
            break;
        }
    }
    putchar('\n');
}
*/

char *preprocess_exp(const char *lineptr, ssize_t *n_p) {
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
    for (ssize_t i = 0; i < n - 1; i++) {
        // n-1 is possible since the last character is a newline.
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

void clear_stack(stack* stack_p) {
    while (!st_empty(stack_p)) {
        st_pop(stack_p);
    }
}

void clear_queue(queue* queue_p) {
    while (!q_empty(queue_p)) {
        q_dequeue(queue_p);
    }
}

int main() {
    // char *lineptr = NULL;
    // size_t line_buffersize = 0;
    // ssize_t n = getline(&lineptr, &line_buffersize, stdin);
    // if (n < 0) {
    //     return 1;
    // }

    // char *str = preprocess_exp(lineptr, &n);

    queue *queue_p = q_new();
    stack *stack_p = st_new();

    st_push(stack_p, 1.);
    printf("%f\n", stack_p->head->value);

//     queue_p = get_postfix_exp(queue_p, stack_p, str, n);
// #ifndef DEBUG
//     print_queue(queue_p);
// #endif
//     clear_stack(stack_p);
//     printf("%.2f\n", eval_postfix_exp(queue_p, stack_p));

    // free(lineptr);
    // free(str);
    q_free(queue_p);
    st_free(stack_p);
    return 0;
}

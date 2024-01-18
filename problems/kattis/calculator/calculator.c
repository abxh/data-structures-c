#include <stdio.h>

#include "queue.h"
#include "stack.h"

typedef enum {
    NUMBER = 0,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    LPARAN,
    RPARAN
} TokenType;

typedef struct {
    TokenType type;
    float value;
} Token;

CREATE_QUEUE_INLINE_FUNCTIONS(token, Token);
CREATE_STACK_INLINE_FUNCTIONS(tokentype, TokenType);
CREATE_STACK_INLINE_FUNCTIONS(token, Token);

void fprintf_token(Token token, FILE* stream) {
    switch (token.type) {
    case NUMBER:
        fprintf(stream, "%.2f", token.value);
        break;
    case ADD_OP:
        fputc('+', stream);
        break;
    case SUB_OP:
        fputc('-', stream);
        break;
    case MUL_OP:
        fputc('*', stream);
        break;
    case DIV_OP:
        fputc('/', stream);
        break;
    case LPARAN:
        fputc('(', stream);
        break;
    case RPARAN:
        fputc(')', stream);
        break;
    }
}

void parse_math_exp(char* line_p, ssize_t len, Queue* queue_p) {
    bool prefix_zero = true;
    Stack* pm_stack_p = stack_new();
    for (ssize_t i = 0; i < len; i++) {
        switch (line_p[i]) {
        case '+':
            if (prefix_zero) {
                queue_enqueue_token(queue_p,
                                    (Token){.type = NUMBER, .value = 0.F});
                prefix_zero = false;
            }
            stack_push_tokentype(pm_stack_p, ADD_OP);
            break;
        case '-':
            if (prefix_zero) {
                queue_enqueue_token(queue_p,
                                    (Token){.type = NUMBER, .value = 0.F});
                prefix_zero = false;
            }
            stack_push_tokentype(pm_stack_p, SUB_OP);
            break;
        case '*':
            queue_enqueue_token(queue_p, (Token){.type = MUL_OP});
            break;
        case '/':
            queue_enqueue_token(queue_p, (Token){.type = DIV_OP});
            break;
        case '(':
            queue_enqueue_token(queue_p, (Token){.type = LPARAN});
            break;
        case ')':
            queue_enqueue_token(queue_p, (Token){.type = RPARAN});
            break;
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
            prefix_zero = false;
            if (!stack_isempty(pm_stack_p)) {
                TokenType tt = ADD_OP;
                while (!stack_isempty(pm_stack_p)) {
                    TokenType tt_next = stack_pop_tokentype(pm_stack_p);
                    if (tt_next == SUB_OP) {
                        switch (tt) {
                        case ADD_OP:
                            tt = SUB_OP;
                            break;
                        case SUB_OP:
                            tt = ADD_OP;
                            break;
                        default:
                            break;
                        }
                    }
                }
                queue_enqueue_token(queue_p, (Token){.type = tt});
            }
            if (!queue_empty(queue_p) &&
                qelement_get_token(queue_p->back_p).type == NUMBER) {
                qelement_set_token(
                    queue_p->back_p,
                    (Token){.type = NUMBER,
                            .value = qelement_get_token(queue_p->back_p).value *
                                         10.F +
                                     line_p[i] - '0'});
            } else {
                queue_enqueue_token(
                    queue_p, (Token){.type = NUMBER, .value = line_p[i] - '0'});
            }
            break;
        default:
            continue;
        }
    }
    if (prefix_zero) {
        queue_enqueue_token(queue_p, (Token){.type = NUMBER, .value = 0.F});
    }
    stack_free(pm_stack_p);
};

void conv_math_infix_exp_to_postfix(Queue** queue_pp) {
    // variation of the shunting yard algorithm.
    Stack* op_stack_p = stack_new();
    Queue* inp_queue_p = *queue_pp;
    Queue* out_queue_p = queue_new();

    while (!queue_empty(inp_queue_p)) {
        Token tk = queue_dequeue_token(inp_queue_p);
        switch (tk.type) {
        case ADD_OP:
        case SUB_OP:
        case MUL_OP:
        case DIV_OP:
        case RPARAN:
            while (!stack_isempty(op_stack_p) &&
                   stack_peek_tokentype(op_stack_p) != LPARAN) {
                queue_enqueue_token(
                    out_queue_p,
                    (Token){.type = stack_pop_tokentype(op_stack_p)});
            }
        default:
            break;
        }
        switch (tk.type) {
        case NUMBER:
            queue_enqueue_token(out_queue_p, tk);
            break;
        case ADD_OP:
            stack_push_tokentype(op_stack_p, ADD_OP);
            break;
        case SUB_OP:
            stack_push_tokentype(op_stack_p, SUB_OP);
            break;
        case MUL_OP:
            stack_push_tokentype(op_stack_p, MUL_OP);
            break;
        case DIV_OP:
            stack_push_tokentype(op_stack_p, DIV_OP);
            break;
        case LPARAN:
            stack_push_tokentype(op_stack_p, LPARAN);
            break;
        case RPARAN:
            stack_pop_token(op_stack_p); // pop LPARAN
            break;
        }
    }
    while (!stack_isempty(op_stack_p) &&
           stack_peek_tokentype(op_stack_p) != LPARAN) {
        queue_enqueue_token(out_queue_p,
                            (Token){.type = stack_pop_tokentype(op_stack_p)});
    }
    *queue_pp = out_queue_p;
    queue_free(inp_queue_p);
    stack_free(op_stack_p);
}

int main() {
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;
    while (0 < (len = getline(&line_p, &n, stdin))) {
        Queue* queue_p = queue_new();
        if (queue_p == NULL) {
            return 1;
        }
        parse_math_exp(line_p, len, queue_p);
        QElement* elm_p = queue_p->front_p;

        printf("[calc] parsing w. +- eval:");
        while (elm_p) {
            putchar(' ');
            fprintf_token(qelement_get_token(elm_p), stdout);
            elm_p = elm_p->next_p;
        }
        putchar('\n');

        conv_math_infix_exp_to_postfix(&queue_p);

        printf("[calc] postfix conversion:");
        elm_p = queue_p->front_p;
        while (elm_p) {
            putchar(' ');
            fprintf_token(qelement_get_token(elm_p), stdout);
            elm_p = elm_p->next_p;
        }
        putchar('\n');

        free(line_p);
        free(queue_p);
        line_p = NULL;
        n = 0;
    }

    return 0;
}

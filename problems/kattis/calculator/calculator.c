#include <math.h> // note: use -lm beside gcc
#include <stdio.h>

#include "queue.h"
#include "stack.h"

typedef enum {
    UNDEFINED_OP = 0,
    NUMBER,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    POW_OP,
    LPARAN,
    RPARAN,
} TokenType;

typedef enum { UNDEFINED_OP_ASC = -1, LEFT_OP_ASC, RIGHT_OP_ASC } OP_ASC;

typedef struct {
    TokenType type;
    double value;
} Token;

CREATE_QUEUE_INLINE_FUNCTIONS(token, Token, (Token){.type = UNDEFINED_OP})
CREATE_STACK_INLINE_FUNCTIONS(tokentype, TokenType, (TokenType)UNDEFINED_OP)
CREATE_STACK_INLINE_FUNCTIONS(token, Token, (Token){.type = UNDEFINED_OP})
CREATE_STACK_INLINE_FUNCTIONS(num, double, 0.)

void fprintf_token(Token token, FILE* stream) {
    switch (token.type) {
    case NUMBER:
#ifdef KATTIS
        fprintf(stream, "%.2f", token.value);
#else
        fprintf(stream, "%.9g", token.value);
#endif
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
    case POW_OP:
        fputc('^', stream);
        break;
    case LPARAN:
        fputc('(', stream);
        break;
    case RPARAN:
        fputc(')', stream);
        break;
    case UNDEFINED_OP:
        fputc(' ', stream);
        break;
    }
}

int cmp_operator_precedence(TokenType operator, TokenType other_operator) {
    // -1: l < r, 0: l = r, 1: l > r
    int l = -1, r = -1;
    switch (operator) {
    case ADD_OP:
    case SUB_OP:
        l = 0;
        break;
    case MUL_OP:
    case DIV_OP:
        l = 1;
        break;
    case POW_OP:
        l = 2;
        break;
    default:
        break;
    }
    switch (other_operator) {
    case ADD_OP:
    case SUB_OP:
        r = 0;
        break;
    case MUL_OP:
    case DIV_OP:
        r = 1;
        break;
    case POW_OP:
        r = 2;
        break;
    default:
        break;
    }
    if (l == r) {
        return 0;
    } else if (l > r) {
        return 1;
    } else {
        return -1;
    }
}

OP_ASC operator_associativity(TokenType operator) {
    switch (operator) {
    case ADD_OP:
    case SUB_OP:
    case MUL_OP:
    case DIV_OP:
        return LEFT_OP_ASC;
    case POW_OP:
        return RIGHT_OP_ASC;
    default:
        return UNDEFINED_OP_ASC;
    }
}

void parse_math_exp(char* line_p, ssize_t len, Queue* queue_p, double last_value) {
    bool prefix_zero = true;
    int first_term_paran_state = 0;
    bool fractional_number = false;
    double multiplierl = 10.;
    double multiplierr = 1.;
    TokenType pm_token;
    TokenType other_token;

    Stack* pm_stack_p = stack_new();
    if (pm_stack_p == NULL) {
        return;
    }
    for (ssize_t i = 0; i < len; i++) {
        pm_token = UNDEFINED_OP;
        other_token = UNDEFINED_OP;
        switch (line_p[i]) {
        case '+':
            if (pm_token == UNDEFINED_OP) {
                pm_token = ADD_OP;
            }
            [[fallthrough]]; // to signal purposeful fallthrough.
        case '-':
            if (pm_token == UNDEFINED_OP) {
                pm_token = SUB_OP;
            }
            fractional_number = false;
            if (prefix_zero) {
                queue_enqueue_token(queue_p, (Token){.type = NUMBER, .value = 0.});
                prefix_zero = false;
            }
            if (first_term_paran_state == 2) {
                queue_enqueue_token(queue_p, (Token){.type = RPARAN});
                first_term_paran_state = 0;
            }
            stack_push_tokentype(pm_stack_p, pm_token);
            break;
        case '*':
            if (other_token == UNDEFINED_OP) {
                other_token = MUL_OP;
            }
            [[fallthrough]];
        case '/':
            if (other_token == UNDEFINED_OP) {
                other_token = DIV_OP;
            }
            [[fallthrough]];
        case '^':
            if (other_token == UNDEFINED_OP) {
                other_token = POW_OP;
            }
            fractional_number = false;
            prefix_zero = true;
            if (first_term_paran_state == 2) {
                queue_enqueue_token(queue_p, (Token){.type = RPARAN});
            }
            queue_enqueue_token(queue_p, (Token){.type = other_token});
            queue_enqueue_token(queue_p, (Token){.type = LPARAN});
            first_term_paran_state = 1;
            break;
        case '(':
            fractional_number = false;
            prefix_zero = true;
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
        case '_':
        case '.':
            prefix_zero = false;
            if (first_term_paran_state == 1) {
                first_term_paran_state = 2;
            }
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
            if (line_p[i] == '_') {
                queue_enqueue_token(queue_p, (Token){.type = NUMBER, .value = last_value});
                break;
            }
            if (!fractional_number && line_p[i] == '.') {
                fractional_number = true;
                multiplierl = 1.;
                multiplierr = 1.;
                continue;
            } else if (fractional_number) {
                multiplierr /= 10.;
            } else {
                multiplierl = 10.;
                multiplierr = 1.;
            }
            if (!queue_empty(queue_p) && qelement_get_token(queue_p->back_p).type == NUMBER) {
                qelement_set_token(queue_p->back_p, (Token){.type = NUMBER,
                                                            .value = qelement_get_token(queue_p->back_p).value * multiplierl +
                                                                     (line_p[i] - '0') * multiplierr});
            } else {
                queue_enqueue_token(queue_p, (Token){.type = NUMBER, .value = (line_p[i] - '0') * multiplierr});
            }
            break;
        default:
            continue;
        }
    }
    if (prefix_zero) {
        queue_enqueue_token(queue_p, (Token){.type = NUMBER, .value = 0.});
    }
    if (first_term_paran_state == 2) {
        queue_enqueue_token(queue_p, (Token){.type = RPARAN});
    }
    stack_free(pm_stack_p);
}

void conv_math_infix_exp_to_postfix(Queue** queue_pp) {
    // using the shunting yard algorithm.
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm

    Stack* op_stack_p = stack_new();
    if (op_stack_p == NULL) {
        return;
    }
    Queue* inp_queue_p = *queue_pp;
    Queue* out_queue_p = queue_new();
    if (out_queue_p == NULL) {
        stack_free(op_stack_p);
        return;
    }
    while (!queue_empty(inp_queue_p)) {
        Token tk = queue_dequeue_token(inp_queue_p);
        switch (tk.type) {
        case ADD_OP:
        case SUB_OP:
        case MUL_OP:
        case DIV_OP:
        case POW_OP:
        case RPARAN:
            while (!stack_isempty(op_stack_p) && stack_peek_tokentype(op_stack_p) != LPARAN &&
                   (0 < cmp_operator_precedence(stack_peek_tokentype(op_stack_p), tk.type) ||
                    (0 == cmp_operator_precedence(stack_peek_tokentype(op_stack_p), tk.type) &&
                     operator_associativity(tk.type) == LEFT_OP_ASC))

            ) {
                queue_enqueue_token(out_queue_p, (Token){.type = stack_pop_tokentype(op_stack_p)});
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
        case POW_OP:
            stack_push_tokentype(op_stack_p, POW_OP);
            break;
        case LPARAN:
            stack_push_tokentype(op_stack_p, LPARAN);
            break;
        case RPARAN:
            stack_pop_token(op_stack_p); // pop LPARAN
            break;
        case UNDEFINED_OP:
            break;
        }
    }
    while (!stack_isempty(op_stack_p) && stack_peek_tokentype(op_stack_p) != LPARAN) {
        queue_enqueue_token(out_queue_p, (Token){.type = stack_pop_tokentype(op_stack_p)});
    }
    *queue_pp = out_queue_p;
    queue_free(inp_queue_p);
    stack_free(op_stack_p);
}

double eval_postfix_exp(Queue* queue_p) {
    Stack* stack_p = stack_new();
    if (stack_p == NULL) {
        return 0.;
    }
    double r, l;
    while (!queue_empty(queue_p)) {
        Token t = queue_dequeue_token(queue_p);
        if (t.type == NUMBER) {
            stack_push_num(stack_p, t.value);
            continue;
        }
        if (!stack_isempty(stack_p)) {
            r = stack_pop_num(stack_p);
        } else {
            stack_free(stack_p);
            return 0.;
        }
        if (!stack_isempty(stack_p)) {
            l = stack_pop_num(stack_p);
        } else {
            stack_free(stack_p);
            return 0.;
        }
        switch (t.type) {
        case ADD_OP:
            stack_push_num(stack_p, l + r);
            break;
        case SUB_OP:
            stack_push_num(stack_p, l - r);
            break;
        case MUL_OP:
            stack_push_num(stack_p, l * r);
            break;
        case DIV_OP:
            stack_push_num(stack_p, l / r);
            break;
        case POW_OP:
            stack_push_num(stack_p, pow(l, r));
            break;
        default:
            break;
        }
    }
    if (!stack_isempty(stack_p)) {
        l = stack_pop_num(stack_p);
    } else {
        stack_free(stack_p);
        return 0.;
    }
    stack_free(stack_p);
    return l;
}

int main() {
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;
    double last_value = 0;
    while (0 < (len = getline(&line_p, &n, stdin))) {
        Queue* queue_p = queue_new();
        if (queue_p == NULL) {
            return 1;
        }
        parse_math_exp(line_p, len, queue_p, last_value);

#ifdef DEBUG
        QElement* elm_p = queue_p->front_p;
        printf("[calc] parsing w. +- eval:");
        while (elm_p) {
            putchar(' ');
            fprintf_token(qelement_get_token(elm_p), stdout);
            elm_p = elm_p->next_p;
        }
        putchar('\n');
#endif

        conv_math_infix_exp_to_postfix(&queue_p);
#ifdef DEBUG
        printf("[calc] postfix conversion:");
        elm_p = queue_p->front_p;
        while (elm_p) {
            putchar(' ');
            fprintf_token(qelement_get_token(elm_p), stdout);
            elm_p = elm_p->next_p;
        }
        putchar('\n');
#endif

#ifdef DEBUG
        printf("[calc] eval of postfix:    ");
#endif
        last_value = eval_postfix_exp(queue_p);
#ifdef KATTIS
        printf("%.2f\n", last_value);
#else
        printf("%.9g\n", last_value);
#endif

        queue_free(queue_p);
        free(line_p);
        line_p = NULL;
        n = 0;
    }
    free(line_p);
    return 0;
}

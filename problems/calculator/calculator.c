#include <ctype.h>  // isdigit
#include <stdio.h>  // getline, stdin, printf, fprintf, stderr, putchar
#include <stdlib.h> // size_t, free, ssize_t

#ifdef KATTIS

double pow(double, double) {
    return 0.;
}
#define EVAL_STR "%.2f"

#else

#include <math.h> // pow
#define EVAL_STR "%g"

#endif

typedef enum { DEFAULTOKEN, NUMBER_TOKEN, OP_TOKEN } token_type;

typedef enum { DEFAULOP, ADD_OP, SUB_OP, MUL_OP, DIV_OP, POW_OP, OPENING_PAREN_OP, CLOSING_PAREN_OP } operation_type;

typedef struct {
    token_type token;
    union {
        double num;
        operation_type op;
    } metadata;
} lexeme_type;

typedef lexeme_type lex;
#define VALUE_TYPE lex
#include "queue.h"

#define VALUE_TYPE lex
#include "stack.h"

char decode_op(operation_type op) {
    switch (op) {
    case ADD_OP:
        return '+';
    case SUB_OP:
        return '-';
    case MUL_OP:
        return '*';
    case DIV_OP:
        return '/';
    case POW_OP:
        return '^';
    case OPENING_PAREN_OP:
        return '(';
    case CLOSING_PAREN_OP:
        return ')';
    default:
        break;
    }
    return 'D';
}

int op_precedence(operation_type op) {
    switch (op) {
    case POW_OP:
        return 3;
    case DIV_OP:
    case MUL_OP:
        return 2;
    case SUB_OP:
    case ADD_OP:
        return 1;
    default:
        return 0;
    }
}

int op_precedence_cmp(operation_type o1, operation_type o2) {
    int o1_prec = op_precedence(o1);
    int o2_prec = op_precedence(o2);
    return -(o1_prec < o2_prec) + (o1_prec > o2_prec);
}

static const double RTR_VALUE_DEFAULT = 0.;
static double RTR_VALUE_LAST = RTR_VALUE_DEFAULT;

#define digit_to_num(v) ((v) - '0')
#define last_token(inp_queue) (lex_queue_is_empty(inp_queue) ? DEFAULTOKEN : lex_queue_peek_last(inp_queue).token)
#define last_op(inp_queue) (lex_queue_is_empty(inp_queue) ? DEFAULOP : lex_queue_peek_last(inp_queue).metadata.op)

double eval(char* str, ssize_t len) {
    if (len < 0) {
        return RTR_VALUE_DEFAULT;
    }
    double rtr_value = RTR_VALUE_DEFAULT;
    lex_queue_type* inp_queue = NULL;
    lex_queue_type* inp_queue_postfix = NULL;
    lex_stack_type* op_stack = NULL;
    lex_stack_type* num_stack = NULL;

    inp_queue = lex_queue_create();
    if (!inp_queue) {
        goto on_oom_error;
    }

    // error handling:
    char* error_msg = NULL;
    ssize_t error_index = 0;
    size_t opening_paren_count = 0;
    size_t closing_paren_count = 0;
    bool incomplete_input = true;

    operation_type sign = DEFAULOP;

    for (ssize_t i = 0; i < len; i++) {
        switch (str[i]) {
        case '+':
        case '-':
            incomplete_input = true;
            error_index = i;

            sign = str[i] == '+' ? ADD_OP : SUB_OP;
            while (i + 1 < len && (str[i + 1] == '-' || str[i + 1] == '+' || str[i + 1] == ' ')) {
                if (str[i + 1] == '-') {
                    sign = sign == SUB_OP ? ADD_OP : SUB_OP;
                }
                if (str[i + 1] == '+' || str[i + 1] == '-') {
                    error_index = i + 1;
                }
                i++;
            }
            break;
        case '*':
        case '/':
            incomplete_input = true;
            error_index = i;
            if (lex_queue_is_empty(inp_queue) ||
                (last_token(inp_queue) == OP_TOKEN && (last_op(inp_queue) == MUL_OP || last_op(inp_queue) == DIV_OP))) {
                error_msg = "Incorrect use of '*' or '/'.";
                error_index = i;
                goto on_inp_error;
            }
            lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = str[i] == '*' ? MUL_OP : DIV_OP}});
            break;
        case '^':
            incomplete_input = true;
            error_index = i;
            if (lex_queue_is_empty(inp_queue) || (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == POW_OP)) {
                error_msg = "Incorrect use of '^'.";
                error_index = i;
                goto on_inp_error;
            }
            lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = POW_OP}});
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
        case '.':
        case '_':
            incomplete_input = false;
            if ((last_token(inp_queue) == NUMBER_TOKEN ||
                 (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP)) &&
                sign != DEFAULOP) {
                lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = sign}});
                sign = DEFAULOP;
            } else if (last_token(inp_queue) == NUMBER_TOKEN) {
                error_msg = "Two numbers in a row.";
                error_index = i;
                goto on_inp_error;
            }
            if (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP) {
                lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
            }
            if (str[i] == '_') {
                lex_queue_enqueue(inp_queue, (lexeme_type){.token = NUMBER_TOKEN, .metadata = {.num = RTR_VALUE_LAST}});
                continue;
            }
            i--;
            double value = 0.;
            while (i + 1 < len && isdigit(str[i + 1])) {
                value = 10. * value + digit_to_num(str[i + 1]);
                i++;
            }
            if (sign == SUB_OP) {
                value = -value;
            }
            sign = DEFAULOP;
            if (str[i + 1] == '.') {
                if (i + 2 < len && !isdigit(str[i + 2])) {
                    error_msg = "No digits after '.'.";
                    error_index = i + 1;
                    goto on_inp_error;
                }
                i++;
                double c = 10.;
                while (i + 1 < len && isdigit(str[i + 1])) {
                    value = value + digit_to_num(str[i + 1]) / c;
                    c *= 10.;
                    i++;
                }
            }
            lex_queue_enqueue(inp_queue, (lexeme_type){.token = NUMBER_TOKEN, .metadata = {.num = value}});
            break;
        case '(':
        case ')':
            opening_paren_count += (str[i] == '(');
            closing_paren_count += (str[i] == ')');
            if (opening_paren_count < closing_paren_count) {
                error_msg = "Closed parenthesis before opening one.";
                error_index = i;
                goto on_inp_error;
            }
            if (str[i] == '(') {
                incomplete_input = true;
                error_index = i;
                if (sign != DEFAULOP) {
                    if (last_token(inp_queue) == NUMBER_TOKEN ||
                        (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP)) {
                        lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = sign}});
                    } else {
                        lex_queue_enqueue(inp_queue, (lexeme_type){.token = NUMBER_TOKEN,
                                                                   .metadata = {.num = -(sign == SUB_OP) + (sign == ADD_OP)}});
                        lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
                    }
                    sign = DEFAULOP;
                }
                if (last_token(inp_queue) == NUMBER_TOKEN ||
                    (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP)) {
                    lex_queue_enqueue(inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
                }
            } else if (incomplete_input) {
                error_msg = "Incomplete input.";
                goto on_inp_error;
            }
            lex_queue_enqueue(
                inp_queue, (lexeme_type){.token = OP_TOKEN, .metadata = {.op = str[i] == '(' ? OPENING_PAREN_OP : CLOSING_PAREN_OP}});
            break;
        case ' ':
        case '\n':
        case '\0':
            continue;
        default:
            error_msg = "Unknown character.";
            error_index = i;
            goto on_inp_error;
        }
    }

    if (incomplete_input) {
        error_msg = "Incomplete input.";
        error_index = len - 2;
        goto on_inp_error;
    }

    if (opening_paren_count != closing_paren_count) {
        error_msg = "Not all open parenthesis are closed.";
        error_index = len - 2;
        goto on_inp_error;
    }

#ifdef DEBUG
    {
        size_t index;
        lexeme_type lex;
        printf("Input queue (prefix): ");
        queue_for_each(inp_queue, index, lex) {
            switch (lex.token) {
            case NUMBER_TOKEN:
                printf(" %g", lex.metadata.num);
                break;
            case OP_TOKEN:
                printf(" %c", decode_op(lex.metadata.op));
                break;
            default:
                break;
            }
        }
        putchar('\n');
    }
#endif

    inp_queue_postfix = lex_queue_create();
    if (!inp_queue_postfix) {
        goto on_oom_error;
    }
    op_stack = lex_stack_create();
    if (!op_stack) {
        goto on_oom_error;
    }

    // shunting yard algorithm (with simplified assumptions).
    {
        size_t index;
        lexeme_type lex;
        queue_for_each(inp_queue, index, lex) {
            switch (lex.token) {
            case NUMBER_TOKEN:
                lex_queue_enqueue(inp_queue_postfix, lex);
                break;
            case OP_TOKEN:
                switch (lex.metadata.op) {
                case OPENING_PAREN_OP:
                    lex_stack_push(op_stack, lex);
                    break;
                case CLOSING_PAREN_OP:
                    while (lex_stack_peek(op_stack).metadata.op != OPENING_PAREN_OP) {
                        lex_queue_enqueue(inp_queue_postfix, lex_stack_pop(op_stack));
                    }
                    lex_stack_pop(op_stack);
                    break;
                default:
                    while (!lex_stack_is_empty(op_stack) && lex_stack_peek(op_stack).metadata.op != OPENING_PAREN_OP &&
                           op_precedence_cmp(lex_stack_peek(op_stack).metadata.op, lex.metadata.op) >= 0) {
                        lex_queue_enqueue(inp_queue_postfix, lex_stack_pop(op_stack));
                    }
                    lex_stack_push(op_stack, lex);
                    break;
                }
            default:
                break;
            }
        }
    }
    while (!lex_stack_is_empty(op_stack)) {
        lex_queue_enqueue(inp_queue_postfix, lex_stack_pop(op_stack));
    }

#ifdef DEBUG
    {
        size_t i;
        lexeme_type lex;
        printf("Input queue (postfix):");
        queue_for_each(inp_queue_postfix, i, lex) {
            switch (lex.token) {
            case NUMBER_TOKEN:
                printf(" %g", lex.metadata.num);
                break;
            case OP_TOKEN:
                printf(" %c", decode_op(lex.metadata.op));
                break;
            default:
                break;
            }
        }
        putchar('\n');
    }
#endif

    {
        size_t i;
        lexeme_type lex;
        num_stack = op_stack; // repurposing the stack
        queue_for_each(inp_queue_postfix, i, lex) {
            switch (lex.token) {
            case NUMBER_TOKEN:
                lex_stack_push(num_stack, lex);
                break;
            case OP_TOKEN: {
                double y = lex_stack_pop(num_stack).metadata.num;
                double x = lex_stack_pop(num_stack).metadata.num;
                switch (lex.metadata.op) {
                case ADD_OP:
                    lex_stack_push(num_stack, (lexeme_type){.metadata = {.num = x + y}});
                    break;
                case SUB_OP:
                    lex_stack_push(num_stack, (lexeme_type){.metadata = {.num = x - y}});
                    break;
                case MUL_OP:
                    lex_stack_push(num_stack, (lexeme_type){.metadata = {.num = x * y}});
                    break;
                case DIV_OP:
                    lex_stack_push(num_stack, (lexeme_type){.metadata = {.num = x / y}});
                    break;
                case POW_OP:
                    lex_stack_push(num_stack, (lexeme_type){.metadata = {.num = pow(x, y)}});
                    break;
                default:
                    break;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    rtr_value = lex_stack_pop(num_stack).metadata.num;
    RTR_VALUE_LAST = rtr_value;

on_inp_error:
    if (error_msg != NULL) {
        fprintf(stderr, "%*c %s\n", (int)(error_index + 1), '^', error_msg);
    }
    if (op_stack != NULL) {
        lex_stack_destroy(op_stack);
    }
    if (inp_queue_postfix != NULL) {
        lex_queue_destroy(inp_queue_postfix);
    }
    if (inp_queue != NULL) {
        lex_queue_destroy(inp_queue);
    }
    return rtr_value;

on_oom_error:
    fprintf(stderr, "Out of memory.\n");

    if (op_stack != NULL) {
        lex_stack_destroy(op_stack);
    }
    if (inp_queue_postfix != NULL) {
        lex_queue_destroy(inp_queue_postfix);
    }
    if (inp_queue != NULL) {
        lex_queue_destroy(inp_queue);
    }

    return rtr_value;
}

int main() {
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;

    while (0 < (len = getline(&line_p, &n, stdin))) {
        printf(EVAL_STR "\n", eval(line_p, len));
    }
    free(line_p);
    return 0;
}

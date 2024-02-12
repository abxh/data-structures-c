#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef KATTIS

double pow(double, double) { return 0.; }
#define EVAL_STR "%.2f"

#else

#include <math.h> // pow
#define EVAL_STR "%g"

// Note:
// Must link math library using `gcc -lm` or the sorts.

#endif

#include "queue.h"
#include "stack.h"

typedef enum { DEFAULT_TOKEN, NUMBER_TOKEN, OP_TOKEN } Token;

typedef enum { DEFAULT_OP, ADD_OP, SUB_OP, MUL_OP, DIV_OP, POW_OP, OPENING_PAREN_OP, CLOSING_PAREN_OP } Operation;

typedef struct {
    Token token;
    union {
        double num;
        Operation op;
    } metadata;
} Lexeme;

QUEUE_CREATE_INLINE_FUNCTIONS(lex, Lexeme)
STACK_CREATE_INLINE_FUNCTIONS(lex, Lexeme)

char decode_op(Operation op) {
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

int op_precedence(Operation op) {
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

int op_precedence_cmp(Operation o1, Operation o2) {
    int o1_prec = op_precedence(o1);
    int o2_prec = op_precedence(o2);
    return -(o1_prec < o2_prec) + (o1_prec > o2_prec);
}

static const double RTR_VALUE_DEFAULT = 0.;
static double RTR_VALUE_LAST = RTR_VALUE_DEFAULT;

#define digit_to_num(v) ((v) - '0')
#define last_token(inp_queue) (queue_isempty(inp_queue) ? DEFAULT_TOKEN : queue_peek_last_lex(inp_queue).token)
#define last_op(inp_queue) (queue_isempty(inp_queue) ? DEFAULT_OP : queue_peek_last_lex(inp_queue).metadata.op)

double eval(char* str, ssize_t len) {
    double rtr_value = RTR_VALUE_DEFAULT;
    Queue* inp_queue = NULL;
    Queue* inp_queue_postfix = NULL;
    Stack* op_stack = NULL;
    Stack* num_stack = NULL;

    inp_queue = queue_new_lex(len);
    if (inp_queue == NULL) {
        goto on_oom_error;
    }

    // error handling:
    char* error_msg = NULL;
    ssize_t error_index = 0;
    size_t opening_paren_count = 0;
    size_t closing_paren_count = 0;
    bool incomplete_input = true;

    Operation sign = DEFAULT_OP;

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
                i++;
                if (str[i] == '+' || str[i] == '-') {
                    error_index = i;
                }
            }
            break;
        case '*':
        case '/':
            incomplete_input = true;
            error_index = i;
            if (queue_isempty(inp_queue) ||
                (last_token(inp_queue) == OP_TOKEN && (last_op(inp_queue) == MUL_OP || last_op(inp_queue) == DIV_OP))) {
                error_msg = "Incorrect use of '*' or '/'.";
                error_index = i;
                goto on_inp_error;
            }
            queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = str[i] == '*' ? MUL_OP : DIV_OP}});
            break;
        case '^':
            incomplete_input = true;
            error_index = i;
            if (queue_isempty(inp_queue) || (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == POW_OP)) {
                error_msg = "Two '^' in a row.";
                error_index = i;
                goto on_inp_error;
            }
            queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = POW_OP}});
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
                sign != DEFAULT_OP) {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = sign}});
                sign = DEFAULT_OP;
            } else if (last_token(inp_queue) == NUMBER_TOKEN) {
                error_msg = "Two numbers in a row.";
                error_index = i;
                goto on_inp_error;
            }
            if (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP) {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
            }
            if (str[i] == '_') {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = NUMBER_TOKEN, .metadata = {.num = RTR_VALUE_LAST}});
                continue;
            }
            i--;
            double value = 0.;
            while (i + 1 < len && isdigit(str[i + 1])) {
                value = 10. * value + digit_to_num(str[i + 1]);
                i++;
            }
            value = (sign != SUB_OP) * value - (sign == SUB_OP) * value;
            sign = DEFAULT_OP;
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
            queue_enqueue_lex(inp_queue, (Lexeme){.token = NUMBER_TOKEN, .metadata = {.num = value}});
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
                if (sign != DEFAULT_OP) {
                    queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = sign}});
                    sign = DEFAULT_OP;
                }
                if (last_token(inp_queue) == NUMBER_TOKEN ||
                    (last_token(inp_queue) == OP_TOKEN && last_op(inp_queue) == CLOSING_PAREN_OP)) {
                    queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
                }
            } else if (incomplete_input) {
                error_msg = "Incomplete input.";
                goto on_inp_error;
            }
            queue_enqueue_lex(inp_queue,
                              (Lexeme){.token = OP_TOKEN, .metadata = {.op = str[i] == '(' ? OPENING_PAREN_OP : CLOSING_PAREN_OP}});
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
        goto on_inp_error;
    }

    if (opening_paren_count != closing_paren_count) {
        error_msg = "Not all open parenthesis are closed.";
        error_index = len - 2;
        goto on_inp_error;
    }

    Lexeme lex;
#ifdef DEBUG
    printf("Input queue (prefix): ");
    QUEUE_FOREACH(inp_queue, lex) {
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
#endif

    inp_queue_postfix = queue_new_lex(inp_queue->used);
    if (inp_queue_postfix == NULL) {
        goto on_oom_error;
    }
    op_stack = stack_new_lex(inp_queue->used);
    if (op_stack == NULL) {
        goto on_oom_error;
    }

    // shunting yard algorithm (with simplified assumptions).
    QUEUE_FOREACH(inp_queue, lex) {
        switch (lex.token) {
        case NUMBER_TOKEN:
            queue_enqueue_lex(inp_queue_postfix, lex);
            break;
        case OP_TOKEN:
            switch (lex.metadata.op) {
            case OPENING_PAREN_OP:
                stack_push_lex(op_stack, lex);
                break;
            case CLOSING_PAREN_OP:
                while (stack_peek_lex(op_stack).metadata.op != OPENING_PAREN_OP) {
                    queue_enqueue_lex(inp_queue_postfix, stack_pop_lex(op_stack));
                }
                stack_pop_lex(op_stack);
                break;
            default:
                while (!stack_isempty(op_stack) && stack_peek_lex(op_stack).metadata.op != OPENING_PAREN_OP &&
                       op_precedence_cmp(stack_peek_lex(op_stack).metadata.op, lex.metadata.op) >= 0) {
                    queue_enqueue_lex(inp_queue_postfix, stack_pop_lex(op_stack));
                }
                stack_push_lex(op_stack, lex);
                break;
            }
        default:
            break;
        }
    }
    while (!stack_isempty(op_stack)) {
        queue_enqueue_lex(inp_queue_postfix, stack_pop_lex(op_stack));
    }

#ifdef DEBUG
    printf("Input queue (postfix):");
    QUEUE_FOREACH(inp_queue_postfix, lex) {
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
#endif

    num_stack = op_stack; // repurposing the stack
    QUEUE_FOREACH(inp_queue_postfix, lex) {
        switch (lex.token) {
        case NUMBER_TOKEN:
            stack_push_lex(num_stack, lex);
            break;
        case OP_TOKEN: {
            double y = stack_pop_lex(num_stack).metadata.num;
            double x = stack_pop_lex(num_stack).metadata.num;
            switch (lex.metadata.op) {
            case ADD_OP:
                stack_push_lex(num_stack, (Lexeme){.metadata = {.num = x + y}});
                break;
            case SUB_OP:
                stack_push_lex(num_stack, (Lexeme){.metadata = {.num = x - y}});
                break;
            case MUL_OP:
                stack_push_lex(num_stack, (Lexeme){.metadata = {.num = x * y}});
                break;
            case DIV_OP:
                stack_push_lex(num_stack, (Lexeme){.metadata = {.num = x / y}});
                break;
            case POW_OP:
                stack_push_lex(num_stack, (Lexeme){.metadata = {.num = pow(x, y)}});
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

    rtr_value = stack_pop_lex(num_stack).metadata.num;
    RTR_VALUE_LAST = rtr_value;

on_inp_error:
    if (error_msg != NULL) {
        fprintf(stderr, "%*c %s\n", (int)(error_index + 1), '^', error_msg);
    }
    stack_free(&op_stack);
    queue_free(&inp_queue_postfix);
    queue_free(&inp_queue);

    return rtr_value;

on_oom_error:
    fprintf(stderr, "Out of memory.\n");

    stack_free(&op_stack);
    queue_free(&inp_queue_postfix);
    queue_free(&inp_queue);

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

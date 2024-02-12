#include <ctype.h> // isdigit
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "stack.h"

typedef enum { DEFAULT_TOKEN, NUMBER_TOKEN, OP_TOKEN } Token;

typedef enum { DEFAULT_OP, ADD_OP, SUB_OP, MUL_OP, DIV_OP, OPENING_PAREN_OP, CLOSING_PAREN_OP } Operation;

typedef struct {
    Token token;
    union {
        double num;
        Operation op;
    } metadata;
} Lexeme;

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
    case OPENING_PAREN_OP:
        return '(';
    case CLOSING_PAREN_OP:
        return ')';
    default:
        break;
    }
    return '\0';
}

#define digit_to_num(v) ((v) - '0')

QUEUE_CREATE_INLINE_FUNCTIONS(lex, Lexeme)
STACK_CREATE_INLINE_FUNCTIONS(op, Operation)

static const double RTR_VALUE_DEFAULT = 0.;
static double RTR_VALUE_LAST = RTR_VALUE_DEFAULT;

double eval(char* str, ssize_t len) {
    double rtr_value = RTR_VALUE_DEFAULT;

    Queue* inp_queue = queue_new_lex(len);
    if (inp_queue == NULL) {
        goto on_oom_inp_queue;
    }

    // error handling:
    char* error_msg = NULL;
    ssize_t error_index = 0;
    size_t opening_paren_count = 0;
    size_t closing_paren_count = 0;
    bool incomplete_input = true;

    Operation sign = DEFAULT_OP;

    for (ssize_t i = 0; i < len; i++) {

        Token last_token = queue_isempty(inp_queue) ? DEFAULT_TOKEN : queue_peek_last_lex(inp_queue).token;
        Operation last_op = queue_isempty(inp_queue) ? DEFAULT_OP : queue_peek_last_lex(inp_queue).metadata.op;

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
            if (queue_isempty(inp_queue) || (last_token == OP_TOKEN && (last_op == MUL_OP || last_op == DIV_OP))) {
                error_msg = "Incorrect use of '*' or '/'.";
                error_index = i;
                goto on_inp_error;
            }
            queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = str[i] == '*' ? MUL_OP : DIV_OP}});
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
            if ((last_token == NUMBER_TOKEN || (last_token == PAREN_TOKEN && last_paren == CLOSING_PAREN)) && sign != DEFAULT_OP) {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = sign}});
                sign = DEFAULT_OP;
            } else if (last_token == NUMBER_TOKEN) {
                error_msg = "Two numbers in a row.";
                error_index = i;
                goto on_inp_error;
            }
            if (last_token == OP_TOKEN && last_op == CLOSING_PAREN_OP) {
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
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = sign}});
                sign = DEFAULT_OP;
                if (last_token == NUMBER_TOKEN || (last_token == OP_TOKEN && last_op == CLOSING_PAREN_OP)) {
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

    // #ifdef DEBUG
    printf("Input queue:");
    Lexeme lex;
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
    // #endif

    Queue* inp_queue_postfix = queue_new_lex(inp_queue->used);
    if (inp_queue_postfix == NULL) {
        goto on_oom_inp_queue_postfix;
    }   
    Stack* op_stack = stack_new_op(inp_queue->used);
    if (op_stack == NULL) {
        goto on_oom_op_stack;
    }

    // shunting yard algorithm (with simplified assumptions).
    size_t nmemb = inp_queue->used;
    for (size_t i = 0; i < nmemb; i++) {

    }

    RTR_VALUE_LAST = rtr_value;

    stack_free(op_stack);
on_oom_op_stack:
    queue_free(inp_queue_postfix);
on_inp_error:
    if (error_msg != NULL) {
        fprintf(stderr, "%*c %s\n", (int)(error_index + 1), '^', error_msg);
    }
on_oom_inp_queue_postfix:
    queue_free(inp_queue);
on_oom_inp_queue:
    if (inp_queue == NULL || inp_queue_postfix == NULL || op_stack == NULL) {
        fprintf(stderr, "Out of memory.\n");
    }
    return rtr_value;
}

int main() {
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;

    while (0 < (len = getline(&line_p, &n, stdin))) {
        eval(line_p, len);
    }
    free(line_p);
    return 0;
}

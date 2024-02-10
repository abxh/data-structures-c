#include <ctype.h> // isdigit
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "stack.h"

typedef struct {
    char* str_p;
    size_t len;
} StringView;

typedef enum { DEFAULT_TOKEN, NUMBER_TOKEN, OP_TOKEN, PAREN_TOKEN } Token;

typedef enum { DEFAULT_OP, ADD_OP, SUB_OP, MUL_OP, DIV_OP } Operation;

typedef enum { OPENING_PAREN, CLOSING_PAREN } Paren;

typedef struct {
    Token token;
    union {
        double num;
        Operation op;
        Paren paren;
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
    default:
        break;
    }
    return DEFAULT_OP;
}

#define digit_to_num(v) ((v) - '0')

QUEUE_CREATE_INLINE_FUNCTIONS(lex, Lexeme)
STACK_CREATE_INLINE_FUNCTIONS(op, Operation)

static const double RTR_VALUE_DEFAULT = 0.;
static double RTR_VALUE_LAST = RTR_VALUE_DEFAULT;

double eval(char* str, size_t n) {
    double rtr_value = RTR_VALUE_DEFAULT;

    Queue* inp_queue = queue_new_lex(n);

    // error handling:
    char* error_msg = NULL;
    size_t error_index = 0;
    size_t opening_paren_count = 0;
    size_t closing_paren_count = 0;
    bool incomplete_input = true;

    // variables used for signed numbers:
    Operation sign = DEFAULT_OP;
    bool first_term_exists = false;

    for (size_t i = 0; i < n; i++) {
        Token last_token = queue_isempty(inp_queue) ? DEFAULT_TOKEN : queue_peek_last_lex(inp_queue).token;
        switch (str[i]) {
        case '+':
        case '-':
            incomplete_input = true;
            sign = str[i] == '+' ? ADD_OP : SUB_OP;
            error_index = i;
            while (i + 1 < n && (str[i + 1] == '-' || str[i + 1] == '+' || str[i + 1] == ' ')) {
                if (str[i + 1] == '-') {
                    sign = sign == SUB_OP ? ADD_OP : SUB_OP;
                }
                i++;
                if (str[i] == '+' || str[i] == '-') {
                    error_index = i;
                }
            }
            if (first_term_exists) {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = sign}});
            }
            break;
        case '*':
        case '/':
            incomplete_input = true;
            error_index = i;
            if (last_token == DEFAULT_TOKEN || (last_token == OP_TOKEN && (queue_peek_last_lex(inp_queue).metadata.op == MUL_OP ||
                                                                           queue_peek_last_lex(inp_queue).metadata.op == DIV_OP))) {
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
            first_term_exists = true;
            incomplete_input = false;
            if (last_token == NUMBER_TOKEN) {
                error_msg = "Two numbers in a row.";
                error_index = i;
                goto on_inp_error;
            }
            if (last_token == PAREN_TOKEN && queue_peek_last_lex(inp_queue).metadata.paren == CLOSING_PAREN) {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
            }
            if (str[i] == '_') {
                queue_enqueue_lex(inp_queue, (Lexeme){.token = NUMBER_TOKEN, .metadata = {.num = RTR_VALUE_LAST}});
                continue;
            }
            i--;
            double value = 0.;
            while (i + 1 < n && isdigit(str[i + 1])) {
                value = 10. * value + digit_to_num(str[i + 1]);
                i++;
            }
            value = (sign != SUB_OP) * value - (sign == SUB_OP) * value;
            if (str[i + 1] == '.') {
                if (i + 2 < n && !isdigit(str[i + 2])) {
                    error_msg = "No digits after '.'.";
                    error_index = i + 1;
                    goto on_inp_error;
                }
                i++;
                double c = 10.;
                while (i + 1 < n && isdigit(str[i + 1])) {
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
                first_term_exists = false;
                incomplete_input = true;
                error_index = i;
                if (last_token == NUMBER_TOKEN) {
                    queue_enqueue_lex(inp_queue, (Lexeme){.token = OP_TOKEN, .metadata = {.op = MUL_OP}});
                }
            } else if (incomplete_input) {
                error_msg = "Incomplete input.";
                goto on_inp_error;
            } 
            queue_enqueue_lex(inp_queue,
                              (Lexeme){.token = PAREN_TOKEN, .metadata = {.paren = str[i] == '(' ? OPENING_PAREN : CLOSING_PAREN}});
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
        error_index = n - 2;
        goto on_inp_error;
    }

    /*
    Queue* inp_queue_postfix = queue_new_lex(inp_queue->used);
    Stack* op_stack = stack_new_op(inp_queue->used);
    */

    RTR_VALUE_LAST = rtr_value;

on_inp_error:
    if (error_msg != NULL) {
        fprintf(stderr, "%*c %s\n", (int)(error_index + 1), '^', error_msg);
    }
    queue_free(inp_queue);
    return rtr_value;
}

int main() {
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;

    while (0 < (len = getline(&line_p, &n, stdin))) {
        printf("res: %g\n", eval(line_p, len));
    }
    free(line_p);
    return 0;
}

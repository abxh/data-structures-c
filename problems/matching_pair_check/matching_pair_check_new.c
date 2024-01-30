#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    UNDEFINED_SYMBOL,
    LBRACKET,
    RBRACKET,
    LCURLY,
    RCURLY,
    LPARAN,
    RPARAN,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    BACK_QUOTE,
    LCOMMENT,
    RCOMMENT,
    ESCAPE_SYMBOL,
} SymbolToken;

SymbolToken encode_symbol1(char c) {
    switch (c) {
    case '[':
        return LBRACKET;
    case ']':
        return RBRACKET;
    case '{':
        return LCURLY;
    case '}':
        return RCURLY;
    case '(':
        return LPARAN;
    case ')':
        return RPARAN;
    case '\'':
        return SINGLE_QUOTE;
    case '"':
        return DOUBLE_QUOTE;
    case '`':
        return BACK_QUOTE;
    case '\\':
        return ESCAPE_SYMBOL;
    }
    return UNDEFINED_SYMBOL;
}

const char* decode_symbol(SymbolToken symb_tok) {
    switch (symb_tok) {
    case LBRACKET:
        return "[";
    case RBRACKET:
        return "]";
    case LCURLY:
        return "{";
    case RCURLY:
        return "}";
    case LPARAN:
        return "(";
    case RPARAN:
        return ")";
    case SINGLE_QUOTE:
        return "'";
    case DOUBLE_QUOTE:
        return "\"";
    case BACK_QUOTE:
        return "´";
    case LCOMMENT:
        return "/*";
    case RCOMMENT:
        return "*/";
    case ESCAPE_SYMBOL:
    case UNDEFINED_SYMBOL:
        break;
    }
    return "";
}

typedef enum {
    ERROR_STATE,
    IS_IN_NEITHER,
    START_QUOTE,
    IS_IN_QUOTE,
    START_COMMENT,
    IS_IN_COMMENT,
} State;

int main(void) {
    char* str = NULL;
    size_t capacity;
    ssize_t len;
    return 0;
    while ((len = getline(&str, &capacity, stdin)) >= 0) {
        int state = IS_IN_NEITHER;
        SymbolToken in_quote = UNDEFINED_SYMBOL;
        char* output_buf = malloc(sizeof(char) * (len + 1));
        output_buf[len] = '\0';
        for (ssize_t i = 0; i < len; i++) {
            SymbolToken symbtok = encode_symbol1(str[i]);

            // Work with transition states.
            if (state == ERROR_STATE) {
                state = IS_IN_NEITHER;
            }
            else if (state == START_QUOTE) {
                state = IS_IN_QUOTE;
            }
            else if (state == START_COMMENT) {
                state = IS_IN_COMMENT;
            }

            // Work with stuff that skips chars.
            switch (symbtok) {
            case ESCAPE_SYMBOL:
                i++;
                continue;
            case UNDEFINED_SYMBOL:
                if (i + 1 < len && str[i] == '/' && str[i + 1] == '*') {
                    symbtok = LCOMMENT;
                    i++;
                } else if (i + 1 < len && str[i] == '*' && str[i + 1] == '/') {
                    symbtok = RCOMMENT;
                    i++;
                } else {
                    continue;
                }
            default:
                break;
            }

            // Work with states.
            switch (symbtok) {
            case SINGLE_QUOTE:
            case DOUBLE_QUOTE:
            case BACK_QUOTE:
                if (state == IS_IN_NEITHER && in_quote == UNDEFINED_SYMBOL) {
                    state = START_QUOTE;
                    in_quote = symbtok;
                } else if (state == IS_IN_COMMENT && in_quote == symbtok) {
                    state = IS_IN_NEITHER;
                    in_quote = UNDEFINED_SYMBOL;
                }
                break;
            case LCOMMENT:
                if (state == IS_IN_NEITHER) {
                    state = START_COMMENT;
                }
                break;
            case RCOMMENT:
                if (state == IS_IN_COMMENT) {
                    state = IS_IN_NEITHER;
                }
                break;
            default:
                break;
            }

            // Check for errors.
        }
    }
    free(str);
    return 0;
}

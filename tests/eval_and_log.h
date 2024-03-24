#include <stdio.h> // fprintf

// reference:
// https://stackoverflow.com/a/168611

static inline bool eval_and_log_internal(bool expr_value, int linenr, const char* pretty_function, const char* expr_str) {
    if (!expr_value) {
        fprintf(stderr, "line %d (%s): precondition `%s' failed.\n", linenr, pretty_function, expr_str);
    }
    return expr_value;
}

#define eval_and_log(expr) eval_and_log_internal((expr), __LINE__, __PRETTY_FUNCTION__, #expr)

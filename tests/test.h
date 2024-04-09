#include <stdbool.h> // bool
#include <stdio.h>   // fprintf, stderr
#include <stdlib.h>  // size_t

// add workaround for non-standard macro:
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

// The following lines uses GNU C extensions and may perhaps not work in other compilers except gcc.

// reference:
// https://stackoverflow.com/a/168611
// https://stackoverflow.com/questions/9804371/syntax-and-sample-usage-of-generic-in-c11
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html

static size_t error_message_count = 0;
#define error_message_count_lim 5

#define is_true(expr)                                                                                        \
    ({                                                                                                       \
        bool expr_value = (expr);                                                                            \
        if (!expr_value && error_message_count < error_message_count_lim) {                                  \
            fprintf(stderr, "line %d (%s): assertion `%s' failed.\n", __LINE__, __PRETTY_FUNCTION__, #expr); \
            error_message_count++;                                                                           \
        }                                                                                                    \
        expr_value;                                                                                          \
    })
#define is_false(expr) is_true(!(expr))
#define is_equal_internal(format, a, b)                                                   \
    ({                                                                                    \
        typeof(a) a_copy = (a);                                                           \
        typeof(b) b_copy = (b);                                                           \
        bool res = a_copy == b_copy;                                                      \
        if (!res && error_message_count < error_message_count_lim) {                      \
            const char* str_p = "line %d (%s): " format " is not equal to " format ".\n"; \
            fprintf(stderr, str_p, __LINE__, __PRETTY_FUNCTION__, a_copy, b_copy);        \
            error_message_count++;                                                        \
        }                                                                                 \
        res;                                                                              \
    })
#define type_mismatch()                                                                                \
    ({                                                                                                 \
        if (error_message_count < error_message_count_lim) {                                           \
            fprintf(stderr, "line %d (%s): is_equal type mismatch.\n", __LINE__, __PRETTY_FUNCTION__); \
            error_message_count++;                                                                     \
        }                                                                                              \
        false;                                                                                         \
    })
#define is_equal(a, b)                                                                                 \
    _Generic((a),                                                                                      \
        int: (_Generic((b), int: is_equal_internal("%d", (a), (b)), default: type_mismatch())),        \
        size_t: (_Generic((b), size_t: is_equal_internal("%zu", (a), (b)), default: type_mismatch())), \
        default: type_mismatch())

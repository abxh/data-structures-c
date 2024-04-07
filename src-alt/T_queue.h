#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef VALUE_NAME
#error "Must declare VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define T_queue JOIN(VALUE_NAME, queue)

typedef struct {
    VALUE_TYPE arr_p[];
} T_queue;

#undef T_queue

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_NAME
#undef VALUE_TYPE

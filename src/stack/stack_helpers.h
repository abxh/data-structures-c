/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with name T are:
    - stack_init_T
    - stack_peek_T
    - stack_push_T
    - stack_pop_T

    A distinction between the type and name is made as the type
    cannot include spaces.
*/

#include "stack.h"
#include <stdlib.h> // NULL
#include <stdbool.h> // bool, true, false

#ifndef VALUE_NAME
#error "Must declare VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(stack_init, VALUE_NAME)(Stack** stack_pp) {
    return stack_init(stack_pp, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(stack_peek, VALUE_NAME)(const Stack* stack_p) {
    return *(VALUE_TYPE*)(stack_peek_node(stack_p)->value_p);
}

static inline bool JOIN(stack_push, VALUE_NAME)(Stack* stack_p, const VALUE_TYPE value) {
    StackNode* node_p = stacknode_create(stack_p);
    *(VALUE_TYPE*)node_p->value_p = value;
    stack_push_node(stack_p, node_p);
    return true;
}

static inline VALUE_TYPE JOIN(stack_pop, VALUE_NAME)(Stack* stack_p) {
    StackNode* node_p = stack_pop_node(stack_p);
    VALUE_TYPE value = *(VALUE_TYPE*)node_p->value_p;
    stacknode_free(stack_p, node_p);
    return value;
}

#undef VALUE_TYPE
#undef VALUE_NAME

#undef CAT
#undef PASTE
#undef JOIN

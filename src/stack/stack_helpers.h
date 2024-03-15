/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with label T are:
    - stacknode_get_value_T
    - stacknode_set_value_T
    - stack_peek_T
    - stack_push_T
    - stack_pop_T

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#include "stack.h"
#include <assert.h> // assert
#include <stdlib.h> // NULL, size_t, malloc, free

#ifndef VALUE_LABEL
#error "Must declare VALUE_LABEL. Defaulting to 'int'."
#define VALUE_LABEL int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define stacknode_get_value_T JOIN(stacknode_get_value, VALUE_LABEL)
#define stacknode_set_value_T JOIN(stacknode_set_value, VALUE_LABEL)
#define stack_peek_T JOIN(stack_peek, VALUE_LABEL)
#define stack_push_T JOIN(stack_push, VALUE_LABEL)
#define stack_pop_T JOIN(stack_pop, VALUE_LABEL)

static inline VALUE_TYPE stacknode_get_value_T(const StackNode* node_p) {
    assert(node_p != NULL);

    return *(VALUE_TYPE*)node_p->value_p;
}

static inline void stacknode_set_value_T(StackNode* node_p, VALUE_TYPE value) {
    assert(node_p != NULL);

    *(VALUE_TYPE*)node_p->value_p = value;
}

static inline VALUE_TYPE stack_peek_T(const Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_p->value_size == sizeof(VALUE_TYPE));

    return stacknode_get_value_T(stack_peek_node(stack_p));
}

static inline bool stack_push_T(Stack* stack_p, VALUE_TYPE value) {
    assert(stack_p != NULL);
    assert(stack_p->value_size == sizeof(VALUE_TYPE));

    StackNode* node_p = (StackNode*)malloc(sizeof(StackNode));
    if (node_p == NULL) {
        return false;
    }
    node_p->value_p = malloc(sizeof(VALUE_TYPE));
    if (node_p->value_p == NULL) {
        free(node_p);
        return false;
    }
    stacknode_set_value_T(node_p, value);

    stack_push_node(stack_p, node_p);

    return true;
}

static inline VALUE_TYPE stack_pop_T(Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_p->value_size == sizeof(VALUE_TYPE));

    StackNode* node_p = stack_pop_node(stack_p);
    void* value_p = node_p->value_p;

    VALUE_TYPE value = stacknode_get_value_T(node_p);

    free(value_p);
    free(node_p);

    return value;
}

#undef stacknode_get_value_T
#undef stacknode_set_value_T
#undef stack_peek_T
#undef stack_push_T
#undef stack_pop_T

#undef VALUE_TYPE
#undef VALUE_LABEL

#undef CAT
#undef PASTE
#undef JOIN

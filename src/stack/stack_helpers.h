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

#define STACKNODE_GET_VALUE_T JOIN(stacknode_get_value, VALUE_TYPE)
#define STACKNODE_SET_VALUE_T JOIN(stacknode_set_value, VALUE_TYPE)
#define STACK_PEEK_T JOIN(stack_peek, VALUE_LABEL)
#define STACK_PUSH_T JOIN(stack_push, VALUE_LABEL)
#define STACK_POP_T JOIN(stack_pop, VALUE_LABEL)

static inline VALUE_TYPE STACKNODE_GET_VALUE_T(const StackNode* node_p) {
    assert(node_p != NULL);

    return *(VALUE_TYPE*)node_p->value_p;
}

static inline void STACKNODE_SET_VALUE_T(StackNode* node_p, VALUE_TYPE value) {
    assert(node_p != NULL);

    *(VALUE_TYPE*)node_p->value_p = value;
}

static inline VALUE_TYPE STACK_PEEK_T(const Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_p->value_size == sizeof(VALUE_TYPE));

    return STACKNODE_GET_VALUE_T(stack_peek(stack_p));
}

static inline bool STACK_PUSH_T(Stack* stack_p, VALUE_TYPE value) {
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
    STACKNODE_SET_VALUE_T(node_p, value);

    stack_push(stack_p, node_p);

    return true;
}

static inline VALUE_TYPE STACK_POP_T(Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_p->value_size == sizeof(VALUE_TYPE));

    StackNode* node_p = stack_pop(stack_p);
    void* value_p = node_p->value_p;

    VALUE_TYPE value = STACKNODE_GET_VALUE_T(node_p);

    free(value_p);
    free(node_p);

    return value;
}

#undef STACKNODE_GET_VALUE_T
#undef STACKNODE_SET_VALUE_T
#undef STACK_PEEK_T
#undef STACK_PUSH_T
#undef STACK_POP_T

#undef VALUE_TYPE
#undef VALUE_LABEL

#undef CAT
#undef PASTE
#undef JOIN

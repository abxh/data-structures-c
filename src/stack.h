/*
   This is a implementation of a stack with a linked-list.
*/

#ifndef __STACK__H
#define __STACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t, free

typedef struct StackNode {
    struct StackNode* next_p;
    void* value_p;
} StackNode;

typedef struct {
    StackNode* head_p;
    size_t value_size;
    size_t used_count;
} Stack;

extern bool stack_init(Stack** stack_pp, size_t value_size);

extern bool stack_deinit(Stack** stack_pp);

extern bool stack_isempty(Stack* stack_p);

extern size_t stack_used_count(Stack* stack_p);

extern StackNode* stack_peek(Stack* stack_p);

extern bool stack_push(Stack* stack_p, void* value, size_t value_size);

extern StackNode* stack_pop(Stack* stack_p);

#define stack_foreach(stack_p, node_p, value)                                            \
    for ((assert((stack_p)->value_size == sizeof(value)), (node_p) = (stack_p)->head_p); \
         (node_p) != NULL && ((value) = *(typeof(value)*)(node_p)->value_p, true); (node_p) = (node_p)->next_p)

#endif

#ifndef NGENERATING_MACROS

#ifndef VALUE_NAME
#error "Must declare stack VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif
#ifndef VALUE_TYPE
#error "Must declare stack VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(stack_init, VALUE_NAME)(Stack** stack_pp) {
    return stack_init(stack_pp, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(stack_peek, VALUE_NAME)(Stack* stack_p) {
    return *(VALUE_TYPE*)stack_peek(stack_p)->value_p;
}

static inline bool JOIN(stack_push, VALUE_NAME)(Stack* stack_p, VALUE_TYPE value) {
    return stack_push(stack_p, &value, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(stack_pop, VALUE_NAME)(Stack* stack_p) {
    StackNode* node_p = stack_pop(stack_p);
    void* value_p = node_p->value_p;
    VALUE_TYPE value = *(VALUE_TYPE*)value_p;
    free(value_p);
    free(node_p);
    return value;
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

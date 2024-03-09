/*
   This is a implementation of a stack as a linked-list.
*/

#ifndef __STACK__H
#define __STACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

typedef struct StackNode {
    struct StackNode* next_p;
    void* value_p;
} StackNode;

typedef struct {
    StackNode* head_p;
    size_t value_size;
    size_t length;
} Stack;

static inline size_t stack_length(Stack* stack_p) {
    return stack_p->length;
}

static inline bool stack_isempty(Stack* stack_p) {
    return stack_p->head_p == NULL;
}

static inline bool stack_init(Stack** stack_pp, size_t value_size) {
    assert(value_size != 0);
    *stack_pp = (Stack*)malloc(sizeof(Stack));
    if (*stack_pp == NULL) {
        return false;
    }
    (*stack_pp)->head_p = NULL;
    (*stack_pp)->length = 0;
    (*stack_pp)->value_size = value_size;
    return true;
}

static inline bool stack_deinit(Stack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    StackNode* head_p = (*stack_pp)->head_p;
    StackNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }
    free(*stack_pp);
    (*stack_pp) = NULL;
    return true;

}

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
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(stack_isempty(stack_p) == false);

    return *(VALUE_TYPE*)stack_p->head_p->value_p;
}

static inline bool JOIN(stack_push, VALUE_NAME)(Stack* stack_p, VALUE_TYPE value) {
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
    memcpy(node_p->value_p, &value, sizeof(VALUE_TYPE));

    node_p->next_p = stack_p->head_p;
    stack_p->head_p = node_p;
    stack_p->length++;

    return true;
}

static inline VALUE_TYPE JOIN(stack_pop, VALUE_NAME)(Stack* stack_p) {
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(stack_isempty(stack_p) == false);

    StackNode* node_p = stack_p->head_p;
    stack_p->head_p = node_p->next_p;
    stack_p->length--;

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

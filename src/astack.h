/*
   This is a implementation of a fixed-size stack as an array.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, calloc, reallocarray, free

typedef struct {
    size_t capacity;
    size_t length;
    size_t value_size;
    void* arr_p;
} AStack;

static inline size_t astack_length(AStack* stack_p) {
    return stack_p->length;
}

static inline size_t astack_capacity(AStack* stack_p) {
    return stack_p->capacity;
}

static inline bool astack_isempty(AStack* stack_p) {
    return stack_p->length == 0;
}

static inline bool astack_isfull(AStack* stack_p) {
    return stack_p->length == stack_p->capacity;
}

static inline bool astack_init(AStack** stack_pp, size_t capacity, size_t value_size) {
    assert(capacity != 0);
    *stack_pp = (AStack*)malloc(sizeof(AStack));
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->arr_p = calloc(capacity, sizeof(value_size));
    if ((*stack_pp)->arr_p == NULL) {
        free(*stack_pp);
        *stack_pp = NULL;
        return false;
    }
    (*stack_pp)->length = 0;
    (*stack_pp)->capacity = capacity;
    (*stack_pp)->value_size = value_size;
    return true;
}

static inline bool astack_deinit(AStack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
    return true;
}

#define astack_foreach(stack_p, index, value)                                                   \
    for ((assert((stack_p)->value_size == sizeof(typeof(value))), (index) = (stack_p)->length); \
         ((index) != 0 && ((value) = ((typeof(value)*)(stack_p)->arr_p)[((index)-1)], true)); (index)--)

#endif

#ifndef NGENERATING_MACROS

#ifndef VALUE_NAME
#error "Must declare astack VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif
#ifndef VALUE_TYPE
#error "Must declare astack VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(astack_init, VALUE_NAME)(AStack** stack_pp, size_t capacity) {
    return astack_init(stack_pp, capacity, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(astack_peek, VALUE_NAME)(AStack* stack_p) {
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(astack_isempty(stack_p) == false);

    return ((VALUE_TYPE*)stack_p->arr_p)[stack_p->length - 1];
}

static inline void JOIN(astack_push, VALUE_NAME)(AStack* stack_p, VALUE_TYPE value) {
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(astack_isfull(stack_p) == false);

    ((VALUE_TYPE*)stack_p->arr_p)[stack_p->length++] = value;
}

static inline VALUE_TYPE JOIN(astack_pop, VALUE_NAME)(AStack* stack_p) {
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(astack_isempty(stack_p) == false);

    return ((VALUE_TYPE*)stack_p->arr_p)[--stack_p->length];
}

static inline bool JOIN(astack_resize, VALUE_NAME)(AStack* stack_p, size_t new_capacity) {
    assert(stack_p->value_size == sizeof(VALUE_TYPE));
    assert(new_capacity != 0);
    void* new_arr_p = reallocarray(stack_p->arr_p, new_capacity, sizeof(VALUE_TYPE));
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

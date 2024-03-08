/*
   This is a implementation of a fixed-size stack with an array.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // NULL

typedef struct {
    size_t capacity;
    size_t used_count;
    size_t value_size;
    void* arr_p;
} AStack;

extern bool astack_init(AStack** stack_pp, size_t capacity, size_t value_size);

extern bool astack_deinit(AStack** stack_pp);

extern bool astack_isempty(AStack* stack_p);

extern bool astack_isfull(AStack* stack_p);

extern bool astack_used_count(AStack* stack_p);

#define astack_foreach(stack_p, index, value)                                                       \
    for ((assert((stack_p)->value_size == sizeof(typeof(value))), (index) = (stack_p)->used_count); \
         ((index) != 0 && ((value) = ((typeof(value)*)(stack_p)->arr_p)[((index)-1)], true)); (index)--)

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

static inline bool JOIN(astack_init, VALUE_NAME)(AStack** stack_pp, size_t capacity) {
    return astack_init(stack_pp, capacity, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(astack_peek, VALUE_NAME)(AStack* stack_p) {
    assert(astack_isempty(stack_p) == false);
    return ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used_count - 1];
}

static inline void JOIN(astack_push, VALUE_NAME)(AStack* stack_p, VALUE_TYPE value) {
    assert(astack_isfull(stack_p) == false);
    ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used_count++] = value;
}

static inline VALUE_TYPE JOIN(astack_pop, VALUE_NAME)(AStack* stack_p) {
    assert(astack_isempty(stack_p) == false);
    return ((VALUE_TYPE*)stack_p->arr_p)[--stack_p->used_count];
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

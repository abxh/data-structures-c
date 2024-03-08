/*
   This is a implementation of a fixed-size stack with an array.
*/

#ifndef __FSTACK__H
#define __FSTACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // NULL

typedef struct {
    size_t capacity;
    size_t used;
    size_t value_size;
    void* arr_p;
} FStack;

extern bool fstack_init(FStack** stack_pp, size_t capacity, size_t value_size);

extern bool fstack_deinit(FStack** stack_pp);

extern bool fstack_isempty(FStack* stack_p);

extern bool fstack_isfull(FStack* stack_p);

extern bool fstack_resize(FStack* stack_p, size_t new_capacity);

#define stack_foreach(stack_p, index, value)                                                  \
    for ((assert((stack_p)->value_size == sizeof(typeof(value))), (index) = (stack_p)->used); \
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

static inline VALUE_TYPE JOIN(fstack_init, VALUE_NAME)(FStack** stack_pp, size_t capacity) {
    return fstack_init(stack_pp, capacity, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(fstack_peek, VALUE_NAME)(FStack* stack_p) {
    assert(stack_p->used > 0);
    return ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used - 1];
}

static inline void JOIN(fstack_push, VALUE_NAME)(FStack* stack_p, VALUE_TYPE value) {
    assert(stack_p->used < stack_p->capacity);
    ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used++] = value;
}

static inline VALUE_TYPE JOIN(fstack_pop, VALUE_NAME)(FStack* stack_p) {
    assert(stack_p->used > 0);
    return ((VALUE_TYPE*)stack_p->arr_p)[--stack_p->used];
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

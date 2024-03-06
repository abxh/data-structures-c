#ifndef STACK_H
#define STACK_H

#include "macros.h" // JOIN
#include <assert.h> // assert
#include <stdlib.h> // size_t

typedef struct {
    size_t used;
    size_t capacity;
    size_t data_size;
    void* arr_p;
} Stack;

bool stack_init(Stack** stack_pp, size_t capacity, size_t data_size);

bool stack_isempty(const Stack* stack_p);

bool stack_isfull(const Stack* stack_p);

bool stack_resize(Stack* stack_p, size_t new_capacity);

void stack_free(Stack** stack_pp);

#define stack_foreach(stack_p, var)                                             \
    for (size_t i_, keep_ = 1; keep_ == 1;)                                     \
        for (typeof(var)* arr_p = (typeof(var)*)stack_p->arr_p; keep_ == 1;)    \
            for ((assert((stack_p)->data_size == sizeof(typeof(var))), i_ = 0); \
                 (i_ < (stack_p)->used && ((var) = arr_p[i_], true)) || (keep_ ^= 1, false); i_++)

#endif

#ifndef NSTACK_DEFINE_INLINE_FUNCTIONS

#ifndef NAME
#error "Must declare stack NAME. Defaulting to 'int'."
#define NAME int
#endif

#ifndef TYPE
#error "Must declare stack TYPE. Defaulting to int."
#define TYPE int
#endif

static inline TYPE JOIN(stack_peek, NAME)(Stack* stack_p) {
    assert(stack_p->data_size == sizeof(TYPE));
    assert(stack_p->used > 0);
    return ((TYPE*)stack_p->arr_p)[stack_p->used - 1];
}

static inline void JOIN(stack_push, NAME)(Stack* stack_p, TYPE value) {
    assert(stack_p->data_size == sizeof(TYPE));
    assert(stack_p->used < stack_p->capacity);
    ((TYPE*)stack_p->arr_p)[stack_p->used++] = value;
}

static inline TYPE JOIN(stack_pop, NAME)(Stack* stack_p) {
    assert(stack_p->data_size == sizeof(TYPE));
    assert(stack_p->used > 0);
    return ((TYPE*)stack_p->arr_p)[--stack_p->used];
}

#undef NAME
#undef TYPE

#endif

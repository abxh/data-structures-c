#pragma once

#include <assert.h> // assert
#include <stdlib.h> // size_t, SIZE_MAX

typedef struct {
    size_t used;
    size_t capacity;
    size_t data_size;
    unsigned char* arr_p;
} Stack;

bool stack_init(Stack** stack_pp, size_t capacity, size_t data_size);

bool stack_isempty(const Stack* stack_p);

bool stack_isfull(const Stack* stack_p);

bool stack_resize(Stack* stack_p, size_t new_capacity);

void stack_free(Stack** stack_pp);

#define STACK_CREATE_INLINE_FUNCTIONS(name, type)                      \
    static inline type stack_peek_##name(Stack* stack_p) {             \
        assert(stack_p->data_size == sizeof(type));                    \
        assert(stack_p->used > 0);                                     \
        return ((type*)stack_p->arr_p)[stack_p->used - 1];             \
    }                                                                  \
    static inline void stack_push_##name(Stack* stack_p, type value) { \
        assert(stack_p->data_size == sizeof(type));                    \
        assert(stack_p->used < stack_p->capacity);                     \
        ((type*)stack_p->arr_p)[stack_p->used++] = value;              \
    }                                                                  \
    static inline type stack_pop_##name(Stack* stack_p) {              \
        assert(stack_p->data_size == sizeof(type));                    \
        assert(stack_p->used > 0);                                     \
        return ((type*)stack_p->arr_p)[--stack_p->used];               \
    }

#define stack_foreach(stack_p, var)                                            \
    for (size_t i, keep = 1; keep;)                                            \
        for (typeof(var)* arr_p = (typeof(var)*)stack_p->arr_p; keep;)         \
            for ((assert((stack_p)->data_size == sizeof(typeof(var))), i = 0); \
                 (i < (stack_p)->used && ((var) = arr_p[i], true)) || (keep ^= 1, false); i++)

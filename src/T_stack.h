/*
    Including this header file generates a struct and functions for a given stack type.

    The following macros gets defined once:
    - T_stack_for_each

    The following structs are generated for a given value type T:
    - T_stack_type

    The following functions are generated for a given value type T:
    - T_stack_init
    - T_stack_deinit
    - T_stack_copy
    - T_stack_count
    - T_stack_is_empty
    - T_stack_is_full
    - T_stack_peek
    - T_stack_push
    - T_stack_pop

    Note that the given types cannot include spaces because C functions
    and variables cannot either.
    Use a typedef and replace spaces with _ if needed.
*/

#ifndef __T_STACK__H
#define __T_STACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stddef.h>  // offsetof
#include <stdint.h>  // SIZE_MAX
#include <stdlib.h>  // size_t, NULL, malloc, free
#include <string.h>  // memcpy

#define T_stack_for_each(stack_p, index_plus_one, value)                                                                        \
    for ((index_plus_one) = (stack_p)->count; ((index_plus_one) > 0 && ((value) = (stack_p)->arr_p[(index_plus_one)-1], true)); \
         (index_plus_one)--)

#define __T_STACK__H
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define T_stack JOIN(VALUE_TYPE, stack)
#define T_stack_type JOIN(T_stack, type)

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE arr_p[];
} T_stack_type;

static inline bool JOIN(T_stack, init)(T_stack_type** stack_pp, size_t capacity) {
    assert(stack_pp != NULL);

    if (capacity == 0 || capacity > (SIZE_MAX - offsetof(T_stack_type, arr_p)) / sizeof(VALUE_TYPE)) {
        return false;
    }
    *stack_pp = malloc(offsetof(T_stack_type, arr_p) + sizeof(VALUE_TYPE) * capacity);
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->count = 0;
    (*stack_pp)->capacity = capacity;

    return true;
}

static inline bool JOIN(T_stack, deinit)(T_stack_type** stack_pp) {
    assert(stack_pp != NULL);

    if (*stack_pp == NULL) {
        return false;
    }
    free(*stack_pp);
    *stack_pp = NULL;

    return true;
}

static inline bool JOIN(T_stack, copy)(T_stack_type** stack_dest_pp, T_stack_type* stack_src_p) {
    assert(stack_src_p != NULL);
    assert(stack_dest_pp != NULL);

    if (!JOIN(T_stack, init)(stack_dest_pp, stack_src_p->capacity)) {
        return false;
    }
    memcpy((*stack_dest_pp)->arr_p, stack_src_p->arr_p, sizeof(VALUE_TYPE) * stack_src_p->capacity);
    (*stack_dest_pp)->count = stack_src_p->count;

    return true;
}

static inline size_t JOIN(T_stack, count)(const T_stack_type* stack_p) {
    assert(stack_p != NULL);

    return stack_p->count;
}

static inline size_t JOIN(T_stack, capacity)(const T_stack_type* stack_p) {
    assert(stack_p != NULL);

    return stack_p->capacity;
}

static inline bool JOIN(T_stack, is_empty)(const T_stack_type* stack_p) {
    assert(stack_p != NULL);

    return stack_p->count == 0;
}

static inline bool JOIN(T_stack, is_full)(const T_stack_type* stack_p) {
    assert(stack_p != NULL);

    return stack_p->count == stack_p->capacity;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

static inline const VALUE_TYPE JOIN(T_stack, peek)(T_stack_type* stack_p) {
    assert(stack_p != NULL);
#define T_stack_is_empty JOIN(T_stack, is_empty)
    assert(T_stack_is_empty(stack_p) == false);
#undef T_stack_is_empty

    return stack_p->arr_p[stack_p->count - 1];
}

#pragma GCC diagnostic pop

static inline void JOIN(T_stack, push)(T_stack_type* stack_p, const VALUE_TYPE value) {
    assert(stack_p != NULL);
#define T_stack_is_full JOIN(T_stack, is_full)
    assert(T_stack_is_full(stack_p) == false);
#undef T_stack_is_full

    stack_p->arr_p[stack_p->count++] = value;
}

static inline VALUE_TYPE JOIN(T_stack, pop)(T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
#define T_stack_is_empty JOIN(T_stack, is_empty)
    assert(T_stack_is_empty(T_stack_p) == false);
#undef T_stack_is_empty

    return T_stack_p->arr_p[--T_stack_p->count];
}

#undef T_stack
#undef T_stack_type
#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN

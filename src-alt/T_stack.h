/*
    Including this header file generates a struct and functions for a given
    stack type.

    The following macros gets defined once:
    - T_stack_for_each

    The following structs are generated for a given value type t with name T:
    - T_stack_type

    The following functions are generated for a given value type t with name T:
    - T_stack_init
    - T_stack_deinit
    - T_stack_count
    - T_stack_is_empty
    - T_stack_is_full
    - T_stack_peek
    - T_stack_push
    - T_stack_pop

    A distinction between the type and name is made as the name
    cannot include spaces.
*/

#ifndef __T_STACK__H
#define __T_STACK__H

#include <assert.h> // assert
#include <stddef.h> // offsetof
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // size_t, NULL, malloc, free
#include <string.h> // memcpy

#define T_stack_for_each(T_stack_p, value, index) \
    for ((index) = 0; ((index) < (T_stack_p)->count && ((value) = (T_stack_p)->arr_p[(index)], true)); (index)++)

#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef VALUE_NAME
#error "Must declare VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define T_stack JOIN(VALUE_NAME, stack)
#define T_stack_type JOIN(T_stack, type)

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE arr_p[];
} T_stack_type;

static inline bool JOIN(T_stack, init)(T_stack_type** T_stack_pp, size_t capacity) {
    assert(T_stack_pp != NULL);
    assert(capacity != 0);
    assert(capacity <= (SIZE_MAX - offsetof(T_stack_type, arr_p)) / sizeof(VALUE_TYPE));
    *T_stack_pp = (T_stack_type*)malloc(offsetof(T_stack_type, arr_p) + sizeof(VALUE_TYPE) * capacity);
    if ((*T_stack_pp) == NULL) {
        return false;
    }
    (*T_stack_pp)->count = 0;
    (*T_stack_pp)->capacity = capacity;
    return true;
}

static inline bool JOIN(T_stack, deinit)(T_stack_type** T_stack_pp) {
    assert(T_stack_pp != NULL);
    if (*T_stack_pp == NULL) {
        return false;
    }
    free(*T_stack_pp);
    *T_stack_pp = NULL;
    return true;
}

static inline size_t JOIN(T_stack, count)(const T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    return T_stack_p->count;
}

static inline size_t JOIN(T_stack, capacity)(const T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    return T_stack_p->capacity;
}

static inline bool JOIN(T_stack, is_empty)(const T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    return T_stack_p->count == 0;
}

static inline bool JOIN(T_stack, is_full)(const T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    return T_stack_p->count == T_stack_p->capacity;
}

static inline VALUE_TYPE JOIN(T_stack, peek)(T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    assert(T_stack_p->count != 0 && "is not empty");
    return T_stack_p->arr_p[T_stack_p->count - 1];
}

static inline void JOIN(T_stack, push)(T_stack_type* T_stack_p, const VALUE_TYPE value) {
    assert(T_stack_p != NULL);
    assert(JOIN(T_stack, is_full)(T_stack_p) == false);
    T_stack_p->arr_p[T_stack_p->count++] = value;
}

static inline VALUE_TYPE JOIN(T_stack, pop)(T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
    assert(JOIN(T_stack, is_empty)(T_stack_p) == false);
    return T_stack_p->arr_p[--T_stack_p->count];
}

#undef T_stack_type_is_empty
#undef T_stack_type_is_full
#undef T_stack_type_count
#undef T_stack_type_capacity
#undef T_stack_type

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_NAME
#undef VALUE_TYPE

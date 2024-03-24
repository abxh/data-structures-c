/*
    Including this header file generates a struct and functions for a given
    stack type.

    The following macros gets defined once:
    - astack_for_each

    The following structs are generated for a given value type t with name T:
    - astack_T

    The following functions are generated for a given value type t with name T:
    - astack_T_init_with_capacity
    - astack_T_deinit
    - astack_T_count
    - astack_T_is_empty
    - astack_T_is_full
    - astack_T_peek
    - astack_T_push
    - astack_T_push_many
    - astack_T_pop

    A distinction between the type and name is made as the name
    cannot include spaces.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h> // assert
#include <stddef.h> // offsetof
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // size_t, NULL, malloc, free
#include <string.h> // memcpy

#define astack_for_each(astack_p, value, index) \
    for ((index) = 0; ((index) < (astack_p)->count && ((value) = (astack_p)->arr_p[(index)], true)); (index)++)

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

#define astack_T JOIN(astack, VALUE_NAME)
#define astack_T_is_empty JOIN(astack_T, is_empty)
#define astack_T_is_full JOIN(astack_T, is_full)
#define astack_T_count JOIN(astack_T, count)
#define astack_T_capacity JOIN(astack_T, capacity)

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE arr_p[];
} astack_T;

static inline bool JOIN(astack_T, init_with_capacity)(astack_T** astack_pp, size_t capacity) {
    assert(astack_pp != NULL);
    assert(capacity != 0);
    assert(capacity <= (SIZE_MAX - offsetof(astack_T, arr_p)) / sizeof(VALUE_TYPE));
    *astack_pp = (astack_T*)malloc(offsetof(astack_T, arr_p) + sizeof(VALUE_TYPE) * capacity);
    if ((*astack_pp) == NULL) {
        return false;
    }
    (*astack_pp)->count = 0;
    (*astack_pp)->capacity = capacity;
    return true;
}

static inline bool JOIN(astack_T, deinit)(astack_T** astack_pp) {
    assert(astack_pp != NULL);
    if (*astack_pp == NULL) {
        return false;
    }
    free(*astack_pp);
    *astack_pp = NULL;
    return true;
}

static inline size_t JOIN(astack_T, count)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count;
}

static inline size_t JOIN(astack_T, capacity)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->capacity;
}

static inline bool JOIN(astack_T, is_empty)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == 0;
}

static inline bool JOIN(astack_T, is_full)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == astack_p->capacity;
}

static inline VALUE_TYPE JOIN(astack_T, peek)(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(astack_T_is_empty(astack_p) == false);
    return astack_p->arr_p[astack_p->count - 1];
}

static inline void JOIN(astack_T, push)(astack_T* astack_p, const VALUE_TYPE value) {
    assert(astack_p != NULL);
    assert(astack_T_is_full(astack_p) == false);
    astack_p->arr_p[astack_p->count++] = value;
}

static inline void JOIN(astack_T, push_many)(astack_T* astack_p, const VALUE_TYPE* values, size_t num_of_values) {
    assert(astack_p != NULL);
    assert(num_of_values <= SIZE_MAX / sizeof(VALUE_TYPE));
    assert(astack_T_count(astack_p) + num_of_values <= astack_T_capacity(astack_p));
    memcpy(&astack_p->arr_p[astack_p->count], values, num_of_values * sizeof(VALUE_TYPE));
    astack_p->count += num_of_values;
}

static inline VALUE_TYPE JOIN(astack_T, pop)(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(astack_T_is_empty(astack_p) == false);
    return astack_p->arr_p[--astack_p->count];
}

#undef astack_T_is_empty
#undef astack_T_is_full
#undef astack_T_count
#undef astack_T_capacity
#undef astack_T

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_NAME
#undef VALUE_TYPE

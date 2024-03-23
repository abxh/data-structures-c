/*
    Including this header file generates structs and
    functions for a given stack type.

    The following macros gets defined once:
    - astack_for_each

    The following structs are generated:
    - astack_T

    The following functions are generated for a given value type t with name T:
    - astack_T_init
    - astack_T_deinit
    - astack_T_count
    - astack_T_is_empty
    - astack_T_is_full
    - astack_T_peek
    - astack_T_push
    - astack_T_pop
    - astack_T_resize

    A distinction between the type and name is made as the name
    cannot include spaces.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h> // assert
#include <stdlib.h> // size_t, NULL, malloc, calloc, free, reallocarray

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

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE* arr_p;
} astack_T;

static inline bool JOIN(astack_T, init)(astack_T** astack_pp, size_t capacity) {
    assert(astack_pp != NULL);
    assert(capacity != 0);
    *astack_pp = (astack_T*)malloc(sizeof(astack_T));
    if ((*astack_pp) == NULL) {
        return false;
    }
    (*astack_pp)->arr_p = (VALUE_TYPE*)calloc(capacity, sizeof(VALUE_TYPE));
    if ((*astack_pp)->arr_p == NULL) {
        free(*astack_pp);
        *astack_pp = NULL;
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
    free((*astack_pp)->arr_p);
    free(*astack_pp);
    *astack_pp = NULL;
    return true;
}

static inline bool JOIN(astack_T, count)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count;
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
    assert(JOIN(astack_T, is_empty)(astack_p) == false);
    return astack_p->arr_p[astack_p->count - 1];
}

static inline void JOIN(astack_T, push)(astack_T* astack_p, const VALUE_TYPE value) {
    assert(astack_p != NULL);
    assert(JOIN(astack_T, is_full)(astack_p) == false);
    astack_p->arr_p[astack_p->count++] = value;
}

static inline VALUE_TYPE JOIN(astack_T, pop)(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(JOIN(astack_T, is_empty)(astack_p) == false);
    return astack_p->arr_p[--astack_p->count];
}

static inline bool JOIN(astack_T, resize)(astack_T* astack_p, size_t new_capacity) {
    assert(astack_p != NULL);
    assert(new_capacity != 0);
    VALUE_TYPE* new_arr_p = (VALUE_TYPE*)reallocarray(astack_p->arr_p, new_capacity, sizeof(VALUE_TYPE));
    if (new_arr_p == NULL) {
        return false;
    }
    astack_p->arr_p = new_arr_p;
    astack_p->capacity = new_capacity;
    return true;
}

#undef astack_T

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_NAME
#undef VALUE_TYPE

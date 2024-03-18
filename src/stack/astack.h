/*
    fixed-size array-based stack

    The following macros are defined:
    - astack_for_each

    The following structs are defined:
    - astack_T

    The following functions are generated for a given value type t with label T:
    - astack_T_init
    - astack_T_deinit
    - astack_T_count
    - astack_T_is_empty
    - astack_T_is_full
    - astack_T_peek
    - astack_T_push
    - astack_T_pop

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h> // assert
#include <stdlib.h> // size_t, NULL, malloc, calloc, free

#define astack_for_each(astack_p, value, index) \
    for ((index) = 0; ((index) < (astack_p)->count && ((value) = (astack_p)->arr_p[(index)], true)); (index)++)

#endif

#ifndef VALUE_LABEL
#error "Must declare VALUE_LABEL. Defaulting to 'int'."
#define VALUE_LABEL int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define astack_T JOIN(astack, VALUE_LABEL)
#define astack_T_init JOIN(astack_T, init)
#define astack_T_deinit JOIN(astack_T, deinit)
#define astack_T_count JOIN(astack_T, count)
#define astack_T_is_empty JOIN(astack_T, is_empty)
#define astack_T_is_full JOIN(astack_T, is_full)
#define astack_T_peek JOIN(astack_T, peek)
#define astack_T_push JOIN(astack_T, push)
#define astack_T_pop JOIN(astack_T, pop)

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE* arr_p;
} astack_T;

static inline bool astack_T_init(astack_T** astack_pp, size_t capacity) {
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

static inline bool astack_T_count(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count;
}

static inline bool astack_T_is_empty(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == 0;
}

static inline bool astack_T_is_full(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == astack_p->capacity;
}

static inline const VALUE_TYPE astack_T_peek(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(astack_T_is_empty(astack_p) == false);
    return astack_p->arr_p[astack_p->count - 1];
}

static inline void astack_T_push(astack_T* astack_p, const VALUE_TYPE value) {
    assert(astack_p != NULL);
    assert(astack_T_is_full(astack_p) == false);
    astack_p->arr_p[astack_p->count++] = value;
}

static inline VALUE_TYPE astack_T_pop(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(astack_T_is_empty(astack_p) == false);
    return ((VALUE_TYPE*)astack_p->arr_p)[--astack_p->count];
}

static inline bool astack_T_deinit(astack_T** astack_pp) {
    if (*astack_pp == NULL) {
        return false;
    }
    free((*astack_pp)->arr_p);
    free(*astack_pp);
    *astack_pp = NULL;
    return true;
}

#undef astack_T
#undef astack_T_init
#undef astack_T_deinit
#undef astack_T_count
#undef astack_T_is_empty
#undef astack_T_peek
#undef astack_T_push
#undef astack_T_pop

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_LABEL
#undef VALUE_TYPE

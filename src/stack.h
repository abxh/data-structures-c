#ifndef __STACK_H__
#define __STACK_H__

#include "macros.h" // JOIN
#include <assert.h> // assert
#include <stdlib.h> // size_t

#define stack_foreach(stack_p, value, index)                                        \
    for ((assert(sizeof(*(stack_p)->arr_p) == sizeof(typeof(value))), (index) = 0); \
         ((index) < (stack_p)->used && ((value) = (stack_p)->arr_p[(index)], true)); (index)++)

#endif

#ifndef VALUE_NAME
#error "Must declare stack VALUE_NAME. Defaulting to 'default'."
#define VALUE_NAME default
#endif

#ifndef VALUE_TYPE
#error "Must declare stack VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef PREFIX
#define PREFIX stack
#endif

#define STACK_STRUCT JOIN(PREFIX, JOIN(VALUE_NAME, t))
#define STACK_METHOD(name) JOIN(PREFIX, JOIN(VALUE_NAME, name))

typedef struct {
    size_t used;
    size_t capacity;
    VALUE_TYPE* arr_p;
} STACK_STRUCT;

static inline bool STACK_METHOD(init)(STACK_STRUCT** stack_pp, size_t capacity) {
    assert(capacity != 0);
    *stack_pp = (STACK_STRUCT*)malloc(sizeof(STACK_STRUCT));
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->arr_p = (VALUE_TYPE*)calloc(capacity, sizeof(VALUE_TYPE));
    if ((*stack_pp)->arr_p == NULL) {
        free(*stack_pp);
        *stack_pp = NULL;
        return false;
    }
    (*stack_pp)->used = 0;
    (*stack_pp)->capacity = capacity;
    return true;
}

static inline bool STACK_METHOD(isempty)(const STACK_STRUCT* stack_p) {
    return stack_p->used == 0;
}

static inline bool STACK_METHOD(isfull)(const STACK_STRUCT* stack_p) {
    return stack_p->used == stack_p->capacity;
}

static inline VALUE_TYPE STACK_METHOD(peek)(STACK_STRUCT* stack_p) {
    assert(stack_p->used > 0);
    return ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used - 1];
}

static inline void STACK_METHOD(push)(STACK_STRUCT* stack_p, VALUE_TYPE value) {
    assert(stack_p->used < stack_p->capacity);
    ((VALUE_TYPE*)stack_p->arr_p)[stack_p->used++] = value;
}

static inline VALUE_TYPE STACK_METHOD(pop)(STACK_STRUCT* stack_p) {
    assert(stack_p->used > 0);
    return ((VALUE_TYPE*)stack_p->arr_p)[--stack_p->used];
}

static inline bool STACK_METHOD(resize)(STACK_STRUCT* stack_p, size_t new_capacity) {
    assert(new_capacity != 0);
    VALUE_TYPE* new_arr_p = (VALUE_TYPE*)reallocarray(stack_p->arr_p, new_capacity, sizeof(VALUE_TYPE));
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

static inline bool STACK_METHOD(deinit)(STACK_STRUCT** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
    return true;
}

#undef SUFFIX
#undef STACK_METHOD
#undef STACK_STRUCT
#undef VALUE_NAME
#undef VALUE_TYPE

/*
    This is a stack implementation based on a fixed size array.

    Including this header file generates a struct and functions for a given stack type.

    The following macros gets defined once:
    - T_stack_for_each

    The following structs are generated for a given value type T:
    - T_stack_type

    The following functions are generated for a given value type T:
    - T_stack_create
    - T_stack_destroy
    - T_stack_clone
    - T_stack_init

    - T_stack_get_count
    - T_stack_get_capacity
    - T_stack_is_empty
    - T_stack_is_full

    - T_stack_peek
    - T_stack_push
    - T_stack_pop

    Define PREFIX to use an other prefix than "T_stack". The macro(s) cannot
    however be redefined to have other prefixes.

    Note that (if PREFIX is not defined), then the given types cannot include
    spaces because C functions and variables cannot either. Use a typedef to work around
    the type name as needed (if you don't define PREFIX).
*/

#ifndef __T_STACK__H
#define __T_STACK__H

#include <assert.h>   // assert
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool, true, false
#include <stddef.h>   // offsetof
#include <stdint.h>   // SIZE_MAX
#include <stdlib.h>   // size_t, NULL
#include <string.h>   // memcpy

#include "allocators/allocator_function_types.h" // allocate_f, deallocate_f
#include "allocators/std_allocator.h"            // std_allocate, std_deallocate

#define T_stack_for_each(stack_p, index_plus_one, value)                                                                      \
    for ((index_plus_one) = (stack_p)->count; ((index_plus_one) > 0 && ((value) = (stack_p)->arr[(index_plus_one)-1], true)); \
         (index_plus_one)--)

#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#ifndef PREFIX
#define T_stack JOIN(VALUE_TYPE, stack)
#else
#define T_stack PREFIX
#endif

#define T_stack_type JOIN(T_stack, type)

typedef struct {
    size_t count;
    size_t capacity;

    void* allocator_struct_p;
    allocate_f allocate_f_p;
    deallocate_f deallocate_f_p;

    VALUE_TYPE arr[];
} T_stack_type;

static inline bool JOIN(T_stack, init)(T_stack_type** stack_pp, size_t capacity, void* allocator_struct_p, allocate_f allocate_f_p,
                                       deallocate_f deallocate_f_p) {
    assert(stack_pp != NULL);

    *stack_pp = NULL;
    if (capacity == 0 || capacity > (SIZE_MAX - offsetof(T_stack_type, arr)) / sizeof(VALUE_TYPE)) {
        return false;
    }
    *stack_pp = (T_stack_type*)allocate_f_p(allocator_struct_p, alignof(T_stack_type),
                                            offsetof(T_stack_type, arr) + sizeof(VALUE_TYPE) * capacity);
    if (*stack_pp == NULL) {
        return false;
    }
    (*stack_pp)->count = 0;
    (*stack_pp)->capacity = capacity;

    (*stack_pp)->allocator_struct_p = allocator_struct_p;
    (*stack_pp)->allocate_f_p = allocate_f_p;
    (*stack_pp)->deallocate_f_p = deallocate_f_p;

    return true;
}

static inline T_stack_type* JOIN(T_stack, create)(size_t capacity) {
    T_stack_type* stack_p = NULL;
    JOIN(T_stack, init)(&stack_p, capacity, NULL, std_allocate, std_deallocate);
    return stack_p;
}

static inline void JOIN(T_stack, destroy)(T_stack_type* stack_p) {
    assert(stack_p != NULL);

    void* allocator_struct_p = stack_p->allocator_struct_p;
    deallocate_f deallocate_f_p = stack_p->deallocate_f_p;

    deallocate_f_p(allocator_struct_p, stack_p);
}

static inline T_stack_type* JOIN(T_stack, clone)(const T_stack_type* stack_src_p) {
    assert(stack_src_p != NULL);

    T_stack_type* stack_dest_p = NULL;
    if (!JOIN(T_stack, init)(&stack_dest_p, stack_src_p->capacity, stack_src_p->allocator_struct_p, stack_src_p->allocate_f_p,
                             stack_src_p->deallocate_f_p)) {
        return NULL;
    }
    memcpy(stack_dest_p->arr, stack_src_p->arr, sizeof(VALUE_TYPE) * stack_src_p->capacity);
    stack_dest_p->count = stack_src_p->count;

    return stack_dest_p;
}

static inline size_t JOIN(T_stack, get_count)(const T_stack_type* stack_p) {
    assert(stack_p != NULL);

    return stack_p->count;
}

static inline size_t JOIN(T_stack, get_capacity)(const T_stack_type* stack_p) {
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

static inline VALUE_TYPE JOIN(T_stack, peek)(T_stack_type* stack_p) {
    assert(stack_p != NULL);
#define T_stack_is_empty JOIN(T_stack, is_empty)
    assert(T_stack_is_empty(stack_p) == false);
#undef T_stack_is_empty

    return stack_p->arr[stack_p->count - 1];
}

static inline void JOIN(T_stack, push)(T_stack_type* stack_p, const VALUE_TYPE value) {
    assert(stack_p != NULL);
#define T_stack_is_full JOIN(T_stack, is_full)
    assert(T_stack_is_full(stack_p) == false);
#undef T_stack_is_full

    stack_p->arr[stack_p->count++] = value;
}

static inline VALUE_TYPE JOIN(T_stack, pop)(T_stack_type* T_stack_p) {
    assert(T_stack_p != NULL);
#define T_stack_is_empty JOIN(T_stack, is_empty)
    assert(T_stack_is_empty(T_stack_p) == false);
#undef T_stack_is_empty

    return T_stack_p->arr[--T_stack_p->count];
}

#undef T_stack
#undef T_stack_type

#undef PREFIX
#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN

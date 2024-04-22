/*
    Stack data structure based on a dynamic array.

    Including this header file generates code. The macro T (standing for value type)
    must be defined. Symbols will be prefixed with T, unless PREFIX is defined as
    well in which case symbols will be prefixed with PREFIX.

    T_stack_for_each identifier can be used to iterate over the stack. All other indentifiers
    are removed once the code is generated.

    See examples/stack.c to see how this header file is used in practice.
*/

#ifndef __STACK__H
#define __STACK__H

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdint.h>  // SIZE_MAX
#include <stdlib.h>  // size_t, NULL, malloc, calloc, reallocarray, free
#include <string.h>  // memcpy

#endif

/* ----------------------------- DEFINES PROLOUGE BEGIN ----------------------------- */
// {{{
#define CONCAT_RAW(a, b) a##b
#define CONCAT(a, b) CONCAT_RAW(a, b)

#ifndef T
#error "Must define value type T. defaulting to int."
#define T int
#endif

#ifndef PREFIX
#define T_stack CONCAT(T, _stack)
#else
#define T_stack PREFIX
#endif

#define T_stack_type CONCAT(T_stack, _type)
#define T_stack_create CONCAT(T_stack, _create)
#define T_stack_create_with_initial_capacity CONCAT(T_stack, _create_with_initial_capacity)
#define T_stack_destroy CONCAT(T_stack, _destroy)
#define T_stack_clone CONCAT(T_stack, _clone)
#define T_stack_is_empty CONCAT(T_stack, _is_empty)
#define T_stack_get_top CONCAT(T_stack, _get_top)
#define T_stack_get_count CONCAT(T_stack, _get_count)
#define T_stack_get_capacity CONCAT(T_stack, _get_capacity)
#define T_stack_peek CONCAT(T_stack, _peek)
#define T_stack_push CONCAT(T_stack, _push)
#define T_stack_pop CONCAT(T_stack, _pop)
// }}}

/* in:
 * - T_stack ptr
 * out:
 * - size_t index_plus_one
 * - T value
 */
#define T_stack_for_each(in_T_stack_ptr, out_size_t_count, out_T_value) \
    for ((out_size_t_count) = (in_T_stack_ptr)->count;                  \
         ((out_size_t_count) > 0 && ((out_T_value) = (in_T_stack_ptr)->arr_ptr[(out_size_t_count)-1], true)); (out_size_t_count)--)
/* ------------------------------ DEFINES PROLOUGE END ------------------------------ */

/* managed stack struct. */
typedef struct {
    size_t count;
    size_t capacity;
    T* arr_ptr;
} T_stack_type;

/* create a new dynamic stack with an initial capacity and return it's pointer,
 * returns NULL if memory could not be allocated. */
static inline T_stack_type* T_stack_create_with_initial_capacity(size_t initial_capacity) {
    assert(0 != initial_capacity);

    T_stack_type* stack_ptr = (T_stack_type*)malloc(sizeof(T_stack_type));
    if (!stack_ptr) {
        return NULL;
    }
    stack_ptr->arr_ptr = (T*)calloc(initial_capacity, sizeof(T));
    if (!stack_ptr->arr_ptr) {
        free(stack_ptr);
        return NULL;
    }
    stack_ptr->count = 0;
    stack_ptr->capacity = initial_capacity;

    return stack_ptr;
}

/* create a new dynamic stack with a default capacity and return it's pointer.
   returns NULL if memory could not be allocated. */
static inline T_stack_type* T_stack_create() {
    return T_stack_create_with_initial_capacity(1024);
}

/* destroy an existing stack. may not be called twice. */
static inline void T_stack_destroy(T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);

    free(stack_ptr->arr_ptr);
    free(stack_ptr);
}

/* clone an existing stack. NULL is returned if memory could not be allocated. */
static inline T_stack_type* T_stack_clone(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);

    T_stack_type* other_stack_ptr = T_stack_create_with_initial_capacity(stack_ptr->capacity);
    if (!other_stack_ptr) {
        return NULL;
    }
    memcpy(other_stack_ptr->arr_ptr, stack_ptr->arr_ptr, sizeof(T) * stack_ptr->count);
    other_stack_ptr->count = other_stack_ptr->count;
    other_stack_ptr->capacity = other_stack_ptr->capacity;

    return other_stack_ptr;
}

/* check if stack is empty. */
static inline T T_stack_is_empty(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);

    return 0 == stack_ptr->count;
}

/* get the top of the stack. stack should not be empty. */
static inline T T_stack_get_top(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);
    assert(!T_stack_is_empty(stack_ptr));

    return stack_ptr->arr_ptr[stack_ptr->count - 1];
}

/* get the number of elements in the stack.*/
static inline size_t T_stack_get_count(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->count;
}

/* get the current capacity the stack. */
static inline size_t T_stack_get_capacity(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->capacity;
}

/* peek at the top of the stack. stack should not be empty. */
static inline T T_stack_peek(const T_stack_type* stack_ptr) {
    assert(NULL != stack_ptr);
    assert(!T_stack_is_empty(stack_ptr));

    return stack_ptr->arr_ptr[stack_ptr->count - 1];
}

/* push a value onto the stack. returns false if the stack cannot be resized
   to allocate memory for the given value. */
static inline bool T_stack_push(T_stack_type* stack_ptr, const T value) {
    assert(NULL != stack_ptr);

    if (stack_ptr->count == stack_ptr->capacity) {
        if (stack_ptr->capacity > SIZE_MAX >> 1) {
            return false;
        }
        void* new_arr_ptr = reallocarray(stack_ptr->arr_ptr, stack_ptr->capacity << 1, sizeof(T));
        if (new_arr_ptr == NULL) {
            return false;
        }
        stack_ptr->arr_ptr = (T*)new_arr_ptr;
        stack_ptr->capacity <<= 1;
    }
    stack_ptr->arr_ptr[stack_ptr->count++] = value;

    return true;
}

/* pop a value from the top of the stack. stack should not be empty. */
static inline T T_stack_pop(T_stack_type* stack_ptr) {
    assert(!T_stack_is_empty(stack_ptr));

    return stack_ptr->arr_ptr[--stack_ptr->count];
}

/* ------------------------------ DEFINES EPILOUGE BEGIN ------------------------------ */
/// {{{
#undef T
#undef T_stack
#undef T_stack_type
#undef T_stack_create
#undef T_stack_create_with_initial_capacity
#undef T_stack_destroy
#undef T_stack_clone
#undef T_stack_is_empty
#undef T_stack_get_top
#undef T_stack_get_count
#undef T_stack_get_capacity
#undef T_stack_peek
#undef T_stack_push
#undef T_stack_pop

#undef CONCAT_RAW
#undef CONCAT
// }}}
/* ------------------------------- DEFINES EPILOUGE END ------------------------------- */

// vim:ft=c:fdm=marker

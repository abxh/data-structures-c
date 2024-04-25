/**
 * @file stack.h
 * @brief Stack data structure based on a dynamic array.
 */

/**
 * @example stack/stack.c
 * Examples of how `stack.h` header file is used in practice.
 */

#include <assert.h>   // assert
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool
#include <stdint.h>   // SIZE_MAX
#include <stdlib.h>   // size_t, NULL
#include <string.h>   // memcpy

#include "allocator_ops.h" // allocate_ops_type
#include "join.h"          // JOIN
#include "std_allocator.h" // std_allocator_ops

#ifndef __stack_for_each
#define __stack_for_each(stack_ptr, count, value) \
    for ((count) = (stack_ptr)->count; (count) > 0 && ((value) = (stack_ptr)->values[(count)-1], true); (count)--)
#endif

/**
 * @def stack_for_each
 * @brief Iterate over the values in the stack from the top value.
 * @param[in] stack_ptr Stack pointer.
 * @param[out] count Number of elements iterated as `size_t`.
 * @param[out] value Value at `count - 1` as `size_t`.
 */
#ifndef stack_for_each
#define stack_for_each(stack_ptr, count, value) __stack_for_each(stack_ptr, count, value)
#endif

/**
 * @def VALUE_TYPE
 * @brief The value type.
 * @attention This must be manually defined before including this header file.
 *
 * Is defaulted to int, so the editor's LSP works better.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#error "must define VALUE_TYPE. defaulting to int."
#endif

/**
 * @def PREFIX
 * @brief The stack prefix. Set to VALUE\_TYPE\#\#\_\#\#stack, unless specified otherwise.
 */
#ifdef PREFIX
#define __STACK_PREFIX PREFIX
#elif !defined(__STACK_PREFIX)
#define __STACK_PREFIX JOIN(VALUE_TYPE, stack)
#endif

#ifndef PREFIX
#define PREFIX VALUE_TYPE##_##stack
#undef PREFIX // above is for doxygen documentation.
#endif

/* The stack type name. Set to PREFIX##_##type. */
#ifndef STACK_TYPE
#define STACK_TYPE JOIN(__STACK_PREFIX, type)
#endif

/**
 * @brief Generated stack struct type for a value type managed by functions.
 */
typedef struct {
    size_t count;       ///< number of values in the stack.
    size_t capacity;    ///< total number of values the stack can store.
    VALUE_TYPE* values; ///< array containing the values in the stack.

    void* allocator_context_ptr;      ///< internal data in allocator.
    allocator_ops_type allocator_ops; ///< allocator operations.
} STACK_TYPE;

/**
 * @brief Create a stack with specified initial capacity and custom allocator.
 *
 * @param initial_capacity The initial capacity of the stack.
 * @param allocator_context_ptr Pointer to allocator context.
 * @param allocator_ops Struct pointing to allocator operations.
 * @return The stack pointer.
 * @retval NULL
 * - If initial_capacity is 0.
 * - If `initial_capacity * sizeof(VALUE_TYPE)` cannot be expressed with size_t.
 * - If no memory space is available.
 */
static inline STACK_TYPE* JOIN(__STACK_PREFIX, create_with_specified)(size_t initial_capacity, void* allocator_context_ptr,
                                                                      allocator_ops_type allocator_ops) {
    if (initial_capacity == 0 || initial_capacity > SIZE_MAX / sizeof(VALUE_TYPE)) {
        return NULL;
    }
    STACK_TYPE* stack_ptr = (STACK_TYPE*)allocator_ops.allocate_f_p(allocator_context_ptr, alignof(STACK_TYPE), sizeof(STACK_TYPE));
    if (!stack_ptr) {
        return NULL;
    }
    stack_ptr->values =
        (VALUE_TYPE*)allocator_ops.allocate_f_p(allocator_context_ptr, alignof(VALUE_TYPE), sizeof(VALUE_TYPE) * initial_capacity);
    if (!stack_ptr->values) {
        free(stack_ptr);
        return NULL;
    }
    stack_ptr->count = 0;
    stack_ptr->capacity = initial_capacity;
    stack_ptr->allocator_context_ptr = allocator_context_ptr;
    stack_ptr->allocator_ops = allocator_ops;

    return stack_ptr;
}

/**
 * @brief Create a stack with a default capacity and standard allocator (aka `malloc`).
 *
 * @return The stack pointer.
 * @retval NULL If no memory space is available.
 */
static inline STACK_TYPE* JOIN(__STACK_PREFIX, create)(void) {
    return JOIN(__STACK_PREFIX, create_with_specified)(512, NULL, std_allocator_ops);
}

/**
 * @brief Destroy a stack and free the used memory.
 *
 * @note May not be called twice.
 */
static inline void JOIN(__STACK_PREFIX, destroy)(STACK_TYPE* stack_ptr) {
    if (!stack_ptr) {
        return;
    }
    void* allocator_context_ptr_copy = stack_ptr->allocator_context_ptr;
    allocator_ops_type allocator_ops_copy = stack_ptr->allocator_ops;

    allocator_ops_copy.deallocate_f_p(allocator_context_ptr_copy, stack_ptr->values);
    allocator_ops_copy.deallocate_f_p(allocator_context_ptr_copy, stack_ptr);
}

/**
 * @brief Clone an existing stack.
 *
 * @param stack\_ptr The pointer of the stack to clone.
 * @return A pointer to the clone of the original stack.
 * @retval NULL
 * - If no memory space is available.
 * - If stack_ptr is NULL.
 */
static inline STACK_TYPE* JOIN(__STACK_PREFIX, clone)(const STACK_TYPE* stack_ptr) {
    if (!stack_ptr) {
        return NULL;
    }
    STACK_TYPE* other_stack_ptr = JOIN(__STACK_PREFIX, create_with_specified)(stack_ptr->capacity, NULL, std_allocator_ops);
    if (!other_stack_ptr) {
        return NULL;
    }
    memcpy(other_stack_ptr->values, stack_ptr->values, sizeof(VALUE_TYPE) * stack_ptr->count);
    other_stack_ptr->count = other_stack_ptr->count;
    other_stack_ptr->capacity = other_stack_ptr->capacity;

    return other_stack_ptr;
}

/**
 * @brief Get value at index.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @param index Index at which the value lies. Assumed to be less than stack count.
 * @return The value as VALUE_TYPE.
 */
static inline VALUE_TYPE JOIN(__STACK_PREFIX, at)(const STACK_TYPE* stack_ptr, size_t index) {
    assert(NULL != stack_ptr);
    assert(index < stack_ptr->count);

    return stack_ptr->values[index];
}

/**
 * @brief Get the value from the top of the stack.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return The value as VALUE_TYPE.
 */
static inline VALUE_TYPE JOIN(__STACK_PREFIX, top)(const STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->values[stack_ptr->count - 1];
}

/**
 * @brief Get the number of elements in the stack.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return The number of elements as size_t.
 */
static inline size_t JOIN(__STACK_PREFIX, count)(const STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->count;
}

/**
 * @brief Get the stack's capacity, which is the number of values memory is
 * preallocated for in the stack currently.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return The current capacity as size_t.
 */
static inline size_t JOIN(__STACK_PREFIX, capacity)(const STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->capacity;
}

/**
 * @brief Check if stack is empty.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return A boolean indicating whether the stack is empty.
 */
static inline bool JOIN(__STACK_PREFIX, is_empty)(const STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return 0 == stack_ptr->count;
}

/**
 * @brief Peek at the stack and get it's top value.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return The value as VALUE_TYPE.
 */
static inline VALUE_TYPE JOIN(__STACK_PREFIX, peek)(const STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->values[stack_ptr->count - 1];
}

/**
 * @brief Pop a value from the stack and return the value.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @return The value as VALUE_TYPE.
 */
static inline VALUE_TYPE JOIN(__STACK_PREFIX, pop)(STACK_TYPE* stack_ptr) {
    assert(NULL != stack_ptr);

    return stack_ptr->values[--stack_ptr->count];
}

/**
 * @brief Push a value from the stack and return the value.
 *
 * @param stack\_ptr The stack pointer. Assumed to not be NULL.
 * @param value The value to work with.
 * @return A boolean indicating whether the value was stored, depending on whether
 *         the stack could be resized when full.
 * @retval false
 * - If `current capacaity * sizeof(VALUE_TYPE) * 2` cannot be expressed with size\_t.
 * - If no memory space is available.
 */
static inline bool JOIN(__STACK_PREFIX, push)(STACK_TYPE* stack_ptr, VALUE_TYPE value) {
    assert(NULL != stack_ptr);

    if (stack_ptr->count == stack_ptr->capacity) {
        if (stack_ptr->capacity > SIZE_MAX / sizeof(VALUE_TYPE) / 2) {
            return false;
        }
        void* temp_ptr = stack_ptr->allocator_ops.reallocate_f_p(stack_ptr->allocator_context_ptr, stack_ptr->values,
                                                                 alignof(VALUE_TYPE), sizeof(VALUE_TYPE) * stack_ptr->capacity,
                                                                 2 * sizeof(VALUE_TYPE) * stack_ptr->capacity);
        if (temp_ptr == NULL) {
            return false;
        }
        stack_ptr->values = (VALUE_TYPE*)temp_ptr;
        stack_ptr->capacity *= 2;
    }
    stack_ptr->values[stack_ptr->count++] = value;

    return true;
}

#undef __STACK_PREFIX
#undef PREFIX
#undef VALUE_TYPE
#undef STACK_TYPE

// vim: ft=c
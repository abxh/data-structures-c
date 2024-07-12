/**
 * @file fstack.h
 * @brief Fixed-size array-based stack data structure.
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `VALUE_TYPE`
 */

/**
 * @example fstack/fstack.c
 * Examples of how `fstack.h` header file is used in practice.
 */

// macro definitions: {{{

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def NAME
 * @brief Prefix to stack type and operations.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef NAME
#define NAME fstack
#error "Must define NAME."
#else
#define FSTACK_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Stack value type.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#error "Must define VALUE_TYPE."
#endif

#ifndef FSTACK_H
/// @cond DO_NOT_DOCUMENT
#define FSTACK_H
/// @endcond

/**
 * @def FSTACK_FOREACH(stack_ptr, value)
 * @brief Iterate over the values in the stack from the top to bottom.
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * temporary variables visible in scope:
 * @li _index
 *
 * @param[in] stack_ptr Stack pointer.
 * @param[out] value Current value. Should be `VALUE_TYPE`.
 */
#define FSTACK_FOREACH(stack_ptr, value)                    \
    for (size_t _index = (stack_ptr)->count;                \
                                                            \
         _index > 0 &&                                      \
                                                            \
         ((value) = (stack_ptr)->values[_index - 1], true); \
                                                            \
         _index--)

/**
 * @def FSTACK_FOREACH_REVERSE(stack_ptr, value)
 * @brief Iterate over the values in the stack from the bottom to top.
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * temporary variables visible in scope:
 * @li _index
 *
 * @param[in] stack_ptr Stack pointer.
 * @param[out] value Current value. Should be `VALUE_TYPE`.
 */
#define FSTACK_FOREACH_REVERSE(stack_ptr, value)        \
    for (size_t _index = 0;                             \
                                                        \
         _index < (stack_ptr)->count &&                 \
                                                        \
         ((value) = (stack_ptr)->values[_index], true); \
                                                        \
         _index++)

#endif // FSTACK_H

/// @cond DO_NOT_DOCUMENT
#define FSTACK_TYPE JOIN(FSTACK_NAME, type)
#define FSTACK_IS_EMPTY JOIN(FSTACK_NAME, is_empty)
#define FSTACK_IS_FULL JOIN(FSTACK_NAME, is_full)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated stack struct type for a given `VALUE_TYPE`.
 */
typedef struct {
    size_t count;        ///< number of values in the stack.
    size_t capacity;     ///< maximum number of values allocated for in the stack.
    VALUE_TYPE values[]; ///< pointer to stack values buffer.
} FSTACK_TYPE;

// }}}

// function definitions: {{{

/**
 * @brief Create an stack with a given capacity with malloc().
 *
 * @param[in] capacity Maximum number of elements expected to be stored in the stack.
 * @return A pointer to the stack.
 * @retval `NULL`
 *   @li If capacity is 0 or the stack size cannot be represented by `size_t`.
 *   @li If malloc fails.
 */
static inline FSTACK_TYPE* JOIN(FSTACK_NAME, create)(const size_t capacity) {
    if (capacity == 0 || capacity > (SIZE_MAX - offsetof(FSTACK_TYPE, values)) / sizeof(VALUE_TYPE)) {
        return NULL;
    }
    FSTACK_TYPE* stack_ptr = malloc(offsetof(FSTACK_TYPE, values) + capacity * sizeof(VALUE_TYPE));

    if (!stack_ptr) {
        return NULL;
    }
    memset(stack_ptr, 0, offsetof(FSTACK_TYPE, values) + capacity * sizeof(VALUE_TYPE));

    stack_ptr->count = 0;
    stack_ptr->capacity = capacity;

    return stack_ptr;
}

/**
 * @brief Destroy an stack and free the underlying memory with free().
 *
 * @param[in] stack_ptr The stack pointer.
 * @warning May not be called twice in a row on the same object.
 */
static inline void JOIN(FSTACK_NAME, destroy)(FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);

    free(stack_ptr);
}

/**
 * @brief Return whether the stack is empty.
 *
 * Assumes the stack pointer is not `NULL`.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return whether the stack is empty.
 */
static inline bool JOIN(FSTACK_NAME, is_empty)(const FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);

    return stack_ptr->count == 0;
}

/**
 * @brief Return whether the stack is full.
 *
 * Assumes the stack pointer is not `NULL`.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return whether the stack is full.
 */
static inline bool JOIN(FSTACK_NAME, is_full)(const FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);

    return stack_ptr->count == stack_ptr->capacity;
}

/**
 * @brief Get the value at index.
 *
 * index starts from the top as `0` and is counted upward to `count - 1` as bottom.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li `index` is strictly less than stack count.
 *
 * @param[in] stack_ptr The stack pointer.
 * @param[in] index The index to retrieve to value from.
 * @return The value at `index`.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, at)(const FSTACK_TYPE* stack_ptr, const size_t index) {
    assert(stack_ptr != NULL);
    assert(index < stack_ptr->count);

    return stack_ptr->values[stack_ptr->count - 1 - index];
}

/**
 * @brief Get the value from the top of the stack.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li The stack is not empty.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return The top value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, get_top)(const FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);
    assert(FSTACK_IS_EMPTY(stack_ptr) == false);

    return stack_ptr->values[stack_ptr->count - 1];
}

/**
 * @brief Get the value from the bottom of the stack.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li The stack is not empty.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return The bottom value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, get_bottom)(const FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);
    assert(FSTACK_IS_EMPTY(stack_ptr) == false);

    return stack_ptr->values[0];
}

/**
 * @brief Peek the stack and get it's next to-be-popped value.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li The stack is not empty.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return The next to-be-popped value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, peek)(const FSTACK_TYPE* stack_ptr) {
    return JOIN(FSTACK_NAME, get_top)(stack_ptr);
}

/**
 * @brief Push a value onto the stack.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li The stack is not full.
 *
 * @param[in] stack_ptr The stack pointer.
 * @param[in] value The value.
 */
static inline void JOIN(FSTACK_NAME, push)(FSTACK_TYPE* stack_ptr, const VALUE_TYPE value) {
    assert(stack_ptr != NULL);
    assert(FSTACK_IS_FULL(stack_ptr) == false);

    stack_ptr->values[stack_ptr->count++] = value;
}

/**
 * @brief Pop a value away from the stack.
 *
 * Assumes:
 * @li `stack_ptr` is not `NULL`.
 * @li The stack is not empty.
 *
 * @param[in] stack_ptr The stack pointer.
 * @return The top value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, pop)(FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);
    assert(FSTACK_IS_EMPTY(stack_ptr) == false);

    return stack_ptr->values[--stack_ptr->count];
}

/**
 * @brief Clear the elements in the stack.
 *
 * Assumes `stack_ptr` is not `NULL`.
 *
 * @param[in] stack_ptr The stack pointer.
 */
static inline void JOIN(FSTACK_NAME, clear)(FSTACK_TYPE* stack_ptr) {
    assert(stack_ptr != NULL);
    stack_ptr->count = 0;
}

/**
 * @brief Copy the values from a source stack to a destination stack.
 *
 * Assumes:
 * @li Source and destination stack pointers are not pointing to the same memory.
 * @li The stack pointers are not `NULL`.
 * @li The destination stack has a capacity that is greater than or equal to source stack count.
 * @li The destination stack is an empty stack.
 *
 * @param[out] dest_stack_ptr The destination stack.
 * @param[in] src_stack_ptr The source stack.
 */
static inline void JOIN(FSTACK_NAME, copy)(FSTACK_TYPE* restrict dest_stack_ptr, const FSTACK_TYPE* restrict src_stack_ptr) {
    assert(src_stack_ptr != NULL);
    assert(dest_stack_ptr != NULL);
    assert(src_stack_ptr->count <= dest_stack_ptr->capacity);
    assert(FSTACK_IS_EMPTY(dest_stack_ptr));

    for (size_t i = 0; i < src_stack_ptr->count; i++) {
        dest_stack_ptr->values[i] = src_stack_ptr->values[i];
    }
    dest_stack_ptr->count = src_stack_ptr->count;
}

// }}}

// macro undefs: {{{
#undef NAME
#undef VALUE_TYPE

#undef FSTACK_NAME
#undef FSTACK_TYPE
#undef FSTACK_IS_EMPTY
#undef FSTACK_ISFULL
// }}}

// vim: ft=c fdm=marker

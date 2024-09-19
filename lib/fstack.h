/*  fstack.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file fstack.h
 * @brief Fixed-size array-based stack
 *
 * The following macros must be defined:
 *      @li `NAME`
 *      @li `VALUE_TYPE`
 */

/**
 * @example examples/fstack/fstack.c
 * Examples of how `fstack.h` header file is used in practice.
 */

// macro definitions: {{{

#ifndef FSTACK_H
#define FSTACK_H

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @def fstack_for_each(self, index, value)
 * @brief Iterate over the values in the stack from the top to bottom.
 *
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * @param[in] self      Stack pointer.
 * @param[in] index     Temporary indexing variable. Should be able to contain
 *                      `uint32_t`.
 * @param[out] value    Current value. Should be `VALUE_TYPE`.
 */
#define fstack_for_each(self, index, value) \
    for ((index) = (self)->count; (index) > 0 && ((value) = (self)->values[(index) - 1], true); (index)--)

/**
 * @def fstack_for_each_reverse(self, index, value)
 * @brief Iterate over the values in the stack from the bottom to top.
 *
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * @param[in] self      Stack pointer.
 * @param[in] index     Temporary indexing variable. Should be able to contain
 *                      `uint32_t`.
 * @param[out] value    Current value. Should be `VALUE_TYPE`.
 */
#define fstack_for_each_reverse(self, index, value) \
    for ((index) = 0; (index) < (self)->count && ((value) = (self)->values[(index)], true); (index)++)

#endif // FSTACK_H

/**
 * @def NAME
 * @brief Prefix to stack type and operations. This must be manually defined
 *        before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define NAME fstack
#error "Must define NAME."
#else
#define FSTACK_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Stack value type. This must be manually defined before including this
 *        header file.
 *
 * Is undefined after header is included.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#error "Must define VALUE_TYPE."
#endif

/// @cond DO_NOT_DOCUMENT
#define FSTACK_TYPE        struct FSTACK_NAME
#define FSTACK_IS_EMPTY    JOIN(FSTACK_NAME, is_empty)
#define FSTACK_IS_FULL     JOIN(FSTACK_NAME, is_full)
#define FSTACK_INIT        JOIN(FSTACK_NAME, init)
#define FSTACK_CALC_SIZEOF JOIN(FSTACK_NAME, calc_sizeof)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated stack struct type for a given `VALUE_TYPE`.
 */
struct FSTACK_NAME {
    uint32_t count;      ///< number of values.
    uint32_t capacity;   ///< maximum number of values allocated for.
    VALUE_TYPE values[100]; ///< array of values.
};

// }}}

// function definitions: {{{

/**
 * @brief Calculate the size of the stack struct.
 *
 * @param[in,out]  capacity_ptr  Pointer to orignal capacity
 * @param[out]     size_ptr      Pointer to size (to be outputted)
 *
 * @retval false If capacity is 0 or the equivalent size is larger than
 *               UINT32_MAX.
 * @retval true  Otherwise.
 */
static inline bool JOIN(FSTACK_NAME, calc_sizeof)(uint32_t *capacity_ptr, uint32_t *size_ptr)
{
    assert(capacity_ptr);
    assert(size_ptr);

    if (*capacity_ptr == 0 || *capacity_ptr > (UINT32_MAX - offsetof(FSTACK_TYPE, values)) / sizeof(VALUE_TYPE)) {
        return false;
    }

    *size_ptr = (uint32_t)(offsetof(FSTACK_TYPE, values) + *capacity_ptr * sizeof(VALUE_TYPE));

    return true;
}

/**
 * @brief Initialize a stack struct, given a capacity.
 *
 * @param[in] self              Stack pointer
 * @param[in] capacity          Capacity
 */
static inline FSTACK_TYPE *JOIN(FSTACK_NAME, init)(FSTACK_TYPE *self, const uint32_t capacity)
{
    assert(self);

    self->count = 0;
    self->capacity = capacity;

    return self;
}

/**
 * @brief Create an stack struct with a given capacity with malloc().
 *
 * @param[in] capacity      Maximum number of elements.
 *
 * @return A pointer to the stack.
 * @retval NULL
 *   @li If capacity is 0 or the equivalent size cannot be stored in
 *       UINT32_MAX.
 *   @li If malloc fails.
 */
static inline FSTACK_TYPE *JOIN(FSTACK_NAME, create)(uint32_t capacity)
{
    uint32_t size = 0;
    if (!FSTACK_CALC_SIZEOF(&capacity, &size)) {
        return NULL;
    }
    FSTACK_TYPE *self = (FSTACK_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FSTACK_INIT(self, capacity);

    return self;
}

/**
 * @brief Destroy an stack struct and free the underlying memory with free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self      The stack pointer.
 */
static inline void JOIN(FSTACK_NAME, destroy)(FSTACK_TYPE *self)
{
    assert(self != NULL);

    free(self);
}

/**
 * @brief Return whether the stack is empty.
 *
 * @param[in] self      The stack pointer.
 *
 * @return Whether the stack is empty.
 */
static inline bool JOIN(FSTACK_NAME, is_empty)(const FSTACK_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

/**
 * @brief Return whether the stack is full.
 *
 * @param[in] self      The stack pointer.
 *
 * @return Whether the stack is full.
 */
static inline bool JOIN(FSTACK_NAME, is_full)(const FSTACK_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

/**
 * @brief Get the value at index.
 *
 * @note Index starts from the top as `0` and is counted upward to `count - 1`
 * as bottom.
 *
 * @param[in] self      The stack pointer.
 * @param[in] index     The index to retrieve to value from.
 *
 * @return The value at `index`.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, at)(const FSTACK_TYPE *self, const uint32_t index)
{
    assert(self != NULL);
    assert(index < self->count);

    return self->values[self->count - 1 - index];
}

/**
 * @brief Get the value from the top of a non-empty stack.
 *
 * @param[in] self      The stack pointer.
 *
 * @return The top value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, get_top)(const FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[self->count - 1];
}

/**
 * @brief Get the value from the bottom of a non-empty stack.
 *
 * @param[in] self      The stack pointer.
 *
 * @return The bottom value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, get_bottom)(const FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[0];
}

/**
 * @brief Peek a non-empty stack and get it's next to-be-popped value.
 *
 * @param[in] self      The stack pointer.
 *
 * @return The next to-be-popped value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, peek)(const FSTACK_TYPE *self)
{
    return JOIN(FSTACK_NAME, get_top)(self);
}

/**
 * @brief Push a value onto a non-full stack.
 *
 * @param[in] self      The stack pointer.
 * @param[in] value     The value.
 */
static inline void JOIN(FSTACK_NAME, push)(FSTACK_TYPE *self, const VALUE_TYPE value)
{
    assert(self != NULL);
    assert(FSTACK_IS_FULL(self) == false);

    self->values[self->count++] = value;
}

/**
 * @brief Pop a value away from a non-empty stack.
 *
 * @param[in] self      The stack pointer.
 *
 * @return The top value.
 */
static inline VALUE_TYPE JOIN(FSTACK_NAME, pop)(FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[--self->count];
}

/**
 * @brief Clear the elements in the stack.
 *
 * @param[in] self      The stack pointer.
 */
static inline void JOIN(FSTACK_NAME, clear)(FSTACK_TYPE *self)
{
    assert(self != NULL);
    self->count = 0;
}

/**
 * @brief Copy the values from a source stack to a destination stack.
 *
 * @param[in,out] dest_ptr      The destination stack.
 * @param[in] src_ptr           The source stack.
 */
static inline void JOIN(FSTACK_NAME, copy)(FSTACK_TYPE *restrict dest_ptr, const FSTACK_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->count <= dest_ptr->capacity);
    assert(FSTACK_IS_EMPTY(dest_ptr));

    for (uint32_t i = 0; i < src_ptr->count; i++) {
        dest_ptr->values[i] = src_ptr->values[i];
    }
    dest_ptr->count = src_ptr->count;
}

// }}}

// macro undefs: {{{
#undef NAME
#undef VALUE_TYPE

#undef FSTACK_NAME
#undef FSTACK_TYPE
#undef FSTACK_IS_EMPTY
#undef FSTACK_IS_FULL
#undef FSTACK_INIT
#undef FSTACK_CALC_SIZEOF
// }}}

// vim: ft=c fdm=marker

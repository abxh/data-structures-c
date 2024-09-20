/*  fpqueue.h
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
 * @file fpqueue.h
 * @brief Fixed-size priority queue based on binary (max-)heap
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `VALUE_TYPE`
 *
 * Source used:
 * @li CLRS
 */

/**
 * @example examples/fpqueue/fpqueue.c
 * Example of how `fpqueue.h` header file is used in practice.
 */

// macro definitions: {{{

#ifndef FPQUEUE_H
#define FPQUEUE_H

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @def fpqueue_left_child(index)
 * @brief Given an element index, get the index of the left child.
 *
 * @param[in] index The element index.
 */
#define fpqueue_left_child(index) (2 * (index) + 1)

/**
 * @def fpqueue_right_child(index)
 * @brief Given an element index, get the index of the right child.
 *
 * @param[in] index The element index.
 */
#define fpqueue_right_child(index) (2 * (index) + 2)

/**
 * @def fpqueue_parent(index)
 * @brief Given an element index, get the index of the parent.
 *
 * @param[in] index The element index.
 */
#define fpqueue_parent(index) (((index) - 1) / 2)

/**
 * @def fpqueue_for_each(self, index, value_)
 * @brief Iterate over the values in the priority queue in breadth-first order.
 *
 * @warning Modifying the priority queue under the iteration may result in
 *          errors.
 *
 * @param[in] self      Priority queue pointer.
 * @param[in] index     Temporary indexing variable. Should be able to contain
 *                      `uint32_t`.
 * @param[out] value_   Current value. Should be `VALUE_TYPE`.
 */
#define fpqueue_for_each(self, index, value_) \
    for ((index) = 0; (index) < (self)->count && ((value_) = (self)->elements[(index)].value, true); (index)++)

#endif // FPQUEUE_H

/**
 * @def NAME
 * @brief Prefix to priority queue types and operations. This must be manually
 *        defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define NAME fpqueue
#error "Must define NAME."
#else
#define FPQUEUE_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Priority queue value type. This must be manually defined before
 *        including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE."
#define VALUE_TYPE int
#endif

/// @cond DO_NOT_DOCUMENT
#define FPQUEUE_TYPE         struct FPQUEUE_NAME
#define FPQUEUE_ELEMENT_TYPE struct JOIN(FPQUEUE_NAME, element)
#define FPQUEUE_INIT         JOIN(FPQUEUE_NAME, init)
#define FPQUEUE_IS_EMPTY     JOIN(FPQUEUE_NAME, is_empty)
#define FPQUEUE_IS_FULL      JOIN(FPQUEUE_NAME, is_full)
#define FPQUEUE_CALC_SIZEOF  JOIN(FPQUEUE_NAME, calc_sizeof)
#define FPQUEUE_UPHEAP       JOIN(internal, JOIN(FPQUEUE_NAME, upheap))
#define FPQUEUE_DOWNHEAP     JOIN(internal, JOIN(FPQUEUE_NAME, downheap))
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated priority queue element struct type for a given `VALUE_TYPE`.
 */
struct JOIN(FPQUEUE_NAME, element) {
    uint32_t priority; ///< Element priority (highest is next to-be-popped).
    VALUE_TYPE value;  ///< Element value member.
};

/**
 * @brief Generated priority queue struct type for a given `VALUE_TYPE`.
 */
struct FPQUEUE_NAME {
    uint32_t count;                  ///< Number of non-empty elements.
    uint32_t capacity;               ///< Number of elements allocated for.
    FPQUEUE_ELEMENT_TYPE elements[]; ///< Array of elements.
};

// }}}

// function definitions: {{{

/// @cond DO_NOT_DOCUMENT

/* push a node down the heap. for restoring the heap property after insertion */
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(FPQUEUE_TYPE *self, const uint32_t index);

/* push a node up the heap. for restoring the heap property after deletion */
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(FPQUEUE_TYPE *self, uint32_t index);

/// @endcond

/**
 * @brief Calculate the size of the priority queue struct.
 *
 * @param[in,out]  capacity_ptr  Pointer to orignal capacity
 * @param[out]     size_ptr      Pointer to size (to be outputted)
 *
 * @retval false If capacity is 0 or the equivalent size is larger than
 *               UINT32_MAX.
 * @retval true  Otherwise.
 */
static inline bool JOIN(FPQUEUE_NAME, calc_sizeof)(uint32_t *capacity_ptr, uint32_t *size_ptr)
{
    assert(capacity_ptr);
    assert(size_ptr);

    if (*capacity_ptr == 0
        || *capacity_ptr > (UINT32_MAX - offsetof(FPQUEUE_TYPE, elements)) / sizeof(FPQUEUE_ELEMENT_TYPE)) {
        return false;
    }

    *size_ptr = (uint32_t)(offsetof(FPQUEUE_TYPE, elements) + *capacity_ptr * sizeof(FPQUEUE_ELEMENT_TYPE));

    return true;
}

/**
 * @brief Initialize a priority queue struct, given a capacity.
 *
 * @param[in] self              Priority queue pointer
 * @param[in] capacity          Capacity
 */
static inline FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, init)(FPQUEUE_TYPE *self, const uint32_t capacity)
{
    assert(self);

    self->count = 0;
    self->capacity = capacity;

    return self;
}

/**
 * @brief Create an priority queue struct with a given capacity with malloc().
 *
 * @param[in] capacity      Maximum number of elements expected to be stored.
 *
 * @return A pointer to the priority queue.
 * @retval NULL
 *   @li If capacity is 0 or the equivalent size is larger than UINT32_MAX.
 *   @li If malloc fails.
 */
static inline FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, create)(uint32_t capacity)
{
    uint32_t size = 0;
    if (!FPQUEUE_CALC_SIZEOF(&capacity, &size)) {
        return NULL;
    }

    FPQUEUE_TYPE *self = (FPQUEUE_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FPQUEUE_INIT(self, capacity);

    return self;
}

/**
 * @brief Destroy an priority queue struct and free the underlying memory with free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self      The priority queue pointer.
 */
static inline void JOIN(FPQUEUE_NAME, destroy)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    free(self);
}

/**
 * @brief Return whether the priority queue is empty.
 *
 * @param[in] self      The priority queue pointer.
 *
 * @return Whether the priority queue is empty.
 */
static inline bool JOIN(FPQUEUE_NAME, is_empty)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

/**
 * @brief Return whether the priority queue is full.
 *
 * @param[in] self      The priority queue pointer.
 *
 * @return Whether the priority queue is full.
 */
static inline bool JOIN(FPQUEUE_NAME, is_full)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

/**
 * @brief Get the max priority value in a non-empty priority queue.
 *
 * @param[in] self      The priority queue pointer.
 *
 * @return The max priority value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, get_value_w_max_priority)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_EMPTY(self) == false);

    return self->elements[0].value;
}

/**
 * @brief Peek a non-empty priority queue and get it's next to-be-popped (max priority)
 *        value.
 *
 * @param[in] self      The priority queue pointer.
 *
 * @return The next to-be-popped (max priority) value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, peek)(const FPQUEUE_TYPE *self)
{
    return JOIN(FPQUEUE_NAME, get_value_w_max_priority)(self);
}

/**
 * @brief Pop the max priority value away from a non-empty priority queue.
 *
 * @param[in] self      The priority queue pointer.
 *
 * @return The max value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, pop_max)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_EMPTY(self) == false);

    VALUE_TYPE max_priority_value = self->elements[0].value;

    self->elements[0] = self->elements[self->count - 1];

    self->count--;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(self, 0);

    return max_priority_value;
}

/**
 * @brief Push a value with given priority onto a non-full priority queue.
 *
 * @param[in] self          The priority queue pointer.
 * @param[in] value         The value.
 * @param[in] priority      The priority (with large number meaning high
 *                          priority and vice versa).
 */
static inline void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE *self, VALUE_TYPE value, uint32_t priority)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_FULL(self) == false);

    const uint32_t index = self->count;

    self->elements[index] = (FPQUEUE_ELEMENT_TYPE){.priority = priority, .value = value};

    self->count++;

    JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(self, index);
}

/**
 * @brief Clear the elements in the priority queue.
 *
 * @param[in] self      The priority queue pointer.
 */
static inline void JOIN(FPQUEUE_NAME, clear)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    self->count = 0;
}

/**
 * @brief Copy the values from a source priority queue to a destination priority
 *        queue.
 *
 * @param[in,out] dest_ptr      The destination priority queue.
 * @param[in] src_ptr           The source priority queue.
 */
static inline void JOIN(FPQUEUE_NAME, copy)(FPQUEUE_TYPE *restrict dest_ptr, const FPQUEUE_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->count <= dest_ptr->capacity);
    assert(FPQUEUE_IS_EMPTY(dest_ptr));

    for (uint32_t i = 0; i < src_ptr->count; i++) {
        dest_ptr->elements[i] = src_ptr->elements[i];
    }
    dest_ptr->count = src_ptr->count;
}

/// @cond DO_NOT_DOCUMENT

static inline void JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(FPQUEUE_TYPE *self, uint32_t index)
{
    assert(self != NULL);
    assert(index < self->count);

    uint32_t parent;
    while (index > 0) {
        parent = fpqueue_parent(index);

        const bool sorted = self->elements[parent].priority >= self->elements[index].priority;

        if (sorted) {
            break;
        }

        const FPQUEUE_ELEMENT_TYPE temp = self->elements[index];
        self->elements[index] = self->elements[parent];
        self->elements[parent] = temp;

        index = parent;
    }
}

static inline void JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(FPQUEUE_TYPE *self, const uint32_t index)
{
    assert(self != NULL);
    assert(self->count == 0 || index < self->count);

    const uint32_t l = fpqueue_left_child(index);
    const uint32_t r = fpqueue_right_child(index);

    uint32_t largest = index;
    if (l < self->count && self->elements[l].priority > self->elements[index].priority) {
        largest = l;
    }
    if (r < self->count && self->elements[r].priority > self->elements[largest].priority) {
        largest = r;
    }

    if (largest == index) {
        return;
    }

    const FPQUEUE_ELEMENT_TYPE temp = self->elements[index];
    self->elements[index] = self->elements[largest];
    self->elements[largest] = temp;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(self, largest);
}
/// @endcond

// }}}

// macro undefs: {{{

#undef NAME
#undef VALUE_TYPE

#undef FPQUEUE_NAME
#undef FPQUEUE_TYPE
#undef FPQUEUE_ELEMENT_TYPE
#undef FPQUEUE_INIT
#undef FPQUEUE_IS_EMPTY
#undef FPQUEUE_IS_FULL
#undef FPQUEUE_CALC_SIZEOF
#undef FHASHTABLE_UPHEAP
#undef FHASHTABLE_DOWNHEAP

// }}}

// vim: ft=c fdm=marker

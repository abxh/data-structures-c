/*  fpqueue.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

/**
 * @file fpqueue.h
 * @brief Fixed-size heap-based priority queue
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `VALUE_TYPE`
 */

/**
 * @example fpqueue/fpqueue.c
 * Example of how `fpqueue.h` header file is used in practice.
 */

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// macro definitions: {{{

/**
 * @def NAME
 * @brief Prefix to priority queue type and operations.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef NAME
#define NAME fpqueue
#error "Must define NAME."
#else
#define FPQUEUE_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Priority queue value type.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE."
#define VALUE_TYPE int
#endif

#ifndef FPQUEUE_H
/// @cond DO_NOT_DOCUMENT
#define FPQUEUE_H
/// @endcond

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
 * @def fpqueue_for_each(pqueue_ptr, index, value_)
 * @brief Iterate over the values in the priority queue in arbitary order.
 * @warning Modifying the priority queue under the iteration may result in errors.
 *
 * @param[in] pqueue_ptr Priority queue pointer.
 * @param[in] index Temporary indexing variable. Should be `uint32_t`
 * @param[out] value_ Current value. Should be `VALUE_TYPE`.
 */
#define fpqueue_for_each(pqueue_ptr, index, value_)                \
    for ((index) = 0;                                              \
                                                                   \
         (index) < (pqueue_ptr)->count &&                          \
                                                                   \
         ((value_) = (pqueue_ptr)->elements[(index)].value, true); \
                                                                   \
         (index)++)

#endif // FPQUEUE_H

/// @cond DO_NOT_DOCUMENT
#define FPQUEUE_TYPE JOIN(FPQUEUE_NAME, type)
#define FPQUEUE_ELEMENT_TYPE JOIN(FPQUEUE_NAME, element_type)
#define FPQUEUE_IS_EMPTY JOIN(FPQUEUE_NAME, is_empty)
#define FPQUEUE_IS_FULL JOIN(FPQUEUE_NAME, is_full)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated priority queue element struct type for a given `VALUE_TYPE`.
 */
typedef struct {
    uint32_t priority; ///< element priority (highest is next to-be-popped).
    VALUE_TYPE value;  ///< element value member.
} FPQUEUE_ELEMENT_TYPE;

/**
 * @brief Generated priority queue struct type for a given `VALUE_TYPE`.
 */
typedef struct {
    uint32_t count;                  ///< Number of non-empty elements.
    uint32_t capacity;               ///< Number of elements allocated for in the priority queue.
    FPQUEUE_ELEMENT_TYPE elements[]; ///< Array of elements.
} FPQUEUE_TYPE;

// }}}

// function definitions: {{{

/**
 * @brief Create an priority queue with a given capacity with malloc().
 *
 * @param[in] capacity Maximum number of elements expected to be stored in the priority queue.
 * @return A pointer to the priority queue.
 * @retval `NULL`
 *   @li If capacity is 0 or is larger than UINT32_MAX.
 *   @li If malloc fails.
 */
static inline FPQUEUE_TYPE* JOIN(FPQUEUE_NAME, create)(const uint32_t capacity) {
    if (capacity == 0 || capacity > (UINT32_MAX - offsetof(FPQUEUE_TYPE, elements)) / sizeof(FPQUEUE_ELEMENT_TYPE)) {
        return NULL;
    }
    FPQUEUE_TYPE* pqueue_ptr = (FPQUEUE_TYPE*)malloc(offsetof(FPQUEUE_TYPE, elements) + capacity * sizeof(FPQUEUE_ELEMENT_TYPE));

    if (!pqueue_ptr) {
        return NULL;
    }
    memset(pqueue_ptr, 0, offsetof(FPQUEUE_TYPE, elements) + capacity * sizeof(FPQUEUE_ELEMENT_TYPE));

    pqueue_ptr->count = 0;
    pqueue_ptr->capacity = capacity;

    return pqueue_ptr;
}

/**
 * @brief Destroy an priority queue and free the underlying memory with free().
 *
 * @param[in,out] pqueue_ptr The priority queue pointer.
 * @warning May not be called twice in a row on the same object.
 */
static inline void JOIN(FPQUEUE_NAME, destroy)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    free(pqueue_ptr);
}

/**
 * @brief Return whether the priority queue is empty.
 *
 * Assumes the priority queue pointer is not `NULL`.
 *
 * @param[in] pqueue_ptr The priority queue pointer.
 * @return whether the priority queue is empty.
 */
static inline bool JOIN(FPQUEUE_NAME, is_empty)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return pqueue_ptr->count == 0;
}

/**
 * @brief Return whether the priority queue is full.
 *
 * Assumes the priority queue pointer is not `NULL`.
 *
 * @param[in] pqueue_ptr The priority queue pointer.
 * @return whether the priority queue is full.
 */
static inline bool JOIN(FPQUEUE_NAME, is_full)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return pqueue_ptr->count == pqueue_ptr->capacity;
}

/**
 * @brief Get the max value in the priority queue.
 *
 * Assumes:
 * @li `pqueue_ptr` is not `NULL`.
 * @li The priority queue is not empty.
 *
 * @param[in] pqueue_ptr The priority queue pointer.
 * @return The max value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, get_max)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);
    assert(FPQUEUE_IS_EMPTY(pqueue_ptr) == false);

    return pqueue_ptr->elements[0].value;
}

/**
 * @brief Peek the priority queue and get it's next to-be-popped (max) value.
 *
 * Assumes:
 * @li `pqueue_ptr` is not `NULL`.
 * @li The priority queue is not empty.
 *
 * @param[in] pqueue_ptr The priority queue pointer.
 * @return The next to-be-popped (max) value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, peek)(const FPQUEUE_TYPE* pqueue_ptr) {
    return JOIN(FPQUEUE_NAME, get_max)(pqueue_ptr);
}

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(FPQUEUE_TYPE* pqueue_ptr, const uint32_t index) {
    assert(pqueue_ptr != NULL);
    assert(pqueue_ptr->count == 0 || index < pqueue_ptr->count);

    const uint32_t l = fpqueue_left_child(index);
    const uint32_t r = fpqueue_right_child(index);

    uint32_t largest = index;
    if (l < pqueue_ptr->count && pqueue_ptr->elements[l].priority > pqueue_ptr->elements[index].priority) {
        largest = l;
    }
    if (r < pqueue_ptr->count && pqueue_ptr->elements[r].priority > pqueue_ptr->elements[largest].priority) {
        largest = r;
    }

    if (largest == index) {
        return;
    }

    const FPQUEUE_ELEMENT_TYPE temp = pqueue_ptr->elements[index];
    pqueue_ptr->elements[index] = pqueue_ptr->elements[largest];
    pqueue_ptr->elements[largest] = temp;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(pqueue_ptr, largest);
}
/// @endcond

/**
 * @brief Pop the max value away from the priority queue.
 *
 * Assumes:
 * @li `pqueue_ptr` is not `NULL`.
 * @li The priority queue is not empty.
 *
 * @param[in,out] pqueue_ptr The priority queue pointer.
 * @return The max value.
 */
static inline VALUE_TYPE JOIN(FPQUEUE_NAME, pop_max)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);
    assert(FPQUEUE_IS_EMPTY(pqueue_ptr) == false);

    VALUE_TYPE max = pqueue_ptr->elements[0].value;

    pqueue_ptr->elements[0] = pqueue_ptr->elements[pqueue_ptr->count - 1];

    pqueue_ptr->count--;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(pqueue_ptr, 0);

    return max;
}

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(FPQUEUE_TYPE* pqueue_ptr, uint32_t index) {
    assert(pqueue_ptr != NULL);
    assert(index < pqueue_ptr->count);

    uint32_t parent;
    while (index > 0 && (parent = fpqueue_parent(index),

                         pqueue_ptr->elements[parent].priority < pqueue_ptr->elements[index].priority)) {

        const FPQUEUE_ELEMENT_TYPE temp = pqueue_ptr->elements[index];
        pqueue_ptr->elements[index] = pqueue_ptr->elements[parent];
        pqueue_ptr->elements[parent] = temp;

        index = parent;
    }
}
/// @endcond

/**
 * @brief Push a value with given priority onto the priority queue.
 *
 * Assumes:
 * @li `pqueue_ptr` is not `NULL`.
 * @li The priority queue is not full.
 *
 * @param[in,out] pqueue_ptr The priority queue pointer.
 * @param[in] value The value.
 * @param[in] priority The priority (with large number meaning high priority and vice versa).
 */
static inline void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE* pqueue_ptr, VALUE_TYPE value, uint32_t priority) {
    assert(pqueue_ptr != NULL);
    assert(FPQUEUE_IS_FULL(pqueue_ptr) == false);

    const uint32_t index = pqueue_ptr->count;

    pqueue_ptr->elements[index] = (FPQUEUE_ELEMENT_TYPE){.priority = priority, .value = value};

    pqueue_ptr->count++;

    JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(pqueue_ptr, index);
}

/**
 * @brief Clear the elements in the priority queue.
 *
 * Assumes `pqueue_ptr` is not `NULL`.
 *
 * @param[in,out] pqueue_ptr The priority queue pointer.
 */
static inline void JOIN(FPQUEUE_NAME, clear)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    pqueue_ptr->count = 0;
}

/**
 * @brief Copy the values from a source priority queue to a destination priority queue.
 *
 * Assumes:
 * @li Source and destination priority queue pointers are not pointing to the same memory.
 * @li The priority queue pointers are not `NULL`.
 * @li The destination priority queue has a capacity that is greater than or equal to source priority queue count.
 * @li The destination priority queue is an empty priority queue.
 *
 * @param[in,out] dest_pqueue_ptr The destination priority queue.
 * @param[in] src_pqueue_ptr The source priority queue.
 */
static inline void JOIN(FPQUEUE_NAME, copy)(FPQUEUE_TYPE* restrict dest_pqueue_ptr, const FPQUEUE_TYPE* restrict src_pqueue_ptr) {
    assert(src_pqueue_ptr != NULL);
    assert(dest_pqueue_ptr != NULL);
    assert(src_pqueue_ptr->count <= dest_pqueue_ptr->capacity);
    assert(FPQUEUE_IS_EMPTY(dest_pqueue_ptr));

    for (uint32_t i = 0; i < src_pqueue_ptr->count; i++) {
        dest_pqueue_ptr->elements[i] = src_pqueue_ptr->elements[i];
    }
    dest_pqueue_ptr->count = src_pqueue_ptr->count;
}

// }}}

// macro undefs: {{{

#undef NAME
#undef VALUE_TYPE

#undef FPQUEUE_NAME
#undef FPQUEUE_TYPE
#undef FPQUEUE_ELEMENT_TYPE
#undef FPQUEUE_IS_EMPTY
#undef FPQUEUE_IS_FULL

// }}}

// vim: ft=c fdm=marker

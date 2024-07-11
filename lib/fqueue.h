/**
 * @file fqueue.h
 * @brief Fixed-size ring-buffer-based queue data structure.
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `VALUE_TYPE`
 */

/**
 * @example fqueue/fqueue.c
 * Examples of how `fqueue.h` header file is used in practice.
 */

// macro definitions: {{{

#include "paste.h"         // PASTE, XPASTE, JOIN
#include "round_up_pow2.h" // round_up_pow2

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def NAME
 * @brief Prefix to queue type and operations. This must be manually defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define NAME fqueue
#error "Must define NAME."
#else
#define FQUEUE_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Queue value type. This must be manually defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#error "Must define VALUE_TYPE."
#endif

#ifndef __FQUEUE__H
/// @cond DO_NOT_DOCUMENT
#define __FQUEUE__H
/// @endcond

/**
 * @def FQUEUE_FOREACH(queue_ptr, temp_index, value)
 * @brief Iterate over the values in the queue from the front to back.
 * @warning Modifying the queue under the iteration may result in errors.
 *
 * @param[in] queue_ptr Queue pointer.
 * @param[out] temp_index Temporary variable used for indexing. Should be `size_t`.
 * @param[out] value Current value. Should be `VALUE_TYPE`.
 */
#define FQUEUE_FOREACH(queue_ptr, temp_index, value)          \
    for ((temp_index) = (queue_ptr)->begin_index;             \
                                                              \
         (temp_index) != (queue_ptr)->end_index &&            \
                                                              \
         ((value) = (queue_ptr)->values[(temp_index)], true); \
                                                              \
         (temp_index) = ((temp_index) + 1) & ((queue_ptr)->capacity - 1))

/**
 * @def FQUEUE_FOREACH_REVERSE(queue_ptr, temp_index, value)
 * @brief Iterate over the values in the queue from the back to front.
 * @warning Modifying the queue under the iteration may result in errors.
 *
 * @param[in] queue_ptr Queue pointer.
 * @param[out] temp_index Temporary variable used for indexing. Should be `size_t`.
 * @param[out] value Current value. Should be `VALUE_TYPE`.
 */
#define FQUEUE_FOREACH_REVERSE(queue_ptr, temp_index, value)                               \
    for ((temp_index) = (queue_ptr)->end_index - 1;                                        \
                                                                                           \
         (temp_index) != (((queue_ptr)->begin_index - 1) & ((queue_ptr)->capacity - 1)) && \
                                                                                           \
         ((value) = (queue_ptr)->values[(temp_index)], true);                              \
                                                                                           \
         (temp_index) = ((temp_index) - 1) & ((queue_ptr)->capacity - 1))

#endif // __FQUEUE__H

/// @cond DO_NOT_DOCUMENT
#define FQUEUE_TYPE JOIN(FQUEUE_NAME, type)
#define FQUEUE_IS_EMPTY JOIN(FQUEUE_NAME, is_empty)
#define FQUEUE_IS_FULL JOIN(FQUEUE_NAME, is_full)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated queue struct type for a `VALUE_TYPE`.
 */
typedef struct {
    size_t begin_index;  ///< index used to track the front of the queue.
    size_t end_index;    ///< index used to track the back of the queue.
    size_t count;        ///< number of values in the ring buffer
    size_t capacity;     ///< maximum number of values allocated for in the stack.
    VALUE_TYPE values[]; ///< array of values.
} FQUEUE_TYPE;

// }}}

// function definitions: {{{

/**
 * @brief Create an queue with a given capacity with malloc().
 *
 * @param[in] capacity Maximum number of elements expected to be stored in the queue.
 * @return A pointer to the queue.
 * @retval `NULL`
 *   @li If malloc fails.
 *   @li If `capacity` is larger than UINTPTR_MAX / 4.
 */
static inline FQUEUE_TYPE* JOIN(FQUEUE_NAME, create)(const size_t capacity) {
    if (capacity > UINTPTR_MAX / 4) {
        fprintf(stderr, "capacity is too large. ring buffer cannot be wrapped as intended. returning NULL.\n");
        return NULL;
    }

    const size_t capacity_new = round_up_pow2(capacity);

    FQUEUE_TYPE* queue_ptr = malloc(offsetof(FQUEUE_TYPE, values) + capacity_new * sizeof(VALUE_TYPE));
    if (!queue_ptr) {
        fprintf(stderr, "%s() failed: %s. returning NULL.\n", __func__, strerror(errno));
        return NULL;
    }
    memset(queue_ptr, 0, offsetof(FQUEUE_TYPE, values) + capacity * sizeof(VALUE_TYPE));

    queue_ptr->begin_index = queue_ptr->end_index = 0;
    queue_ptr->count = 0;
    queue_ptr->capacity = capacity_new;

    return queue_ptr;
}

/**
 * @brief Destroy an queue and free the underlying memory with free().
 *
 * Assumes:
 * @li The queue pointer is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @warning May not be called twice in a row on the same object.
 */
static inline void JOIN(FQUEUE_NAME, destroy)(FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);

    free(queue_ptr);
}

/**
 * @brief Return whether the queue is empty.
 *
 * Assumes:
 * @li The queue pointer is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return whether the queue is empty.
 */
static inline bool JOIN(FQUEUE_NAME, is_empty)(const FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);

    return queue_ptr->count == 0;
}

/**
 * @brief Return whether the queue is full.
 *
 * Assumes:
 * @li The queue pointer is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return whether the queue is full.
 */
static inline bool JOIN(FQUEUE_NAME, is_full)(const FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);

    return queue_ptr->count == queue_ptr->capacity;
}

/**
 * @brief Get the value at index.
 *
 * index starts from the front as `0` and is counted upward to `count - 1` as back.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li `index` is strictly less than queue count.
 *
 * @param[in] queue_ptr The queue pointer.
 * @param[in] index The index to retrieve to value from.
 * @return The value at `index`.
 */
static inline VALUE_TYPE JOIN(FQUEUE_NAME, at)(const FQUEUE_TYPE* queue_ptr, const size_t index) {
    assert(queue_ptr != NULL);
    assert(index < queue_ptr->count);

    return queue_ptr->values[index];
}

/**
 * @brief Get the value from the front of the queue.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li The queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The front value.
 */
static inline VALUE_TYPE JOIN(FQUEUE_NAME, get_front)(const FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);
    assert(!FQUEUE_IS_EMPTY(queue_ptr));

    return queue_ptr->values[queue_ptr->begin_index];
}

/**
 * @brief Get the value from the back of the queue.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li The queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The back value.
 */
static inline VALUE_TYPE JOIN(FQUEUE_NAME, get_back)(const FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);
    assert(!FQUEUE_IS_EMPTY(queue_ptr));

    const int index_mask = (queue_ptr->capacity - 1);

    return queue_ptr->values[(queue_ptr->end_index - 1) & index_mask];
}

/**
 * @brief Peek the queue and get it's next to-be-dequeued value.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li The queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The next to-be-dequeued value.
 */
static inline VALUE_TYPE JOIN(FQUEUE_NAME, peek)(const FQUEUE_TYPE* queue_ptr) {
    return JOIN(FQUEUE_NAME, get_front)(queue_ptr);
}

/**
 * @brief Enqueue a value at the back of the queue.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li The queue is not full.
 *
 * @param[in] queue_ptr The queue pointer.
 * @param[in] value The value to enqueue.
 */
static inline bool JOIN(FQUEUE_NAME, enqueue)(FQUEUE_TYPE* queue_ptr, const VALUE_TYPE value) {
    assert(queue_ptr != NULL);
    assert(!FQUEUE_IS_FULL(queue_ptr));

    const int index_mask = (queue_ptr->capacity - 1);

    queue_ptr->values[queue_ptr->end_index] = value;
    queue_ptr->end_index++;
    queue_ptr->end_index &= index_mask;
    queue_ptr->count++;

    return true;
}

/**
 * @brief Dequeue a value from the front of the queue.
 *
 * Assumes:
 * @li `queue_ptr` is not `NULL`.
 * @li The queue is not full.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The front value.
 */
static inline VALUE_TYPE JOIN(FQUEUE_NAME, dequeue)(FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);
    assert(!FQUEUE_IS_EMPTY(queue_ptr));

    const int index_mask = (queue_ptr->capacity - 1);

    const VALUE_TYPE value = queue_ptr->values[queue_ptr->begin_index];
    queue_ptr->begin_index++;
    queue_ptr->begin_index &= index_mask;
    queue_ptr->count--;

    return value;
}

/**
 * @brief Clear the elements in the queue.
 *
 * Assumes `queue_ptr` is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 */
static inline void JOIN(FQUEUE_NAME, clear)(FQUEUE_TYPE* queue_ptr) {
    assert(queue_ptr != NULL);

    queue_ptr->count = 0;
    queue_ptr->begin_index = queue_ptr->end_index = 0;
}

/**
 * @brief Copy the values from a source queue to a destination queue.
 *
 * Assumes:
 * @li Source and destination queue pointers are not pointing to the same memory.
 * @li The queue pointers are not `NULL`.
 * @li The destination queue has a capacity that is greater than or equal to source queue count.
 * @li The destination queue is an empty queue.
 *
 * @param[in] src_queue_ptr The source queue.
 * @param[out] dest_queue_ptr The destination queue.
 */
static inline void JOIN(FQUEUE_NAME, copy)(FQUEUE_TYPE* restrict dest_queue_ptr, const FQUEUE_TYPE* restrict src_queue_ptr) {
    assert(src_queue_ptr != NULL);
    assert(dest_queue_ptr != NULL);
    assert(src_queue_ptr->count <= dest_queue_ptr->capacity);
    assert(FQUEUE_IS_EMPTY(dest_queue_ptr));

    if (src_queue_ptr->begin_index <= src_queue_ptr->end_index) {
        for (size_t i = src_queue_ptr->begin_index; i < src_queue_ptr->end_index; i++) {
            dest_queue_ptr->values[i] = src_queue_ptr->values[i];
        }
    } else {
        for (size_t i = 0; i < src_queue_ptr->end_index; i++) {
            dest_queue_ptr->values[i] = src_queue_ptr->values[i];
        }
        for (size_t i = src_queue_ptr->begin_index; i < src_queue_ptr->capacity; i++) {
            dest_queue_ptr->values[i] = src_queue_ptr->values[i];
        }
    }

    dest_queue_ptr->count = src_queue_ptr->count;
    dest_queue_ptr->begin_index = src_queue_ptr->begin_index;
    dest_queue_ptr->end_index = src_queue_ptr->end_index;
}

// }}}

// macro undefs: {{{

#undef NAME
#undef VALUE_TYPE

#undef FQUEUE_NAME
#undef FQUEUE_TYPE
#undef FQUEUE_IS_EMPTY
#undef FQUEUE_ISFULL

// }}}

// vim: ft=c fdm=marker

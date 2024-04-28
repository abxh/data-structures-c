/**
 * @file queue.h
 * @brief Queue data structure based on a dynamic circular buffer.
 *
 * The array is only grown - not shrunk - for performance reasons.
 *
 * Note to distinguish between empty and full circular buffer with a fixed
 * capacity, extra information must be stored in the form of a variable
 * (count, nullables), or queue must reserve a empty slot at all times
 * ("waisting" some memory in the array). See:
 * https://en.wikipedia.org/wiki/Circular_buffer#Circular_buffer_mechanics
 *
 * Other Source(s):
 * - for naming: https://en.wikipedia.org/wiki/Queue_(abstract_data_type)
 * - for resizing: https://queueoverflow.com/questions/55343683/resizing-of-the-circular-queue-using-dynamic-array
 */

/**
 * @example queue/queue.c
 * Examples of how `queue.h` header file is used in practice.
 */

#include <assert.h>   // assert
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool
#include <stdint.h>   // SIZE_MAX
#include <stdlib.h>   // size_t, NULL
#include <string.h>   // memcpy

#include "allocator_ops.h" // allocate_ops_type
#include "is_pow2.h"       // is_pow2
#include "macros.h"        // JOIN
#include "std_allocator.h" // std_allocator_ops

#ifndef __queue_for_each
#define __queue_for_each(queue_ptr, index, value)                                                                                 \
    for ((index) = (queue_ptr)->start_index; (index) != (queue_ptr)->end_index && ((value) = (queue_ptr)->values[(index)], true); \
         (index) = ((index) + 1) & (queue_ptr)->index_mask)
#endif

/**
 * @def queue_for_each
 * @brief Iterate over the values in the queue from the front value or first-to-be-dequeued value.
 * @param[in] queue_ptr queue pointer.
 * @param[out] index The index of the value.
 * @param[out] value Value at `index` as `VALUE_TYPE`.
 */
#ifndef queue_for_each
#define queue_for_each(queue_ptr, count, value) __queue_for_each(queue_ptr, count, value)
#endif

/**
 * @def VALUE_TYPE
 * @brief The value type.
 * @attention This must be manually defined before including this header file.
 *
 * Is defaulted to int, so the editor's LSP works better.
 */
#ifndef VALUE_TYPE
#error "must define VALUE_TYPE. defaulting to int."
#define VALUE_TYPE int
#endif

/**
 * @def PREFIX
 * @brief The queue prefix. Set to VALUE_TYPE\#\#_\#\#queue, unless specified otherwise.
 */
#ifdef PREFIX
#define __QUEUE_PREFIX PREFIX
#elif !defined(__QUEUE_PREFIX)
#define __QUEUE_PREFIX JOIN(VALUE_TYPE, queue)
#endif

#ifndef PREFIX
#define PREFIX VALUE_TYPE##_##queue
#undef PREFIX // above is for doxygen documentation.
#endif

/* The queue type name. Set to VALUE_TYPE##_##type. */
#ifndef QUEUE_TYPE
#define QUEUE_TYPE JOIN(__QUEUE_PREFIX, type)
#endif

/* Aliases. Used by other functions. Will be cleaned up by the end. */
#ifndef __T_queue_is_empty
#define __T_queue_is_empty JOIN(__QUEUE_PREFIX, is_empty)
#endif

/**
 * @brief Generated queue struct type for a value type.
 */
typedef struct {
    size_t count;       ///< number of values in the circular buffer
    size_t index_mask;  ///< the mask used to wrap indicies around the circular buffer.
    size_t start_index; ///< index used to track the front of the queue.
    size_t end_index;   ///< index used to track the back of the queue.
    VALUE_TYPE* values; ///< buffer containing the values in the queue.

    void* allocator_context_ptr;      ///< pointer to internal data in an allocator.
    allocator_ops_type allocator_ops; ///< allocator operation functions grouped together as a struct.
} QUEUE_TYPE;

/**
 * @brief Create a queue with specified initial power of two capacity and custom allocator.
 *
 * @param[in] initial_pow2_capacity The initial capacity of the queue, which must be a power of 2 and not 1.
 * @param[in] allocator_context_ptr Pointer to allocator context.
 * @param[in] allocator_ops Struct pointing to allocator operations.
 * @return The queue pointer.
 * @retval `NULL`
 * - If `initial_pow2_capacity` is 0 or 1.
 * - If `initial_pow2_capacity * sizeof(VALUE_TYPE)` cannot be expressed with `size_t`.
 * - If no memory space is available.
 */
static inline QUEUE_TYPE* JOIN(__QUEUE_PREFIX, create_with_specified)(size_t initial_pow2_capacity, void* allocator_context_ptr,
                                                                      allocator_ops_type allocator_ops) {
    if (!is_pow2(initial_pow2_capacity) || initial_pow2_capacity == 0 || initial_pow2_capacity - 1 == 0 ||
        initial_pow2_capacity > SIZE_MAX / sizeof(VALUE_TYPE)) {
        return NULL;
    }
    QUEUE_TYPE* queue_ptr = (QUEUE_TYPE*)allocator_ops.allocate_f_p(allocator_context_ptr, alignof(QUEUE_TYPE), sizeof(QUEUE_TYPE));
    if (!queue_ptr) {
        return NULL;
    }
    queue_ptr->values = (VALUE_TYPE*)allocator_ops.allocate_f_p(allocator_context_ptr, alignof(VALUE_TYPE),
                                                                sizeof(VALUE_TYPE) * initial_pow2_capacity);
    if (!queue_ptr->values) {
        free(queue_ptr);
        return NULL;
    }
    queue_ptr->count = 0;
    queue_ptr->index_mask = initial_pow2_capacity - 1;
    queue_ptr->start_index = 0;
    queue_ptr->end_index = 0;

    queue_ptr->allocator_context_ptr = allocator_context_ptr;
    queue_ptr->allocator_ops = allocator_ops;

    return queue_ptr;
}

/**
 * @brief Create a queue with a default capacity and standard allocator (aka `malloc`).
 *
 * @return The queue pointer.
 * @retval `NULL` If no memory space is available.
 */
static inline QUEUE_TYPE* JOIN(__QUEUE_PREFIX, create)(void) {
    return JOIN(__QUEUE_PREFIX, create_with_specified)(512, NULL, std_allocator_ops);
}

/**
 * @brief Destroy a queue and free the used memory.
 *
 * @warning May not be called twice in a row on the same queue.
 */
static inline void JOIN(__QUEUE_PREFIX, destroy)(QUEUE_TYPE* queue_ptr) {
    if (!queue_ptr) {
        return;
    }
    void* allocator_context_ptr_copy = queue_ptr->allocator_context_ptr;
    allocator_ops_type allocator_ops_copy = queue_ptr->allocator_ops;

    allocator_ops_copy.deallocate_f_p(allocator_context_ptr_copy, queue_ptr->values);
    allocator_ops_copy.deallocate_f_p(allocator_context_ptr_copy, queue_ptr);
}

/**
 * @brief Clone an existing queue.
 *
 * @param[in] queue_ptr The pointer of the queue to clone.
 * @return A pointer to the clone of the original queue.
 * @retval `NULL`
 * - If no memory space is available.
 * - If queue_ptr is `NULL`.
 */
static inline QUEUE_TYPE* JOIN(__QUEUE_PREFIX, clone)(const QUEUE_TYPE* queue_ptr) {
    if (!queue_ptr) {
        return NULL;
    }
    size_t capacity = queue_ptr->index_mask + 1;
    QUEUE_TYPE* other_queue_ptr =
        JOIN(__QUEUE_PREFIX, create_with_specified)(capacity, queue_ptr->allocator_context_ptr, queue_ptr->allocator_ops);
    if (!other_queue_ptr) {
        return NULL;
    }
    if (queue_ptr->start_index <= queue_ptr->end_index) {
        memcpy(&other_queue_ptr->values[queue_ptr->start_index], &queue_ptr->values[queue_ptr->start_index],
               sizeof(VALUE_TYPE) * queue_ptr->count);
    } else {
        size_t n = queue_ptr->end_index;
        size_t m = (capacity - queue_ptr->start_index);

        assert(n + m == queue_ptr->count);

        memcpy(&other_queue_ptr->values[0], &queue_ptr->values[0], sizeof(VALUE_TYPE) * n);
        memcpy(&other_queue_ptr->values[queue_ptr->start_index], &queue_ptr->values[queue_ptr->start_index], sizeof(VALUE_TYPE) * m);
    }
    other_queue_ptr->count = queue_ptr->count;
    other_queue_ptr->start_index = queue_ptr->start_index;
    other_queue_ptr->end_index = queue_ptr->end_index;

    return other_queue_ptr;
}

/**
 * @brief Get the number of values in the queue.
 *
 * Asserts queue_ptr is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The number of elements as `size_t`.
 */
static inline size_t JOIN(__QUEUE_PREFIX, count)(const QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);

    return queue_ptr->count;
}

/**
 * @brief Get the number of values allocated for in the queue. Capacity is grown as needed.
 *
 * Asserts queue_ptr is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The current capacity as `size_t`.
 */
static inline size_t JOIN(__QUEUE_PREFIX, capacity)(const QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);

    return queue_ptr->index_mask + 1;
}

/**
 * @brief Check if queue is empty.
 *
 * Asserts queue_ptr is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return A boolean indicating whether the queue is empty.
 */
static inline bool JOIN(__QUEUE_PREFIX, is_empty)(const QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);

    return 0 == queue_ptr->count;
}

/**
 * @brief Get value at index relative to queue front or start index as 0.
 *
 * Asserts queue_ptr is not `NULL` and index is strictly less than queue count.
 *
 * @param[in] queue_ptr The queue pointer.
 * @param[in] index Index at which the value lies.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, at)(const QUEUE_TYPE* queue_ptr, size_t index) {
    assert(NULL != queue_ptr);
    assert(index < queue_ptr->count);

    size_t rel_index = ((queue_ptr->start_index + index) & queue_ptr->index_mask);

    return queue_ptr->values[rel_index];
}

/**
 * @brief Get the value from the front of the queue.
 *
 * Asserts queue_ptr is not `NULL` and queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, front)(const QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);
    assert(!__T_queue_is_empty(queue_ptr));

    return queue_ptr->values[queue_ptr->start_index];
}

/**
 * @brief Get the value from the back of the queue.
 *
 * Asserts queue_ptr is not `NULL` and queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, back)(const QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);
    assert(!__T_queue_is_empty(queue_ptr));

    return queue_ptr->values[(queue_ptr->end_index - 1) & queue_ptr->index_mask];
}

/**
 * @brief Peek at the queue and get it's front value.
 *
 * Asserts queue_ptr is not `NULL` and queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, peek)(const QUEUE_TYPE* queue_ptr) {
    return JOIN(__QUEUE_PREFIX, front)(queue_ptr);
}

/**
 * @brief Peek at the queue and get it's first-to-be-dequeued value.
 *
 * Asserts queue_ptr is not `NULL` and queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, peek_first)(const QUEUE_TYPE* queue_ptr) {
    return JOIN(__QUEUE_PREFIX, front)(queue_ptr);
}

/**
 * @brief Peek at the queue and get it's last-to-be-dequeued value.
 *
 * Asserts queue_ptr is not `NULL` and queue is not empty.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, peek_last)(const QUEUE_TYPE* queue_ptr) {
    return JOIN(__QUEUE_PREFIX, back)(queue_ptr);
}

/**
 * @brief Enqueue a value onto the queue.
 *
 * Asserts queue_ptr is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @param[in] value The value to work with.
 * @return A boolean indicating whether the value was stored, depending on whether
 *         the queue could be resized when about to full.
 * @retval false
 * - If `current capacity * sizeof(VALUE_TYPE) * 2` cannot be expressed with `size_t`.
 * - If no memory space is available.
 */
static inline bool JOIN(__QUEUE_PREFIX, enqueue)(QUEUE_TYPE* queue_ptr, VALUE_TYPE value) {
    assert(NULL != queue_ptr);

    size_t old_capacity = queue_ptr->index_mask + 1;
    if (queue_ptr->count + 1 == old_capacity) {
        if (old_capacity > SIZE_MAX / sizeof(VALUE_TYPE) / 2) {
            return false;
        }
        size_t new_capacity = 2 * old_capacity;
        void* temp_ptr =
            queue_ptr->allocator_ops.reallocate_f_p(queue_ptr->allocator_context_ptr, queue_ptr->values, alignof(VALUE_TYPE),
                                                    sizeof(VALUE_TYPE) * old_capacity, sizeof(VALUE_TYPE) * new_capacity);
        if (temp_ptr == NULL) {
            return false;
        }
        queue_ptr->values = (VALUE_TYPE*)temp_ptr;
        queue_ptr->index_mask = new_capacity - 1;
        if (queue_ptr->end_index < queue_ptr->start_index) {
            size_t n = queue_ptr->end_index;
            size_t m = (old_capacity - queue_ptr->start_index);

            assert(n + m == queue_ptr->count);

            if (n <= m) {
                memcpy(&queue_ptr->values[old_capacity], &queue_ptr->values[0], n);
                queue_ptr->end_index += old_capacity;
            } else {
                memcpy(&queue_ptr->values[new_capacity - m], &queue_ptr->values[queue_ptr->start_index], m);
                queue_ptr->start_index += old_capacity; // because new_capacity = 2 * old_capacity
            }
        }
    }
    queue_ptr->values[queue_ptr->end_index] = value;
    queue_ptr->end_index = (queue_ptr->end_index + 1) & queue_ptr->index_mask;
    queue_ptr->count++;

    return true;
}

/**
 * @brief Dequeue a value from the queue and return the value.
 *
 * Asserts queue_ptr is not `NULL`.
 *
 * @param[in] queue_ptr The queue pointer.
 * @return The value as `VALUE_TYPE`.
 */
static inline VALUE_TYPE JOIN(__QUEUE_PREFIX, dequeue)(QUEUE_TYPE* queue_ptr) {
    assert(NULL != queue_ptr);
    assert(!__T_queue_is_empty(queue_ptr));

    VALUE_TYPE value = queue_ptr->values[queue_ptr->start_index];
    queue_ptr->start_index = (queue_ptr->start_index + 1) & queue_ptr->index_mask;
    queue_ptr->count--;
    return value;
}

#undef __T_queue_is_empty
#undef __QUEUE_PREFIX
#undef VALUE_TYPE
#undef QUEUE_TYPE

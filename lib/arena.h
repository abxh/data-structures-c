/**
 * @file arena.h
 * @brief Arena allocator.
 *
 * Source used:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#pragma once

#include "align_forward.h" // align_forward
#include "is_pow2.h"       // is_pow2

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def DEFAULT_ALIGNMENT
 * @brief Set to the shown default value. Can be defined before including the header.
 */
#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

/**
 * @brief Arena data struct.
 */
typedef struct arena_type {
    size_t buffer_length;      ///< Underlying buffer length.
    unsigned char* buffer_ptr; ///< Underlying buffer pointer.
    size_t previous_offset;    ///< Previous offset describing space allocated in buffer.
    size_t current_offset;     ///< Current offset describing space allocated in buffer.
} arena_type;

/**
 * @brief Initialize the arena.
 *
 * @note The arena struct could stack / heap-allocated depending on program structure.
 *
 * @param[in] arena_ptr The arena pointer.
 * @param[in] n The length of the backing buffer.
 * @param[in] backing_buffer The backing buffer to use.
 */
static inline void arena_init(arena_type* arena_ptr, const size_t n, unsigned char backing_buffer[n]) {
    arena_ptr->buffer_ptr = backing_buffer;
    arena_ptr->buffer_length = n;
    arena_ptr->current_offset = 0;
    arena_ptr->previous_offset = 0;
}

/**
 * @brief Deallocate all allocations in the arena.
 *
 * @param[in] arena_ptr The arena pointer.
 */
static inline void arena_deallocate_all(arena_type* arena_ptr) {
    arena_ptr->current_offset = 0;
    arena_ptr->previous_offset = 0;
}

/**
 * @brief Get the pointer to a chunk of the arena. With alignment.
 *
 * @param[in] arena_ptr The arena pointer.
 * @param[in] alignment The alignment size in bytes.
 * @param[in] size The section size in bytes.
 * @return A pointer to the memory chunk.
 *  @retval NULL If the arena doesn't have enough memory for the allocation.
 */
static inline void* arena_allocate_aligned(arena_type* arena_ptr, const size_t alignment, const size_t size) {
    // clang-format off

    // Align `current_offset` forward to the specified alignment

    const uintptr_t current_ptr =
        + (uintptr_t) arena_ptr->buffer_ptr
        + (uintptr_t) arena_ptr->current_offset;

    const uintptr_t relative_offset = 
        + (uintptr_t) align_forward(current_ptr, alignment)
        - (uintptr_t) arena_ptr->buffer_ptr;

    // clang-format on

    // Check to see if the backing memory has space left
    if (relative_offset + size <= arena_ptr->buffer_length) {

        arena_ptr->previous_offset = relative_offset;
        arena_ptr->current_offset = relative_offset + size;

        void* ptr = (unsigned char*)&arena_ptr->buffer_ptr[relative_offset];
        memset(ptr, 0, size); // zero memory by default

        return ptr;
    }
    return NULL;
}

/**
 * @brief Get the pointer to a chunk of the arena.
 *
 * @param[in] arena_ptr The arena pointer.
 * @param[in] size The section size in bytes.
 * @return A pointer to the memory chunk.
 *  @retval NULL If the arena doesn't have enough memory for the allocation.
 */
static inline void* arena_allocate(arena_type* arena_ptr, const size_t size) {
    return arena_allocate_aligned(arena_ptr, DEFAULT_ALIGNMENT, size);
}

/**
 * @brief Reallocate a previously allocated chunk in the arena. With aligment.
 *
 * @param[in] arena_ptr The arena pointer.
 * @param[in] old_memory_ptr A pointer, pointing to the beginning of the memory chunk.
 * @param[in] alignment Alignment size.
 * @param[in] old_size Old size allocated.
 * @param[in] new_size New size to grow/shrink to.
 * @return A pointer to reallocated the memory chunk.
 *  @retval NULL If arena doesn't have enough memory for the reallocation.
 */
static inline void* arena_reallocate_aligned(arena_type* arena_ptr, void* old_memory_ptr, const size_t alignment,
                                             const size_t old_size, const size_t new_size) {
    assert(is_pow2(alignment));

    const unsigned char* old_mem_buf = (unsigned char*)old_memory_ptr;

    if (old_mem_buf == NULL || old_size == 0) {
        return arena_allocate_aligned(arena_ptr, alignment, new_size);
    } else if (!(arena_ptr->buffer_ptr <= old_mem_buf && old_mem_buf < arena_ptr->buffer_ptr + arena_ptr->buffer_length)) {
        return NULL;
    }

    // optimization using previous_offset
    if (arena_ptr->buffer_ptr + arena_ptr->previous_offset == old_mem_buf) {
        arena_ptr->current_offset = arena_ptr->previous_offset + new_size;
        if (new_size > old_size) {
            memset(&arena_ptr->buffer_ptr[arena_ptr->current_offset], 0, new_size - old_size);
        }
        return old_memory_ptr;
    }

    const size_t copy_size = old_size < new_size ? old_size : new_size;

    void* new_memory = arena_allocate_aligned(arena_ptr, alignment, new_size);

    // Copy across old memory to the new memory
    memmove(new_memory, old_mem_buf, copy_size);

    return new_memory;
}

/**
 * @brief Reallocate a previously allocated chunk in the arena.
 *
 * @param[in] arena_ptr The arena pointer.
 * @param[in] old_memory_ptr A pointer, pointing to the beginning of the memory chunk.
 * @param[in] old_size Old size allocated.
 * @param[in] new_size New size to grow/shrink to.
 * @return A pointer to reallocated the memory chunk.
 *  @retval NULL If arena doesn't have enough memory for the reallocation.
 */
static inline void* arena_reallocate(arena_type* arena_ptr, void* old_memory_ptr, const size_t old_size, const size_t new_size) {
    return arena_reallocate_aligned(arena_ptr, old_memory_ptr, DEFAULT_ALIGNMENT, old_size, new_size);
}

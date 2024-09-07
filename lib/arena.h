/*  arena.h
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
 * @file arena.h
 * @brief Arena allocator
 *
 * For a comprehensive source, read:
 * @li https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

/**
 * @example examples/arena/fhashtable.c
 * Example of how `arena.h` header file is used in practice.
 */

/**
 * @example examples/arena/char_array.c
 * Example of how `arena.h` header file is used in practice.
 */

#pragma once

#include "align.h"   // align
#include "is_pow2.h" // is_pow2

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Arena data struct.
 */
struct arena {
    unsigned char *buf_ptr; ///< Underlying buffer pointer.
    size_t buf_len;         ///< Underlying buffer length.
    size_t prev_offset;     ///< Previous offset relative to buf_ptr.
    size_t curr_offset;     ///< Current offset relative to buf_ptr.
};

/**
 * @brief Initialize the arena.
 *
 * @param[in] self              arena pointer
 * @param[in] n                 backing buffer length.
 * @param[in] backing_buf       backing buffer
 */
static inline void arena_init(struct arena *self, const size_t n, unsigned char backing_buf[n])
{
    assert(self);
    assert(backing_buf);

    self->buf_ptr = &backing_buf[0];
    self->buf_len = n;
    self->curr_offset = 0;
    self->prev_offset = 0;
}

/**
 * @brief Deallocate all allocations in the arena.
 *
 * @param[in] self      arena pointer.
 */
static inline void arena_deallocate_all(struct arena *self)
{
    assert(self);

    self->curr_offset = 0;
    self->prev_offset = 0;
}

/**
 * @brief Get the pointer to a chunk of the arena. With specific alignment.
 *
 * @param[in] self              arena pointer.
 * @param[in] alignment         alignment size
 * @param[in] size              chunk size
 *
 * @return A pointer to the memory chunk.
 *      @retval NULL If the arena doesn't have enough memory for the allocation.
 */
static inline void *arena_allocate_aligned(struct arena *self, const size_t alignment, const size_t size)
{
    assert(self);

    void *ptr = (void *)&self->buf_ptr[self->curr_offset];

    size_t space_left = self->buf_len - (size_t)self->curr_offset;

    if (!align(alignment, size, &ptr, &space_left)) {
        return NULL;
    }

    const uintptr_t relative_offset = (unsigned char *)ptr - &self->buf_ptr[0];

    self->prev_offset = relative_offset;
    self->curr_offset = relative_offset + size;

    memset(ptr, 0, size);

    return ptr;
}

/**
 * @brief Get the pointer to a chunk of the arena.
 *
 * @param[in] self      The arena pointer.
 * @param[in] size      The section size in bytes.
 *
 * @return A pointer to the memory chunk.
 *      @retval NULL If the arena doesn't have enough memory for the allocation.
 */
static inline void *arena_allocate(struct arena *self, const size_t size)
{
    assert(self);

    return arena_allocate_aligned(self, alignof(max_align_t), size);
}

/// @cond DO_NOT_DOCUMENT
static inline void *internal_arena_optimize_w_prev_offset(struct arena *self, unsigned char *old_ptr,
                                                          const size_t old_size, const size_t new_size)
{
    if (&self->buf_ptr[self->prev_offset] != old_ptr) {
        return NULL;
    }

    self->curr_offset = self->prev_offset + new_size;

    if (new_size > old_size) {
        const size_t diff = new_size - old_size;

        memset(&self->buf_ptr[self->curr_offset], 0, diff);
    }

    return old_ptr;
}
/// @endcond

/**
 * @brief Reallocate a previously allocated chunk in the arena. With specific
 *        aligment.
 *
 * @param[in] self              arena pointer.
 * @param[in] old_ptr_          pointer to the buffer to reallocate
 * @param[in] alignment         alignment size.
 * @param[in] old_size          old size.
 * @param[in] new_size          new size to grow/shrink to.
 *
 * @return A pointer to reallocated the memory chunk.
 *      @retval NULL If arena doesn't have enough memory for the reallocation.
 */
static inline void *arena_reallocate_aligned(struct arena *self, void *old_ptr_, const size_t alignment,
                                             const size_t old_size, const size_t new_size)
{
    assert(self);
    assert(is_pow2(alignment));

    unsigned char *old_ptr = (unsigned char *)old_ptr_;

    if (old_ptr == NULL || old_size == 0) {
        return arena_allocate_aligned(self, alignment, new_size);
    }

    const bool inside_buf = self->buf_ptr <= old_ptr && old_ptr < self->buf_ptr + self->buf_len;

    if (!inside_buf) {
        return NULL;
    }

    if (internal_arena_optimize_w_prev_offset(self, old_ptr, old_size, new_size)) {
        return old_ptr;
    }

    const size_t copy_size = old_size < new_size ? old_size : new_size;

    void *new_mem = arena_allocate_aligned(self, alignment, new_size);

    memmove(new_mem, old_ptr, copy_size);

    return new_mem;
}

/**
 * @brief Reallocate a previously allocated chunk in the arena.
 *
 * @param[in] self              The arena pointer.
 * @param[in] old_ptr           Pointer to the buffer to reallocate
 * @param[in] old_size          Old size.
 * @param[in] new_size          New size to grow/shrink to.
 *
 * @return A pointer to reallocated the memory chunk.
 *      @retval NULL            If arena doesn't have enough memory for the reallocation.
 */
static inline void *arena_reallocate(struct arena *self, void *old_ptr, const size_t old_size, const size_t new_size)
{
    assert(self);

    return arena_reallocate_aligned(self, old_ptr, alignof(max_align_t), old_size, new_size);
}

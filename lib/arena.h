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

#pragma once

#include "align.h"   // align, calc_alignment_padding
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
    size_t buf_len;         ///< Underlying buffer length.
    size_t prev_offset;     ///< Previous offset relative to buf_ptr.
    size_t curr_offset;     ///< Current offset relative to buf_ptr.
    unsigned char *buf_ptr; ///< Underlying buffer pointer.
};

/**
 * @brief Tempory arena state struct.
 */
struct temp_arena_state {
    struct arena *arena_ptr; ///< arena pointer
    size_t prev_offset;      ///< arena prev offset
    size_t curr_offset;      ///< arena curr offset
};

/**
 * @brief Save the arena state temporarily.
 *
 * @param[in] arena_ptr The arena whose state to save.
 */
struct temp_arena_state temp_arena_state_save(struct arena *arena_ptr)
{
    struct temp_arena_state curr_state;
    curr_state.arena_ptr = arena_ptr;
    curr_state.prev_offset = arena_ptr->prev_offset;
    curr_state.curr_offset = arena_ptr->curr_offset;
    return curr_state;
}

/**
 * @brief Restore the arena state.
 *
 * @param[in] temp  Stored arena state.
 */
void temp_arena_state_restore(struct temp_arena_state prev_state)
{
    prev_state.arena_ptr->prev_offset = prev_state.prev_offset;
    prev_state.arena_ptr->curr_offset = prev_state.curr_offset;
}

/**
 * @brief Initialize the arena.
 *
 * @param[in] self              arena pointer
 * @param[in] len               backing buffer length.
 * @param[in] backing_buf       backing buffer
 */
static inline void arena_init(struct arena *self, const size_t len, unsigned char *backing_buf)
{
    assert(self);
    assert(backing_buf);

    const uintptr_t padding = calc_alignment_padding(alignof(max_align_t), (uintptr_t)backing_buf);

    assert(len >= padding);

    self->buf_ptr = &backing_buf[padding];
    self->buf_len = len - padding;
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

    const bool has_space_left = align(alignment, size, &ptr, &space_left);
    if (!has_space_left) {
        return NULL;
    }

    const uintptr_t relative_offset = (uintptr_t)((unsigned char *)ptr - &self->buf_ptr[0]);

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
static inline void *internal_arena_try_optimizing_w_prev_offset(struct arena *self, unsigned char *old_ptr,
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
 *      @retval NULL If arena doesn't have enough memory for the reallocation or invalid parameters are given.
 */
static inline void *arena_reallocate_aligned(struct arena *self, void *old_ptr_, const size_t alignment,
                                             const size_t old_size, const size_t new_size)
{
    assert(self);
    assert(is_pow2(alignment));

    unsigned char *old_ptr = (unsigned char *)old_ptr_;

    const bool misc_input = old_ptr == NULL || old_size == 0 || new_size == 0;
    const bool inside_arena_buf = &self->buf_ptr[0] <= old_ptr && old_ptr <= &self->buf_ptr[self->buf_len - 1];
    if (misc_input || !inside_arena_buf) {
        return NULL;
    }

    const bool has_optimized_w_prev_buf =
        internal_arena_try_optimizing_w_prev_offset(self, old_ptr, old_size, new_size);
    if (has_optimized_w_prev_buf) {
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
 *      @retval NULL If arena doesn't have enough memory for the reallocation or invalid parameters are given.
 */
static inline void *arena_reallocate(struct arena *self, void *old_ptr, const size_t old_size, const size_t new_size)
{
    assert(self);

    return arena_reallocate_aligned(self, old_ptr, alignof(max_align_t), old_size, new_size);
}

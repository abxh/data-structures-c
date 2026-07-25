// Copyright (c) 2026 abxh
// SPDX-License-Identifier: MIT

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "align.h" // align, calc_alignment_padding
#include "arena.h"

void arena_init(struct arena *self, const size_t len, unsigned char *backing_buf)
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

void arena_deallocate_all(struct arena *self)
{
    assert(self);

    self->curr_offset = 0;
    self->prev_offset = 0;
}

void *arena_allocate_aligned(struct arena *self, const size_t alignment, const size_t size)
{
    assert(self);

    void *ptr = (void *)&self->buf_ptr[self->curr_offset];

    size_t space_left = self->buf_len - (size_t)self->curr_offset;

    const void *has_space_left = align(alignment, size, &ptr, &space_left);
    if (!has_space_left) {
        return NULL;
    }

    const uintptr_t relative_offset = (uintptr_t)((unsigned char *)ptr - &self->buf_ptr[0]);

    self->prev_offset = relative_offset;
    self->curr_offset = relative_offset + size;

    memset(ptr, 0, size);

    return ptr;
}

void *arena_allocate(struct arena *self, const size_t size)
{
    assert(self);

    return arena_allocate_aligned(self, alignof(max_align_t), size);
}

/// @cond DO_NOT_DOCUMENT
static inline bool arena_try_optimizing_w_prev_offset(struct arena *self, unsigned char *old_ptr, const size_t old_size,
                                                      const size_t new_size)
{
    if (&self->buf_ptr[self->prev_offset] != old_ptr) {
        return false;
    }

    self->curr_offset = self->prev_offset + new_size;

    if (new_size > old_size) {
        const size_t diff = new_size - old_size;

        memset(&self->buf_ptr[self->curr_offset], 0, diff);
    }

    return true;
}
/// @endcond

void *arena_reallocate_aligned(struct arena *self, void *old_ptr_, const size_t alignment, const size_t old_size,
                               const size_t new_size)
{
    assert(self);
    assert(IS_POW2(alignment));

    unsigned char *old_ptr = (unsigned char *)old_ptr_;

    const bool misc_input = old_ptr == NULL || old_size == 0 || new_size == 0;
    const bool inside_arena_buf = &self->buf_ptr[0] <= old_ptr && old_ptr <= &self->buf_ptr[self->buf_len - 1];
    if (misc_input || !inside_arena_buf) {
        return NULL;
    }

    const bool has_optimized_w_prev_buf = arena_try_optimizing_w_prev_offset(self, old_ptr, old_size, new_size);
    if (has_optimized_w_prev_buf) {
        return old_ptr;
    }

    const size_t copy_size = old_size < new_size ? old_size : new_size;

    void *new_mem = arena_allocate_aligned(self, alignment, new_size);

    memmove(new_mem, old_ptr, copy_size);

    return new_mem;
}

void *arena_reallocate(struct arena *self, void *old_ptr, const size_t old_size, const size_t new_size)
{
    assert(self);

    return arena_reallocate_aligned(self, old_ptr, alignof(max_align_t), old_size, new_size);
}

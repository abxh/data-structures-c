// Copyright (c) 2026 abxh
// SPDX-License-Identifier: MIT

/**
 * @file arena.h
 * @brief Arena allocator
 *
 * For a comprehensive source, read:
 * @li https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

/**
 * @example arena_example.c
 * Example of how `arena.h` header file is used in practice.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

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
struct arena_state {
    struct arena *arena_ptr; ///< Arena pointer.
    size_t prev_offset;      ///< Arena prev offset.
    size_t curr_offset;      ///< Arena curr offset.
};

/**
 * @brief Save the arena state temporarily.
 *
 * @param[in] arena_ptr         The arena whose state to save.
 */
static inline struct arena_state arena_state_save(struct arena *arena_ptr)
{
    struct arena_state curr_state;
    curr_state.arena_ptr = arena_ptr;
    curr_state.prev_offset = arena_ptr->prev_offset;
    curr_state.curr_offset = arena_ptr->curr_offset;
    return curr_state;
}

/**
 * @brief Restore the arena state.
 *
 * @param[in] prev_state        Stored arena state.
 */
static inline void arena_state_restore(struct arena_state prev_state)
{
    prev_state.arena_ptr->prev_offset = prev_state.prev_offset;
    prev_state.arena_ptr->curr_offset = prev_state.curr_offset;
}

/**
 * @brief Initialize the arena.
 *
 * @param[in] self              Arena pointer.
 * @param[in] len               Backing buffer length.
 * @param[in] backing_buf       Backing buffer.
 */
void arena_init(struct arena *self, const size_t len, unsigned char *backing_buf);

/**
 * @brief Deallocate all allocations in the arena.
 *
 * @param[in] self              Arena pointer.
 */
void arena_deallocate_all(struct arena *self);

/**
 * @brief Get the pointer to a chunk of the arena. With specific alignment.
 *
 * @param[in] self              arena pointer.
 * @param[in] alignment         alignment size
 * @param[in] size              chunk size
 *
 * @return                      A pointer to a zeroed-out memory chunk.
 * @retval NULL                 If the arena doesn't have enough memory for the allocation.
 */
void *arena_allocate_aligned(struct arena *self, const size_t alignment, const size_t size);

/**
 * @brief Get the pointer to a chunk of the arena.
 *
 * @param[in] self              The arena pointer.
 * @param[in] size              The section size in bytes.
 *
 * @return                      A pointer to a zeroed-out memory chunk.
 * @retval NULL                 If the arena doesn't have enough memory for the allocation.
 */
void *arena_allocate(struct arena *self, const size_t size);

/**
 * @brief Reallocate a previously allocated chunk in the arena. With specific
 *        aligment.
 *
 * @param[in] self              Arena pointer.
 * @param[in] old_ptr_          Pointer to the buffer to reallocate
 * @param[in] alignment         Alignment size.
 * @param[in] old_size          Old size.
 * @param[in] new_size          New size to grow/shrink to.
 *
 * @return                      A pointer to the reallocated memory chunk.
 * @retval NULL                 If arena doesn't have enough memory for the reallocation or invalid parameters are
 *                              given.
 */
void *arena_reallocate_aligned(struct arena *self, void *old_ptr_, const size_t alignment, const size_t old_size,
                               const size_t new_size);

/**
 * @brief Reallocate a previously allocated chunk in the arena.
 *
 * @param[in] self              The arena pointer.
 * @param[in] old_ptr           Pointer to the buffer to reallocate
 * @param[in] old_size          Old size.
 * @param[in] new_size          New size to grow/shrink to.
 *
 * @return                      A pointer to the reallocated memory chunk.
 * @retval NULL                 If arena doesn't have enough memory for the reallocation or invalid parameters are
 *                              given.
 */
void *arena_reallocate(struct arena *self, void *old_ptr, const size_t old_size, const size_t new_size);

#ifdef __cplusplus
}
#endif

// vim: ft=c fdm=marker

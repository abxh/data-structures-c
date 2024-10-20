/*  pool.h
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
 * @file pool.h
 * @brief Pool allocator
 *
 * For a comprehensive source, read:
 * @li https://www.gingerbill.org/article/2019/02/16/memory-allocation-strategies-004/
 */

#include "align.h" // align, calc_alignment_padding

#include <stdalign.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Pool node struct
 */
struct pool_free_node {
    struct pool_free_node *next_ptr; ///< pointer to next node
};

/**
 * @brief Pool struct
 */
struct pool {
    size_t buf_len;                  ///< Underlying buffer length.
    size_t chunk_size;               ///< Chunk size
    unsigned char *buf_ptr;          ///< Underlying buffer
    struct pool_free_node *head_ptr; ///< Pointer to head node
};

/// @cond DO_NOT_DOCUMENT
static inline void internal_pool_push_free_node(struct pool *self, struct pool_free_node *node)
{
    node->next_ptr = self->head_ptr;
    self->head_ptr = node;
}
/// @endcond

/**
 * @brief Deallocate pool and restore all free nodes.
 *
 * @param[in] self      The pool pointer.
 */
static inline void pool_deallocate_all(struct pool *self)
{
    assert(self);

    const size_t chunk_count = self->buf_len / self->chunk_size;

    for (size_t i = 0; i < chunk_count; i++) {
        unsigned char *ptr = &self->buf_ptr[i * self->chunk_size];

        struct pool_free_node *node = (struct pool_free_node *)ptr;

        internal_pool_push_free_node(self, node);
    }
}

/**
 * @brief Initialize pool for given data size and alignment.
 *
 * @param[in] self              pool pointer
 * @param[in] len               backing buffer length
 * @param[in] backing_buf       backing buffer
 * @param[in] data_size         data size
 * @param[in] data_alignment    data alignment
 */
static inline void pool_init(struct pool *self, const size_t len, unsigned char *backing_buf, const size_t data_size,
                             const size_t data_alignment)
{
    assert(self);
    assert(backing_buf);

    const uintptr_t padding = calc_alignment_padding(data_alignment, (uintptr_t)backing_buf);

    size_t chunk_size = data_size >= sizeof(struct pool_free_node) ? data_size : sizeof(struct pool_free_node);

    chunk_size = chunk_size + calc_alignment_padding(data_alignment, chunk_size);

    assert(padding + chunk_size <= len);

    self->buf_ptr = &backing_buf[padding];
    self->buf_len = len - padding;
    self->chunk_size = chunk_size;
    self->head_ptr = NULL;

    pool_deallocate_all(self);
}

/**
 * @brief Get a pointer to a free block of previously given data size
 *
 * @param[in] self              Pool pointer
 *
 * @returns                     A pointer to a zeroed-out memory chunk of data size.
 */
static inline void *pool_allocate(struct pool *self)
{
    struct pool_free_node *node = self->head_ptr;

    if (node == NULL) {
        return NULL;
    }

    self->head_ptr = node->next_ptr;

    memset(node, 0, self->chunk_size);

    return node;
}

/**
 * @brief Free a memory chunk for further reuse.
 *
 * @param[in] self              Pool pointer
 * @param[in] ptr               Pointer to memory chunk
 */
static inline void pool_deallocate(struct pool *self, void *ptr)
{
    assert(self);
    assert(ptr);
    assert(&self->buf_ptr[0] <= (unsigned char *)ptr && (unsigned char *)ptr < &self->buf_ptr[self->buf_len]);

    struct pool_free_node *node = (struct pool_free_node *)ptr;

    internal_pool_push_free_node(self, node);
}

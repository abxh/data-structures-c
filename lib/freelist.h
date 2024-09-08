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
 * @file freelist.h
 * @brief Free list allocator (with underlying free tree)
 *
 * For a comprehensive source, read:
 * @li https://www.gingerbill.org/article/2021/11/30/memory-allocation-strategies-005/
 */

/**
 * @example examples/freelist/chars_test.c
 * Example of how `arena.h` header file is used in practice.
 */

#pragma once

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "align.h"   // align, calc_alignment_padding, CALC_ALIGNMENT_PADDING
#include "is_pow2.h" // is_pow2

/**
 * @brief Freelist header definition. This lies at the front of every block.
 */
struct freelist_header {
    size_t __prev_block_size_w_freed_bit;                                    ///< Variable containing both the
                                                                             ///< Previous block size and freed bit
    size_t curr_block_size;                                                  ///< Current block size
    char : CALC_ALIGNMENT_PADDING(alignof(max_align_t), 2 * sizeof(size_t)); ///< Padding for aligning user data
};

#define NAME                       freetree
#define KEY_TYPE                   struct freelist_header
#define KEY_IS_STRICTLY_LESS(a, b) ((a).curr_block_size < (b).curr_block_size)
#define KEY_MEMBER_IS_FIRST
#define ALLOW_DUPLICATES
#include "rbtree.h"

/**
 * @brief Freelist struct definition.
 */
struct freelist {
    unsigned char *buf_ptr;           ///< Underlying buffer
    size_t buf_len;                   ///< Buffer length
    size_t buf_used;                  ///< Number of bytes used of buffer.
    struct freelist_header *head;     ///< Header of freelist headers (all memory blocks)
    struct freetree_node *rb_rootptr; ///< Header of freetree (freed memory blocks)
};

/// @cond DO_NOT_DOCUMENT
static inline struct freetree_node *internal_freetree_search_best_block(struct freetree_node **rootptr_ptr,
                                                                        const size_t block_size);

static inline void internal_freelist_coalescence(struct freelist *self, struct freelist_header *header);
/// @endcond

/* Create a freelist header from given params */
static inline struct freelist_header freelist_header_from(const bool is_freed, const size_t prev_size,
                                                          const size_t curr_size)
{
    return (struct freelist_header){.__prev_block_size_w_freed_bit = prev_size + (is_freed ? 1 : 0),
                                    .curr_block_size = curr_size};
}

/* Get the prev size from header */
static inline size_t freelist_header_prev_size(const struct freelist_header *header)
{
    return (header->__prev_block_size_w_freed_bit & ~(size_t)1);
}

/* Check if the memory block under header is freed */
static inline bool freelist_header_is_freed(const struct freelist_header *header)
{
    return header->__prev_block_size_w_freed_bit & 1;
}

/* Get the pointer to previous header */
static inline struct freelist_header *freelist_header_prev(struct freelist_header *header)
{
    const size_t prev_size = freelist_header_prev_size(header);

    return prev_size == 0 ? NULL : (struct freelist_header *)((char *)header - prev_size);
}

/* Get the pointer to next header */
static inline struct freelist_header *freelist_header_next(struct freelist_header *header,
                                                           const struct freelist *freelist_ptr)
{
    const char *next = (char *)header + header->curr_block_size;
    const bool is_end = next >= (char *)freelist_ptr->buf_ptr + freelist_ptr->buf_len;

    return is_end ? NULL : (struct freelist_header *)next;
}

/* Check if the freelist header *should* be in the freetree */
static inline bool freelist_header_is_in_freetree(const struct freelist_header *header)
{
    return freelist_header_is_freed(header) && header->curr_block_size >= sizeof(struct freetree_node);
}

/* Deallocate everything contained in the freelist */
static inline void freelist_deallocate_all(struct freelist *self)
{
    assert(self);

    struct freetree_node *node = (struct freetree_node *)self->buf_ptr;

    freetree_node_init(node, freelist_header_from(true, 0, self->buf_len));

    self->buf_used = self->buf_len - node->key.curr_block_size;
    self->rb_rootptr = node;
    self->head = &node->key;
}

/* Initialize the freelist */
static inline void freelist_init(struct freelist *self, const size_t len, unsigned char backing_buf[len])
{
    assert(self);
    assert(backing_buf);

    const uintptr_t padding = calc_alignment_padding(alignof(max_align_t), (uintptr_t)&backing_buf[0]);

    assert(len - padding >= sizeof(struct freetree_node));

    self->buf_ptr = &backing_buf[padding];
    self->buf_len = len - padding;

    freelist_deallocate_all(self);
}

/* Get the pointer to a block of the freelist. With specific alignment.*/
static inline void *freelist_allocate_aligned(struct freelist *self, const size_t requested_size,
                                              const size_t requested_alignment)
{
    assert(self);
    assert(is_pow2(requested_alignment));
    assert(requested_size != 0);

    const size_t alignment = requested_alignment < alignof(max_align_t) ? alignof(max_align_t) : requested_alignment;
    size_t curr_block_size = sizeof(struct freelist_header) + requested_size;
    curr_block_size = curr_block_size < sizeof(struct freetree_node)
                        ? sizeof(struct freetree_node)
                        : curr_block_size + calc_alignment_padding(alignment, curr_block_size);

    struct freetree_node *node = internal_freetree_search_best_block(&self->rb_rootptr, curr_block_size);

    if (node == NULL) {
        return NULL;
    }

    freetree_delete_node(&self->rb_rootptr, node);

    const size_t remaining_size = node->key.curr_block_size - curr_block_size;
    if (remaining_size > 0) {
        struct freelist_header *remaining_header = (struct freelist_header *)((char *)node + curr_block_size);
        *remaining_header = freelist_header_from(true, curr_block_size, remaining_size);

        struct freelist_header *next = freelist_header_next(&node->key, self);
        if (next) {
            *next = freelist_header_from(freelist_header_is_freed(next), remaining_size, next->curr_block_size);
        }

        if (freelist_header_is_in_freetree(remaining_header)) {
            struct freetree_node *new_node = (struct freetree_node *)remaining_header;
            freetree_node_init(new_node, *remaining_header);
            freetree_insert_node(&self->rb_rootptr, new_node);
        }
    }
    struct freelist_header *curr_header = (struct freelist_header *)node;
    *curr_header = freelist_header_from(false, freelist_header_prev_size(&node->key), curr_block_size);

    return (void *)((char *)curr_header + sizeof(struct freelist_header));
}

/* Get the pointer to a block of the freelist. */
static inline void *freelist_allocate(struct freelist *self, const size_t size)
{
    return freelist_allocate_aligned(self, size, alignof(max_align_t));
}

/* Deallocate a block from the freelist for further use. */
static inline void freelist_deallocate(struct freelist *self, void *ptr)
{
    struct freelist_header *header = (struct freelist_header *)((char *)ptr - sizeof(struct freelist_header));

    assert(!freelist_header_is_freed(header) && "double free detected!");

    self->buf_used -= header->curr_block_size;

    internal_freelist_coalescence(self, header);
}

/// @cond DO_NOT_DOCUMENT
static inline struct freetree_node *internal_freetree_search_best_block(struct freetree_node **rootptr_ptr,
                                                                        const size_t block_size)
{
    assert(rootptr_ptr != NULL);

    struct freetree_node *prev_ptr = NULL;
    struct freetree_node *curr_ptr = *rootptr_ptr;

    while (curr_ptr != NULL) {
        if (block_size > curr_ptr->key.curr_block_size) {
            curr_ptr = curr_ptr->right_ptr;
        }
        else {
            prev_ptr = curr_ptr;
            curr_ptr = curr_ptr->left_ptr;
        }
    }
    return prev_ptr;
}

static inline void internal_freelist_coalescence(struct freelist *self, struct freelist_header *header)
{
    assert(self);
    assert(header);

    struct freelist_header *prev = freelist_header_prev(header);
    struct freelist_header *next = freelist_header_next(header, self);

    struct freelist_header header_new = *header;
    struct freelist_header *header_addr = header;
    struct freelist_header *header_next = next;

    if (prev && freelist_header_is_freed(prev)) {
        header_new.curr_block_size += prev->curr_block_size;
        header_new.__prev_block_size_w_freed_bit = prev->__prev_block_size_w_freed_bit & ~(uintptr_t)1;
        header_addr = prev;

        if (freelist_header_is_in_freetree(prev)) {
            freetree_delete_node(&self->rb_rootptr, (struct freetree_node *)prev);
        }
    }
    if (next && freelist_header_is_freed(next)) {
        header_new.curr_block_size += next->curr_block_size;
        header_next = freelist_header_next(next, self);

        if (freelist_header_is_in_freetree(next)) {
            freetree_delete_node(&self->rb_rootptr, (struct freetree_node *)next);
        }
    }
    const size_t diff = header_new.curr_block_size - header->curr_block_size;
    if (header_next) {
        *header_next =
            freelist_header_from(freelist_header_is_freed(header_next), freelist_header_prev_size(header_next) + diff,
                                 header_next->curr_block_size);
    }
    if (freelist_header_is_in_freetree(header)) {
        freetree_delete_node(&self->rb_rootptr, (struct freetree_node *)header);
    }
    header_new.__prev_block_size_w_freed_bit |= 1;
    if (freelist_header_is_in_freetree(&header_new)) {
        struct freetree_node *node_addr = (struct freetree_node *)header_addr;
        freetree_node_init(node_addr, header_new);
        freetree_insert_node(&self->rb_rootptr, node_addr);
    }
    else {
        *header_addr = header_new;
    }
}
/// @endcond

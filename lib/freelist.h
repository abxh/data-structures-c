#pragma once

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "is_pow2.h"

/**
 * @def DEFAULT_ALIGNMENT
 * @brief Default alignment when no alignment size is given.
 *
 * Can be defined before including the header.
 */
#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (alignof(max_align_t))
#endif

#define NAME                       internal_freelist
#define KEY_TYPE                   size_t
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#define ALLOW_DUPLICATES
#include "rbtree.h"

typedef struct {
    size_t block_size;
    size_t padding;
} internal_freelist_allocation_header_type;

typedef struct {
    unsigned char* buffer_ptr;
    size_t buffer_length;
    size_t used_length;
    internal_freelist_node_type* rb_rootptr;
} freelist_type;

/// @cond DO_NOT_DOCUMENT
static inline size_t internal_calculate_padding_with_header_size(const uintptr_t ptr, const uintptr_t alignment,
                                                                 const size_t header_size);
static inline bool internal_freelist_node_is_best(internal_freelist_node_type* node, const size_t required_space,
                                                  const size_t smallest_diff);
static inline internal_freelist_node_type* internal_freelist_find_best(freelist_type* freelist_ptr, const size_t alignment,
                                                                       const size_t size, size_t* padding_);
static inline void internal_freelist_merge_nodes(freelist_type* freelist_ptr, internal_freelist_node_type* a,
                                                 internal_freelist_node_type* b);
static inline void internal_freelist_coalescence(freelist_type* freelist_ptr, internal_freelist_node_type* free_node);
/// @endcond

static inline void freelist_deallocate_all(freelist_type* freelist_ptr)
{
    assert(freelist_ptr);

    freelist_ptr->used_length = 0;
    internal_freelist_node_type* first = (internal_freelist_node_type*)freelist_ptr->buffer_ptr;
    freelist_ptr->rb_rootptr = first;
    internal_freelist_node_init(first, freelist_ptr->buffer_length);
}

static inline void freelist_init(freelist_type* freelist_ptr, const size_t n, unsigned char backing_buffer[n])
{
    assert(freelist_ptr);
    assert(backing_buffer);

    freelist_ptr->buffer_ptr = backing_buffer;
    freelist_ptr->buffer_length = n;
    freelist_deallocate_all(freelist_ptr);
}

static inline void* freelist_allocate_aligned(freelist_type* freelist_ptr, const size_t size, const size_t alignment)
{
    assert(freelist_ptr);
    assert(is_pow2(alignment));

    const size_t size_ = size < sizeof(internal_freelist_node_type) ? sizeof(internal_freelist_node_type) : size;
    const size_t alignment_ = alignment < DEFAULT_ALIGNMENT ? DEFAULT_ALIGNMENT : alignment;

    size_t padding_with_header_size = 0;
    internal_freelist_node_type* node = internal_freelist_find_best(freelist_ptr, alignment_, size_, &padding_with_header_size);

    if (node == NULL) {
        return NULL;
    }

    const size_t alignment_padding = padding_with_header_size - sizeof(internal_freelist_allocation_header_type);
    const size_t required_space = padding_with_header_size + size_;
    const size_t block_size = node->key;
    const size_t remaining = block_size - required_space;

    if (remaining > 0) {
        internal_freelist_node_type* new_node = (internal_freelist_node_type*)((char*)node + required_space);
        internal_freelist_node_init(new_node, remaining);
        internal_freelist_insert_node(&freelist_ptr->rb_rootptr, new_node);
    }

    internal_freelist_delete_node(&freelist_ptr->rb_rootptr, node);

    internal_freelist_allocation_header_type* header_ptr;
    header_ptr = (internal_freelist_allocation_header_type*)((char*)node + alignment_padding);
    header_ptr->block_size = required_space;
    header_ptr->padding = alignment_padding;

    freelist_ptr->used_length += required_space;

    return (void*)((char*)header_ptr + sizeof(internal_freelist_allocation_header_type));
}

static inline void* freelist_allocate(freelist_type* freelist_ptr, const size_t size)
{
    return freelist_allocate_aligned(freelist_ptr, size, DEFAULT_ALIGNMENT);
}

static inline void freelist_deallocate(freelist_type* freelist_ptr, void* ptr)
{
    assert(freelist_ptr);
    assert(ptr);

    internal_freelist_allocation_header_type* header;
    header = (internal_freelist_allocation_header_type*)((char*)ptr - sizeof(internal_freelist_allocation_header_type));
    internal_freelist_node_type* free_node = (internal_freelist_node_type*)header;

    internal_freelist_node_init(free_node, header->padding + header->block_size);

    internal_freelist_insert_node(&freelist_ptr->rb_rootptr, free_node);

    freelist_ptr->used_length -= free_node->key;

    internal_freelist_coalescence(freelist_ptr, free_node);
}

/// @cond DO_NOT_DOCUMENT

static inline size_t internal_calculate_padding_with_header_size(const uintptr_t ptr, const uintptr_t alignment,
                                                                 const size_t header_size)
{
    assert(is_pow2(alignment));
    assert(header_size != 0);

    const uintptr_t p = ptr;
    const uintptr_t a = alignment;
    const uintptr_t r = p & (a - 1); // p % a

    /*
        for alignment 8:
            alignment_padding = ... , 0, 7, 6, 5, 4, 3, 2, 1, 0, ...
                                     /|\                     /|\
           (at alignment boundaries)  |                       |
    */
    const uintptr_t alignment_padding = ((r != 0) ? 1 : 0) * (a - r);

    if (alignment_padding >= (uintptr_t)header_size) {
        /*
            If alignment_padding >= header_size,
            then the section `(uintptr_t)alignment_padding - header_size` can be used.
        */
        return (size_t)alignment_padding;
    }
    else {
        const uintptr_t needed_space = (uintptr_t)header_size - alignment_padding;
        const bool header_is_aligned = (needed_space & (a - 1)) == 0;

        if (header_is_aligned) {
            return header_size;
        }
        else {
            return (size_t)(alignment_padding + a * (1 + (needed_space / a)));
        }
    }
}

static inline bool internal_freelist_node_is_best(internal_freelist_node_type* node, const size_t required_space,
                                                  const size_t smallest_diff)
{
    const size_t block_size = node->key;
    return block_size >= required_space && (block_size - required_space < smallest_diff);
}

static inline internal_freelist_node_type* internal_freelist_find_best(freelist_type* freelist_ptr, const size_t alignment,
                                                                       const size_t size, size_t* padding_)
{
    size_t smallest_diff = SIZE_MAX;

    internal_freelist_node_type* node = freelist_ptr->rb_rootptr;
    internal_freelist_node_type* best_node = NULL;

    while (node != NULL) {
        const size_t padding_with_header_size = internal_calculate_padding_with_header_size(
            (uintptr_t)node, (uintptr_t)alignment, sizeof(internal_freelist_allocation_header_type));
        const size_t required_space = padding_with_header_size + size;

        if (internal_freelist_node_is_best(node, required_space, smallest_diff)) {
            best_node = node;
            const size_t block_size = node->key;
            smallest_diff = block_size - required_space;
        }
        if (node->left_ptr != NULL) {
            node = node->left_ptr;
        }
        else {
            break;
        }
    }

    if (padding_) {
        *padding_ = (!best_node) ? 0
                                 : internal_calculate_padding_with_header_size((uintptr_t)best_node, (uintptr_t)alignment,
                                                                               sizeof(internal_freelist_allocation_header_type));
    }

    return best_node;
}

static inline void internal_freelist_merge_nodes(freelist_type* freelist_ptr, internal_freelist_node_type* a,
                                                 internal_freelist_node_type* b)
{
    const size_t new_block_size = a->key + b->key;

    internal_freelist_delete_node(&freelist_ptr->rb_rootptr, b);
    internal_freelist_delete_node(&freelist_ptr->rb_rootptr, a);

    internal_freelist_node_init(a, new_block_size);

    internal_freelist_insert_node(&freelist_ptr->rb_rootptr, a);

    // note:
    // possible optimization with some specialized rbtree operation to increase key?
}

static inline void* internal_freelist_get_next_addr(const internal_freelist_node_type* n)
{
    return (void*)((char*)n + n->key);
}

static inline void internal_freelist_coalescence(freelist_type* freelist_ptr, internal_freelist_node_type* node_ptr)
{
    internal_freelist_node_type* const parent_ptr = internal_freelist_node_get_parent_ptr(node_ptr);

    if (node_ptr->left_ptr != NULL && internal_freelist_get_next_addr(node_ptr) == node_ptr->left_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, node_ptr, node_ptr->left_ptr);
    }
    else if (node_ptr->right_ptr != NULL && internal_freelist_get_next_addr(node_ptr) == node_ptr->right_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, node_ptr, node_ptr->right_ptr);
    }
    else if (parent_ptr != NULL && internal_freelist_get_next_addr(node_ptr) == parent_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, parent_ptr, node_ptr);
    }

    if (node_ptr->left_ptr != NULL && internal_freelist_get_next_addr(node_ptr->left_ptr) == node_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, node_ptr, node_ptr->left_ptr);
    }
    else if (node_ptr->right_ptr != NULL && internal_freelist_get_next_addr(node_ptr->right_ptr) == node_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, node_ptr, node_ptr->right_ptr);
    }
    else if (parent_ptr != NULL && internal_freelist_get_next_addr(parent_ptr) == node_ptr) {
        internal_freelist_merge_nodes(freelist_ptr, parent_ptr, node_ptr);
    }
}
/// @endcond

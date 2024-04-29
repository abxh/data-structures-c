/*
 * @file bitarray.h
 * @brief bitarray
 * @todo Fill the rest of the documentation
 *
 * This is a implementation of a bitarray using a fixed length array of one byte long bytes.
 * The operations treat the bytes as one contigious whole and get and set bits at a given index.
 */

#pragma once

#include <stdbool.h> // bool
#include <stdint.h>  // uint8_t
#include <stdlib.h>  // size_t

#include "allocator_ops.h" // allocator_ops_type

typedef struct bitarray_type {
    size_t num_of_bytes;
    void* allocator_context_ptr;
    allocator_ops_type allocator_ops;

    uint8_t bytes[];
} bitarray_type;

bitarray_type* bitarray_create(size_t num_of_bytes);

bitarray_type* bitarray_create_with_min_num_of_bits(size_t num_of_bits);

bitarray_type* bitarray_create_with_specified(size_t num_of_bytes, void* allocator_context_ptr, allocator_ops_type allocator_ops);

bitarray_type* bitarray_create_from(const uint8_t* bytes, size_t num_of_bytes);

void bitarray_destroy(bitarray_type* bitarray_p);

bitarray_type* bitarray_clone(const bitarray_type* bitarray_src_p);

void bitarray_print(const bitarray_type* bitarray_p);

bool bitarray_at(const bitarray_type* bitarray_p, size_t index);

void bitarray_toggle_at(bitarray_type* bitarray_p, size_t index);

void bitarray_set_true_at(bitarray_type* bitarray_p, size_t index);

void bitarray_set_false_at(bitarray_type* bitarray_p, size_t index);

void bitarray_set_at(bitarray_type* bitarray_p, size_t index, bool bit);

void bitarray_rotate(bitarray_type* bitarray_p, int shift);

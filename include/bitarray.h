/*
    This is a implementation of a bitarray using a fixed length array of one byte long words.
    The operations treat the words as one contigious whole and get and set bits at a given index.

    Big endian order is used. This means that we start from the most significant bit for the
    operations. For example, for the bits `1000`, we start from `1`.

    Little endian order is assumed to be on the actual computer. This might require some changes,
    if used on a cpu using big endian order.
*/

#pragma once

#include "allocators/allocator_function_types.h" // allocate_f, deallocate_f
#include <stdbool.h>                             // bool
#include <stdlib.h>                              // size_t

typedef struct bitarray_type bitarray_type;

bitarray_type* bitarray_create(size_t num_of_bits);

void bitarray_destroy(bitarray_type* bitarray_p);

bitarray_type* bitarray_copy(const bitarray_type* bitarray_src_p);

void bitarray_print(const bitarray_type* bitarray_p);

bitarray_type* bitarray_create_from(const unsigned char* bytes, size_t num_of_bits);

bool bitarray_is_equal(const bitarray_type* bitarray_p, const bitarray_type* bitarray_other_p);

size_t bitarray_get_num_of_bits(const bitarray_type* bitarray_p);

size_t bitarray_get_num_of_words(const bitarray_type* bitarray_p);

bool bitarray_get(const bitarray_type* bitarray_p, size_t index);

void bitarray_set_true(bitarray_type* bitarray_p, size_t index);

void bitarray_set_false(bitarray_type* bitarray_p, size_t index);

void bitarray_set(bitarray_type* bitarray_p, size_t index, bool bit);

void bitarray_toggle(bitarray_type* bitarray_p, size_t index);

bitarray_type* bitarray_init(size_t num_of_bits, void* allocator_struct_p, allocate_f allocate_f_p, deallocate_f deallocate_f_p);

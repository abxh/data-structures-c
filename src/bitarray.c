// Sources used:
// - https://stackoverflow.com/questions/47981/how-to-set-clear-and-toggle-a-single-bit
// - https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function

#include <assert.h>   // assert
#include <stdalign.h> // alignof
#include <stddef.h>   // offsetoff
#include <stdint.h>   // uint8_t
#include <stdio.h>    // printf, putchar
#include <stdlib.h>   // size_t, NULL, SIZE_MAX
#include <string.h>   // memcpy

#include "bitarray.h"
#include "std_allocator.h" // std_allocator_ops

static inline size_t byte_index(size_t index) {
    return index >> 3;
}

static inline size_t bit_index(size_t index) {
    return ~index & 7;
}

static inline size_t round_up_to_next_mul8(size_t x) {
    return (x + (8 - 1)) / 8;
}

static inline size_t num_of_bits(size_t num_of_bytes) {
    return 8 * num_of_bytes;
}

bitarray_type* bitarray_create_with_specified(size_t num_of_bytes, void* allocator_context_ptr, allocator_ops_type allocator_ops) {
    if (num_of_bytes == 0 || num_of_bytes > SIZE_MAX / sizeof(uint8_t)) {
        return NULL;
    }
    bitarray_type* bitarray_ptr =
        allocator_ops.allocate_f_ptr(allocator_context_ptr, alignof(bitarray_type), offsetof(bitarray_type, bytes) + num_of_bytes);
    if (!bitarray_ptr) {
        return NULL;
    }
    memset(&bitarray_ptr->bytes[0], false, num_of_bytes);
    bitarray_ptr->num_of_bytes = num_of_bytes;
    bitarray_ptr->allocator_context_ptr = allocator_context_ptr;
    bitarray_ptr->allocator_ops = allocator_ops;
    return bitarray_ptr;
}

bitarray_type* bitarray_create(size_t num_of_bytes) {
    return bitarray_create_with_specified(num_of_bytes, NULL, std_allocator_ops);
}

bitarray_type* bitarray_create_with_min_num_of_bits(size_t num_of_bits) {
    return bitarray_create(round_up_to_next_mul8(num_of_bits));
}

void bitarray_destroy(bitarray_type* bitarray_ptr) {
    assert(bitarray_ptr != NULL);

    void* allocator_context_ptr_copy = bitarray_ptr->allocator_context_ptr;
    allocator_ops_type allocator_ops_copy = bitarray_ptr->allocator_ops;

    allocator_ops_copy.deallocate_f_ptr(allocator_context_ptr_copy, bitarray_ptr);
}

void bitarray_print(const bitarray_type* bitarray_ptr) {
    assert(bitarray_ptr != NULL);

    size_t i;
    for (i = 0; i < 8 * bitarray_ptr->num_of_bytes; i++) {
        if (i % 8 == 0 && i % 32 != 0) {
            putchar(' ');
        }
        printf("%d", (bitarray_ptr->bytes[byte_index(i)] >> bit_index(i)) & 1);
        if ((i + 1) % 32 == 0) {
            putchar('\n');
        }
    }
    if ((i - 1) % 32 != 0) {
        putchar('\n');
    }
}

bitarray_type* bitarray_create_from(const uint8_t* bytes, size_t num_of_bytes) {
    bitarray_type* bitarray_ptr = bitarray_create(num_of_bytes);
    if (bitarray_ptr == NULL) {
        return false;
    }
    memcpy(bitarray_ptr->bytes, bytes, num_of_bytes);
    return bitarray_ptr;
}

bitarray_type* bitarray_clone(const bitarray_type* bitarray_ptr) {
    assert(bitarray_ptr != NULL);
    bitarray_type* bitarray_copy_ptr = bitarray_create(bitarray_ptr->num_of_bytes);
    if (bitarray_copy_ptr == NULL) {
        return false;
    }
    memcpy(bitarray_copy_ptr->bytes, bitarray_ptr->bytes, bitarray_copy_ptr->num_of_bytes);
    return bitarray_copy_ptr;
}

bool bitarray_at(const bitarray_type* bitarray_ptr, size_t index) {
    assert(bitarray_ptr != NULL);
    assert(index < num_of_bits(bitarray_ptr->num_of_bytes));

    size_t n = byte_index(index);
    size_t m = bit_index(index);

    return (bitarray_ptr->bytes[n] >> m) & 1;
}

void bitarray_set_true_at(bitarray_type* bitarray_ptr, size_t index) {
    assert(bitarray_ptr != NULL);
    assert(index < num_of_bits(bitarray_ptr->num_of_bytes));

    size_t n = byte_index(index);
    size_t m = bit_index(index);

    bitarray_ptr->bytes[n] |= 1 << m;
}

void bitarray_set_false_at(bitarray_type* bitarray_ptr, size_t index) {
    assert(bitarray_ptr != NULL);
    assert(index < num_of_bits(bitarray_ptr->num_of_bytes));

    size_t n = byte_index(index);
    size_t m = bit_index(index);

    bitarray_ptr->bytes[n] &= ~(1 << m);
}

void bitarray_set_at(bitarray_type* bitarray_ptr, size_t index, bool value) {
    assert(bitarray_ptr != NULL);
    assert(index < num_of_bits(bitarray_ptr->num_of_bytes));

    size_t n = byte_index(index);
    size_t m = bit_index(index);

    bitarray_ptr->bytes[n] &= ~(1 << m);
    bitarray_ptr->bytes[n] |= (value << m);
}

void bitarray_toggle_at(bitarray_type* bitarray_ptr, size_t index) {
    assert(bitarray_ptr != NULL);
    assert(index < num_of_bits(bitarray_ptr->num_of_bytes));

    size_t n = byte_index(index);
    size_t m = bit_index(index);

    bitarray_ptr->bytes[n] ^= 1 << m;
}

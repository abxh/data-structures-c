#include <assert.h>   // assert, static_assert
#include <limits.h>   // CHAR_BIT
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool, true, false
#include <stdint.h>   // SIZE_MAX
#include <stdio.h>    // putchar, printf
#include <stdlib.h>   // size_t, NULL
#include <string.h>   // memcmp, memcpy

#include "allocators/allocator_function_types.h" // allocate_f, deallocate_f
#include "allocators/std_allocator.h"            // std_allocate, std_deallocate
#include "bitarray.h"                            // bitarray*

static_assert(CHAR_BIT == 8, "a char is 8 bits.");

typedef struct bitarray_type {
    size_t num_of_bits;
    size_t num_of_words;
    unsigned char* words;

    void* allocator_struct_p;
    allocate_f allocate_f_p;
    deallocate_f deallocate_f_p;
} bitarray_type;

size_t word_index(size_t index) {
    return index >> 3;
}

size_t bit_index(size_t index) {
    return ~index & 7;
}

bool bitarray_init(bitarray_type** bitarray_pp, size_t num_of_bits, void* allocator_struct_p, allocate_f allocate_f_p,
                   deallocate_f deallocate_f_p) {
    assert(num_of_bits != 0);
    assert(bitarray_pp != NULL);

    *bitarray_pp = NULL;

    size_t num_of_words = (num_of_bits + (8 - 1)) / 8; // round up to the next multiple of 8
    if (num_of_words > SIZE_MAX / sizeof(unsigned char)) {
        return false;
    }

    *bitarray_pp = allocate_f_p(allocator_struct_p, alignof(bitarray_type), sizeof(bitarray_type));
    if (bitarray_pp == NULL) {
        return false;
    }
    (*bitarray_pp)->words = allocate_f_p(allocator_struct_p, alignof(unsigned char), num_of_words * sizeof(unsigned char));
    if ((*bitarray_pp)->words == NULL) {
        free(*bitarray_pp);
        *bitarray_pp = NULL;
        return false;
    }
    memset((*bitarray_pp)->words, 0, num_of_words);
    (*bitarray_pp)->num_of_bits = num_of_bits;
    (*bitarray_pp)->num_of_words = num_of_words;
    (*bitarray_pp)->allocator_struct_p = allocator_struct_p;
    (*bitarray_pp)->allocate_f_p = allocate_f_p;
    (*bitarray_pp)->deallocate_f_p = deallocate_f_p;

    return true;
}

bitarray_type* bitarray_create(size_t num_of_bits) {
    bitarray_type* bitarray_p = NULL;
    bitarray_init(&bitarray_p, num_of_bits, NULL, std_allocate, std_deallocate);
    return bitarray_p;
}

void bitarray_destroy(bitarray_type* bitarray_p) {
    assert(bitarray_p != NULL);

    void* allocator_struct_p = bitarray_p->allocator_struct_p;
    deallocate_f deallocate_f_p = bitarray_p->deallocate_f_p;

    deallocate_f_p(allocator_struct_p, bitarray_p->words);
    deallocate_f_p(allocator_struct_p, bitarray_p);
}

void bitarray_print(const bitarray_type* bitarray_p) {
    assert(bitarray_p != NULL);

    size_t i;
    for (i = 0; i < bitarray_p->num_of_bits; i++) {
        if (i % 8 == 0 && i % 32 != 0) {
            putchar(' ');
        }
        printf("%d", (bitarray_p->words[word_index(i)] >> bit_index(i)) & 1);
        if ((i + 1) % 32 == 0) {
            putchar('\n');
        }
    }
    if ((i - 1) % 32 != 0) {
        putchar('\n');
    }
}

bitarray_type* bitarray_create_from(const unsigned char* bytes, size_t num_of_bits) {
    assert(bytes != NULL);
    assert(num_of_bits != 0);

    bitarray_type* bitarray_p = bitarray_create(num_of_bits);
    if (bitarray_p == NULL) {
        return false;
    }
    memcpy(bitarray_p->words, bytes, bitarray_p->num_of_words);

    return bitarray_p;
}

bitarray_type* bitarray_clone(const bitarray_type* bitarray_src_p) {
    assert(bitarray_src_p != NULL);

    bitarray_type* bitarray_copy_p = NULL;
    if (!bitarray_init(&bitarray_copy_p, bitarray_src_p->num_of_bits, bitarray_src_p->allocator_struct_p, bitarray_src_p->allocate_f_p,
                       bitarray_src_p->deallocate_f_p)) {
        return NULL;
    }
    memcpy(bitarray_copy_p->words, bitarray_src_p->words, bitarray_src_p->num_of_words);

    return bitarray_copy_p;
}

bool bitarray_is_equal(const bitarray_type* bitarray_p, const bitarray_type* bitarray_other_p) {
    assert(bitarray_p != NULL);
    assert(bitarray_other_p != NULL);
    assert(bitarray_p->num_of_bits == bitarray_other_p->num_of_bits);

    return memcmp(bitarray_p->words, bitarray_other_p->words, bitarray_p->num_of_words) == 0;
}

size_t bitarray_get_num_of_bits(const bitarray_type* bitarray_p) {
    return bitarray_p->num_of_bits;
}

size_t bitarray_get_num_of_words(const bitarray_type* bitarray_p) {
    return bitarray_p->num_of_words;
}

bool bitarray_get(const bitarray_type* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    return (bitarray_p->words[n] >> m) & 1;
}

void bitarray_set_true(bitarray_type* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] |= 1 << m;
}

void bitarray_set_false(bitarray_type* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] &= ~(1 << m);
}

void bitarray_set(bitarray_type* bitarray_p, size_t index, bool value) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= (value << m);
}

void bitarray_toggle(bitarray_type* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] ^= 1 << m;
}

#include <assert.h>  // assert, static_assert
#include <limits.h>  // CHAR_BIT
#include <stdbool.h> // bool
#include <stdio.h>   // putchar, printf
#include <stdlib.h>  // malloc, calloc, free, size_t, NULL
#include <string.h>  // memcmp, memcpy

#include "other/bitarray.h" // bitarray*

static_assert(CHAR_BIT == 8, "a char is 8 bits.");

size_t word_index(size_t index) {
    return index >> 3;
}

size_t bit_index(size_t index) {
    return ~index & 7;
}

bool bitarray_init(bitarray_type** bitarray_pp, size_t num_of_bits) {
    assert(bitarray_pp != NULL);
    assert(num_of_bits != 0);

    *bitarray_pp = malloc(sizeof(bitarray_type));
    if (*bitarray_pp == NULL) {
        return false;
    }
    (*bitarray_pp)->words = calloc((*bitarray_pp)->num_of_words, sizeof(unsigned char));
    if ((*bitarray_pp)->words == NULL) {
        free(*bitarray_pp);
        *bitarray_pp = NULL;
        return false;
    }
    (*bitarray_pp)->num_of_bits = num_of_bits;
    (*bitarray_pp)->num_of_words = (num_of_bits + (8 - 1)) / 8; // round up to the next multiple of 8

    return true;
}

bool bitarray_deinit(bitarray_type** bitarray_pp) {
    assert(bitarray_pp != NULL);

    if (*bitarray_pp == NULL) {
        return false;
    }
    free((*bitarray_pp)->words);
    free(*bitarray_pp);
    *bitarray_pp = NULL;

    return true;
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

    bitarray_type* bitarray_p = NULL;
    if (!bitarray_init(&bitarray_p, num_of_bits)) {
        return false;
    }
    memcpy(bitarray_p->words, bytes, bitarray_p->num_of_words);

    return bitarray_p;
}

bool bitarray_copy(bitarray_type** bitarray_dest_pp, const bitarray_type* bitarray_src_p) {
    assert(bitarray_src_p != NULL);
    assert(bitarray_dest_pp != NULL);

    bitarray_type* bitarray_copy_p = NULL;
    if (!bitarray_init(&bitarray_copy_p, bitarray_src_p->num_of_bits)) {
        return false;
    }
    memcpy((*bitarray_dest_pp)->words, bitarray_src_p->words, bitarray_src_p->num_of_words);

    return true;
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

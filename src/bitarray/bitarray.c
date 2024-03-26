#include <assert.h>  // assert, static_assert
#include <limits.h>  // CHAR_BIT
#include <stdbool.h> // bool
#include <stdio.h>   // putchar, printf
#include <stdlib.h>  // malloc, calloc, size_t, NULL
#include <string.h>  // memcmp, memcpy

#include "bitarray.h"

static_assert(CHAR_BIT == 8, "a char is 8 bits.");

#define word_index(index) ((index) >> 3)
#define bit_index(index) (~(index) & 7)

bool bitarray_init(Bitarray** bitarray_pp, size_t num_of_bits) {
    assert(bitarray_pp != NULL);
    assert(num_of_bits != 0);

    *bitarray_pp = malloc(sizeof(Bitarray));
    if (*bitarray_pp == NULL) {
        return false;
    }
    (*bitarray_pp)->words = calloc((*bitarray_pp)->capacity, sizeof(unsigned char));
    if ((*bitarray_pp)->words == NULL) {
        free(*bitarray_pp);
        *bitarray_pp = NULL;
        return false;
    }
    (*bitarray_pp)->num_of_bits = num_of_bits;
    (*bitarray_pp)->capacity = (num_of_bits + (CHAR_BIT - 1)) / CHAR_BIT; // round up to the next multiple of CHAR_BIT 
    
    return true;
}

bool bitarray_deinit(Bitarray** bitarray_pp) {
    assert(bitarray_pp != NULL);

    if (*bitarray_pp == NULL) {
        return false;
    }
    free((*bitarray_pp)->words);
    free(*bitarray_pp);
    *bitarray_pp = NULL;

    return true;
}

void bitarray_print(const Bitarray* bitarray_p) {
    assert(bitarray_p != NULL);

    size_t i;
    for (i = 0; i < bitarray_p->num_of_bits; i++) {
        if (i % (CHAR_BIT * 4) != 0 && i % CHAR_BIT == 0) {
            putchar(' ');
        }
        printf("%d", (bitarray_p->words[word_index(i)] >> bit_index(i)) & 1);
        if ((i + 1) % (CHAR_BIT * 4) == 0) {
            putchar('\n');
        }
    }
    if ((i - 1) % (CHAR_BIT * 4) != 0) {
        putchar('\n');
    }
}

Bitarray* bitarray_from(const unsigned char* bytes, size_t num_of_bits) {
    assert(bytes != NULL);
    assert(num_of_bits != 0);

    Bitarray* bitarray_p = malloc(sizeof(Bitarray));
    if (bitarray_p == NULL) {
        return NULL;
    }
    bitarray_p->num_of_bits = num_of_bits;
    bitarray_p->capacity = (num_of_bits + (CHAR_BIT - 1)) / CHAR_BIT; // round up to the next multiple of CHAR_BIT
    bitarray_p->words = malloc(bitarray_p->capacity);
    if (bitarray_p->words == NULL) {
        free(bitarray_p);
        return NULL;
    }
    memcpy(bitarray_p->words, bytes, bitarray_p->capacity);

    return bitarray_p;
}

Bitarray* bitarray_copy(const Bitarray* bitarray_p) {
    assert(bitarray_p != NULL);

    Bitarray* bitarray_clone_p = malloc(sizeof(Bitarray));
    if (bitarray_clone_p == NULL) {
        return NULL;
    }
    bitarray_clone_p->words = malloc(bitarray_p->capacity);
    if (bitarray_clone_p->words == NULL) {
        free(bitarray_clone_p);
        return NULL;
    }
    bitarray_clone_p->num_of_bits = bitarray_p->num_of_bits;
    bitarray_clone_p->capacity = bitarray_p->capacity;

    return bitarray_clone_p;
}

bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p) {
    assert(bitarray_p != NULL);
    assert(bitarray_other_p != NULL);
    assert(bitarray_p->num_of_bits == bitarray_other_p->num_of_bits);

    return memcmp((unsigned char*)bitarray_p->words, (unsigned char*)bitarray_other_p->words, bitarray_p->capacity) == 0;
}

bool bitarray_get(const Bitarray* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    return (bitarray_p->words[n] >> m) & 1;
}

void bitarray_set_true(Bitarray* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] |= 1 << m;
}

void bitarray_set_false(Bitarray* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] &= ~(1 << m);
}

void bitarray_set(Bitarray* bitarray_p, size_t index, bool value) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= (value << m);
}

void bitarray_toggle(Bitarray* bitarray_p, size_t index) {
    assert(bitarray_p != NULL);
    assert(index < bitarray_p->num_of_bits);

    size_t n = word_index(index);
    size_t m = bit_index(index);

    bitarray_p->words[n] ^= 1 << m;
}

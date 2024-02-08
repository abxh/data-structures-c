#include <assert.h>  // assert.
#include <limits.h>  // CHAR_BIT
#include <stdbool.h> // bool
#include <stdio.h>   // putchar, printf
#include <stdlib.h>  // malloc, calloc, size_t, NULL
#include <string.h>  // memcmp, memcpy

#include "bitarray.h"

static inline size_t BITARRAY_WORD_INDEX(size_t index) {
    return (CHAR_BIT == 8) ? index >> 3 : index / CHAR_BIT;
}

static inline size_t BITARRAY_BIT_INDEX(size_t index) {
    return (CHAR_BIT == 8) ? ~index & (CHAR_BIT - 1) : (CHAR_BIT - 1) - (index % CHAR_BIT);
}

Bitarray* bitarray_new(size_t num_of_bits) {
    assert(num_of_bits != 0);
    Bitarray* bitarray_p = malloc(sizeof(Bitarray));
    if (bitarray_p == NULL) {
        return NULL;
    }
    bitarray_p->num_of_bits = num_of_bits;
    bitarray_p->capacity = (num_of_bits + (CHAR_BIT - 1)) / CHAR_BIT; // round up to the next multiple of CHAR_BIT
    bitarray_p->words = calloc(bitarray_p->capacity, sizeof(unsigned char));
    if (bitarray_p->words == NULL) {
        free(bitarray_p);
        return NULL;
    }
    return bitarray_p;
}

Bitarray* bitarray_from(void* bytes, size_t num_of_bits) {
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
    memcpy(bitarray_p->words, bytes, num_of_bits);
    return bitarray_p;
}

Bitarray* bitarray_copy(const Bitarray* bitarray_p) {
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

void bitarray_print(const Bitarray* bitarray_p) {
    unsigned char* ptr = bitarray_p->words;
    size_t i;
    for (i = 0; i < bitarray_p->num_of_bits; i++) {
        if (i % (CHAR_BIT * 4) == 0) {
            printf("%p:", ptr);
            ptr += 4;
        }
        if (i % CHAR_BIT == 0) {
            putchar(' ');
        }
        printf("%d", (bitarray_p->words[BITARRAY_WORD_INDEX(i)] >> BITARRAY_BIT_INDEX(i)) & 1);
        if ((i + 1) % (CHAR_BIT * 4) == 0) {
            putchar('\n');
        }
    }
    if ((i - 1) % (CHAR_BIT * 4) != 0) {
        putchar('\n');
    }
}

void bitarray_free(Bitarray* bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p) {
    assert(bitarray_p->num_of_bits != bitarray_other_p->num_of_bits);
    return memcmp((unsigned char*)bitarray_p->words, (unsigned char*)bitarray_other_p->words, bitarray_p->capacity) == 0;
}

bool bitarray_get(const Bitarray* bitarray_p, size_t index) {
    assert(index < bitarray_p->num_of_bits);
    size_t n = BITARRAY_WORD_INDEX(index);
    size_t m = BITARRAY_BIT_INDEX(index);
    return (bitarray_p->words[n] >> m) & 1;
}

void bitarray_set_true(Bitarray* bitarray_p, size_t index) {
    assert(index < bitarray_p->num_of_bits);
    size_t n = BITARRAY_WORD_INDEX(index);
    size_t m = BITARRAY_BIT_INDEX(index);
    bitarray_p->words[n] |= 1 << m;
}

void bitarray_set_false(Bitarray* bitarray_p, size_t index) {
    assert(index < bitarray_p->num_of_bits);
    size_t n = BITARRAY_WORD_INDEX(index);
    size_t m = BITARRAY_BIT_INDEX(index);
    bitarray_p->words[n] &= ~(1 << m);
}

void bitarray_set(Bitarray* bitarray_p, size_t index, bool value) {
    assert(index < bitarray_p->num_of_bits);
    size_t n = BITARRAY_WORD_INDEX(index);
    size_t m = BITARRAY_BIT_INDEX(index);
    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= (value << m);
}

void bitarray_toggle(Bitarray* bitarray_p, size_t index) {
    assert(index < bitarray_p->num_of_bits);
    size_t n = BITARRAY_WORD_INDEX(index);
    size_t m = BITARRAY_BIT_INDEX(index);
    bitarray_p->words[n] ^= 1 << m;
}

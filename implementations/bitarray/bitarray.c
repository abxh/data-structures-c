#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitarray.h"

#define bitarray_word_index(index) (index >> 3)
#define bitarray_bit_index(index) (~index & 7)

uint64_t nextpow2(uint32_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

Bitarray* bitarray_new(size_t num_of_bits) {
    if (num_of_bits == 0) {
        return NULL;
    }
    Bitarray* bitarr = malloc(sizeof(Bitarray));
    if (bitarr == NULL) {
        return NULL;
    }
    bitarr->num_of_bits = num_of_bits;
    bitarr->capacity = nextpow2(num_of_bits);
    bitarr->words = calloc(bitarr->capacity, sizeof(uint8_t));
    if (bitarr->words == NULL) {
        free(bitarr);
        return NULL;
    }
    return bitarr;
}

Bitarray* bitarray_copy(const Bitarray* bitarray_p) {
    Bitarray* bitarray_clone_p = malloc(sizeof(Bitarray));
    bitarray_clone_p->words = (uint8_t*)strndup((char*)bitarray_p->words, bitarray_p->capacity);
    bitarray_clone_p->capacity = bitarray_p->capacity;
    return bitarray_clone_p;
}

void bitarray_print(const Bitarray* bitarray_p) {
    uint8_t* ptr = bitarray_p->words;
    size_t i;
    for (i = 0; i < bitarray_p->num_of_bits; i++) {
        if (i % 32 == 0) {
            printf("%p:", ptr);
            ptr += 4;
        }
        if (i % 8 == 0) {
            putchar(' ');
        }
        printf("%d", (bitarray_p->words[bitarray_word_index(i)] >> bitarray_bit_index(i)) & 1);
        if ((i + 1) % 32 == 0) {
            putchar('\n');
        }
    }
    if ((i - 1) % 32 != 0) {
        putchar('\n');
    }
}

void bitarray_free(Bitarray* bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p) {
    if (bitarray_p->num_of_bits != bitarray_other_p->num_of_bits) {
        return false;
    }
    return memcmp((char*)bitarray_p->words, (char*)bitarray_other_p->words, bitarray_p->capacity) == 0;
}

bool bitarray_get_unsafe(const Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    return (bitarray_p->words[n] >> m) & 1;
}

void bitarray_set_true_unsafe(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    bitarray_p->words[n] |= 1 << m;
}

void bitarray_set_false_unsafe(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    bitarray_p->words[n] &= ~(1 << m);
}

void bitarray_set_unsafe(Bitarray* bitarray_p, size_t index, bool value) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= (value << m);
}

void bitarray_toggle_unsafe(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    bitarray_p->words[n] ^= 1 << m;
}

bool bitarray_get_safe(const Bitarray* bitarray_p, size_t index, char* filename, int linenr) {
    if (index > bitarray_p->num_of_bits) {
        fprintf(stderr, "Index %lu is OOB. Going OOB in file '%s' at line %i\n", index, filename, linenr);
        exit(1);
    }
    return bitarray_get_unsafe(bitarray_p, index);
}

void bitarray_set_true_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr) {
    if (index > bitarray_p->num_of_bits) {
        fprintf(stderr, "Index %lu is OOB. Going OOB in file '%s' at line %i\n", index, filename, linenr);
        exit(1);
    }
    bitarray_set_true_unsafe(bitarray_p, index);
}

void bitarray_set_false_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr) {
    if (index > bitarray_p->num_of_bits) {
        fprintf(stderr, "Index %lu is OOB. Going OOB in file '%s' at line %i\n", index, filename, linenr);
        exit(1);
    }
    bitarray_set_false_unsafe(bitarray_p, index);
}

void bitarray_set_safe(Bitarray* bitarray_p, size_t index, bool bit, char* filename, int linenr) {
    if (index > bitarray_p->num_of_bits) {
        fprintf(stderr, "Index %lu is OOB. Going OOB in file '%s' at line %i\n", index, filename, linenr);
        exit(1);
    }
    bitarray_set_unsafe(bitarray_p, index, bit);
}

void bitarray_toggle_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr) {
    if (index > bitarray_p->num_of_bits) {
        fprintf(stderr, "Index %lu is OOB. Going OOB in file '%s' at line %i\n", index, filename, linenr);
        exit(1);
    }
    bitarray_toggle_unsafe(bitarray_p, index);
}

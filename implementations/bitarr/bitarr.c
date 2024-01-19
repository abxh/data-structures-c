#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitarr.h"

Bitarr* bitarr_new(size_t num_of_words) {
    Bitarr* bitarray = malloc(sizeof(Bitarr));
    if (bitarray == NULL) {
        return bitarray;
    }
    bitarray->words = calloc(num_of_words, sizeof(uint8_t));
    bitarray->num_of_words = num_of_words;
    return bitarray;
}

Bitarr* bitarr_clone(const Bitarr* bitarr_p) {
    Bitarr* bitarr_clone_p = malloc(sizeof(Bitarr));
    bitarr_clone_p->words = (uint8_t*)strndup((char*)bitarr_p->words, bitarr_p->num_of_words);
    bitarr_clone_p->num_of_words = bitarr_p->num_of_words;
    return bitarr_clone_p;
}

bool bitarr_from_bytes(const char* bytes, size_t n) {
    Bitarr* bitarr_p = malloc(sizeof(Bitarr));
    if (bitarr_p == NULL) {
        return false;
    }
    bitarr_p->words = (uint8_t*)bytes;
    bitarr_p->num_of_words = n;
    return true;
}

bool bitarr_equal(const Bitarr* bitarr_p, const Bitarr* bitarr_other_p) {
    if (bitarr_p->num_of_words != bitarr_other_p->num_of_words) {
        return false;
    }
    return strncmp((char*)bitarr_p->words, (char*)bitarr_other_p->words, bitarr_p->num_of_words) == 0;
}

int bitarr_get(const Bitarr* bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return -1;
    }
    return (bitarr_p->words[n] >> m) & 1;
}

bool bitarr_set_true(Bitarr* bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] |= 1 << m;
    return true;
}

bool bitarr_set_false(Bitarr* bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] &= ~(1 << m);
    return true;
}

bool bitarr_set(Bitarr* bitarr_p, size_t index, bool value) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] &= ~(1 << m);
    bitarr_p->words[n] |= (value << m);
    return true;
}

bool bitarr_toggle(Bitarr* bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] ^= 1 << m;
    return true;
}

void bitarr_print(const Bitarr* bitarr_p) {
    for (size_t i = 0; i < bitarr_p->num_of_words; i++) {
        printf("%p: ", (void*)bitarr_p->words + i);
        for (size_t j = sizeof(uint8_t) * 8 - 1; j > 0; j--) {
            putchar('0' + ((bitarr_p->words[i] >> j) & 1));
        }
        putchar('\n');
    }
}

void bitarr_free(Bitarr* bitarr_p) {
    free(bitarr_p->words);
    free(bitarr_p);
}

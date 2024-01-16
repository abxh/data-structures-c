#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitarr.h"

bitarr *bitarr_new(size_t num_of_words) {
    bitarr *bitarray = malloc(sizeof(bitarr));
    if (bitarray == NULL) {
        return bitarray;
    }
    bitarray->words = calloc(num_of_words, sizeof(uint8_t));
    bitarray->num_of_words = num_of_words;
    return bitarray;
}

bitarr *bitarr_clone(const bitarr *bitarr_p) {
    bitarr *bitarr_clone_p = malloc(sizeof(bitarr));
    bitarr_clone_p->words = (uint8_t*)strndup((char *)bitarr_p->words, bitarr_p->num_of_words);
    bitarr_clone_p->num_of_words = bitarr_p->num_of_words;
    return bitarr_clone_p;
}

bool bitarr_equal(const bitarr *bitarr1_p, const bitarr *bitarr2_p) {
    for (size_t i = 0; i < bitarr1_p->num_of_words; i++) {
        for (size_t j = 0; j < sizeof(uint8_t) * 8; j++) {
            int b1 = (bitarr1_p->words[i] >> j) & 1;
            int b2 = (bitarr2_p->words[i] >> j) & 1;
            if (b1 != b2) {
                return false;
            }
        }
    }
    return true;
}

int bitarr_get(const bitarr *bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return -1;
    }
    return (bitarr_p->words[n] >> m) & 1;
}

bool bitarr_set_true(bitarr *bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] |= 1 << m;
    return true;
}

bool bitarr_set_false(bitarr *bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] &= ~(1 << m);
    return true;
}

bool bitarr_set(bitarr *bitarr_p, size_t index, int value) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] &= ~(1 << m);
    bitarr_p->words[n] |= (value << m);
    return true;
}

bool bitarr_toggle(bitarr *bitarr_p, size_t index) {
    size_t n = bitarr_word_index(index);
    size_t m = bitarr_bit_index(index);
    if (n >= bitarr_p->num_of_words) {
        return false;
    }
    bitarr_p->words[n] ^= 1 << m;
    return true;
}

void bitarr_print(const bitarr *bitarr_p) {
    for (size_t i = 0; i < bitarr_p->num_of_words; i++) {
        printf("%p: ", (void *)bitarr_p->words + i);
        for (size_t j = sizeof(uint8_t) * 8 - 1; j > 0; j--) {
            putchar('0' + ((bitarr_p->words[i] >> j) & 1));
        }
        putchar('\n');
    }
    printf("%d\n", alignof(bitarr));
}

void bitarr_free(bitarr *bitarr_p) {
    free(bitarr_p->words);
    free(bitarr_p);
}

#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

#define BA_WORD_INDEX(index) (index >> 3)     // index / 8
#define BA_BIT_INDEX(index)  (~index & 0b111) // 7 - (index % 8)

Bitarray *ba_new(size_t num_of_words) {
    if (num_of_words == 0) {
        return NULL;
    }
    Bitarray *bitarray = malloc(sizeof(Bitarray));
    if (bitarray == NULL) {
        return bitarray;
    }
    bitarray->words = calloc(num_of_words, sizeof(ba_word));
    bitarray->num_of_words = num_of_words;
    return bitarray;
}

int ba_get(Bitarray *bitarray_p, size_t index) {
    size_t n = BA_WORD_INDEX(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    size_t m = BA_BIT_INDEX(index);
    return (bitarray_p->words[n] >> m) & 1;
}

int ba_set_true(Bitarray *bitarray_p, size_t index) {
    size_t n = BA_WORD_INDEX(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    size_t m = BA_BIT_INDEX(index);
    bitarray_p->words[n] |= 1 << m;
    return 0;
}

int ba_set_false(Bitarray *bitarray_p, size_t index) {
    size_t n = BA_WORD_INDEX(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    size_t m = BA_BIT_INDEX(index);
    bitarray_p->words[n] &= ~(1 << m);
    return 0;
}

int ba_set(Bitarray *bitarray_p, size_t index, int value) {
    size_t n = BA_WORD_INDEX(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    size_t m = BA_BIT_INDEX(index);
    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= value << m;
    return 0;
}

int ba_toggle(Bitarray *bitarray_p, size_t index) {
    size_t n = BA_WORD_INDEX(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    size_t m = BA_BIT_INDEX(index);
    bitarray_p->words[n] ^= 1 << m;
    return 0;
}

void ba_print(const Bitarray *bitarray_p) {
    for (size_t i = 0; i < bitarray_p->num_of_words; i++) {
        printf("%p: ", (void*) bitarray_p->words + i);
        for (size_t j = sizeof(ba_word) * 8 - 1; j > 0; j--) {
            putchar('0' + ((bitarray_p->words[i] >> j) & 1));
        }
        putchar('\n');
    }
    printf("%d\n", alignof(Bitarray));
}

void ba_free(Bitarray *bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

Bitarray* ba_new(const unsigned int num_of_words) {
    Bitarray* bitarray = malloc(sizeof(Bitarray));
    if (bitarray == NULL) {
        return bitarray;
    }
    bitarray->words = calloc(num_of_words, sizeof(ba_word));
    bitarray->num_of_words = num_of_words;
    return bitarray;
}

void ba_print(const Bitarray *bitarray_p) {
    size_t lim = bitarray_p->num_of_words;
    for (size_t i = 1; i <= lim; i++) {
        if (i % 4 != 1) {
            putchar(' ');
        }
        putchar('0' + (bitarray_p->words[i] & 0x80));
        putchar('0' + (bitarray_p->words[i] & 0x40));
        putchar('0' + (bitarray_p->words[i] & 0x20));
        putchar('0' + (bitarray_p->words[i] & 0x10));
        putchar('0' + (bitarray_p->words[i] & 0x08));
        putchar('0' + (bitarray_p->words[i] & 0x04));
        putchar('0' + (bitarray_p->words[i] & 0x02));
        putchar('0' + (bitarray_p->words[i] & 0x01));
        if (i % 4 == 0) {
            putchar('\n');
        }
    }
    putchar('\n');
}

void ba_free(Bitarray* bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

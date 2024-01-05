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
    /*
       ~sp              nl
        | 1 | 2 | 3 | 4 |
        | 5 | 6 | 7 | 8 |
        ...
    */
    size_t i;
    for (i = 1; i <= bitarray_p->num_of_words; i++) {
        if (!((i & 0b111) == 5 || (i & 0b111) == 1)) // i % 4 != 1
            putchar(' ');
        putchar('0' + ((bitarray_p->words[i] >> 7) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 6) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 5) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 4) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 3) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 2) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 1) & 1));
        putchar('0' + ((bitarray_p->words[i] >> 0) & 1));
        if ((i & 0b111) == 4 || (i & 0b111) == 0) // i % 4 == 0
            putchar('\n');
    }
    if (!((i & 0b111) == 0b101 || (i & 0b111) == 0b1)) // i % 4 != 1
        putchar('\n');
}

void ba_free(Bitarray* bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

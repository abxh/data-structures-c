#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitarray.h"

Bitarray* bitarray_new(size_t num_of_words) {
    if (num_of_words == 0) {
        return NULL;
    }
    Bitarray* bitarr = malloc(sizeof(Bitarray));
    if (bitarr == NULL) {
        return NULL;
    }
    bitarr->words = calloc(num_of_words, sizeof(uint8_t));
    if (bitarr->words == NULL) {
        free(bitarr);
        return NULL;
    }
    bitarr->num_of_words = num_of_words;
    return bitarr;
}

Bitarray* bitarray_clone(const Bitarray* bitarray_p) {
    Bitarray* bitarray_clone_p = malloc(sizeof(Bitarray));
    bitarray_clone_p->words = (uint8_t*)strndup((char*)bitarray_p->words, bitarray_p->num_of_words);
    bitarray_clone_p->num_of_words = bitarray_p->num_of_words;
    return bitarray_clone_p;
}

bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p) {
    if (bitarray_p->num_of_words != bitarray_other_p->num_of_words) {
        return false;
    }
    return memcmp((char*)bitarray_p->words, (char*)bitarray_other_p->words, bitarray_p->num_of_words) == 0;
}

int bitarray_get(const Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    if (n >= bitarray_p->num_of_words) {
        return -1;
    }
    return (bitarray_p->words[n] >> m) & 1;
}

bool bitarray_set_true(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    if (n >= bitarray_p->num_of_words) {
        return false;
    }
    bitarray_p->words[n] |= 1 << m;
    return true;
}

bool bitarray_set_false(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    if (n >= bitarray_p->num_of_words) {
        return false;
    }
    bitarray_p->words[n] &= ~(1 << m);
    return true;
}

bool bitarray_set(Bitarray* bitarray_p, size_t index, bool value) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    if (n >= bitarray_p->num_of_words) {
        return false;
    }
    bitarray_p->words[n] &= ~(1 << m);
    bitarray_p->words[n] |= (value << m);
    return true;
}

bool bitarray_toggle(Bitarray* bitarray_p, size_t index) {
    size_t n = bitarray_word_index(index);
    size_t m = bitarray_bit_index(index);
    if (n >= bitarray_p->num_of_words) {
        return false;
    }
    bitarray_p->words[n] ^= 1 << m;
    return true;
}

void bitarray_print(const Bitarray* bitarray_p) {
    size_t i;
    uint8_t* ptr = bitarray_p->words;

    for (i = 0; i < bitarray_p->num_of_words; i++) {
    	if (i % 4 == 0) {
            printf("%p:", ptr);
	    ptr += 4;
	}
	putchar(' ');

	putchar('0' + ((bitarray_p->words[i] >> 7) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 6) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 5) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 4) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 3) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 2) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 1) & 1));
	putchar('0' + ((bitarray_p->words[i] >> 0) & 1));

    	if ((i + 1) % 4 == 0) {
            putchar('\n');
	}
    }
    if ((i + 1) % 4 != 1) {
	putchar('\n');
    }
}

void bitarray_free(Bitarray* bitarray_p) {
    free(bitarray_p->words);
    free(bitarray_p);
}

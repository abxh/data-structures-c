#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

/* create a new bytearray of size num_of_words * ba_word bits. initializes bits to 0. */
Bitarray* ba_new(const unsigned int num_of_words);

/* print the bits in the bitarray. */
void ba_print(const Bitarray *bitarray_p);

/* free memory from the heap appropiately */
void ba_free(Bitarray* bitarray_p);

#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t num_of_words;
    uint8_t* words;
} Bitarray;

/* Get index relative to words in the array */
static inline size_t bitarray_word_index(const size_t index) {
    return index >> 3;
}

/* Get index relative to bits in the array */
static inline size_t bitarray_bit_index(const size_t index) {
    return ~index & 7;
}

/* Create a new bitarray of size num_of_words bytes. */
Bitarray* bitarray_new(size_t num_of_words);

/* Clone the bitarray. */
Bitarray* bitarray_clone(const Bitarray* bitarray_p);

/* Check if two bitarrays are equal. */
bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p);

/* Return the value at an index. Return -1 if OOB.*/
int bitarray_get(const Bitarray* bitarray_p, size_t index);

/* Set the value to true at an index. Return if not OOB.*/
bool bitarray_set_true(Bitarray* bitarray_p, size_t index);

/* Set the value to false at an index. Return if not OOB.*/
bool bitarray_set_false(Bitarray* bitarray_p, size_t index);

/* Set a provided value at an index at run time. Return if not OOB.*/
bool bitarray_set(Bitarray* bitarray_p, size_t index, bool bit);

/* Set a provided value at an index at compile time. Return if not OOB.*/
#define BITARRAY_SET(bitarray_p, index, bit) \
    ((bit) ? bitarray_set_true((bitarray_p), (index)) : bitarray_set_false((bitarray_p), (index)))

/* Toggle the value at an index. Return if not OOB. */
bool bitarray_toggle(Bitarray* bitarray_p, size_t index);

/* Print the bits in the bitarray. */
void bitarray_print(const Bitarray* bitarray_p);

/* Free the memory of the bitarray appropiately. */
void bitarray_free(Bitarray* bitarray_p);

#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t num_of_words;
    uint8_t* words;
} Bitarr;

/* Get index relative to words in the array */
static inline size_t bitarr_word_index(const size_t index) {
    return index >> 3;
}

/* Get index relative to bits in the array */
static inline size_t bitarr_bit_index(const size_t index) {
    return ~index & 0b111;
}

/* Create a new bitarray of size num_of_words bytes. */
Bitarr* bitarr_new(size_t num_of_words);

/* Clone the bitarray. */
Bitarr* bitarr_clone(const Bitarr* bitarr_p);

/* Convert bytes to bitarray.*/
bool bitarr_from_bytes(const char* bytes, size_t n);

/* Check if two bitarrays are equal. */
bool bitarr_equal(const Bitarr* bitarr_p, const Bitarr* bitarr_other_p);

/* Return the value at an index. Return -1 if OOB.*/
int bitarr_get(const Bitarr* bitarray_p, size_t index);

/* Set the value to true at an index. Return if not OOB.*/
bool bitarr_set_true(Bitarr* bitarray_p, size_t index);

/* Set the value to false at an index. Return if not OOB.*/
bool bitarr_set_false(Bitarr* bitarray_p, size_t index);

/* Set the value at an index at run time. Return if not OOB.*/
bool bitarr_set(Bitarr* bitarray_p, size_t index, bool bit);

/* Set the value at an index at compile time. Return if not OOB.*/
#define BITARR_SET(bitarray_p, index, bit) ((bit) ? bitarr_set_true((bitarray_p), (index)) : bitarr_set_false((bitarray_p), (index)))

/* Toggle the value at an index. Return if not OOB. */
bool bitarr_toggle(Bitarr* bitarray_p, size_t index);

/* Print the bits in the bitarray. */
void bitarr_print(const Bitarr* bitarray_p);

/* Free the memory of the bitarray appropiately. */
void bitarr_free(Bitarr* bitarray_p);

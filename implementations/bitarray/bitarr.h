#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    size_t num_of_words;
    uint8_t* words;
} bitarr;

/* Get index relative to words in the array */
static inline size_t bitarr_word_index(size_t index) { return index >> 3; }

/* Get index relative to bits in the array */
static inline size_t bitarr_bit_index(size_t index) { return ~index & 0b111; }

/* Create a new bitarray of size num_of_words bytes. */
bitarr* bitarr_new(size_t num_of_words);

/* Clone the bitarray. */
bitarr* bitarr_clone(const bitarr* bitarr_p);

/* Check if two bitarrays are equal. */
bool bitarr_equal(const bitarr* bitarray1_p, const bitarr* bitarray2_p);

/* Return the value at an index. Return -1 if OOB.*/
int bitarr_get(const bitarr* bitarray_p, size_t index);

/* Set the value to true at an index. Return if not OOB.*/
bool bitarr_set_true(bitarr* bitarray_p, size_t index);

/* Set the value to false at an index. Return if not OOB.*/
bool bitarr_set_false(bitarr* bitarray_p, size_t index);

/* Set the value to a provided bit at run time at an index. Return if not OOB.*/
bool bitarr_set(bitarr* bitarray_p, size_t index, bool bit);

/* Toggle the value at an index. Return if not OOB. */
bool bitarr_toggle(bitarr* bitarray_p, size_t index);

/* Print the bits in the bitarray. */
void bitarr_print(const bitarr* bitarray_p);

/* Free the memory of the bitarray appropiately. */
void bitarr_free(bitarr* bitarray_p);

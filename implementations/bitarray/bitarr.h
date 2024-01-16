#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

/* get index relative to words in the array */
static inline int bitarr_word_index(int index) { return index >> 3; }

/* get index relative to bits in the array */
static inline int bitarr_bit_index(int index) { return ~index & 0b111; }

typedef struct {
    size_t num_of_words;
    uint8_t* words;
} bitarr;

/* Create a new bitarray of size num_of_words bytes. */
bitarr *bitarr_new(size_t num_of_words);

/* Return the value at an index. Return -1 if OOB.*/
int bitarr_get(const bitarr *bitarray_p, size_t index);

/* Set the value to true at an index. Return if not OOB.*/
bool bitarr_set_true(bitarr *bitarray_p, size_t index);

/* Set the value to false at an index. Return if not OOB.*/
bool bitarr_set_false(bitarr *bitarray_p, size_t index);

/* Set the value to a provided bit at an index. Return if not OOB.*/
bool bitarr_set(bitarr *bitarray_p, size_t index, int bit);

/* Toggle the value at an index. Return if not OOB. */
bool bitarr_toggle(bitarr *bitarray_p, size_t index);

/* Print the bits in the bitarray. */
void bitarr_print(const bitarr *bitarray_p);

/* Check if two bitarrays are equal. */
bool bitarr_equal(const bitarr *bitarray1_p, const bitarr *bitarray2_p);

/* Free the memory of the bitarray appropiately. */
void bitarr_free(bitarr *bitarray_p);

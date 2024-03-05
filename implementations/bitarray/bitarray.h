#pragma once

#include <assert.h>  // static_assert
#include <limits.h>  // CHAR_BIT
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

static_assert(CHAR_BIT == 8);

typedef struct {
    size_t num_of_bits;
    size_t capacity;
    unsigned char* words;
} Bitarray;

#define BITARRAY_WORD_INDEX(index) ((index) >> 3)
#define BITARRAY_BIT_INDEX(index) (~(index) & 7)

/* Create a new bitarray of a given size with values initalized to 0 and return it's pointer. Returns NULL if OOM or `
   roundup_char_bit(num_of_bits)` exceeds SIZE_MAX. */
Bitarray* bitarray_new(size_t num_of_bits);

/* Create a new bitarray from given bytes and number of bits to copy and return it's pointer. Returns NULL if OOM. */
Bitarray* bitarray_from(const unsigned char* bytes, size_t num_of_bits);

/* Copy a given bitarray and returns it's pointer. Returns NULL if OOM. */
Bitarray* bitarray_copy(const Bitarray* bitarray_p);

/* Check if two bitarrays are equal. */
bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p);

/* Print the bits in the bitarray. */
void bitarray_print(const Bitarray* bitarray_p);

/* Free the memory of the bitarray appropiately and set it's pointer to NULL.
   This may be called even after the bitarray pointer is NULL. */
void bitarray_free(Bitarray** bitarray_p);

/* Return the value at an index. */
bool bitarray_get(const Bitarray* bitarray_p, size_t index);

/* Set the value to true at an index. */
void bitarray_set_true(Bitarray* bitarray_p, size_t index);

/* Set the value to false at an index. */
void bitarray_set_false(Bitarray* bitarray_p, size_t index);

/* Set a provided value at an index. */
void bitarray_set(Bitarray* bitarray_p, size_t index, bool bit);

/* Toggle the value at an index. */
void bitarray_toggle(Bitarray* bitarray_p, size_t index);

#pragma once

#include <stdlib.h> // size_t
#include <stdbool.h> // bool

typedef struct {
    size_t num_of_bits;
    size_t capacity;
    unsigned char* words;
} Bitarray;

/* Try create a new bitarray of a given size with values initalized to 0 and return it's pointer. Returns NULL if OOM. */
Bitarray* bitarray_new(size_t num_of_bits);

/* Try create a new bitarray from given bytes and number of bits to copy and return it's pointer. Returns NULL if OOM. */
Bitarray* bitarray_from(const unsigned char* bytes, size_t num_of_bits);

/* Try copy a given bitarray and returns it's pointer. Returns NULL if OOM. */
Bitarray* bitarray_copy(const Bitarray* bitarray_p);

/* Check if two bitarrays are equal. */
bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p);

/* Print the bits in the bitarray. */
void bitarray_print(const Bitarray* bitarray_p);

/* Free the memory of the bitarray appropiately. */
void bitarray_free(Bitarray* bitarray_p);

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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    size_t num_of_bits;
    size_t capacity;
    uint8_t* words;
} Bitarray;

Bitarray* bitarray_new(size_t num_of_bits); /* Create a new bitarray of a given size with values initalized to 0. Returns NULL if OOM or num_of_bits is 0. */
Bitarray* bitarray_new_from(void* bytes, size_t num_of_bits); /* Create a new bitarray from given bytes and number of bits to copy. Return value is NULL if OOM or num_of_bits is 0. */
Bitarray* bitarray_copy(const Bitarray* bitarray_p); /* Copy a given bitarray. */
bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p); /* Check if two bitarrays are equal. */
void bitarray_print(const Bitarray* bitarray_p); /* Print the bits in the bitarray. */
void bitarray_free(Bitarray* bitarray_p); /* Free the memory of the bitarray appropiately. */

bool bitarray_get_unsafe(const Bitarray* bitarray_p, size_t index); /* Return the value at an index. Without OOB check. */
void bitarray_set_true_unsafe(Bitarray* bitarray_p, size_t index); /* Set the value to true at an index. Without OOB check. */
void bitarray_set_false_unsafe(Bitarray* bitarray_p, size_t index); /* Set the value to false at an index. Without OOB check. */
void bitarray_set_unsafe(Bitarray* bitarray_p, size_t index, bool bit); /* Set a provided value at an index. Without OOB check. */
void bitarray_toggle_unsafe(Bitarray* bitarray_p, size_t index); /* Toggle the value at an index. Without OOB check. */

bool bitarray_get_safe(const Bitarray* bitarray_p, size_t index, char* filename, int linenr); /* Return the value at an index. Exits gracefully if OOB. */
void bitarray_set_true_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr); /* Set the value to true at an index. Exits gracefully if OOB. */
void bitarray_set_false_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr); /* Set the value to false at an index. Exits gracefully if OOB. */
void bitarray_set_safe(Bitarray* bitarray_p, size_t index, bool bit, char* filename, int linenr); /* Set a provided value at an index. Exits gracefully if OOB. */
void bitarray_toggle_safe(Bitarray* bitarray_p, size_t index, char* filename, int linenr); /* Toggle the value at an index. Exits gracefully if OOB. */

// macro wrappers:
#define bitarray_get(bitarray_p, index) (bitarray_get_safe((bitarray_p), (index), __FILE__, __LINE__))
#define bitarray_set_true(bitarray_p, index) (bitarray_set_true_safe((bitarray_p), (index), __FILE__, __LINE__))
#define bitarray_set_false(bitarray_p, index) (bitarray_set_false_safe((bitarray_p), (index), __FILE__, __LINE__))
#define bitarray_set(bitarray_p, index, bit) (bitarray_set_safe((bitarray_p), (index), (bit), __FILE__, __LINE__))
#define bitarray_toggle(bitarray_p, index) (bitarray_toggle_safe((bitarray_p), (index), __FILE__, __LINE__))

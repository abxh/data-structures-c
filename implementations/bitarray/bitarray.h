#include <inttypes.h>
#include <stdlib.h>

#pragma once

#define BA_WORD uint8_t
#define BA_WORD_INDEX(index) (index >> 3)     // index / 8
#define BA_BIT_INDEX(index)  (~index & 0b111) // 7 - (index % 8)

typedef struct {
    size_t num_of_words;
    BA_WORD *words;
} Bitarray;



/* 
   Create a new bitarray of size num_of_words * BA_WORD bits.
   num_of_words should not be 0. Initialize bits to zero.
   Can return NULL and it must be checked manually.
*/
Bitarray* ba_new(size_t num_of_words);

/* 
   Return the value at an index x, where 0 <= x < num_of_words * 8
   if not OOB else -1. Bitarray pointer cannot be NULL.
*/
int ba_get(Bitarray *bitarray_p, size_t index);

/* 
   Set the value to true at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. Bitarray pointer cannot be NULL.
*/
int ba_set_true(Bitarray *bitarray_p, size_t index);

/* 
   Set the value to false at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. Bitarray pointer cannot be NULL.
*/
int ba_set_false(Bitarray *bitarray_p, size_t index);

/* 
   Set the value to a provided bit at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. Bitarray pointer cannot be NULL.
*/
int ba_set(Bitarray *bitarray_p, size_t index, int bit);

/* 
   Toggle the value at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. Bitarray pointer cannot be NULL.
*/
int ba_toggle(Bitarray *bitarray_p, size_t index);

/* Print the bits in the bitarray. Bitarray pointer cannot be NULL. */
void ba_print(const Bitarray *bitarray_p);

/* Free the bitarray from the heap appropiately. Bitarray pointer cannot be NULL. */
void ba_free(Bitarray* bitarray_p);

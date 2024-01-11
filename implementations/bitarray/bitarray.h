#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

/* 
   create a new bitarray of size num_of_words * ba_word bits.
   num_of_words should not be 0. initialize bits to zero.
   can return NULL and it must be checked manually.
*/
Bitarray* ba_new(const unsigned int num_of_words);

/* 
   return the value at an index x, where 0 <= x < num_of_words * 8
   if not OOB else -1. bitarray pointer cannot be NULL.
*/
int ba_get(Bitarray *bitarray_p, size_t index);

/* 
   set the value to true at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. bitarray pointer cannot be NULL.
*/
int ba_set_true(Bitarray *bitarray_p, size_t index);

/* 
   set the value to false at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. bitarray pointer cannot be NULL.
*/
int ba_set_false(Bitarray *bitarray_p, size_t index);

/* 
   set the value to a provided bit at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. bitarray pointer cannot be NULL.
*/
int ba_set(Bitarray *bitarray_p, size_t index, int bit);

/* 
   toggle the value at an index x, where 0 <= x < num_of_words * 8
   and return 0 if not OOB else -1. bitarray pointer cannot be NULL.
*/
int ba_toggle(Bitarray *bitarray_p, size_t index);

/* print the bits in the bitarray. bitarray pointer cannot be NULL. */
void ba_print(const Bitarray *bitarray_p);

/* free the bitarray from the heap appropiately. bitarray pointer cannot be NULL. */
void ba_free(Bitarray* bitarray_p);

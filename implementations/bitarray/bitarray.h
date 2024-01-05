#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

typedef int(ba_update_func)(int);

/* create a new bytearray of size num_of_words * ba_word bits. initializes bits to 0. */
Bitarray* ba_new(const unsigned int num_of_words);

/* 
   return the value at an index x, where 0 <= x < num_of_words * 8.
   returns -1 if out of bounds.
*/
int ba_get(Bitarray *bitarray, size_t index);

/* 
   set the value at an index x, where 0 <= x < num_of_words * 8.
   behaviour is undefined if less than 0 or greater than 1.
   returns 0 if successful. returns -1 if out of bounds.
*/
int ba_set(Bitarray *bitarray, size_t index, int value);

/* 
   set the value at an index x using a function, where 0 <= x < num_of_words * 8. 
   update function takes in the original value and returns the new value.
   returns 0 if successful. returns -1 if out of bounds.
*/
int ba_update_at(Bitarray *bitarray_p, size_t index, ba_update_func update_func);

/* print the bits in the bitarray. */
void ba_print(const Bitarray *bitarray_p);

/* free memory from the heap appropiately */
void ba_free(Bitarray* bitarray_p);

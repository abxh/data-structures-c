#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t ba_word;

typedef struct {
    size_t num_of_words;
    ba_word *words;
} Bitarray;

typedef ba_word (ba_update_word_f)(Bitarray*, ba_word, size_t, size_t);
typedef int (ba_update_at_f)(Bitarray*, int, size_t);

/* create a new bytearray of size num_of_words * ba_word bits. initializes bits to 0. */
Bitarray* ba_new(const unsigned int num_of_words);

/* 
   return the value at an index x, where 0 <= x < num_of_words * 8.
   returns 0 if successfull. returns -1 if out of bounds.
*/
int ba_get(Bitarray *bitarray, size_t index);

/* 
   set the value at an index x, where 0 <= x < num_of_words * 8.
   returns 0 if successful. returns -1 if out of bounds.
*/
int ba_set(Bitarray *bitarray, size_t index, int value);

/* 
   set the value at an index x using a function, where 0 <= x < num_of_words * 8. 
   update function takes in the bitarray, original word, the index relative
   to entire bit array and the index relative to the word (in that order)
   and returns the new word.
   returns 0 if successful. returns -1 if out of bounds.
*/
int ba_update_by_word(Bitarray *bitarray_p, size_t index, ba_update_word_f update_func);

/* 
   set the value at an index x using a function, where 0 <= x < num_of_words * 8. 
   update function takes in the bitarray, the original value and the index (in
   that order) and returns the new value.
   returns 0 if successful. returns -1 if out of bounds.
*/
int ba_update_at(Bitarray *bitarray_p, size_t index, ba_update_at_f update_func);

/* print the bits in the bitarray. */
void ba_print(const Bitarray *bitarray_p);

/* free memory from the heap appropiately */
void ba_free(Bitarray* bitarray_p);

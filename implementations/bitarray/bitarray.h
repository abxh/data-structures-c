#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t num_of_bits;
    size_t capacity;
    uint8_t* words;
} Bitarray;

/* Create a new bitarray of a given size. */
Bitarray* bitarray_new(size_t num_of_bits);

/* Clone the bitarray. */
Bitarray* bitarray_clone(const Bitarray* bitarray_p);

/* Check if two bitarrays are equal. */
bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p);

/* Return the value at an index. */
bool bitarray_get(const Bitarray* bitarray_p, size_t index);

/* Set the value to true at an index. */
void bitarray_set_true(Bitarray* bitarray_p, size_t index);

/* Set the value to false at an index. */
void bitarray_set_false(Bitarray* bitarray_p, size_t index);

/* Set a provided value at an index at run time. */
void bitarray_set(Bitarray* bitarray_p, size_t index, bool bit);

/* Toggle the value at an index. */
void bitarray_toggle(Bitarray* bitarray_p, size_t index);

/* Print the bits in the bitarray. */
void bitarray_print(const Bitarray* bitarray_p);

/* Free the memory of the bitarray appropiately. */
void bitarray_free(Bitarray* bitarray_p);

/* macro wrappers which guard against OOB and stops the program. */
#define bitarray_get_safe(bitarray_p, index)                                                 \
    (((size_t)(index) < (bitarray_p)->num_of_bits)                                           \
         ? bitarray_get((bitarray_p), index)                                                 \
         : fprintf(stderr, "Exception: OOB in file '%s' at line %i.\n", __FILE__, __LINE__), \
     exit(1), false)

#define bitarray_set_true_safe(bitarray_p, index)                                             \
    {                                                                                         \
        if ((size_t)index >= (bitarray_p)->num_of_bits) {                                     \
            fprintf(stderr, "Exception: OOB in file '%s' at line %i.\n", __FILE__, __LINE__); \
            exit(1);                                                                          \
        } else {                                                                              \
            bitarray_set_true((bitarray_p), index);                                           \
        }                                                                                     \
    }

#define bitarray_set_false_safe(bitarray_p, index)                                            \
    {                                                                                         \
        if ((size_t)(index) >= (bitarray_p)->num_of_bits) {                                   \
            fprintf(stderr, "Exception: OOB in file '%s' at line %i.\n", __FILE__, __LINE__); \
            exit(1);                                                                          \
        } else {                                                                              \
            bitarray_set_false((bitarray_p), index);                                          \
        }                                                                                     \
    }

#define bitarray_set_safe(bitarray_p, index, bit)                                             \
    {                                                                                         \
        if ((size_t)index >= (bitarray_p)->num_of_bits) {                                     \
            fprintf(stderr, "Exception: OOB in file '%s' at line %i.\n", __FILE__, __LINE__); \
            exit(1);                                                                          \
        } else {                                                                              \
            bitarray_set((bitarray_p), index, bit);                                           \
        }                                                                                     \
    }

#define bitarray_toggle_safe(bitarray_p, index)                                               \
    {                                                                                         \
        if ((size_t)index >= (bitarray_p)->num_of_bits) {                                     \
            fprintf(stderr, "Exception: OOB in file '%s' at line %i.\n", __FILE__, __LINE__); \
            exit(1);                                                                          \
        } else {                                                                              \
            bitarray_toggle((bitarray_p), index);                                             \
        }                                                                                     \
    }

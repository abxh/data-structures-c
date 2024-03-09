#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct {
    size_t num_of_bits;
    size_t capacity;
    unsigned char* words;
} Bitarray;

bool bitarray_init(Bitarray** bitarray_pp, size_t num_of_bits);

bool bitarray_deinit(Bitarray** bitarray_pp);

void bitarray_print(const Bitarray* bitarray_p);

Bitarray* bitarray_from(const unsigned char* bytes, size_t num_of_bits);

Bitarray* bitarray_copy(const Bitarray* bitarray_p);

bool bitarray_equal(const Bitarray* bitarray_p, const Bitarray* bitarray_other_p);

bool bitarray_get(const Bitarray* bitarray_p, size_t index);

void bitarray_set_true(Bitarray* bitarray_p, size_t index);

void bitarray_set_false(Bitarray* bitarray_p, size_t index);

void bitarray_set(Bitarray* bitarray_p, size_t index, bool bit);

void bitarray_toggle(Bitarray* bitarray_p, size_t index);

#pragma once

#include <stdbool.h> // bool
#include <stdint.h>  // uint64_t
#include <stdlib.h>  // size_t

typedef struct {
    uint64_t key_hash;
    size_t value_index;
    size_t offset_from_ideal_index;
} HashmapEntry;

typedef struct {
    size_t used;
    size_t capacity_sub_one;
    size_t value_size;
    unsigned char* value_arr_p;
    HashmapEntry* arr_p;
} Hashmap;

Hashmap* dict_new(size_t capacity, size_t value_size);

bool dict_exists(Hashmap* dict_p, uint64_t key_hash);

HashmapEntry dict_get(Hashmap* dict_p, uint64_t key_hash);

bool dict_set(Hashmap* dict_p, uint64_t key_hash, unsigned char* value_p);

bool dict_del(Hashmap* dict_p, uint64_t key_hash);

void dict_free(Hashmap** dict_pp);

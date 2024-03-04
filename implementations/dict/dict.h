#pragma once

#include <stdbool.h> // bool
#include <stdint.h>  // uint64_t
#include <stdlib.h>  // size_t

typedef struct {
    uint64_t hash;
    ssize_t offset;
} DictEntry;

typedef struct {
    size_t capacity_sub_one;
    size_t used;
    DictEntry* entries_arr_p;

    uint64_t (*key_get_hash)(unsigned char*, size_t);
    bool (*key_isequal)(unsigned char*, unsigned char*);

    size_t key_size;
    size_t value_size;
    unsigned char* keys_arr_p;
    unsigned char* values_arr_p;

    unsigned char* key_buf1;
    unsigned char* key_buf2;
    unsigned char* value_buf1;
    unsigned char* value_buf2;
} Dict;

uint64_t fnv_hash64(unsigned char* data_p, size_t data_size);

Dict* dict_new(size_t capacity, size_t key_size, size_t value_size, uint64_t (*key_get_hash)(unsigned char*, size_t),
               bool (*key_isequal)(unsigned char*, unsigned char*));

bool dict_exists(Dict* dict_p, unsigned char* key_p);

unsigned char* dict_get(Dict* dict_p, unsigned char* key_p);

void dict_set(Dict* dict_p, unsigned char* key_p, unsigned char* value_p);

bool dict_del(Dict* dict_p, unsigned char* key_p);

void dict_free(Dict** dict_pp);

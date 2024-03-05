#pragma once

#include <stdbool.h> // bool
#include <stdint.h>  // uint64_t
#include <stdlib.h>  // size_t

typedef struct {
    uint64_t hash;
    int64_t offset;
    unsigned char data[];
} DictEntry;

typedef struct {
    size_t capacity_sub_one;
    size_t used;

    uint64_t (*key_get_hash)(unsigned char*, size_t);
    bool (*key_isequal)(unsigned char*, unsigned char*);

    size_t key_size;
    size_t value_size;
    size_t entry_size;

    unsigned char* temp_buf_p;
    unsigned char* entries_arr_p;
} Dict;

uint64_t fnv_hash64(unsigned char* data_p, size_t data_size);

Dict* dict_new(size_t capacity, size_t key_size, size_t value_size, uint64_t (*key_get_hash)(unsigned char*, size_t),
               bool (*key_isequal)(unsigned char*, unsigned char*));

bool dict_exists(Dict* dict_p, unsigned char* key_p);

unsigned char* dict_get(Dict* dict_p, unsigned char* key_p);

void dict_set(Dict* dict_p, unsigned char* key_p, unsigned char* value_p);

bool dict_del(Dict* dict_p, unsigned char* key_p);

void dict_free(Dict** dict_pp);

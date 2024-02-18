#pragma once

#include <assert.h>  // static_assert
#include <stdbool.h> // bool
#include <stdint.h>  // hash
#include <stdlib.h>  // size_t

typedef struct DictEntry {
    unsigned char* key_p;
    void* value_p;
} DictEntry;

typedef struct {
    size_t capacity_sub_one;
    size_t used_slots;
    size_t key_size;
    uint64_t* indicies;
} Dict;

Dict* dict_new(size_t initial_capacity, size_t key_size);

bool dict_exists(Dict* dict_p, unsigned char* key_p);

void* dict_get(Dict* dict_p, unsigned char* key_p);

bool dict_set(Dict* dict_p, unsigned char* key_p, void* value_p, size_t value_size);

bool dict_del(Dict* dict_p, unsigned char* key_p);

void dict_free(Dict** dict_pp);

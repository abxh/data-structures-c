#pragma once

#include <assert.h>  // static_assert
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct {
    size_t offset;
    unsigned char* key_p;
    unsigned char* value_p;
} DictEntry;

typedef struct {
    DictEntry* arr_p;
    unsigned char* key_pool;
    unsigned char* value_pool;
    size_t capacity_sub_one;
    size_t used;
    size_t key_size;
    size_t value_size;
} Dict;

Dict* dict_new(size_t nelm, size_t key_size, size_t value_size);

bool dict_exists(Dict* dict_p, void* key_p, size_t key_size);

void* dict_get(Dict* dict_p, void* key_p, size_t key_size);

bool dict_set(Dict* dict_p, void* key_p, size_t key_size, void* value_p, size_t value_size);

bool dict_del(Dict* dict_p, void* key_p, size_t key_size);

void dict_free(Dict** dict_pp);

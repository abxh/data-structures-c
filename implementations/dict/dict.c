#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // SIZE_MAX
#include <stdlib.h>  // size_t, malloc, free, reallocarray, NULL
#include <string.h>  // memcpy, memcmp

#include "dict.h"

#define FNV_OFFSET (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

uint64_t hash(uint8_t* data, size_t data_size) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < data_size; i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

size_t rounduppow2(uint32_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    assert(v != 0);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

Dict* dict_new(size_t nelm, size_t key_size, size_t value_size) {
    assert(nelm != 0);
    assert(key_size != 0);
    assert(value_size != 0);
    size_t capacity = rounduppow2(nelm * 1.25); // (*) 1.25 to keep load factor below 0.8
    if (capacity < nelm || capacity > SIZE_MAX / sizeof(DictEntry) || capacity > SIZE_MAX / key_size ||
        capacity > SIZE_MAX / value_size) {
        return NULL;
    }
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->arr_p = malloc(capacity * sizeof(DictEntry));
    if (dict_p->arr_p == NULL) {
        free(dict_p);
        return NULL;
    }
    dict_p->key_pool = malloc(capacity * key_size);
    if (dict_p->arr_p == NULL) {
        free(dict_p->arr_p);
        free(dict_p);
        return NULL;
    }
    dict_p->value_pool = malloc(capacity * value_size);
    if (dict_p->arr_p == NULL) {
        free(dict_p->key_pool);
        free(dict_p->arr_p);
        free(dict_p);
        return NULL;
    }
    unsigned char* key_p = &dict_p->key_pool[0];
    unsigned char* value_p = &dict_p->value_pool[0];
    for (size_t i = 0; i < capacity; i++) {
        dict_p->arr_p[i] = (DictEntry){.offset = -1, .key_p = key_p, .value_p = value_p};
        key_p += key_size;
        value_p += value_size;
    }
    dict_p->capacity_sub_one = capacity - 1;
    dict_p->used = 0;
    dict_p->key_size = key_size;
    dict_p->value_size = value_size;
    return dict_p;
}



void dict_free(Dict** dict_pp) {
    if (*dict_pp == NULL) {
        return;
    }
    free((*dict_pp)->key_pool);
    free((*dict_pp)->value_pool);
    free((*dict_pp)->arr_p);
    free((*dict_pp));
}

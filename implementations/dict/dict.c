#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // SIZE_MAX, uint64_t, uint32_t
#include <stdlib.h>  // size_t, malloc, free, NULL
#include <string.h>  // memcpy
#include <stddef.h>  // offsetof

#include "dict.h"

uint64_t fnv_hash64(unsigned char* data_p, size_t data_size) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash

#define FNV_OFFSET (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < data_size; i++) {
        hash ^= data_p[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

uint64_t rounduppow2(uint32_t v) {
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

uint64_t roundupmul8(uint64_t v) {
    return (v + (8 - 1)) / 8;
}

Dict* dict_new(size_t capacity, size_t key_size, size_t value_size, uint64_t (*key_get_hash)(unsigned char*, size_t),
               bool (*key_isequal)(unsigned char*, unsigned char*)) {
    assert(capacity != 0);
    assert(key_size != 0);
    assert(value_size != 0);

    size_t new_capacity = rounduppow2(capacity);
    size_t entry_size = roundupmul8(offsetof(DictEntry, data) + key_size + value_size);

    if (new_capacity < capacity || new_capacity > SIZE_MAX / entry_size) {
        return NULL;
    }
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->temp_buf_p = malloc(2 * (key_size + value_size));
    if (dict_p->temp_buf_p == NULL) {
        free(dict_p->temp_buf_p);
        return NULL;
    }
    dict_p->entries_arr_p = malloc(new_capacity * entry_size);
    if (dict_p->entries_arr_p == NULL) {
        free(dict_p->temp_buf_p);
        free(dict_p);
        return NULL;
    }
    for (size_t i = 0; i < new_capacity; i++) {
        (*(DictEntry*)(dict_p->entries_arr_p + i * entry_size)).offset = -1;
    }
    dict_p->capacity_sub_one = new_capacity - 1;
    dict_p->used = 0;
    dict_p->key_get_hash = key_get_hash;
    dict_p->key_isequal = key_isequal;
    dict_p->key_size = key_size;
    dict_p->value_size = value_size;
    dict_p->entry_size = entry_size;

    return dict_p;
}

void dict_free(Dict** dict_pp) {
    if (*dict_pp == NULL) {
        return;
    }
    free((*dict_pp)->entries_arr_p);
    free((*dict_pp));
    *dict_pp = NULL;
}


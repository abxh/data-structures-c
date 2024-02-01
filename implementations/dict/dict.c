#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dict.h"

uint64_t fnv_hash64(uint8_t* data, size_t data_size) {
    // Source:
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

    static const uint64_t FNV_OFFSET = 0xcbf29ce484222325;
    static const uint64_t FNV_PRIME = 0x100000001b3;

    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < data_size; i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

Dict* dict_new(size_t key_size, size_t value_size) {
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->darr = calloc(INITIAL_CAPACITY, sizeof(DictEntry));
    if (dict_p->darr == NULL) {
        free(dict_p);
        return NULL;
    }
    dict_p->capacity = INITIAL_CAPACITY;
    dict_p->used = 0;
    dict_p->key_size = key_size;
    dict_p->value_size = value_size;
    return dict_p;
}

bool dict_exists(Dict* dict_p, void* key_p, size_t key_size) {
    assert(dict_p->key_size == key_size);
    uint64_t and_val = (1 << dict_p->capacity) - 1;
    uint64_t hash = fnv_hash64((uint8_t*) key_p, key_size);
    uintptr_t idx = (uintptr_t)dict_p->darr + hash;
    while ((void*)(++idx & and_val) != NULL);
    return memcmp((void*) idx, key_p, key_size);
}

void dict_free(Dict* dict_p) {
    for (size_t i = 0; i < dict_p->used; i++) {
        free(dict_p->darr[i].key_p);
        free(dict_p->darr[i].value_p);
    }
    free(dict_p->darr);
    free(dict_p);
}

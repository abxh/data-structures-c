#include <stdlib.h> // size_t, malloc, calloc, free, reallocarray, NULL
#include <stdint.h> // uint8_t, uint64_t
#include <stdbool.h> // bool, true, false
#include <string.h> // memcpy, memcmp
#include <assert.h> // assert

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

bool dict_grow(Dict* dict_p, size_t used_new) {
    if (used_new >= dict_p->capacity >> 1) {
        size_t capacity_new = dict_p->capacity << 1;
        DictEntry* darr_new = reallocarray(darr_new, capacity_new, sizeof(DictEntry));
        if (darr_new == NULL) {
            return false;
        }
        dict_p->darr = darr_new;
        dict_p->capacity = capacity_new;
        dict_p->used = used_new;
    }
    return true;
}

bool dict_shrink(Dict* dict_p, size_t used_new) {
    if (dict_p->capacity > 1 && used_new <= dict_p->capacity >> 2) {
        size_t capacity_new = dict_p->capacity >> 1;
        DictEntry* darr_new = reallocarray(darr_new, capacity_new, sizeof(DictEntry));
        if (darr_new == NULL) {
            return false;
        }
        dict_p->darr = darr_new;
        dict_p->capacity = capacity_new;
        dict_p->used = used_new;
    }
    return true;
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
    uint64_t m = dict_p->capacity - 1;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, dict_p->key_size) & m;
    DictEntry* darr = dict_p->darr;
    while (darr + idx != NULL) {
        if (memcmp(darr[idx].key_p, key_p, dict_p->key_size)) {
            return true;
        } else if ((fnv_hash64(darr[idx].key_p, dict_p->key_size) & m) == idx) {
            break;
        }
        idx++;
        idx &= m;
    }
    return false;
}

void* dict_get(Dict* dict_p, void* key_p, size_t key_size) {
    assert(dict_p->key_size == key_size);
    uint64_t m = dict_p->capacity - 1;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, key_size) & m;
    DictEntry* darr = dict_p->darr;
    while (darr + idx != NULL) {
        if (memcmp(darr[idx].key_p, key_p, dict_p->key_size)) {
            return darr[idx].value_p;
        } else if ((fnv_hash64(darr[idx].key_p, dict_p->key_size) & m) == idx) {
            break;
        }
        idx = (idx + 1) & m;
    }
    return NULL;
}

bool dict_set(Dict* dict_p, void* key_p, size_t key_size, void* value_p, size_t value_size) {
    assert(dict_p->key_size == key_size);
    assert(dict_p->value_size == value_size);
    uint64_t m = dict_p->capacity - 1;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, dict_p->key_size) & m;
    DictEntry* darr = dict_p->darr;
    if (!dict_grow(dict_p, dict_p->used + 1)) {
        return false;
    }
    DictEntry entry = {};
    entry.key_p = malloc(dict_p->key_size);
    if (entry.key_p == NULL) {
        return false;
    }
    entry.value_p = malloc(dict_p->value_size);
    if (entry.key_p == NULL) {
        free(entry.key_p);
        return false;
    }
    memcpy(entry.key_p, key_p, dict_p->key_size);
    memcpy(entry.value_p, value_p, dict_p->value_size);
    DictEntry entry_next = {};
    while (darr + idx != NULL) {
        entry_next = darr[idx];
        memcpy(darr + idx, &entry, sizeof(DictEntry));
        entry = entry_next;
        idx = (idx + 1) & m;
    }
    darr[idx] = entry;
    return true;
}

bool dict_del(Dict* dict_p, void* key_p, size_t key_size) {
    assert(dict_p->used >= 1);
    assert(dict_p->key_size == key_size);
    uint64_t m = dict_p->capacity - 1;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, dict_p->key_size) & m;
    DictEntry* darr = dict_p->darr;
    if (darr + idx == NULL) {
        return false;
    }
    if (!dict_shrink(dict_p, dict_p->used - 1)) {
        return false;
    }
    uint64_t idx_next = (idx + 1) & m;
    while ((fnv_hash64(darr[idx_next].key_p, dict_p->key_size) & m) != idx_next) {
        memcpy(darr + idx, darr + idx_next, sizeof(DictEntry));
        idx = idx_next;
        idx_next = (idx_next + 1) & m;
    }
    return true;
}

void dict_free(Dict* dict_p) {
    for (size_t i = 0; i < dict_p->used; i++) {
        free(dict_p->darr[i].key_p);
        free(dict_p->darr[i].value_p);
    }
    free(dict_p->darr);
    free(dict_p);
}

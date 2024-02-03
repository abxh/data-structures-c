#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // uint8_t, uint64_t
#include <stdlib.h>  // size_t, malloc, free, reallocarray, NULL
#include <string.h>  // memcpy, memcmp
#include <stdio.h>

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

void dict_insert(DictEntry* darr, DictEntry entry, size_t key_size, size_t capacity) {
    uint64_t m = (capacity - 1);
    uint64_t idx = fnv_hash64((uint8_t*)entry.key_p, key_size) & m;
    if (darr[idx].key_p == NULL) {
        darr[idx] = entry;
        return;
    }
    do {
        DictEntry other = darr[idx];
        darr[idx] = entry;

        entry = other;
        idx = (idx + 1) & m;
    } while (darr[idx].key_p != NULL);
    darr[idx] = entry;
}

void dict_del_backtrace(DictEntry* darr, size_t idx, size_t key_size, size_t capacity) {
    uint64_t m = capacity - 1;
    uint64_t idx_next = (idx + 1) & m;
    while (darr[idx_next].key_p != NULL && fnv_hash64(darr[idx_next].key_p, key_size) != idx_next) {
        darr[idx].key_p = darr[idx_next].key_p;
        darr[idx].value_p = darr[idx_next].value_p;
        darr[idx_next].key_p = NULL;
        idx = idx_next;
        idx_next = (idx + 1) & m;
    }
}

bool dict_rehash(Dict* dict_p, size_t capacity_new, DictEntry* darr_new) {
    DictEntry* darr = dict_p->darr;
    for (size_t i = 0; i < dict_p->capacity; i++) {
        if (darr[i].key_p == NULL) {
            continue;
        }
        DictEntry entry = {darr[i].key_p, darr[i].value_p};
        dict_insert(darr_new, entry, dict_p->key_size, capacity_new);
    }
    return true;
}

bool dict_grow_if_neccessary(Dict* dict_p, size_t used_new) {
    assert(dict_p->capacity <= (SIZE_MAX / sizeof(DictEntry)) >> 1);
    if (used_new >= dict_p->capacity >> 1) {
        size_t capacity_new = dict_p->capacity << 1;
        DictEntry* darr_new = calloc(capacity_new, sizeof(DictEntry));
        if (darr_new == NULL) {
            return false;
        }
        dict_rehash(dict_p, capacity_new, darr_new);
        free(dict_p->darr);
        dict_p->darr = darr_new;
        dict_p->capacity = capacity_new;
        dict_p->used = used_new;
    }
    return true;
}

bool dict_shrink_if_sufficient(Dict* dict_p, size_t used_new) {
    if (dict_p->capacity >= 4 && used_new <= dict_p->capacity >> 2) {
        size_t capacity_new = dict_p->capacity >> 1;
        DictEntry* darr_new = calloc(capacity_new, sizeof(DictEntry));
        if (darr_new == NULL) {
            return false;
        }
        dict_rehash(dict_p, capacity_new, darr_new);
        free(dict_p->darr);
        dict_p->darr = darr_new;
        dict_p->capacity = capacity_new;
        dict_p->used = used_new;
    }
    return true;
}

Dict* dict_new(size_t initial_capacity, size_t key_size, size_t value_size) {
    assert(initial_capacity != 0);
    assert(key_size != 0);
    assert(value_size != 0);
    assert((initial_capacity & (initial_capacity - 1)) == 0);
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->darr = calloc(initial_capacity, sizeof(DictEntry));
    if (dict_p->darr == NULL) {
        free(dict_p);
        return NULL;
    }
    dict_p->capacity = initial_capacity;
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
    while (darr[idx].key_p != NULL) {
        if (memcmp(darr[idx].key_p, key_p, dict_p->key_size) == 0) {
            return true;
        } else if ((fnv_hash64(darr[idx].key_p, dict_p->key_size) & m) == idx) {
            break;
        }
        idx = (idx + 1) & m;
    }
    return false;
}

void* dict_get(Dict* dict_p, void* key_p, size_t key_size) {
    assert(dict_p->key_size == key_size);
    uint64_t m = dict_p->capacity - 1;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, key_size) & m;
    DictEntry* darr = dict_p->darr;
    while (darr[idx].key_p != NULL) {
        if (memcmp(darr[idx].key_p, key_p, dict_p->key_size) == 0) {
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
    if (!dict_grow_if_neccessary(dict_p, dict_p->used + 1)) {
        return false;
    }
    DictEntry entry;
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
    dict_insert(dict_p->darr, entry, dict_p->key_size, dict_p->capacity);
    return true;
}

bool dict_del(Dict* dict_p, void* key_p, size_t key_size) {
    assert(dict_p->key_size == key_size);
    DictEntry* darr = dict_p->darr;
    uint64_t idx = fnv_hash64((uint8_t*)key_p, dict_p->key_size) & (dict_p->capacity - 1);
    while (darr[idx].key_p == NULL) {
        return false;
    }
    free(darr[idx].key_p);
    free(darr[idx].value_p);
    darr[idx].key_p = NULL;
    darr[idx].value_p = NULL;
    dict_del_backtrace(dict_p->darr, idx, dict_p->key_size, dict_p->capacity);
    if (!dict_shrink_if_sufficient(dict_p, dict_p->used - 1)) {
        return false;
    }
    return true;
}

void dict_free(Dict* dict_p) {
    for (size_t i = 0; i < dict_p->capacity; i++) {
        free(dict_p->darr[i].key_p);
        free(dict_p->darr[i].value_p);
    }
    free(dict_p->darr);
    free(dict_p);
}

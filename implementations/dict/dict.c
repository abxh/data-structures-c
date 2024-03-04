#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // SIZE_MAX
#include <stdlib.h>  // size_t, malloc, free, NULL
#include <string.h>  // memcpy, memcmp

#include "dict.h"

// note:
// - Room for one extra slot is ensured. So the the get- and exists-methods do not
//   run forever with an apparent 100% load factor when the key does not exist.
// - Two additional slots (in addition to the above) are added to the end of the 
//   key and value buffer to be used temporarily. These slots are not counted as
//   a part of the dictionary.

#define FNV_OFFSET (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

uint64_t fnv_hash64(unsigned char* data_p, size_t data_size) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < data_size; i++) {
        hash ^= data_p[i];
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

Dict* dict_new(size_t capacity, size_t key_size, size_t value_size, uint64_t (*key_get_hash)(unsigned char*, size_t),
               bool (*key_isequal)(unsigned char*, unsigned char*)) {
    assert(capacity != 0);
    assert(key_size != 0);
    assert(value_size != 0);
    size_t new_capacity = rounduppow2(capacity + 1);
    if (new_capacity < capacity || new_capacity > SIZE_MAX / sizeof(DictEntry) || new_capacity > SIZE_MAX / key_size ||
        new_capacity > SIZE_MAX / value_size) {
        return NULL;
    }
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->keys_arr_p = malloc(key_size * (new_capacity + 2));
    if (dict_p->keys_arr_p == NULL) {
        free(dict_p);
        return NULL;
    }
    dict_p->values_arr_p = malloc(value_size * (new_capacity + 2));
    if (dict_p->values_arr_p == NULL) {
        free(dict_p->values_arr_p);
        free(dict_p);
        return NULL;
    }
    dict_p->entries_arr_p = malloc(sizeof(DictEntry) * new_capacity);
    if (dict_p->entries_arr_p == NULL) {
        free(dict_p->keys_arr_p);
        free(dict_p->values_arr_p);
        free(dict_p);
        return NULL;
    }
    for (size_t i = 0; i < new_capacity; i++) {
        dict_p->entries_arr_p[i].offset = -1;
    }
    dict_p->capacity_sub_one = new_capacity - 1;
    dict_p->used = 0;
    dict_p->key_get_hash = key_get_hash;
    dict_p->key_isequal = key_isequal;
    dict_p->key_size = key_size;
    dict_p->value_size = value_size;

    return dict_p;
}

bool dict_exists(Dict* dict_p, unsigned char* key_p) {
    assert(dict_p->capacity_sub_one != dict_p->used);

    uint64_t hash = dict_p->key_get_hash(key_p, dict_p->key_size);
    size_t i = hash & dict_p->capacity_sub_one;

    while (dict_p->entries_arr_p[i].offset != -1) {
        if (dict_p->entries_arr_p[i].hash == hash) {
            unsigned char* current_key = dict_p->keys_arr_p + i * dict_p->key_size;
            if (memcmp(current_key, key_p, dict_p->key_size) == 0) {
                return true;
            }
        }
        if (dict_p->entries_arr_p[i].offset == 0) {
            break;
        }
        i++;
        i &= dict_p->capacity_sub_one;
    }
    return false;
}

unsigned char* dict_get(Dict* dict_p, unsigned char* key_p) {
    assert(dict_p->capacity_sub_one != dict_p->used);

    uint64_t hash = dict_p->key_get_hash(key_p, dict_p->key_size);
    size_t i = hash & dict_p->capacity_sub_one;

    while (dict_p->entries_arr_p[i].offset != -1) {
        if (dict_p->entries_arr_p[i].hash == hash) {
            unsigned char* current_key = dict_p->keys_arr_p + i * dict_p->key_size;
            if (memcmp(current_key, key_p, dict_p->key_size) == 0) {
                return current_key;
            }
        }
        if (dict_p->entries_arr_p[i].offset == 0) {
            break;
        }
        i++;
        i &= dict_p->capacity_sub_one;
    }
    return NULL;
}

void dict_set(Dict* dict_p, unsigned char* key_p, unsigned char* value_p) {
    assert(dict_p->capacity_sub_one != dict_p->used);

    uint64_t hash = dict_p->key_get_hash(key_p, dict_p->key_size);
    size_t i = hash & dict_p->capacity_sub_one;
    ssize_t offset = 0;

    unsigned char* KB1 = dict_p->keys_arr_p + (dict_p->capacity_sub_one + 1) * dict_p->key_size;
    unsigned char* KB2 = dict_p->keys_arr_p + (dict_p->capacity_sub_one + 2) * dict_p->key_size;
    unsigned char* VB1 = dict_p->values_arr_p + (dict_p->capacity_sub_one + 1) * dict_p->value_size;
    unsigned char* VB2 = dict_p->values_arr_p + (dict_p->capacity_sub_one + 2) * dict_p->value_size;

    memcpy(KB1, key_p, dict_p->key_size);
    memcpy(VB1, value_p, dict_p->value_size);

    while (dict_p->entries_arr_p[i].offset != -1) {
        if (dict_p->entries_arr_p[i].offset < offset) {
            unsigned char* current_key_p = dict_p->keys_arr_p + i * dict_p->key_size;

            memcpy(KB2, current_key_p, dict_p->key_size);
            memcpy(current_key_p, KB1, dict_p->key_size);
            memcpy(KB1, KB2, dict_p->key_size);

            unsigned char* current_value_p = dict_p->values_arr_p + i * dict_p->value_size;

            memcpy(VB2, current_value_p, dict_p->value_size);
            memcpy(current_value_p, KB1, dict_p->value_size);
            memcpy(VB1, VB2, dict_p->value_size);

            uint64_t current_hash = dict_p->entries_arr_p[i].hash;
            ssize_t current_offset = dict_p->entries_arr_p[i].offset;

            dict_p->entries_arr_p[i].hash = hash;
            dict_p->entries_arr_p[i].offset = offset;
            hash = current_hash;
            offset = current_offset;
        }
        offset++;
        i++;
        i &= dict_p->capacity_sub_one;
    }

    unsigned char* current_key_p = dict_p->keys_arr_p + i * dict_p->key_size;
    unsigned char* current_value_p = dict_p->values_arr_p + i * dict_p->value_size;

    memcpy(current_key_p, KB1, dict_p->key_size);
    memcpy(current_value_p, VB1, dict_p->value_size);

    dict_p->entries_arr_p[i].hash = hash;
    dict_p->entries_arr_p[i].offset = offset;
    dict_p->used++;
}

void dict_free(Dict** dict_pp) {
    if (*dict_pp == NULL) {
        return;
    }
    free((*dict_pp)->entries_arr_p);
    free((*dict_pp)->keys_arr_p);
    free((*dict_pp)->values_arr_p);
    free((*dict_pp));
    *dict_pp = NULL;
}


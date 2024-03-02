#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // SIZE_MAX
#include <stdlib.h>  // size_t, malloc, free, NULL

#include "dict.h"

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
    size_t new_capacity = rounduppow2(capacity);
    if (new_capacity < capacity || new_capacity > SIZE_MAX / sizeof(DictEntry) || new_capacity > SIZE_MAX / key_size ||
        new_capacity > SIZE_MAX / value_size) {
        return NULL;
    }
    Dict* dict_p = malloc(sizeof(Dict));
    if (dict_p == NULL) {
        return NULL;
    }
    dict_p->keys_arr_p = malloc(key_size * new_capacity);
    if (dict_p->keys_arr_p == NULL) {
        free(dict_p);
        return NULL;
    }
    dict_p->values_arr_p = malloc(value_size * new_capacity);
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
    return false;
}

unsigned char* dict_get(Dict* dict_p, unsigned char* key_p) {
    return NULL;
}

bool dict_set(Dict* dict_p, unsigned char* key_p, unsigned char* value_p) {
    return false;
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

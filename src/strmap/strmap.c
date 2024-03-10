#include "strmap.h"
#include <stdint.h> // uint64_t
#include <string.h> // strcmp, strdup, strcpy
#include <assert.h> // static_assert

#define DEFAULT_CAPACITY 2

// static_assert(DEFAULT_CAPACITY >> 2 > 1, "default capacity must be larger than 4");
// static_assert((DEFAULT_CAPACITY & (DEFAULT_CAPACITY - 1)) == 0, "default capacity must be a power of 2");

static inline uint64_t fnv_hash64(const unsigned char* char_p) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function

#define FNV_OFFSET (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

    uint64_t hash = FNV_OFFSET;
    while (*char_p) {
        hash ^= *(char_p++);
        hash *= FNV_PRIME;
    }
    return hash;
}

static inline void* strmapnode_create(const char* key_p, const char* value_p) {
    StrMapNode* node_p = malloc(sizeof(StrMapNode));
    if (node_p == NULL) {
        return NULL;
    }
    node_p->key_p = strdup(key_p);
    if (node_p->key_p == NULL) {
        free(node_p);
        return NULL;
    }
    node_p->value_p = strdup(value_p);
    if (node_p->value_p == NULL) {
        free(node_p->key_p);
        free(node_p);
        return NULL;
    }
    node_p->next_p = NULL;
    return node_p;
}

static inline void strmapnode_destroy(StrMapNode* node_p) {
    free(node_p->key_p);
    free(node_p->value_p);
    free(node_p);
}

static inline void strmapnode_list_destroy(StrMapNode* head_p) {
    StrMapNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        strmapnode_destroy(head_p);
        head_p = next_p;
    }
}

bool strmap_init(StrMap** strmap_pp) {
    *strmap_pp = malloc(sizeof(StrMap));
    if (*strmap_pp == NULL) {
        return false;
    }
    (*strmap_pp)->arr_pp = calloc(DEFAULT_CAPACITY, sizeof(StrMapNode*));
    if ((*strmap_pp)->arr_pp == NULL) {
        free(*strmap_pp);
        *strmap_pp = NULL;
        return false;
    }
    (*strmap_pp)->used = 0;
    (*strmap_pp)->capacity_sub_one = DEFAULT_CAPACITY - 1;
    return true;
}

bool strmap_deinit(StrMap** strmap_pp) {
    if (*strmap_pp == NULL) {
        return false;
    }
    StrMapNode** arr_pp = (*strmap_pp)->arr_pp;
    size_t capacity = (*strmap_pp)->capacity_sub_one + 1;
    for (size_t i = 0; i < capacity; i++) {
        strmapnode_list_destroy(arr_pp[i]);
    }
    free((*strmap_pp)->arr_pp);
    free(*strmap_pp);
    *strmap_pp = NULL;
    return true;
}

bool strmap_exists(StrMap* strmap_p, const char* key_p) {
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & strmap_p->capacity_sub_one;
    StrMapNode* node_p = strmap_p->arr_pp[index];
    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return true;
        }
        node_p = node_p->next_p;
    }
    return false;
}

char* strmap_get(StrMap* strmap_p, const char* key_p) {
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    StrMapNode* node_p = strmap_p->arr_pp[hash & strmap_p->capacity_sub_one];
    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return node_p->value_p;
        }
        node_p = node_p->next_p;
    }
    return NULL;
}

bool strmap_set(StrMap* strmap_p, const char* key_p, const char* value_p) {
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & strmap_p->capacity_sub_one;

    StrMapNode* node_p = strmap_p->arr_pp[index];
    StrMapNode* prev_p = NULL;
    while (node_p != NULL) {
        prev_p = node_p;
        if (strcmp(node_p->key_p, key_p) == 0) {
            free(node_p->value_p);
            node_p->value_p = strdup(value_p);
            return (node_p->value_p == NULL);
        }
        node_p = node_p->next_p;
    }
    node_p = strmapnode_create(key_p, value_p);
    if (node_p == NULL) {
        return false;
    }
    if (prev_p == NULL) {
        strmap_p->arr_pp[index] = node_p;
    } else {
        prev_p->next_p = node_p;
    }
    strmap_p->used++;

    return true;
}

bool strmap_del(StrMap* strmap_p, const char* key_p) {
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & strmap_p->capacity_sub_one;

    StrMapNode* node_p = strmap_p->arr_pp[index];
    StrMapNode* prev_p;
    if (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            strmap_p->arr_pp[index] = node_p->next_p;
            strmapnode_destroy(node_p);
            strmap_p->used--;
            return true;
        }
    } else {
        return false;
    }
    prev_p = node_p;
    node_p = node_p->next_p;
    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            prev_p->next_p = node_p->next_p;
            strmapnode_destroy(node_p);
            strmap_p->used--;
            return true;
        }
        prev_p = node_p;
        node_p = node_p->next_p;
    }
    return false;
}

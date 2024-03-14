#include "strmap.h"
#include <assert.h> // static_assert
#include <stdint.h> // uint64_t
#include <stdlib.h> // reallocarray
#include <string.h> // strcmp, strdup, strcpy, memset

#define DEFAULT_CAPACITY 16
#define MAX_CHAIN_LENGTH 5

static_assert(DEFAULT_CAPACITY > 1, "capacity is assumed to be larger than 1");
static_assert((DEFAULT_CAPACITY & (DEFAULT_CAPACITY - 1)) == 0, "capacity is assummed to be a power of 2");

static uint64_t fnv_hash64(const unsigned char* char_p) {
    assert(char_p != NULL);

    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function

    static const uint64_t FNV_OFFSET = 14695981039346656037UL;
    static const uint64_t FNV_PRIME = 1099511628211UL;

    uint64_t hash = FNV_OFFSET;
    while (*char_p != '\0') {
        hash ^= *(char_p++);
        hash *= FNV_PRIME;
    }

    return hash;
}

bool strmap_init(StrMap** strmap_pp) {
    assert(strmap_pp != NULL);

    *strmap_pp = malloc(sizeof(StrMap));
    if (*strmap_pp == NULL) {
        return false;
    }
    (*strmap_pp)->lists_p = calloc(DEFAULT_CAPACITY, sizeof(StrMapNodeList));
    if ((*strmap_pp)->lists_p == NULL) {
        free(*strmap_pp);
        *strmap_pp = NULL;
        return false;
    }
    (*strmap_pp)->list_count = DEFAULT_CAPACITY;

    return true;
}

bool strmap_deinit(StrMap** strmap_pp) {
    assert(strmap_pp != NULL);

    if (*strmap_pp == NULL) {
        return false;
    }

    // iterate through lists
    StrMapNodeList* arr_p = (*strmap_pp)->lists_p;
    size_t list_count = (*strmap_pp)->list_count;
    for (size_t i = 0; i < list_count; i++) {
        StrMapNode* node_p = arr_p[i].head_p;
        StrMapNode* next_p = NULL;

        // traverse list and free nodes one by one
        while (node_p != NULL) {
            next_p = node_p->next_p;
            free(node_p->key_p);
            free(node_p->value_p);
            free(node_p);
            node_p = next_p;
        }
    }

    free((*strmap_pp)->lists_p);
    free(*strmap_pp);
    *strmap_pp = NULL;

    return true;
}

bool strmap_exists(const StrMap* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    StrMapNode* node_p = strmap_p->lists_p[index].head_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return true;
        }
        node_p = node_p->next_p;
    }

    return false;
}

const char* strmap_get(const StrMap* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnv_hash64((const unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    StrMapNode* node_p = strmap_p->lists_p[index].head_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return node_p->value_p;
        }
        node_p = node_p->next_p;
    }

    return STRMAP_GET_VALUE_DEFAULT;
}

bool strmap_del(StrMap* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    StrMapNode* node_p = strmap_p->lists_p[index].head_p;

    if (node_p == NULL) {
        return false;
    }

    // check if key matches with the head of the list
    if (strcmp(node_p->key_p, key_p) == 0) {
        strmap_p->lists_p[index].head_p = node_p->next_p;
        if (strmap_p->lists_p[index].head_p == NULL) {
            strmap_p->lists_p[index].tail_p = NULL;
        }
        free(node_p->key_p);
        free(node_p->value_p);
        free(node_p);
        strmap_p->lists_p[index].node_count--;
        return true;
    }

    // otherwise iterate through list and compare key one by one
    StrMapNode* prev_p;

    prev_p = node_p;
    node_p = node_p->next_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            prev_p->next_p = node_p->next_p;
            if (prev_p->next_p == NULL) {
                strmap_p->lists_p[index].tail_p = prev_p;
            }

            free(node_p->key_p);
            free(node_p->value_p);
            free(node_p);

            strmap_p->lists_p[index].node_count--;

            return true;
        }

        prev_p = node_p;
        node_p = node_p->next_p;
    }

    return false;
}

static StrMapNode* strmap_create_flattened_list(StrMap* strmap_p, size_t list_count) {
    assert(strmap_p != NULL);

    StrMapNode* head_p = NULL;
    StrMapNode* tail_p = NULL;

    StrMapNodeList* lists_p = strmap_p->lists_p;
    size_t next_index = list_count;

    // find first nonempty list
    for (size_t i = 0; i < list_count; i++) {
        if (lists_p[i].head_p == NULL) {
            continue;
        }
        head_p = lists_p[i].head_p;
        tail_p = lists_p[i].tail_p;
        next_index = i + 1;
        break;
    }

    // flattern other nonempty lists by stringing them together
    for (size_t i = next_index; i < list_count; i++) {
        if (lists_p[i].head_p == NULL) {
            continue;
        }
        tail_p->next_p = lists_p[i].head_p;
        tail_p = lists_p[i].tail_p;
    }

    return head_p;
}

static int strmap_grow_if_necessary(StrMap* strmap_p, size_t chain_length) {
    assert(strmap_p != NULL);

    if (chain_length <= MAX_CHAIN_LENGTH) {
        return 0;
    }

    size_t old_list_count = strmap_p->list_count;
    size_t new_list_count = strmap_p->list_count << 1;

    StrMapNodeList* lists_p = reallocarray(strmap_p->lists_p, new_list_count, sizeof(StrMapNodeList));
    if (lists_p == NULL) {
        return -1;
    }
    strmap_p->lists_p = lists_p;

    // create seperate flattened list
    StrMapNode* node_p = strmap_create_flattened_list(strmap_p, old_list_count);

    // memset array values to 0 or NULL.
    memset(lists_p, 0, sizeof(StrMapNodeList) * new_list_count);

    // reinsert every node
    while (node_p != NULL) {
        uint64_t hash = fnv_hash64((unsigned char*)node_p->key_p);
        uint64_t index = hash & (new_list_count - 1);

        StrMapNode* head_p = lists_p[index].head_p;
        StrMapNode* next_temp_p = node_p->next_p;

        // insert node by the head
        if (head_p == NULL) {
            lists_p[index].head_p = node_p;
            lists_p[index].tail_p = node_p;
            lists_p[index].node_count++;
            node_p->next_p = NULL;
        } else {
            node_p->next_p = lists_p[index].head_p;
            lists_p[index].head_p = node_p;
            lists_p[index].node_count++;
        }
        
        node_p = next_temp_p;
    }
    strmap_p->list_count = new_list_count;

    return 1;
}

static void* strmapnode_create(const char* key_p, const char* value_p) {
    assert(key_p != NULL);
    assert(value_p != NULL);

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

bool strmap_set(StrMap* strmap_p, const char* key_p, const char* value_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);
    assert(value_p != NULL);

    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    int rtr_val = strmap_grow_if_necessary(strmap_p, strmap_p->lists_p[index].node_count + 1);
    if (rtr_val == -1) {
        return false;
    } else if (rtr_val == 1) {
        index = hash & (strmap_p->list_count - 1);
    }

    StrMapNode* node_p = strmap_p->lists_p[index].head_p;
    StrMapNode* prev_p = NULL;

    // iterate through list, compare keys, and replace value if key matches.
    while (node_p != NULL) {
        prev_p = node_p;

        if (strcmp(node_p->key_p, key_p) == 0) {
            free(node_p->value_p);
            node_p->value_p = strdup(value_p);
            if (node_p->value_p == NULL) {
                return false;
            }
            strmap_p->lists_p[index].node_count++;
            return true;
        }

        node_p = node_p->next_p;
    }

    // otherwise create a new node
    node_p = strmapnode_create(key_p, value_p);
    if (node_p == NULL) {
        return false;
    }

    // and insert the node at by the head
    if (prev_p == NULL) {
        strmap_p->lists_p[index].head_p = node_p;
        strmap_p->lists_p[index].tail_p = node_p;
        strmap_p->lists_p[index].node_count++;
    } else {
        node_p->next_p = strmap_p->lists_p[index].head_p;
        strmap_p->lists_p[index].head_p = node_p;
        strmap_p->lists_p[index].node_count++;
    }

    return true;
}

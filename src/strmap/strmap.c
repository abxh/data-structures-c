#include "strmap.h"
#include <assert.h> // static_assert
#include <stdint.h> // uint64_t
#include <stdlib.h> // reallocarray
#include <string.h> // strcmp, strdup, strcpy, memset

#define DEFAULT_CAPACITY 16
#define MAX_CHAIN_LENGTH 6

static_assert(DEFAULT_CAPACITY > 1, "capacity is assumed to be larger than 1");
static_assert((DEFAULT_CAPACITY & (DEFAULT_CAPACITY - 1)) == 0, "capacity is assummed to be a power of 2");

static uint64_t fnv_hash64(const unsigned char* char_p) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function

#define FNV_OFFSET (14695981039346656037UL)
#define FNV_PRIME (1099511628211UL)

    uint64_t hash = FNV_OFFSET;
    while (*char_p) {
        hash ^= *(char_p++);
        hash *= FNV_PRIME;
    }
    return hash;
}

bool strmap_init(StrMap** strmap_pp) {
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
    if (*strmap_pp == NULL) {
        return false;
    }
    StrMapNodeList* arr_p = (*strmap_pp)->lists_p;
    size_t bucket_count = (*strmap_pp)->list_count;

    for (size_t i = 0; i < bucket_count; i++) {
        StrMapNode* node_p = arr_p[i].head_p;
        StrMapNode* next_p = NULL;

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

bool strmap_exists(StrMap* strmap_p, const char* key_p) {
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

char* strmap_get(StrMap* strmap_p, const char* key_p) {
    uint64_t hash = fnv_hash64((const unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);
    StrMapNode* node_p = strmap_p->lists_p[index].head_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return node_p->value_p;
        }
        node_p = node_p->next_p;
    }
    return strmap_default_get_value;
}

bool strmap_del(StrMap* strmap_p, const char* key_p) {
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);
    StrMapNode* node_p = strmap_p->lists_p[index].head_p;

    if (node_p == NULL) {
        return false;
    }
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

static StrMapNode* strmap_create_chain(StrMap* strmap_p, size_t list_count) {
    StrMapNode* head_p = NULL;
    StrMapNode* tail_p = NULL;
    StrMapNodeList* buckets_p = strmap_p->lists_p;

    size_t next_index = list_count;
    for (size_t i = 0; i < list_count; i++) {
        if (buckets_p[i].head_p == NULL) {
            continue;
        }
        head_p = buckets_p[i].head_p;
        tail_p = buckets_p[i].tail_p;
        next_index = i + 1;
        break;
    }
    for (size_t i = next_index; i < list_count; i++) {
        if (buckets_p[i].head_p == NULL) {
            continue;
        }
        tail_p->next_p = buckets_p[i].head_p;
        tail_p = buckets_p[i].tail_p;
    }

    return head_p;
}

static int strmap_resize_if_necessary(StrMap* strmap_p, size_t chain_length) {
    if (chain_length <= MAX_CHAIN_LENGTH) {
        return 0;
    }

    size_t old_list_count = strmap_p->list_count;
    size_t new_list_count = strmap_p->list_count << 1;
    StrMapNodeList* buckets_p = reallocarray(strmap_p->lists_p, new_list_count, sizeof(StrMapNodeList));
    if (buckets_p == NULL) {
        return -1;
    }
    strmap_p->lists_p = buckets_p;
    StrMapNode* node_p = strmap_create_chain(strmap_p, old_list_count);
    memset(buckets_p, 0, sizeof(StrMapNodeList) * new_list_count);

    size_t max_node_count = 0;
    while (node_p != NULL) {
        uint64_t hash = fnv_hash64((unsigned char*)node_p->key_p);
        uint64_t index = hash & (new_list_count - 1);

        StrMapNode* head_p = buckets_p[index].head_p;
        StrMapNode* next_temp_p = node_p->next_p;

        if (head_p == NULL) {
            buckets_p[index].head_p = node_p;
            buckets_p[index].tail_p = node_p;
            buckets_p[index].node_count++;
            node_p->next_p = NULL;
        } else {
            node_p->next_p = buckets_p[index].head_p;
            buckets_p[index].head_p = node_p;
            buckets_p[index].node_count++;
        }
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
        max_node_count = MAX(buckets_p[index].node_count, max_node_count);

        node_p = next_temp_p;
    }
    strmap_p->list_count = new_list_count;

    if (max_node_count > MAX_CHAIN_LENGTH) {
        return strmap_resize_if_necessary(strmap_p, max_node_count);
    }

    return 1;
}

static void* strmapnode_create(const char* key_p, const char* value_p) {
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
    uint64_t hash = fnv_hash64((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    int rtr_val = strmap_resize_if_necessary(strmap_p, strmap_p->lists_p[index].node_count + 1);
    if (rtr_val == -1) {
        return false;
    } else if (rtr_val == 1) {
        index = hash & (strmap_p->list_count - 1);
    }

    StrMapNode* node_p = strmap_p->lists_p[index].head_p;
    StrMapNode* prev_p = NULL;

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

    node_p = strmapnode_create(key_p, value_p);
    if (node_p == NULL) {
        return false;
    }

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
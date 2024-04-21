/*
    `strmap` is an implementation of string-to-string hash table using chaining.

    The hash function is hard coded to `fnv_hash64`, and the hash table is resized when the chain length
    exceeds a fixed number of nodes.

    Buckets are implemented as linked lists for simplicity, but may be replaced by fixed size arrays for additional
    performance.

    Strings are expected to have a null character (`\0` byte) at the end. Otherwise the functions may loop
    forever. Be careful about user inputs if security is important.
*/

#include <assert.h>   // static_assert
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool, true, false
#include <stdint.h>   // uint64_t
#include <stdlib.h>   // size_t, SIZE_MAX
#include <string.h>   // strcmp, strdup, strcpy

#include "allocators/allocator_function_types.h" // allocate_f, reallocate_f, deallocate_f
#include "allocators/std_allocator.h"            // std_allocate, std_reallocate, std_deallocate
#include "header-only/is_pow2.h"                 // is_pow2
#include "strmap.h"                              // strmap_*, STRMAP_GET_VALUE_DEFAULT

#define INITIAL_CAPACITY 16
#define MAX_CHAIN_LENGTH 5

static_assert(INITIAL_CAPACITY != 1, "subtracting initial capacity by one does not yield zero");
static_assert(INITIAL_CAPACITY != 0 && (INITIAL_CAPACITY & (INITIAL_CAPACITY - 1)) == 0, "initial capacity is a power of 2");

uint64_t fnvhash(const unsigned char* char_p) {
    // FNV-1a hash
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function

    assert(char_p != NULL);

    static const uint64_t FNV_OFFSET = 14695981039346656037UL;
    static const uint64_t FNV_PRIME = 1099511628211UL;

    uint64_t hash = FNV_OFFSET;
    while (*char_p != '\0') {
        hash ^= *(char_p++);
        hash *= FNV_PRIME;
    }

    return hash;
}

bool strmap_init_with_initial_capacity(strmap_type** strmap_pp, size_t pow2_capacity, void* allocator_struct_p,
                                       allocate_f allocate_f_p, reallocate_f reallocate_f_p, deallocate_f deallocate_f_p) {
    assert(strmap_pp != NULL);
    assert(is_pow2(pow2_capacity) && "initial capacity is a power of 2");
    assert(pow2_capacity - 1 != 0 && "subtracting initial capacity by one does not yield zero");

    *strmap_pp = NULL;
    if (pow2_capacity > SIZE_MAX / sizeof(strmap_node_list_type)) {
        return NULL;
    }
    *strmap_pp = allocate_f_p(allocator_struct_p, alignof(strmap_type), sizeof(strmap_type));
    if (*strmap_pp == NULL) {
        return false;
    }
    (*strmap_pp)->lists_arr_p =
        allocate_f_p(allocator_struct_p, alignof(strmap_node_list_type), pow2_capacity * sizeof(strmap_node_list_type));
    if ((*strmap_pp)->lists_arr_p == NULL) {
        deallocate_f_p(allocator_struct_p, *strmap_pp);
        *strmap_pp = NULL;
        return false;
    }
    (*strmap_pp)->list_count = pow2_capacity;
    for (size_t i = 0; i < (*strmap_pp)->list_count; i++) {
        (*strmap_pp)->lists_arr_p[i] = (strmap_node_list_type){.head_p = NULL, .tail_p = NULL, .node_count = 0};
    }
    (*strmap_pp)->total_nodes_count = 0;

    (*strmap_pp)->allocator_struct_p = allocator_struct_p;
    (*strmap_pp)->allocate_f_p = allocate_f_p;
    (*strmap_pp)->reallocate_f_p = reallocate_f_p;
    (*strmap_pp)->deallocate_f_p = deallocate_f_p;

    return true;
}

bool strmap_init(strmap_type** strmap_pp, void* allocator_struct_p, allocate_f allocate_f_p, reallocate_f realloc_f_p,
                 deallocate_f deallocate_f_p) {
    return strmap_init_with_initial_capacity(strmap_pp, INITIAL_CAPACITY, allocator_struct_p, allocate_f_p, realloc_f_p,
                                             deallocate_f_p);
}

strmap_type* strmap_create(void) {
    strmap_type* strmap_p = NULL;
    strmap_init(&strmap_p, NULL, std_allocate, std_reallocate, std_deallocate);
    return strmap_p;
}

void strmap_destroy(strmap_type* strmap_p) {
    assert(strmap_p != NULL);

    void* allocator_struct_p = strmap_p->allocator_struct_p;
    deallocate_f deallocate_f_p = strmap_p->deallocate_f_p;

    // iterate through lists
    for (size_t i = 0; i < strmap_p->list_count; i++) {
        strmap_node_type* node_p = strmap_p->lists_arr_p[i].head_p;
        strmap_node_type* next_p = NULL;

        // traverse list and free nodes one by one
        while (node_p != NULL) {
            next_p = node_p->next_p;

            deallocate_f_p(allocator_struct_p, node_p->key_p);
            deallocate_f_p(allocator_struct_p, node_p->value_p);
            deallocate_f_p(allocator_struct_p, node_p);

            node_p = next_p;
        }
    }

    deallocate_f_p(allocator_struct_p, strmap_p->lists_arr_p);
    deallocate_f_p(allocator_struct_p, strmap_p);
}

size_t strmap_get_count(const strmap_type* strmap_p) {
    return strmap_p->total_nodes_count;
}

bool strmap_contains(const strmap_type* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnvhash((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    strmap_node_type* node_p = strmap_p->lists_arr_p[index].head_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return true;
        }
        node_p = node_p->next_p;
    }

    return false;
}

const char* strmap_get(const strmap_type* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnvhash((const unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    strmap_node_type* node_p = strmap_p->lists_arr_p[index].head_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            return node_p->value_p;
        }
        node_p = node_p->next_p;
    }

    return STRMAP_GET_VALUE_DEFAULT;
}

bool strmap_del(strmap_type* strmap_p, const char* key_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);

    uint64_t hash = fnvhash((unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    strmap_node_type* node_p = strmap_p->lists_arr_p[index].head_p;

    if (node_p == NULL) {
        return false;
    }

    // check if key matches with the head of the list
    if (strcmp(node_p->key_p, key_p) == 0) {
        strmap_p->lists_arr_p[index].head_p = node_p->next_p;
        if (strmap_p->lists_arr_p[index].head_p == NULL) {
            strmap_p->lists_arr_p[index].tail_p = NULL;
        }

        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->key_p);
        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->value_p);
        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p);

        strmap_p->lists_arr_p[index].node_count--;
        strmap_p->total_nodes_count--;

        return true;
    }

    // otherwise iterate through list and compare key one by one
    strmap_node_type* prev_p = NULL;

    prev_p = node_p;
    node_p = node_p->next_p;

    while (node_p != NULL) {
        if (strcmp(node_p->key_p, key_p) == 0) {
            prev_p->next_p = node_p->next_p;
            if (prev_p->next_p == NULL) {
                strmap_p->lists_arr_p[index].tail_p = prev_p;
            }

            strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->key_p);
            strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->value_p);
            strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p);

            strmap_p->lists_arr_p[index].node_count--;
            strmap_p->total_nodes_count--;

            return true;
        }

        prev_p = node_p;
        node_p = node_p->next_p;
    }

    return false;
}

strmap_node_type* strmap_create_flattened_list(strmap_type* strmap_p, size_t list_count) {
    assert(strmap_p != NULL);

    strmap_node_type* head_p = NULL;
    strmap_node_type* tail_p = NULL;

    strmap_node_list_type* lists_p = strmap_p->lists_arr_p;
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

int strmap_grow_if_necessary(strmap_type* strmap_p, size_t chain_length) {
    assert(strmap_p != NULL);

    if (chain_length <= MAX_CHAIN_LENGTH) {
        return 0;
    }

    size_t old_list_count = strmap_p->list_count;
    size_t new_list_count = strmap_p->list_count << 1;

    if (new_list_count > SIZE_MAX / sizeof(strmap_node_list_type)) {
        return -1;
    }

    strmap_node_list_type* lists_p =
        strmap_p->reallocate_f_p(strmap_p->allocator_struct_p, strmap_p->lists_arr_p, alignof(strmap_node_list_type),
                                 old_list_count * sizeof(strmap_node_list_type), new_list_count * sizeof(strmap_node_list_type));
    if (lists_p == NULL) {
        return -1;
    }
    strmap_p->lists_arr_p = lists_p;

    // create seperate flattened list (only using memory on the stack)
    strmap_node_type* node_p = strmap_create_flattened_list(strmap_p, old_list_count);

    // set to default values
    for (size_t i = 0; i < new_list_count; i++) {
        lists_p[i] = (strmap_node_list_type){.head_p = NULL, .tail_p = NULL, .node_count = 0};
    }
    strmap_p->total_nodes_count = 0;

    // reinsert every node
    while (node_p != NULL) {
        uint64_t hash = fnvhash((unsigned char*)node_p->key_p);
        uint64_t index = hash & (new_list_count - 1);

        strmap_node_type* head_p = lists_p[index].head_p;
        strmap_node_type* next_temp_p = node_p->next_p;

        // insert node by the head
        if (head_p == NULL) {
            lists_p[index].head_p = node_p;
            lists_p[index].tail_p = node_p;
            node_p->next_p = NULL;
        } else {
            node_p->next_p = lists_p[index].head_p;
            lists_p[index].head_p = node_p;
        }

        lists_p[index].node_count++;
        strmap_p->total_nodes_count++;

        node_p = next_temp_p;
    }
    strmap_p->list_count = new_list_count;

    return 1;
}

strmap_node_type* strmap_node_create(strmap_type* strmap_p, const char* key_p, const char* value_p) {
    assert(key_p != NULL);
    assert(value_p != NULL);

    strmap_node_type* node_p =
        strmap_p->allocate_f_p(strmap_p->allocator_struct_p, alignof(strmap_node_type), sizeof(strmap_node_type));
    if (node_p == NULL) {
        return NULL;
    }
    size_t key_size = strlen(key_p) + 1;
    node_p->key_p = strmap_p->allocate_f_p(strmap_p->allocator_struct_p, alignof(char), sizeof(char) * key_size);
    if (node_p->key_p == NULL) {
        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p);
        return NULL;
    }
    size_t value_size = strlen(value_p) + 1;
    node_p->value_p = strmap_p->allocate_f_p(strmap_p->allocator_struct_p, alignof(char), sizeof(char) * value_size);
    if (node_p->value_p == NULL) {
        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->key_p);
        strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p);
        return NULL;
    }
    node_p->next_p = NULL;

    memcpy(node_p->key_p, key_p, key_size);
    memcpy(node_p->value_p, value_p, value_size);

    return node_p;
}

bool strmap_set(strmap_type* strmap_p, const char* key_p, const char* value_p) {
    assert(strmap_p != NULL);
    assert(key_p != NULL);
    assert(value_p != NULL);

    uint64_t hash = fnvhash((const unsigned char*)key_p);
    uint64_t index = hash & (strmap_p->list_count - 1);

    int rtr_val = strmap_grow_if_necessary(strmap_p, strmap_p->lists_arr_p[index].node_count + 1);
    if (rtr_val == -1) {
        return false;
    } else if (rtr_val == 1) {
        index = hash & (strmap_p->list_count - 1);
    }

    strmap_node_type* node_p = strmap_p->lists_arr_p[index].head_p;
    strmap_node_type* prev_p = NULL;

    // iterate through list, compare keys, and replace value if key matches.
    while (node_p != NULL) {
        prev_p = node_p;

        if (strcmp(node_p->key_p, key_p) == 0) {
            strmap_p->deallocate_f_p(strmap_p->allocator_struct_p, node_p->value_p);

            size_t value_size = strlen(value_p) + 1;
            node_p->value_p = strmap_p->allocate_f_p(strmap_p->allocator_struct_p, alignof(char), sizeof(char) * value_size);

            if (node_p->value_p == NULL) {
                return false;
            }

            memcpy(node_p->value_p, value_p, value_size);

            return true;
        }

        node_p = node_p->next_p;
    }

    // otherwise create a new node
    node_p = strmap_node_create(strmap_p, key_p, value_p);
    if (node_p == NULL) {
        return false;
    }

    // and insert the node at by the head
    if (prev_p == NULL) {
        strmap_p->lists_arr_p[index].head_p = node_p;
        strmap_p->lists_arr_p[index].tail_p = node_p;
    } else {
        node_p->next_p = strmap_p->lists_arr_p[index].head_p;
        strmap_p->lists_arr_p[index].head_p = node_p;
    }
    strmap_p->lists_arr_p[index].node_count++;
    strmap_p->total_nodes_count++;

    return true;
}

strmap_type* strmap_clone(const strmap_type* strmap_src_p) {
    assert(strmap_src_p != NULL);

    strmap_type* strmap_dest_p = NULL;
    if (!strmap_init_with_initial_capacity(&strmap_dest_p, strmap_src_p->list_count, strmap_src_p->allocator_struct_p,
                                           strmap_src_p->allocate_f_p, strmap_src_p->reallocate_f_p, strmap_src_p->deallocate_f_p)) {
        return NULL;
    }
    size_t list_index;
    strmap_node_type* next_p = NULL;
    char* key_p = NULL;
    char* value_p = NULL;
    strmap_for_each(strmap_src_p, list_index, next_p, key_p, value_p) {
        strmap_set(strmap_dest_p, key_p, value_p);
    }

    return strmap_dest_p;
}

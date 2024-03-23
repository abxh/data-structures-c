#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t, NULL

typedef struct StrMapNode {
    struct StrMapNode* next_p;
    char* key_p;
    char* value_p;
} StrMapNode;

typedef struct {
    size_t node_count;
    StrMapNode* head_p;
    StrMapNode* tail_p;
} StrMapNodeList;

typedef struct {
    size_t list_count;
    StrMapNodeList* lists_p;
} StrMap;

#ifndef STRMAP_GET_VALUE_DEFAULT
#define STRMAP_GET_VALUE_DEFAULT NULL
#endif

#ifndef DEFAULT_CAPACITY
#define DEFAULT_CAPACITY 16
#endif

#ifndef MAX_CHAIN_LENGTH
#define MAX_CHAIN_LENGTH 5
#endif

bool strmap_init(StrMap** strmap_pp);

bool strmap_deinit(StrMap** strmap_pp);

size_t strmap_count(const StrMap* strmap_p);

bool strmap_exists(const StrMap* strmap_p, const char* key_p);

const char* strmap_get(const StrMap* strmap_p, const char* key_p);

bool strmap_set(StrMap* strmap_p, const char* key_p, const char* value_p);

bool strmap_del(StrMap* strmap_p, const char* key_p);

#define strmap_for_each(strmap_p, list_index, next_p, key_p, value_p)                                                            \
    for ((list_index) = 0; (list_index) < (strmap_p)->list_count && ((next_p) = (strmap_p)->lists_p[(list_index)].head_p, true); \
         (list_index)++)                                                                                                         \
        while ((next_p) != NULL && ((key_p) = (next_p)->key_p, (value_p) = (next_p)->value_p, (next_p) = (next_p)->next_p, true))

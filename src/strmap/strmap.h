#pragma once

#include <stdbool.h> // bool
#include <stdint.h>  // uint64_t
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

#define STRMAP_DEFAULT_GET_VALUE NULL

bool strmap_init(StrMap** strmap_pp);

bool strmap_deinit(StrMap** strmap_pp);

bool strmap_exists(const StrMap* strmap_p, const char* key_p);

char* strmap_get(const StrMap* strmap_p, const char* key_p);

bool strmap_set(StrMap* strmap_p, const char* key_p, const char* value_p);

bool strmap_del(StrMap* strmap_p, const char* key_p);

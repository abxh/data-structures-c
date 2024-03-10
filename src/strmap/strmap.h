#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct StrMapNode {
    struct StrMapNode* next_p;
    char* key_p;
    char* value_p;
} StrMapNode;

typedef struct {
    size_t capacity_sub_one;
    size_t used;
    size_t value_size;
    StrMapNode** arr_pp;
} StrMap;

bool strmap_init(StrMap** strmap_pp);

bool strmap_deinit(StrMap** strmap_pp);

bool strmap_exists(StrMap* strmap_p, const char* key_p);

char* strmap_get(StrMap* strmap_p, const char* key_p);

bool strmap_set(StrMap* strmap_p, const char* key_p, const char* value_p);

bool strmap_del(StrMap* strmap_p, const char* key_p);

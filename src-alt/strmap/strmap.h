#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t, NULL

typedef struct strmap_node_type {
    struct strmap_node_type* next_p;
    char* key_p;
    char* value_p;
} strmap_node_type;

typedef struct {
    size_t node_count;
    strmap_node_type* head_p;
    strmap_node_type* tail_p;
} strmap_node_list_type;

typedef struct {
    size_t total_nodes_count;
    size_t list_count;
    strmap_node_list_type* lists_arr_p;
} strmap_type;

#define STRMAP_GET_VALUE_DEFAULT NULL

bool strmap_init(strmap_type** strmap_pp);

bool strmap_init_with_initial_capacity(strmap_type** strmap_pp, size_t pow2_capacity);

bool strmap_deinit(strmap_type** strmap_pp);

bool strmap_copy(strmap_type** strmap_dest_pp, const strmap_type* strmap_src_p);

size_t strmap_get_count(const strmap_type* strmap_p);

bool strmap_exists(const strmap_type* strmap_p, const char* key_p);

const char* strmap_get(const strmap_type* strmap_p, const char* key_p);

bool strmap_set(strmap_type* strmap_p, const char* key_p, const char* value_p);

bool strmap_del(strmap_type* strmap_p, const char* key_p);

#define strmap_for_each(strmap_p, list_index, strmap_node_next_p, key_p, value_p)                                                 \
    for ((list_index) = 0;                                                                                                        \
         (list_index) < (strmap_p)->list_count && ((strmap_node_next_p) = (strmap_p)->lists_arr_p[(list_index)].head_p, true);        \
         (list_index)++)                                                                                                          \
        while ((strmap_node_next_p) != NULL && ((key_p) = (strmap_node_next_p)->key_p, (value_p) = (strmap_node_next_p)->value_p, \
                                                (strmap_node_next_p) = (strmap_node_next_p)->strmap_node_next_p, true))

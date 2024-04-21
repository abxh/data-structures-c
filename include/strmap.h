#pragma once

#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // size_t, NULL

#include "allocators/allocator_function_types.h" // allocate_f, reallocate_f, deallocate_f

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

    void* allocator_struct_p;
    allocate_f allocate_f_p;
    reallocate_f reallocate_f_p;
    deallocate_f deallocate_f_p;
} strmap_type;

#define STRMAP_GET_VALUE_DEFAULT NULL

strmap_type* strmap_create();

void strmap_destroy(strmap_type* strmap_p);

strmap_type* strmap_clone(const strmap_type* strmap_src_p);

bool strmap_init(strmap_type** strmap_pp, void* allocator_struct_p, allocate_f allocate_f_p, reallocate_f realloc_f_p,
                 deallocate_f deallocate_f_p);

bool strmap_init_with_initial_capacity(strmap_type** strmap_pp, size_t pow2_capacity, void* allocator_struct_p,
                                       allocate_f allocate_f_p, reallocate_f reallocate_f_p, deallocate_f deallocate_f_p);

size_t strmap_get_count(const strmap_type* strmap_p);

bool strmap_contains(const strmap_type* strmap_p, const char* key_p);

const char* strmap_get(const strmap_type* strmap_p, const char* key_p);

bool strmap_set(strmap_type* strmap_p, const char* key_p, const char* value_p);

bool strmap_del(strmap_type* strmap_p, const char* key_p);

#define strmap_for_each(strmap_p, out_list_index, out_strmap_node_next_p, out_key_p, out_value_p)                   \
    for ((out_list_index) = 0; (out_list_index) < (strmap_p)->list_count &&                                         \
                               ((out_strmap_node_next_p) = (strmap_p)->lists_arr_p[(out_list_index)].head_p, true); \
         (out_list_index)++)                                                                                        \
        while ((out_strmap_node_next_p) != NULL &&                                                                  \
               ((out_key_p) = (out_strmap_node_next_p)->key_p, (out_value_p) = (out_strmap_node_next_p)->value_p,   \
                (out_strmap_node_next_p) = (out_strmap_node_next_p)->next_p, true))

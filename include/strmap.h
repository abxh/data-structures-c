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

#define strmap_for_each(p, i, n, k, v)                                                        \
    for ((i) = 0; (i) < (p)->list_count && ((n) = (p)->lists_arr_p[(i)].head_p, true); (i)++) \
        while ((n) != NULL && ((k) = (n)->key_p, (v) = (n)->value_p, (n) = (n)->next_p, true))

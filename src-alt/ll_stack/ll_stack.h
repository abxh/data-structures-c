#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct ll_stack_node_type {
    struct ll_stack_node_type* next_p;
    void* value_p;
} ll_stack_node_type;

typedef struct {
    ll_stack_node_type* head_p;
    size_t value_size;
    size_t count;

    ll_stack_node_type* freelist_p;
} ll_stack_type;

bool ll_stack_init(ll_stack_type** ll_stack_pp, const size_t value_size);

bool ll_stack_deinit(ll_stack_type** ll_stack_pp);

bool ll_stack_copy(ll_stack_type** ll_stack_dest_pp, const ll_stack_type* ll_stack_src_p);

const ll_stack_node_type* ll_stack_peek_node(const ll_stack_type* ll_stack_p);

size_t ll_stack_count(const ll_stack_type* ll_stack_p);

bool ll_stack_is_empty(const ll_stack_type* ll_stack_p);

void ll_stack_push_node(ll_stack_type* ll_stack_p, ll_stack_node_type* ll_stack_node_p);

ll_stack_node_type* ll_stack_pop_node(ll_stack_type* ll_stack_p);

ll_stack_node_type* ll_stack_node_create(ll_stack_type* ll_stack_p);

void ll_stack_node_free(ll_stack_type* ll_stack_p, ll_stack_node_type* ll_stack_node_p);

#define ll_stack_for_each(ll_stack_p, ll_stack_node_p, value)                                                                         \
    for ((ll_stack_node_p) = (ll_stack_p)->head_p; (node_p) != NULL && ((value) = *(typeof(value)*)(ll_stack_node_p)->value_p, true); \
         (ll_stack_node_p) = (ll_stack_node_p)->next_p)

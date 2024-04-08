#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct ll_queue_node_type {
    struct ll_queue_node_type* next_p;
    void* value_p;
} ll_queue_node_type;

typedef struct {
    ll_queue_node_type* head_p;
    ll_queue_node_type* tail_p;
    size_t count;
    size_t value_size;

    ll_queue_node_type* freelist_p;
} ll_queue_type;

bool ll_queue_init(ll_queue_type** ll_queue_pp, size_t value_size);

bool ll_queue_deinit(ll_queue_type** ll_queue_pp);

bool ll_queue_copy(ll_queue_type** ll_queue_dest_pp, const ll_queue_type* ll_queue_src_p);

bool ll_queue_is_empty(const ll_queue_type* ll_queue_p);

size_t ll_queue_get_count(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_node(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_first_node(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_last_node(const ll_queue_type* ll_queue_p);

bool ll_queue_enqueue_node(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p);

ll_queue_node_type* ll_queue_dequeue_node(ll_queue_type* ll_queue_p);

ll_queue_node_type* ll_queue_node_create(ll_queue_type* ll_queue_p);

void ll_queue_node_free(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p);

#define ll_queue_for_each(ll_queue_p, ll_queue_node_p, value)                                        \
    for ((ll_queue_node_p) = (ll_queue_p)->head_p;                                                   \
         (ll_queue_node_p) != NULL && ((value) = *(typeof(value)*)(ll_queue_node_p)->value_p, true); \
         (ll_queue_node_p) = (ll_queue_node_p)->next_p)

#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct ll_queue_node_type {
    struct ll_queue_node_type* next_p;
    void* value_p;
    size_t value_size;
} ll_queue_node_type;

typedef struct ll_queue_type {
    ll_queue_node_type* head_p;
    ll_queue_node_type* tail_p;
    size_t count;
} ll_queue_type;

bool ll_queue_init(ll_queue_type** ll_queue_pp);

bool ll_queue_deinit(ll_queue_type** ll_queue_pp);

bool ll_queue_copy(ll_queue_type** ll_queue_dest_pp, const ll_queue_type* ll_queue_src_p);

bool ll_queue_is_empty(const ll_queue_type* ll_queue_p);

size_t ll_queue_get_count(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_node(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_first_node(const ll_queue_type* ll_queue_p);

const ll_queue_node_type* ll_queue_peek_last_node(const ll_queue_type* ll_queue_p);

bool ll_queue_enqueue_node(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p);

ll_queue_node_type* ll_queue_dequeue_node(ll_queue_type* ll_queue_p);

ll_queue_node_type* ll_queue_node_create(size_t value_size);

void ll_queue_node_destroy(ll_queue_node_type* ll_queue_node_p);

#define ll_queue_peek(type, ll_queue_p) (*((type*)(ll_queue_peek_node(ll_queue_p)->value_p)))

#define ll_queue_peek_first(type, ll_queue_p) (*((type*)(ll_queue_peek_first_node(ll_queue_p)->value_p)))

#define ll_queue_peek_last(type, ll_queue_p) (*((type*)(ll_queue_peek_last_node(ll_queue_p)->value_p)))

#define ll_queue_enqueue(type, ll_queue_p, value, out_rtr_bool)          \
    do {                                                                 \
        ll_queue_node_type* node_p = ll_queue_node_create(sizeof(type)); \
        if (node_p == NULL) {                                            \
            (out_rtr_bool) = false;                                      \
        } else {                                                         \
            *(type*)node_p->value_p = value;                             \
            ll_queue_enqueue_node((ll_queue_p), node_p);                 \
            (out_rtr_bool) = true;                                       \
        }                                                                \
    } while (false)

#define ll_queue_dequeue(type, ll_queue_p, out_rtr_value)               \
    do {                                                                \
        ll_queue_node_type* node_p = ll_queue_dequeue_node(ll_queue_p); \
        out_rtr_value = *(type*)node_p->value_p;                        \
        ll_queue_node_destroy(node_p);                                  \
    } while (false)

#define ll_queue_for_each(ll_queue_p, out_ll_queue_node_p, out_value_p)                           \
    for ((out_ll_queue_node_p) = (ll_queue_p)->head_p;                                            \
         (out_ll_queue_node_p) != NULL && ((out_value_p) = (out_ll_queue_node_p)->value_p, true); \
         (out_ll_queue_node_p) = (out_ll_queue_node_p)->next_p)

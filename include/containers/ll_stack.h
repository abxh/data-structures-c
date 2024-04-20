#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct ll_stack_node_type {
    struct ll_stack_node_type* next_p;
    void* value_p;
    size_t value_size;
} ll_stack_node_type;

typedef struct ll_stack_type {
    ll_stack_node_type* head_p;
    size_t count;
} ll_stack_type;

bool ll_stack_init(ll_stack_type** ll_stack_pp);

bool ll_stack_deinit(ll_stack_type** ll_stack_pp);

bool ll_stack_copy(ll_stack_type** ll_stack_dest_pp, const ll_stack_type* ll_stack_src_p);

const ll_stack_node_type* ll_stack_peek_node(const ll_stack_type* ll_stack_p);

size_t ll_stack_get_count(const ll_stack_type* ll_stack_p);

bool ll_stack_is_empty(const ll_stack_type* ll_stack_p);

void ll_stack_push_node(ll_stack_type* ll_stack_p, ll_stack_node_type* ll_stack_node_p);

ll_stack_node_type* ll_stack_pop_node(ll_stack_type* ll_stack_p);

ll_stack_node_type* ll_stack_node_create(size_t value_size);

void ll_stack_node_destroy(ll_stack_node_type* ll_stack_node_p);

#define ll_stack_peek(type, ll_stack_p) (*(type*)(ll_stack_peek_node(ll_stack_p)->value_p))

#define ll_stack_push(type, ll_stack_p, value, out_rtr_bool)             \
    do {                                                                 \
        ll_stack_node_type* node_p = ll_stack_node_create(sizeof(type)); \
        if (node_p == NULL) {                                            \
            (out_rtr_bool) = false;                                      \
        } else {                                                         \
            *(type*)node_p->value_p = value;                             \
            ll_stack_push_node((ll_stack_p), node_p);                    \
            (out_rtr_bool) = true;                                       \
        }                                                                \
    } while (false)

#define ll_stack_pop(type, ll_stack_p, out_rtr_value)               \
    do {                                                            \
        ll_stack_node_type* node_p = ll_stack_pop_node(ll_stack_p); \
        (out_rtr_value) = *(type*)node_p->value_p;                  \
        ll_stack_node_destroy(node_p);                              \
    } while (false)

#define ll_stack_for_each(ll_stack_p, out_ll_stack_node_p, out_value_p)                           \
    for ((out_ll_stack_node_p) = (ll_stack_p)->head_p;                                            \
         (out_ll_stack_node_p) != NULL && ((out_value_p) = (out_ll_stack_node_p)->value_p, true); \
         (out_ll_stack_node_p) = (out_ll_stack_node_p)->next_p)

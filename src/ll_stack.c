/*
    `ll_stack` is an implementation of a generic stack using a linked list.
*/

#include <assert.h>   // assert
#include <stdalign.h> // alignof
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // NULL, size_t
#include <string.h>   // memcpy

#include "allocators/allocator_function_types.h" // allocate_f, deallocate_f
#include "allocators/std_allocator.h"            // std_allocate, std_deallocate
#include "containers/ll_stack.h"                 // ll_stack_*

bool ll_stack_init(ll_stack_type** ll_stack_pp, void* allocator_struct_p, allocate_f allocate_f_p, deallocate_f deallocate_f_p) {
    *ll_stack_pp = allocate_f_p(allocator_struct_p, alignof(ll_stack_type), sizeof(ll_stack_type));
    if (*ll_stack_pp == NULL) {
        return false;
    }
    (*ll_stack_pp)->head_p = NULL;
    (*ll_stack_pp)->count = 0;

    (*ll_stack_pp)->allocator_struct_p = allocator_struct_p;
    (*ll_stack_pp)->allocate_f_p = allocate_f_p;
    (*ll_stack_pp)->deallocate_f_p = deallocate_f_p;

    return true;
}

ll_stack_type* ll_stack_create() {
    ll_stack_type* ll_stack_p = NULL;
    ll_stack_init(&ll_stack_p, NULL, std_allocate, std_deallocate);
    return ll_stack_p;
}

void ll_stack_destroy(ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);

    void* allocator_struct_p = ll_stack_p->allocator_struct_p;
    deallocate_f deallocate_f_p = ll_stack_p->deallocate_f_p;

    // traverse list and free nodes one by one
    ll_stack_node_type* head_p = ll_stack_p->head_p;
    ll_stack_node_type* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        deallocate_f_p(allocator_struct_p, head_p->value_p);
        deallocate_f_p(allocator_struct_p, head_p);
        head_p = next_p;
    }

    deallocate_f_p(allocator_struct_p, ll_stack_p);
}

size_t ll_stack_get_count(const ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);

    return ll_stack_p->count;
}

bool ll_stack_is_empty(const ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);

    return ll_stack_p->head_p == NULL;
}

ll_stack_node_type* ll_stack_peek_node(const ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);
    assert(ll_stack_is_empty(ll_stack_p) == false);

    return ll_stack_p->head_p;
}

void ll_stack_push_node(ll_stack_type* ll_stack_p, ll_stack_node_type* ll_stack_node_p) {
    assert(ll_stack_p != NULL);

    ll_stack_node_p->next_p = ll_stack_p->head_p;
    ll_stack_p->head_p = ll_stack_node_p;
    ll_stack_p->count++;
}

ll_stack_node_type* ll_stack_pop_node(ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);
    assert(ll_stack_is_empty(ll_stack_p) == false);

    ll_stack_node_type* ll_stack_node_p = ll_stack_p->head_p;
    ll_stack_p->head_p = ll_stack_node_p->next_p;
    ll_stack_p->count--;

    return ll_stack_node_p;
}

ll_stack_node_type* ll_stack_node_create(ll_stack_type* ll_stack_p, size_t value_alignment, size_t value_size) {
    assert(ll_stack_p != NULL);
    assert(value_size != 0);

    ll_stack_node_type* ll_stack_node_p =
        ll_stack_p->allocate_f_p(ll_stack_p->allocator_struct_p, alignof(ll_stack_node_type), sizeof(ll_stack_node_type));
    if (ll_stack_node_p == NULL) {
        return NULL;
    }
    ll_stack_node_p->value_p = ll_stack_p->allocate_f_p(ll_stack_p->allocator_struct_p, value_alignment, value_size);
    if (ll_stack_node_p->value_p == NULL) {
        ll_stack_p->deallocate_f_p(ll_stack_p->allocator_struct_p, ll_stack_node_p);
        return NULL;
    }
    ll_stack_node_p->value_size = value_size;
    ll_stack_node_p->value_alignment = value_alignment;
    ll_stack_node_p->next_p = NULL;

    return ll_stack_node_p;
}

void ll_stack_node_destroy(ll_stack_type* ll_stack_p, ll_stack_node_type* ll_stack_node_p) {
    assert(ll_stack_p != NULL);
    assert(ll_stack_node_p != NULL);

    ll_stack_p->deallocate_f_p(ll_stack_p->allocator_struct_p, ll_stack_node_p->value_p);
    ll_stack_p->deallocate_f_p(ll_stack_p->allocator_struct_p, ll_stack_node_p);
}

ll_stack_type* ll_stack_clone(const ll_stack_type* ll_stack_src_p) {
    assert(ll_stack_src_p != NULL);

    ll_stack_type* ll_stack_dest_p = NULL;
    if (!ll_stack_init(&ll_stack_dest_p, ll_stack_src_p->allocator_struct_p, ll_stack_src_p->allocate_f_p,
                       ll_stack_src_p->deallocate_f_p)) {
        return NULL;
    }

    ll_stack_node_type* src_head_p = ll_stack_src_p->head_p;
    ll_stack_node_type* dest_tail_p = NULL;
    while (src_head_p != NULL) {
        ll_stack_node_type* head_copy_p = ll_stack_node_create(ll_stack_dest_p, src_head_p->value_alignment, src_head_p->value_size);
        if (head_copy_p == NULL) {
            ll_stack_destroy(ll_stack_dest_p);
            return NULL;
        }
        memcpy(head_copy_p->value_p, src_head_p->value_p, src_head_p->value_size);
        if (ll_stack_dest_p->head_p == NULL) {
            ll_stack_dest_p->head_p = head_copy_p;
            dest_tail_p = head_copy_p;
        } else {
            dest_tail_p->next_p = head_copy_p;
            dest_tail_p = head_copy_p;
        }
        src_head_p = src_head_p->next_p;
    }
    ll_stack_dest_p->count = ll_stack_src_p->count;

    return ll_stack_dest_p;
}

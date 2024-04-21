#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t
#include <string.h>  // memcpy

#include "allocators/allocator_function_types.h" // allocate_f, deallocate_f
#include "allocators/std_allocator.h"            // std_allocate, std_deallocate
#include "containers/ll_queue.h"                 // ll_queue_*

bool ll_queue_init(ll_queue_type** ll_queue_pp, void* allocator_struct_p, allocate_f allocate_f_p, deallocate_f deallocate_f_p) {
    assert(ll_queue_pp != NULL);

    *ll_queue_pp = allocate_f_p(allocator_struct_p, alignof(ll_queue_type), sizeof(ll_queue_type));
    if ((*ll_queue_pp) == NULL) {
        return false;
    }
    (*ll_queue_pp)->head_p = NULL;
    (*ll_queue_pp)->tail_p = NULL;
    (*ll_queue_pp)->count = 0;

    (*ll_queue_pp)->allocator_struct_p = allocator_struct_p;
    (*ll_queue_pp)->allocate_f_p = allocate_f_p;
    (*ll_queue_pp)->deallocate_f_p = deallocate_f_p;

    return true;
}

ll_queue_type* ll_queue_create() {
    ll_queue_type* ll_queue_p = NULL;
    ll_queue_init(&ll_queue_p, NULL, std_allocate, std_deallocate);
    return ll_queue_p;
}

void ll_queue_destroy(ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    void* allocator_struct_p = ll_queue_p->allocator_struct_p;
    deallocate_f deallocate_f_p = ll_queue_p->deallocate_f_p;

    // traverse list and free nodes one by one
    ll_queue_node_type* head_p = ll_queue_p->head_p;
    ll_queue_node_type* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        deallocate_f_p(allocator_struct_p, head_p->value_p);
        deallocate_f_p(allocator_struct_p, head_p);
        head_p = next_p;
    }

    deallocate_f_p(allocator_struct_p, ll_queue_p);
}

bool ll_queue_is_empty(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    return ll_queue_p->head_p == NULL;
}

size_t ll_queue_get_count(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    return ll_queue_p->count;
}

ll_queue_node_type* ll_queue_peek_node(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    return ll_queue_p->head_p;
}

ll_queue_node_type* ll_queue_peek_first_node(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    return ll_queue_p->head_p;
}

ll_queue_node_type* ll_queue_peek_last_node(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    return ll_queue_p->tail_p;
}

bool ll_queue_enqueue_node(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_node_p != NULL);

    ll_queue_node_p->next_p = NULL;
    if (ll_queue_p->head_p == NULL) {
        ll_queue_p->head_p = ll_queue_node_p;
        ll_queue_p->tail_p = ll_queue_node_p;
    } else {
        ll_queue_p->tail_p->next_p = ll_queue_node_p;
        ll_queue_p->tail_p = ll_queue_node_p;
    }
    ll_queue_p->count++;

    return true;
}

ll_queue_node_type* ll_queue_dequeue_node(ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    ll_queue_node_type* ll_queue_node_p = ll_queue_p->head_p;
    ll_queue_p->head_p = ll_queue_node_p->next_p;
    if (ll_queue_p->head_p == NULL) {
        ll_queue_p->tail_p = NULL;
    }
    ll_queue_p->count--;

    return ll_queue_node_p;
}

ll_queue_node_type* ll_queue_node_create(ll_queue_type* ll_queue_p, size_t value_alignment, size_t value_size) {
    assert(ll_queue_p != NULL);
    assert(value_size != 0);

    ll_queue_node_type* ll_queue_node_p =
        ll_queue_p->allocate_f_p(ll_queue_p->allocator_struct_p, alignof(ll_queue_node_type), sizeof(ll_queue_node_type));
    if (ll_queue_node_p == NULL) {
        return NULL;
    }
    ll_queue_node_p->value_p = ll_queue_p->allocate_f_p(ll_queue_p->allocator_struct_p, value_alignment, value_size);
    if (ll_queue_node_p->value_p == NULL) {
        ll_queue_p->deallocate_f_p(ll_queue_p->allocator_struct_p, ll_queue_node_p);
        return NULL;
    }
    ll_queue_node_p->value_size = value_size;
    ll_queue_node_p->value_alignment = value_alignment;
    ll_queue_node_p->next_p = NULL;

    return ll_queue_node_p;
}

void ll_queue_node_destroy(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p) {
    assert(ll_queue_node_p != NULL);

    ll_queue_p->deallocate_f_p(ll_queue_p->allocator_struct_p, ll_queue_node_p->value_p);
    ll_queue_p->deallocate_f_p(ll_queue_p->allocator_struct_p, ll_queue_node_p);
}

ll_queue_type* ll_queue_clone(const ll_queue_type* ll_queue_src_p) {
    assert(ll_queue_src_p != NULL);

    ll_queue_type* ll_queue_dest_p = NULL;
    if (!ll_queue_init(&ll_queue_dest_p, ll_queue_src_p->allocator_struct_p, ll_queue_src_p->allocate_f_p,
                       ll_queue_src_p->deallocate_f_p)) {
        return NULL;
    }

    ll_queue_node_type* head_p = ll_queue_src_p->head_p;
    while (head_p != NULL) {
        ll_queue_node_type* head_copy_p = ll_queue_node_create(ll_queue_dest_p, head_p->value_alignment, head_p->value_size);
        if (head_copy_p == NULL) {
            ll_queue_destroy(ll_queue_dest_p);
            return NULL;
        }
        memcpy(head_copy_p->value_p, head_p->value_p, head_p->value_size);
        ll_queue_enqueue_node(ll_queue_dest_p, head_copy_p);
        head_p = head_p->next_p;
    }
    ll_queue_dest_p->count = ll_queue_src_p->count;

    return ll_queue_dest_p;
}

/*
    `ll_stack` is an implementation of a generic stack using a linked list.
*/

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "containers/ll_stack.h" // ll_stack_*

bool ll_stack_init(ll_stack_type** ll_stack_pp) {
    assert(ll_stack_pp != NULL);

    *ll_stack_pp = malloc(sizeof(ll_stack_type));
    if (*ll_stack_pp == NULL) {
        return false;
    }
    (*ll_stack_pp)->head_p = NULL;
    (*ll_stack_pp)->count = 0;

    return true;
}

bool ll_stack_deinit(ll_stack_type** ll_stack_pp) {
    assert(ll_stack_pp != NULL);

    // traverse list and free nodes one by one
    ll_stack_node_type* head_p = (*ll_stack_pp)->head_p;
    ll_stack_node_type* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }

    free(*ll_stack_pp);
    (*ll_stack_pp) = NULL;

    return true;
}

size_t ll_stack_get_count(const ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);

    return ll_stack_p->count;
}

bool ll_stack_is_empty(const ll_stack_type* ll_stack_p) {
    assert(ll_stack_p != NULL);

    return ll_stack_p->head_p == NULL;
}

const ll_stack_node_type* ll_stack_peek_node(const ll_stack_type* ll_stack_p) {
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

ll_stack_node_type* ll_stack_node_create(size_t value_size) {
    assert(value_size != 0);

    ll_stack_node_type* ll_stack_node_p = malloc(sizeof(ll_stack_node_type));
    if (ll_stack_node_p == NULL) {
        return NULL;
    }
    ll_stack_node_p->value_p = malloc(value_size);
    if (ll_stack_node_p->value_p == NULL) {
        free(ll_stack_node_p);
        return NULL;
    }
    ll_stack_node_p->value_size = value_size;
    ll_stack_node_p->next_p = NULL;

    return ll_stack_node_p;
}

void ll_stack_node_destroy(ll_stack_node_type* ll_stack_node_p) {
    assert(ll_stack_node_p != NULL);

    free(ll_stack_node_p->value_p);
    free(ll_stack_node_p);
}

bool ll_stack_copy(ll_stack_type** ll_dest_stack_pp, const ll_stack_type* ll_stack_src_p) {
    assert(ll_stack_src_p != NULL);
    assert(ll_dest_stack_pp != NULL);

    if (!ll_stack_init(ll_dest_stack_pp)) {
        return false;
    }

    ll_stack_node_type* src_head_p = ll_stack_src_p->head_p;
    ll_stack_node_type* dest_tail_p = NULL;
    while (src_head_p != NULL) {
        ll_stack_node_type* head_copy_p = ll_stack_node_create(src_head_p->value_size);
        if (head_copy_p == NULL) {
            ll_stack_deinit(ll_dest_stack_pp);
            return false;
        }
        memcpy(head_copy_p->value_p, src_head_p->value_p, src_head_p->value_size);
        if ((*ll_dest_stack_pp)->head_p == NULL) {
            (*ll_dest_stack_pp)->head_p = head_copy_p;
            dest_tail_p = head_copy_p;
        } else {
            dest_tail_p->next_p = head_copy_p;
            dest_tail_p = head_copy_p;
        }
        src_head_p = src_head_p->next_p;
    }
    (*ll_dest_stack_pp)->count = ll_stack_src_p->count;

    return true;
}

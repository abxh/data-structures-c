#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "ll_queue.h" // ll_queue*

bool ll_queue_init(ll_queue_type** ll_queue_pp, size_t value_size) {
    assert(ll_queue_pp != NULL);
    assert(value_size != 0);

    *ll_queue_pp = malloc(sizeof(ll_queue_type));
    if (*ll_queue_pp == NULL) {
        return false;
    }
    (*ll_queue_pp)->head_p = NULL;
    (*ll_queue_pp)->tail_p = NULL;
    (*ll_queue_pp)->count = 0;
    (*ll_queue_pp)->value_size = value_size;
    (*ll_queue_pp)->freelist_p = NULL;

    return true;
}

bool ll_queue_deinit(ll_queue_type** ll_queue_pp) {
    assert(ll_queue_pp != NULL);

    // free freelist nodes as well
    if ((*ll_queue_pp)->head_p == NULL) {
        (*ll_queue_pp)->head_p = (*ll_queue_pp)->freelist_p;
    } else {
        (*ll_queue_pp)->tail_p->next_p = (*ll_queue_pp)->freelist_p;
    }

    // traverse list and free nodes one by one
    ll_queue_node_type* head_p = (*ll_queue_pp)->head_p;
    ll_queue_node_type* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }

    free(*ll_queue_pp);
    (*ll_queue_pp) = NULL;

    return true;
}

bool ll_queue_is_empty(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    return ll_queue_p->head_p == NULL;
}

size_t ll_queue_count(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    return ll_queue_p->count;
}

const ll_queue_node_type* ll_queue_peek_node(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    return ll_queue_p->head_p;
}

const ll_queue_node_type* ll_queue_peek_first_node(const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_is_empty(ll_queue_p) == false);

    return ll_queue_p->head_p;
}

const ll_queue_node_type* ll_queue_peek_last_node(const ll_queue_type* ll_queue_p) {
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

ll_queue_node_type* ll_queue_node_create(ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);

    if (ll_queue_p->freelist_p != NULL) {
        ll_queue_node_type* ll_queue_node_p = ll_queue_p->freelist_p;
        ll_queue_p->freelist_p = ll_queue_p->freelist_p->next_p;
        return ll_queue_node_p;
    }
    ll_queue_node_type* ll_queue_node_p = malloc(sizeof(ll_queue_node_type));
    if (ll_queue_node_p == NULL) {
        return NULL;
    }
    ll_queue_node_p->value_p = malloc(ll_queue_p->value_size);
    if (ll_queue_node_p->value_p == NULL) {
        free(ll_queue_node_p);
        return NULL;
    }

    return ll_queue_node_p;
}

void ll_queue_node_free(ll_queue_type* ll_queue_p, ll_queue_node_type* ll_queue_node_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_node_p != NULL);

    ll_queue_node_p->next_p = ll_queue_p->freelist_p;
    ll_queue_p->freelist_p = ll_queue_node_p;
}

bool ll_queue_copy(ll_queue_type** ll_queue_dest_pp, const ll_queue_type* ll_queue_p) {
    assert(ll_queue_p != NULL);
    assert(ll_queue_dest_pp != NULL);

    if (!ll_queue_init(ll_queue_dest_pp, ll_queue_p->value_size)) {
        return false;
    }

    ll_queue_node_type* head_p = ll_queue_p->head_p;
    while (head_p != NULL) {
        ll_queue_node_type* head_copy_p = ll_queue_node_create(*ll_queue_dest_pp);
        if (head_copy_p == NULL) {
            ll_queue_deinit(ll_queue_dest_pp);
            return false;
        }
        memcpy(head_copy_p->value_p, head_p->value_p, ll_queue_p->value_size);
        ll_queue_enqueue_node(*ll_queue_dest_pp, head_copy_p);
        head_p = head_p->next_p;
    }
    (*ll_queue_dest_pp)->count = ll_queue_p->count;

    return true;
}

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "queue.h"

bool queue_init(Queue** queue_pp, size_t value_size) {
    assert(queue_pp != NULL);
    assert(value_size != 0);

    *queue_pp = (Queue*)malloc(sizeof(Queue));
    if (*queue_pp == NULL) {
        return false;
    }
    (*queue_pp)->head_p = NULL;
    (*queue_pp)->tail_p = NULL;
    (*queue_pp)->count = 0;
    (*queue_pp)->value_size = value_size;
    (*queue_pp)->freelist_p = NULL;

    return true;
}

bool queue_deinit(Queue** queue_pp) {
    assert(queue_pp != NULL);

    if (*queue_pp == NULL) {
        return false;
    }

    // free freelist nodes as well
    if ((*queue_pp)->head_p == NULL) {
        (*queue_pp)->head_p = (*queue_pp)->freelist_p;
    } else {
        (*queue_pp)->tail_p->next_p = (*queue_pp)->freelist_p;
    }

    // traverse list and free nodes one by one
    QueueNode* head_p = (*queue_pp)->head_p;
    QueueNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }

    free(*queue_pp);
    (*queue_pp) = NULL;

    return true;
}

bool queue_is_empty(const Queue* queue_p) {
    assert(queue_p != NULL);

    return queue_p->head_p == NULL;
}

size_t queue_count(const Queue* queue_p) {
    assert(queue_p != NULL);

    return queue_p->count;
}

const QueueNode* queue_peek_node(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_is_empty(queue_p) == false);

    return queue_p->head_p;
}

const QueueNode* queue_peek_first_node(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_is_empty(queue_p) == false);

    return queue_p->head_p;
}

const QueueNode* queue_peek_last_node(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_is_empty(queue_p) == false);

    return queue_p->tail_p;
}

bool queue_enqueue_node(Queue* queue_p, QueueNode* node_p) {
    assert(queue_p != NULL);
    assert(node_p != NULL);

    node_p->next_p = NULL;
    if (queue_p->head_p == NULL) {
        queue_p->head_p = node_p;
        queue_p->tail_p = node_p;
    } else {
        queue_p->tail_p->next_p = node_p;
        queue_p->tail_p = node_p;
    }
    queue_p->count++;

    return true;
}

QueueNode* queue_dequeue_node(Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_is_empty(queue_p) == false);

    QueueNode* node_p = queue_p->head_p;
    queue_p->head_p = node_p->next_p;
    if (queue_p->head_p == NULL) {
        queue_p->tail_p = NULL;
    }
    queue_p->count--;

    return node_p;
}

QueueNode* queuenode_create(Queue* queue_p) {
    assert(queue_p != NULL);

    if (queue_p->freelist_p != NULL) {
        QueueNode* node_p = queue_p->freelist_p;
        queue_p->freelist_p = queue_p->freelist_p->next_p;
        return node_p;
    }
    QueueNode* node_p = (QueueNode*)malloc(sizeof(QueueNode));
    if (node_p == NULL) {
        return NULL;
    }
    node_p->value_p = malloc(sizeof(queue_p->value_size));
    if (node_p->value_p == NULL) {
        free(node_p);
        return NULL;
    }
    return node_p;
}

void queuenode_free(Queue* queue_p, QueueNode* node_p) {
    assert(queue_p != NULL);
    assert(node_p != NULL);

    node_p->next_p = queue_p->freelist_p;
    queue_p->freelist_p = node_p;
}

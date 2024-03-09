/*
   implementation file of a queue implemented as a linked-list.
*/

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "queue.h"

bool queue_init(Queue** queue_pp, size_t value_size) {
    assert(value_size != 0);
    *queue_pp = (Queue*)malloc(sizeof(Queue));
    if (*queue_pp == NULL) {
        return false;
    }
    (*queue_pp)->head_p = NULL;
    (*queue_pp)->tail_p = NULL;
    (*queue_pp)->length = 0;
    (*queue_pp)->value_size = value_size;
    return true;
}

bool queue_deinit(Queue** queue_pp) {
    if (*queue_pp == NULL) {
        return false;
    }
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

bool queue_isempty(const Queue* queue_p) {
    return queue_p->head_p == NULL;
}

size_t queue_length(const Queue* queue_p) {
    return queue_p->length;
}

QueueNode* queue_peek(const Queue* queue_p) {
    assert(queue_isempty(queue_p) == false);
    return queue_p->head_p;
}

QueueNode* queue_peek_first(const Queue* queue_p) {
    assert(queue_isempty(queue_p) == false);
    return queue_p->head_p;
}

QueueNode* queue_peek_last(const Queue* queue_p) {
    assert(queue_isempty(queue_p) == false);
    return queue_p->tail_p;
}

bool queue_enqueue(Queue* queue_p, QueueNode* node_p) {
    node_p->next_p = NULL;
    if (queue_p->head_p == NULL) {
        queue_p->head_p = node_p;
        queue_p->tail_p = node_p;
    } else {
        queue_p->tail_p->next_p = node_p;
        queue_p->tail_p = node_p;
    }
    queue_p->length++;

    return true;
}

QueueNode* queue_dequeue(Queue* queue_p) {
    assert(queue_isempty(queue_p) == false);

    QueueNode* node_p = queue_p->head_p;
    queue_p->head_p = node_p->next_p;
    queue_p->length--;
    if (queue_p->head_p == NULL) {
        queue_p->tail_p = NULL;
    }
    return node_p;
}

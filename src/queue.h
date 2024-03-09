/*
   This is a implementation of a queue as a linked-list.
*/

#ifndef __QUEUE__H
#define __QUEUE__H

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

typedef struct QueueNode {
    struct QueueNode* next_p;
    void* value_p;
} QueueNode;

typedef struct {
    QueueNode* head_p;
    QueueNode* tail_p;
    size_t length;
    size_t value_size;
} Queue;

static inline bool queue_isempty(Queue* queue_p) {
    return queue_p->head_p == NULL;
}

static inline size_t queue_length(Queue* queue_p) {
    return queue_p->length;
}

static inline bool queue_init(Queue** queue_pp, size_t value_size) {
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

static inline bool queue_deinit(Queue** queue_pp) {
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

#define queue_foreach(queue_p, node_p, value)                                            \
    for ((assert((queue_p)->value_size == sizeof(value)), (node_p) = (queue_p)->head_p); \
         (node_p) != NULL && ((value) = *(typeof(value)*)(node_p)->value_p, true); (node_p) = (node_p)->next_p)

#endif

#ifndef NGENERATING_MACROS

#ifndef VALUE_NAME
#error "Must declare queue VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif
#ifndef VALUE_TYPE
#error "Must declare queue VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(queue_init, VALUE_NAME)(Queue** queue_pp) {
    return queue_init(queue_pp, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(queue_peek, VALUE_NAME)(Queue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(queue_isempty(queue_p) == false);

    return *(VALUE_TYPE*)queue_p->head_p->value_p;
}

static inline VALUE_TYPE JOIN(queue_peek_last, VALUE_NAME)(Queue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(queue_isempty(queue_p) == false);

    return *(VALUE_TYPE*)queue_p->tail_p->value_p;
}

static inline bool JOIN(queue_enqueue, VALUE_NAME)(Queue* queue_p, VALUE_TYPE value) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    QueueNode* node_p = (QueueNode*)malloc(sizeof(QueueNode));
    if (node_p == NULL) {
        return false;
    }
    node_p->value_p = malloc(sizeof(VALUE_TYPE));
    if (node_p->value_p == NULL) {
        free(node_p);
        return false;
    }
    memcpy(node_p->value_p, &value, sizeof(VALUE_TYPE));

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

static inline VALUE_TYPE JOIN(queue_dequeue, VALUE_NAME)(Queue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(queue_isempty(queue_p) == false);

    QueueNode* node_p = queue_p->head_p;
    queue_p->head_p = node_p->next_p;
    queue_p->length--;
    if (queue_p->head_p == NULL) {
        queue_p->tail_p = NULL;
    }

    void* value_p = node_p->value_p;
    VALUE_TYPE value = *(VALUE_TYPE*)value_p;

    free(value_p);
    free(node_p);

    return value;
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

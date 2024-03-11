#pragma once

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct QueueNode {
    struct QueueNode* next_p;
    void* value_p;
} QueueNode;

typedef struct {
    QueueNode* head_p;
    QueueNode* tail_p;
    size_t count;
    size_t value_size;
} Queue;

bool queue_init(Queue** queue_pp, size_t value_size);

bool queue_deinit(Queue** queue_pp);

bool queue_isempty(const Queue* queue_p);

size_t queue_count(const Queue* queue_p);

QueueNode* queue_peek(const Queue* queue_p);

QueueNode* queue_peek_first(const Queue* queue_p);

QueueNode* queue_peek_last(const Queue* queue_p);

bool queue_enqueue(Queue* queue_p, QueueNode* node_p);

QueueNode* queue_dequeue(Queue* queue_p);

#define queue_foreach(queue_p, node_p, value)                                            \
    for ((assert((queue_p)->value_size == sizeof(value)), (node_p) = (queue_p)->head_p); \
         (node_p) != NULL && ((value) = *(typeof(value)*)(node_p)->value_p, true); (node_p) = (node_p)->next_p)

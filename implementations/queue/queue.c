#include <stdlib.h> // size_t, malloc, free, NULL
#include <stdbool.h> // bool, true, false
#include <assert.h> // assert
#include <string.h> // memcpy

#include "queue.h"

Queue* queue_new(size_t data_size) {
    Queue* queue_p = malloc(sizeof(Queue));
    if (queue_p == NULL) {
        return queue_p;
    }
    queue_p->data_size = data_size;
    queue_p->front_p = NULL;
    queue_p->back_p = NULL;
    return queue_p;
}

bool queue_isempty(const Queue* queue_p) {
    return queue_p->front_p == NULL;
}

void* queue_peek(const Queue* queue_p) {
    assert(queue_p->front_p != NULL);
    return queue_p->front_p->value_p;
}

bool queue_enqueue(Queue* queue_p, void* value_p, size_t size) {
    assert(size == queue_p->data_size);
    QueueElement* elm_p = malloc(sizeof(QueueElement));
    if (elm_p == NULL) {
        return false;
    }
    elm_p->value_p = malloc(queue_p->data_size);
    if (elm_p->value_p == NULL) {
        free(elm_p);
        return false;
    }
    memcpy(elm_p->value_p, value_p, queue_p->data_size);
    elm_p->next_p = NULL;
    if (queue_p->front_p == NULL) {
        queue_p->front_p = elm_p;
        queue_p->back_p = elm_p;
    } else {
        queue_p->back_p->next_p = elm_p;
        queue_p->back_p = elm_p;
    }
    return true;
}

void* queue_dequeue(Queue* queue_p, size_t size) {
    assert(queue_p->front_p != NULL);
    assert(size == queue_p->data_size);
    void* value_p = queue_p->front_p->value_p;
    QueueElement* next_p = queue_p->front_p->next_p;
    free(queue_p->front_p);
    queue_p->front_p = next_p;
    if (next_p == NULL) {
        queue_p->back_p = NULL; // optional
    }
    return value_p;
}

void queue_free(Queue* queue_p) {
    QueueElement* front_p = queue_p->front_p;
    QueueElement* next_p = NULL;
    while (front_p != NULL) {
        next_p = front_p->next_p;
        free(front_p->value_p);
        free(front_p);
        front_p = next_p;
    }
    free(queue_p);
}

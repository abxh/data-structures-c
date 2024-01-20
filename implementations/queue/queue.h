#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct QElement {
    void* value_p;
    struct QElement* next_p;
} QElement;

typedef struct {
    QElement* back_p;
    QElement* front_p;
} Queue;

/* Create a new queue and returns it's pointer. Can return NULL and it should be
 * checked manually. */
static inline Queue* queue_new(void) {
    return (Queue*)calloc(1, sizeof(Queue)); // also sets the front and back pointer to NULL
}

/* Return if the queue is empty.*/
static inline bool queue_empty(const Queue* queue_p) {
    return queue_p->front_p == NULL;
}

/* Peek at the next value pointer to be dequeued. Must check if queue is empty
 * beforehand. */
static inline void* queue_peek(const Queue* queue_p) {
    return queue_p->front_p->value_p;
}

/* Enqueue a value pointer onto the queue. Returns if successful. */
bool queue_enqueue(Queue* queue_p, void* value_p);

/* Dequeue a value pointer from the queue and return it. */
void* queue_dequeue(Queue* queue_p);

/* Free the queue from the heap. */
void queue_free(Queue* queue_p);

/* Create inline functions to directly work with queue values with appropiate
 * memory handling.*/
#define CREATE_QUEUE_INLINE_FUNCTIONS(name, type)                              \
    static inline type queue_peek_##name(Queue* queue_p) {                     \
        return *(type*)queue_peek(queue_p);                                    \
    }                                                                          \
    static inline bool queue_enqueue_##name(Queue* queue_p, type value) {      \
        void* value_p = malloc(sizeof(type));                                  \
        if (value_p == NULL) {                                                 \
            return false;                                                      \
        }                                                                      \
        memcpy(value_p, &value, sizeof(type));                                 \
        if (queue_enqueue(queue_p, value_p)) {                                 \
            return true;                                                       \
        }                                                                      \
        free(value_p);                                                         \
        return false;                                                          \
    }                                                                          \
    static inline type queue_dequeue_##name(Queue* queue_p) {                  \
        void* value_p = queue_dequeue(queue_p);                                \
        type value = *(type*)value_p;                                          \
        free(value_p);                                                         \
        return value;                                                          \
    }                                                                          \
    static inline type qelement_get_##name(QElement* qelement_p) {             \
        return *(type*)qelement_p->value_p;                                    \
    }                                                                          \
    static inline void qelement_set_##name(QElement* qelement_p, type value) { \
        memcpy(qelement_p->value_p, &value, sizeof(type));                     \
    }

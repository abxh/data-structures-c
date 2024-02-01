#pragma once

#include <stdlib.h> // size_t, free
#include <string.h> // memcpy
#include <stdbool.h> // bool

typedef struct QueueElement {
    void* value_p;
    struct QueueElement* next_p;
} QueueElement;

typedef struct {
    size_t data_size;
    QueueElement* back_p;
    QueueElement* front_p;
} Queue;

/* Try create a new queue and returns it's pointer. Returns NULL if OOM. */
Queue* queue_new(size_t data_size);

/* Return if the queue is empty. */
bool queue_isempty(const Queue* queue_p);

/* Peek at the next value to be dequeued. Should check for empty queue beforehand. */
void* queue_peek(const Queue* queue_p);

/* Enqueue a value onto the queue. Returns if successful. */
bool queue_enqueue(Queue* queue_p, void* value_p, size_t size);

/* Dequeue a value from the queue and return it. Should check for empty queue beforehand
 * and free the value returned from heap. */
void* queue_dequeue(Queue* queue_p, size_t size);

/* Free the memory of the queue appropiately. */
void queue_free(Queue* queue_p);

/* Create inline functions to directly work with queue values. */
#define CREATE_QUEUE_INLINE_FUNCTIONS(name, type)                                            \
    static inline Queue* queue_new_##name(void) {                                            \
        return queue_new(sizeof(type));                                                      \
    }                                                                                        \
    static inline type queue_peek_##name(Queue* queue_p) {                                   \
        return *(type*)queue_peek(queue_p);                                                  \
    }                                                                                        \
    static inline bool queue_enqueue_##name(Queue* queue_p, type value) {                    \
        return queue_enqueue(queue_p, &value, sizeof(type));                                 \
    }                                                                                        \
    static inline type queue_dequeue_##name(Queue* queue_p) {                                \
        void* value_p = queue_dequeue(queue_p, sizeof(type));                                \
        type value = *(type*)value_p;                                                        \
        free(value_p);                                                                       \
        return value;                                                                        \
    }                                                                                        \
    static inline type queue_element_get_##name(QueueElement* queue_element_p) {             \
        return *(type*)queue_element_p->value_p;                                             \
    }                                                                                        \
    static inline void queue_element_set_##name(QueueElement* queue_element_p, type value) { \
        memcpy(queue_element_p->value_p, &value, sizeof(type));                              \
    }

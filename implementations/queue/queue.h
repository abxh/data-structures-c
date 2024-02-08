#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t, free

typedef struct {
    size_t start_index;
    size_t end_index;
    size_t used;
    size_t capacity_sub_one;
    size_t data_size;
    unsigned char* arr_p;
} Queue;

/* Create a new queue of some maximum capacity for some data size. Capacity is
   rounded up to a power of 2. Returns NULL if OOM or `capacity * data_size`
   exceeds SIZE_MAX. */
Queue* queue_new(size_t capacity, size_t data_size);

/* Return if the queue is empty. */
bool queue_isempty(const Queue* queue_p);

/* Return if the queue is full. */
bool queue_isfull(const Queue* queue_p);

/* Peek at the next dequeued value of a non-empty queue as bytes. */
unsigned char* queue_peek_next(Queue* queue_p);

/* Peek at the last enqueued value of a non-empty queue as bytes. */
unsigned char* queue_peek_last(Queue* queue_p);

/* Enqueue a value onto a non-full queue as bytes. */
void queue_enqueue(Queue* queue_p, unsigned char* bytes);

/* Dequeue a value from a non-empty queue and return it as bytes. */
unsigned char* queue_dequeue(Queue* queue_p);

/* Resize queue to have a new capacity that is rounded up to a power of 2.
   Returns false if OOM or `new_capacity x data_size` exceeds SIZE_MAX. Should
   ensure new capacity is not zero. */
bool queue_resize(Queue* queue_p, size_t new_capacity);

/* Free the memory of a queue appropiately. */
void queue_free(Queue* queue_p);

/* Create inline functions to directly work with queue values. */
#define QUEUE_CREATE_INLINE_FUNCTIONS(name, type)                         \
    static inline Queue* queue_new_##name(size_t capacity) {              \
        return queue_new(capacity, sizeof(type));                         \
    }                                                                     \
    static inline type queue_peek_next_##name(Queue* queue_p) {           \
        return *(type*)queue_peek_next(queue_p);                          \
    }                                                                     \
    static inline type queue_peek_last_##name(Queue* queue_p) {           \
        return *(type*)queue_peek_last(queue_p);                          \
    }                                                                     \
    static inline void queue_enqueue_##name(Queue* queue_p, type value) { \
        queue_enqueue(queue_p, (unsigned char*)&value);                   \
    }                                                                     \
    static inline type queue_dequeue_##name(Queue* queue_p) {             \
        return *(type*)queue_dequeue(queue_p);                            \
    }

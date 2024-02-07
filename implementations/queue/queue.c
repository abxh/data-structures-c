#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stddef.h>  // offsetof
#include <stdint.h>  // uint32_t
#include <stdlib.h>  // size_t, malloc, free, NULL
#include <string.h>  // memcpy

#include "queue.h"

size_t roundpow2(uint32_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

Queue* queue_new(size_t capacity, size_t data_size) {
    assert(capacity != 0);
    assert(data_size != 0);
    capacity = roundpow2(capacity);
    size_t capacity_sub_one = capacity - 1;
    Queue* queue_p = malloc(offsetof(Queue, arr) + capacity * data_size);
    if (queue_p == NULL) {
        return queue_p;
    }
    queue_p->start_index = 0;
    queue_p->end_index = 0;
    queue_p->capacity_sub_one = capacity_sub_one;
    queue_p->data_size = data_size;
    queue_p->isempty = true;
    return queue_p;
}

bool queue_isempty(const Queue* queue_p) {
    return queue_p->isempty;
}

bool queue_isfull(const Queue* queue_p) {
    return !queue_p->isempty && queue_p->start_index == queue_p->end_index;
}

unsigned char* queue_peek_next(Queue* queue_p) {
    assert(!queue_p->isempty);
    return &queue_p->arr[queue_p->data_size * queue_p->start_index];
}

unsigned char* queue_peek_last(Queue* queue_p) {
    assert(!queue_p->isempty);
    return &queue_p->arr[queue_p->data_size * (queue_p->end_index - 1)];
}

void queue_enqueue(Queue* queue_p, unsigned char* value) {
    assert(queue_p->isempty || queue_p->start_index != queue_p->end_index);
    queue_p->isempty = false;
    memcpy(queue_p->arr + queue_p->data_size * queue_p->end_index, value, queue_p->data_size);
    queue_p->end_index++;
    queue_p->end_index &= queue_p->capacity_sub_one;
}

unsigned char* queue_dequeue(Queue* queue_p) {
    assert(!queue_p->isempty);
    unsigned char* value = &queue_p->arr[queue_p->data_size * queue_p->start_index];
    queue_p->start_index++;
    queue_p->start_index &= queue_p->capacity_sub_one;
    queue_p->isempty = queue_p->start_index == queue_p->end_index;
    return value;
}

void queue_free(Queue* queue_p) {
    free(queue_p);
}

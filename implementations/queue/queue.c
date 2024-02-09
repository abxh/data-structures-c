#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdint.h>  // uint32_t
#include <stdlib.h>  // size_t, malloc, free, NULL
#include <string.h>  // memcpy, memmove

#include "queue.h"

size_t rounduppow2(uint32_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    assert(v != 0);
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
    if (capacity > SIZE_MAX / data_size) {
        return NULL;
    }
    capacity = rounduppow2(capacity);
    Queue* queue_p = malloc(sizeof(Queue));
    if (queue_p == NULL) {
        return NULL;
    }
    queue_p->arr_p = malloc(capacity * data_size);
    if (queue_p->arr_p == NULL) {
        free(queue_p);
        return NULL;
    }
    queue_p->start_index = 0;
    queue_p->end_index = 0;
    queue_p->used = 0;
    queue_p->capacity_sub_one = capacity - 1;
    queue_p->data_size = data_size;
    return queue_p;
}

bool queue_isempty(const Queue* queue_p) {
    return queue_p->used == 0;
}

bool queue_isfull(const Queue* queue_p) {
    return queue_p->used == queue_p->capacity_sub_one + 1;
}

unsigned char* queue_peek_next(Queue* queue_p) {
    assert(queue_p->used != 0);
    return queue_p->arr_p + queue_p->data_size * queue_p->start_index;
}

unsigned char* queue_peek_last(Queue* queue_p) {
    assert(queue_p->used != 0);
    return queue_p->arr_p + queue_p->data_size * (queue_p->end_index - 1);
}

void queue_enqueue(Queue* queue_p, unsigned char* bytes) {
    assert(queue_p->used != queue_p->capacity_sub_one + 1);
    memcpy(queue_p->arr_p + queue_p->data_size * queue_p->end_index, bytes, queue_p->data_size);
    queue_p->end_index++;
    queue_p->end_index &= queue_p->capacity_sub_one;
    queue_p->used++;
}

unsigned char* queue_dequeue(Queue* queue_p) {
    assert(queue_p->used != 0);
    unsigned char* bytes = queue_p->arr_p + queue_p->data_size * queue_p->start_index;
    queue_p->start_index++;
    queue_p->start_index &= queue_p->capacity_sub_one;
    queue_p->used--;
    return bytes;
}

void rotleft(unsigned char* bytes, size_t len, size_t n) {
    for (size_t i = 0; i < n; i++) {
        unsigned char temp = bytes[0];
        memmove(bytes, bytes + 1, len - 1);
        bytes[len - 1] = temp;
    }
}

bool queue_resize(Queue* queue_p, size_t new_capacity) {
    assert(new_capacity != 0);
    new_capacity = rounduppow2(new_capacity);
    if (new_capacity == queue_p->capacity_sub_one + 1) {
        return true;
    }

    rotleft(queue_p->arr_p, queue_p->data_size * (queue_p->capacity_sub_one + 1), queue_p->data_size * queue_p->start_index);
    queue_p->start_index = 0;
    queue_p->end_index = queue_p->used;

    void* new_arr_p = reallocarray(queue_p->arr_p, new_capacity, queue_p->data_size);
    if (new_arr_p == NULL) {
        return false;
    }
    queue_p->arr_p = new_arr_p;
    queue_p->capacity_sub_one = new_capacity - 1;
    return true;
}

void queue_free(Queue* queue_p) {
    free(queue_p->arr_p);
    free(queue_p);
}

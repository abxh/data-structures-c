/*
   This is a implementation of a fixed-sized queue as an array (circular queue).

   Provided capacity is rounded to a power of 2 automatically for convenience.
*/

#ifndef __AQUEUE__H
#define __AQUEUE__H

#include "bithacks.h" // rounduppow2
#include <assert.h>   // assert
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // NULL, size_t, malloc, calloc, reallocarray, free
#include <string.h>   // memcpy

typedef struct {
    size_t capacity_sub_one;
    size_t length;
    size_t begin_index;
    size_t end_index;
    size_t value_size;
    void* arr_p;
} AQueue;

static inline size_t aqueue_length(AQueue* stack_p) {
    return stack_p->length;
}

static inline size_t aqueue_capacity(AQueue* stack_p) {
    return stack_p->capacity_sub_one + 1;
}

static inline bool aqueue_isempty(AQueue* queue_p) {
    return queue_p->length == 0;
}

static inline bool aqueue_isfull(AQueue* queue_p) {
    return queue_p->length == queue_p->capacity_sub_one + 1;
}

static inline bool aqueue_init(AQueue** queue_pp, size_t capacity, size_t value_size) {
    assert(capacity != 0);
    capacity += (capacity == 1);
    size_t capacity_rounded = rounduppow2(capacity);
    if (capacity_rounded < capacity) {
        return false;
    }
    *queue_pp = (AQueue*)malloc(sizeof(AQueue));
    if ((*queue_pp) == NULL) {
        return false;
    }
    (*queue_pp)->arr_p = calloc(capacity_rounded, value_size);
    if ((*queue_pp)->arr_p == NULL) {
        free(*queue_pp);
        *queue_pp = NULL;
        return false;
    }
    (*queue_pp)->length = 0;
    (*queue_pp)->begin_index = 0;
    (*queue_pp)->end_index = 0;
    (*queue_pp)->capacity_sub_one = capacity_rounded - 1;
    (*queue_pp)->value_size = value_size;
    return true;
}

static inline bool aqueue_deinit(AQueue** queue_pp) {
    if (*queue_pp == NULL) {
        return false;
    }
    free((*queue_pp)->arr_p);
    free((*queue_pp));
    *queue_pp = NULL;
    return true;
}

#define aqueue_foreach(queue_p, index, value)                                                            \
    for ((assert((queue_p)->value_size == sizeof(typeof(value))), (index) = 0);                          \
         ((index) < (queue_p)->length && ((value) = ((typeof(value)*)(queue_p)->arr_p)[(index)], true)); \
         (index) = ((index) + 1) & (queue_p)->capacity_sub_one)

#endif

#ifndef NGENERATING_MACROS

#ifndef VALUE_NAME
#error "Must declare aqueue VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif
#ifndef VALUE_TYPE
#error "Must declare aqueue VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(aqueue_init, VALUE_NAME)(AQueue** queue_pp, size_t capacity) {
    return aqueue_init(queue_pp, capacity, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(aqueue_peek, VALUE_NAME)(AQueue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(aqueue_isempty(queue_p) == false);

    return ((VALUE_TYPE*)queue_p->arr_p)[queue_p->begin_index];
}

static inline VALUE_TYPE JOIN(aqueue_peek_last, VALUE_NAME)(AQueue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(aqueue_isempty(queue_p) == false);

    return ((VALUE_TYPE*)queue_p->arr_p)[(queue_p->end_index - 1) & queue_p->capacity_sub_one];
}

static inline void JOIN(aqueue_enqueue, VALUE_NAME)(AQueue* queue_p, const VALUE_TYPE value) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(aqueue_isfull(queue_p) == false);

    ((VALUE_TYPE*)queue_p->arr_p)[queue_p->end_index] = value;
    queue_p->end_index++;
    queue_p->end_index &= queue_p->capacity_sub_one;
    queue_p->length++;
}

static inline VALUE_TYPE JOIN(aqueue_dequeue, VALUE_NAME)(AQueue* queue_p) {
    assert(queue_p->value_size == sizeof(VALUE_TYPE));
    assert(aqueue_isempty(queue_p) == false);

    VALUE_TYPE value = ((VALUE_TYPE*)queue_p->arr_p)[queue_p->begin_index];
    queue_p->begin_index++;
    queue_p->begin_index &= queue_p->capacity_sub_one;
    queue_p->length--;
    return value;
}

static inline bool JOIN(aqueue_resize, VALUE_NAME)(AQueue* queue_p, size_t new_capacity) {
    assert(new_capacity != 0);
    new_capacity += (new_capacity == 1);
    size_t new_capacity_rounded = rounduppow2(new_capacity);
    new_capacity_rounded += (new_capacity == 1);

    if (new_capacity_rounded < new_capacity || new_capacity_rounded < queue_p->length) {
        return false;
    } else if (new_capacity_rounded == queue_p->capacity_sub_one + 1) {
        return true;
    }

    // note:
    // the following can be replaced with a slow rotation algorithm

    bool growing = new_capacity_rounded > queue_p->capacity_sub_one + 1;
    bool shrinking = new_capacity_rounded < queue_p->capacity_sub_one + 1;

    if (queue_p->length != 0 && shrinking) {
        if (queue_p->begin_index != 0 && queue_p->begin_index < queue_p->end_index) {
            memcpy(queue_p->arr_p, (VALUE_TYPE*)queue_p->arr_p + queue_p->begin_index, sizeof(VALUE_TYPE) * queue_p->length);
            queue_p->begin_index = 0;
            queue_p->end_index = queue_p->length;
        } else if (queue_p->end_index <= queue_p->begin_index) {
            size_t nelm = queue_p->capacity_sub_one + 1 - queue_p->begin_index;
            memcpy((VALUE_TYPE*)queue_p->arr_p + new_capacity_rounded - nelm, (VALUE_TYPE*)queue_p->arr_p + queue_p->begin_index,
                   sizeof(VALUE_TYPE) * nelm);
            queue_p->begin_index = new_capacity_rounded - nelm;
        }
    }

    void* new_arr_p = reallocarray(queue_p->arr_p, new_capacity_rounded, sizeof(VALUE_TYPE));
    if (new_arr_p == NULL) {

        // restore queue:
        if (queue_p->length != 0 && shrinking && queue_p->end_index <= queue_p->begin_index) {
            size_t nelm = new_capacity_rounded - queue_p->begin_index;
            memcpy((VALUE_TYPE*)queue_p->arr_p + queue_p->capacity_sub_one + 1 - nelm,
                   (VALUE_TYPE*)queue_p->arr_p + queue_p->begin_index, sizeof(VALUE_TYPE) * nelm);
            queue_p->begin_index = queue_p->capacity_sub_one + 1 - nelm;
        }

        return false;
    }
    queue_p->arr_p = new_arr_p;

    if (queue_p->length != 0 && growing && queue_p->begin_index >= queue_p->end_index) {
        memcpy((VALUE_TYPE*)queue_p->arr_p + queue_p->capacity_sub_one + 1, queue_p->arr_p, sizeof(VALUE_TYPE) * queue_p->end_index);
        queue_p->end_index += queue_p->capacity_sub_one + 1;
    }

    queue_p->capacity_sub_one = new_capacity_rounded - 1;
    return true;
}

#undef JOIN
#undef PASTE
#undef CAT
#undef VALUE_TYPE
#undef VALUE_NAME
#else
#undef NGENERATING_MACROS
#endif

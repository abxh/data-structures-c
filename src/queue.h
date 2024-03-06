#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "bithacks.h" // rounduppow2
#include "macros.h"   // JOIN
#include <assert.h>   // assert
#include <stdlib.h>   // size_t, reallocarray
#include <string.h>   // memcpy

#endif

#ifndef VALUE_NAME
#error "Must declare queue VALUE_NAME. Defaulting to 'default'."
#define VALUE_NAME default
#endif

#ifndef VALUE_TYPE
#error "Must declare queue VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef PREFIX
#define PREFIX queue
#endif

#define QUEUE_STRUCT JOIN(PREFIX, JOIN(VALUE_NAME, t))
#define QUEUE_METHOD(name) JOIN(PREFIX, JOIN(VALUE_NAME, name))

typedef struct {
    size_t start_index;
    size_t end_index;
    size_t used;
    size_t capacity_sub_one;
    VALUE_TYPE* arr_p;
} QUEUE_STRUCT;

static inline bool QUEUE_METHOD(init)(QUEUE_STRUCT** queue_pp, size_t capacity) {
    assert(capacity != 0);
    size_t capacity_rounded = rounduppow2(capacity);
    if (capacity_rounded < capacity) {
        return false;
    }
    *queue_pp = (QUEUE_STRUCT*)malloc(sizeof(QUEUE_STRUCT));
    if ((*queue_pp) == NULL) {
        return false;
    }
    (*queue_pp)->arr_p = (VALUE_TYPE*)calloc(capacity_rounded, sizeof(VALUE_TYPE));
    if ((*queue_pp)->arr_p == NULL) {
        free(*queue_pp);
        *queue_pp = NULL;
        return false;
    }
    (*queue_pp)->used = 0;
    (*queue_pp)->start_index = 0;
    (*queue_pp)->end_index = 0;
    (*queue_pp)->capacity_sub_one = capacity_rounded - 1;
    return true;
}

static inline bool QUEUE_METHOD(isempty)(const QUEUE_STRUCT* queue_p) {
    return queue_p->used == 0;
}

static inline bool QUEUE_METHOD(isfull)(const QUEUE_STRUCT* queue_p) {
    return queue_p->used == queue_p->capacity_sub_one + 1;
}

static inline VALUE_TYPE QUEUE_METHOD(peek_first)(const QUEUE_STRUCT* queue_p) {
    assert(queue_p->used > 0);
    return queue_p->arr_p[queue_p->start_index];
}

static inline VALUE_TYPE QUEUE_METHOD(peek_last)(const QUEUE_STRUCT* queue_p) {
    assert(queue_p->used > 0);
    return queue_p->arr_p[queue_p->end_index - 1];
}

static inline void QUEUE_METHOD(enqueue)(QUEUE_STRUCT* queue_p, const VALUE_TYPE value) {
    assert(queue_p->used < queue_p->capacity_sub_one + 1);
    queue_p->arr_p[queue_p->end_index] = value;
    queue_p->end_index++;
    queue_p->end_index &= queue_p->capacity_sub_one;
    queue_p->used++;
}

static inline VALUE_TYPE QUEUE_METHOD(dequeue)(QUEUE_STRUCT* queue_p) {
    assert(queue_p->used > 0);
    VALUE_TYPE value = queue_p->arr_p[queue_p->start_index];
    queue_p->start_index++;
    queue_p->start_index &= queue_p->capacity_sub_one;
    queue_p->used--;
    return value;
}

static inline bool QUEUE_METHOD(resize)(QUEUE_STRUCT* queue_p, size_t new_capacity) {
    assert(new_capacity != 0);
    size_t new_capacity_rounded = rounduppow2(new_capacity);
    if (new_capacity_rounded < new_capacity || new_capacity_rounded < queue_p->used) {
        return false;
    } else if (new_capacity_rounded == queue_p->capacity_sub_one + 1) {
        return true;
    }
    bool growing = queue_p->used != 0 && new_capacity_rounded > queue_p->capacity_sub_one + 1;
    bool shrinking = queue_p->used != 0 && new_capacity_rounded < queue_p->capacity_sub_one + 1;

    bool shrinking_case_1 = shrinking && queue_p->end_index > queue_p->start_index && queue_p->start_index != 0;
    bool shrinking_case_2 = shrinking && queue_p->end_index <= queue_p->start_index;

    if (shrinking_case_1) {
        memcpy(&queue_p->arr_p[0], &queue_p->arr_p[queue_p->start_index], queue_p->used);
        queue_p->start_index = 0;
        queue_p->end_index = queue_p->used;
    } else if (shrinking_case_2) {
        size_t nelm = queue_p->capacity_sub_one + 1 - queue_p->start_index;
        memcpy(&queue_p->arr_p[new_capacity_rounded - nelm], &queue_p->arr_p[queue_p->start_index], nelm);
        queue_p->start_index = new_capacity_rounded - nelm;
    }
    VALUE_TYPE* new_arr_p = (VALUE_TYPE*)reallocarray(queue_p->arr_p, new_capacity_rounded, sizeof(*queue_p->arr_p));
    if (new_arr_p == NULL) {
        if (shrinking_case_2) {
            size_t nelm = new_capacity_rounded - queue_p->start_index;
            memcpy(&queue_p->arr_p[queue_p->capacity_sub_one + 1 - nelm], &queue_p->arr_p[queue_p->start_index], nelm);
            queue_p->start_index = queue_p->capacity_sub_one + 1 - nelm;
        }
        return false;
    }
    queue_p->arr_p = new_arr_p;
    if (growing && queue_p->end_index <= queue_p->start_index) {
        memcpy(&queue_p->arr_p[0] + queue_p->capacity_sub_one + 1, &queue_p->arr_p[0], queue_p->end_index);
        queue_p->end_index += queue_p->capacity_sub_one + 1;
    }
    queue_p->capacity_sub_one = new_capacity_rounded - 1;
    return true;
}

static inline bool QUEUE_METHOD(deinit)(QUEUE_STRUCT** queue_pp) {
    if (*queue_pp == NULL) {
        return false;
    }
    free((*queue_pp)->arr_p);
    free((*queue_pp));
    *queue_pp = NULL;
    return true;
}

#undef SUFFIX
#undef QUEUE_METHOD
#undef QUEUE_STRUCT
#undef QUEUE_NAME
#undef QUEUE_TYPE

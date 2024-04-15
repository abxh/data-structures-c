/*
    `T_queue` is a queue implementation based on a fixed size array.

    Including this header file generates a struct and functions for a given queue type.

    The following macros gets defined once:
    - T_queue_for_each

    The following structs are generated for a given value type T:
    - T_queue_type

    The following functions are generated for a given value type T:
    - T_queue_init
    - T_queue_init_internal
    - T_queue_init_with_capacity_rounded
    - T_queue_deinit
    - T_queue_copy
    - T_queue_get_count
    - T_queue_get_capacity
    - T_queue_is_empty
    - T_queue_is_full
    - T_queue_peek
    - T_queue_peek_first
    - T_queue_peek_last
    - T_queue_enqueue
    - T_queue_dequeue

    Define PREFIX to use an other prefix than "T_stack". The macro(s) cannot
    however be redefined to have other prefixes.

    Note that (if PREFIX is not defined), then the given types cannot include
    spaces because C functions and variables cannot either.
    Use a typedef and replace spaces with _ or change the type name as needed.
*/

#ifndef __T_QUEUE__H
#define __T_QUEUE__H

#include "is_pow2.h"  // is_pow2
#include "nextpow2.h" // nextpow2
#include <assert.h>   // assert
#include <stdbool.h>  // bool, true, false
#include <stddef.h>   // offsetoff
#include <stdint.h>   // SIZE_MAX
#include <stdlib.h>   // size_t, NULL, malloc, free
#include <string.h>   // memcpy

#define T_queue_for_each(queue_p, index, value)                                                                          \
    for ((index) = (queue_p)->begin_index; (index) != (queue_p)->end_index && ((value) = (queue_p)->arr[(index)], true); \
         (index) = ((index) + 1) & ((queue_p)->capacity - 1))

#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#ifndef PREFIX
#define T_queue JOIN(VALUE_TYPE, queue)
#else
#define T_queue PREFIX
#endif
#define T_queue_type JOIN(T_queue, type)

typedef struct {
    size_t begin_index;
    size_t end_index;
    size_t count;
    size_t capacity;
    VALUE_TYPE arr[];
} T_queue_type;

static inline bool JOIN(T_queue, init_internal)(T_queue_type** queue_pp, size_t pow2_capacity) {
    assert(queue_pp != NULL);
    assert(is_pow2(pow2_capacity) && "initial capacity is a power of 2");

    if (pow2_capacity > (SIZE_MAX - offsetof(T_queue_type, arr)) / sizeof(VALUE_TYPE)) {
        return false;
    }
    *queue_pp = (T_queue_type*)malloc(offsetof(T_queue_type, arr) + sizeof(VALUE_TYPE) * pow2_capacity);
    if ((*queue_pp) == NULL) {
        return false;
    }
    (*queue_pp)->begin_index = 0;
    (*queue_pp)->end_index = 0;
    (*queue_pp)->count = 0;
    (*queue_pp)->capacity = pow2_capacity;

    return true;
}

static inline bool JOIN(T_queue, init_with_capacity_rounded)(T_queue_type** queue_pp, size_t capacity) {
    assert(queue_pp != NULL);

    if (capacity == 0) {
        return false;
    }
    size_t rounded_capacity = nextpow2(capacity);
    if (rounded_capacity < capacity) {
        return false;
    }

    return JOIN(T_queue, init_internal)(queue_pp, rounded_capacity);
}

static inline bool JOIN(T_queue, init)(T_queue_type** queue_pp, size_t capacity) {
    assert(queue_pp != NULL);

    return JOIN(T_queue, init_with_capacity_rounded)(queue_pp, capacity);
}

static inline bool JOIN(T_queue, deinit)(T_queue_type** queue_pp) {
    assert(queue_pp != NULL);

    if (*queue_pp == NULL) {
        return false;
    }
    free(*queue_pp);
    *queue_pp = NULL;

    return true;
}

static inline bool JOIN(T_queue, copy)(T_queue_type** queue_dest_pp, T_queue_type* queue_src_p) {
    assert(queue_src_p != NULL);
    assert(queue_dest_pp != NULL);

    if (!JOIN(T_queue, init)(queue_dest_pp, queue_src_p->capacity)) {
        return false;
    }
    memcpy((*queue_dest_pp)->arr, queue_src_p->arr, sizeof(VALUE_TYPE) * queue_src_p->capacity);
    (*queue_dest_pp)->begin_index = queue_src_p->begin_index;
    (*queue_dest_pp)->end_index = queue_src_p->end_index;
    (*queue_dest_pp)->count = queue_src_p->count;

    return true;
}

static inline size_t JOIN(T_queue, get_count)(const T_queue_type* queue_p) {
    assert(queue_p != NULL);

    return queue_p->count;
}

static inline size_t JOIN(T_queue, get_capacity)(const T_queue_type* queue_p) {
    assert(queue_p != NULL);

    return queue_p->capacity;
}

static inline bool JOIN(T_queue, is_empty)(const T_queue_type* queue_p) {
    assert(queue_p != NULL);

    return queue_p->count == 0;
}

static inline bool JOIN(T_queue, is_full)(const T_queue_type* queue_p) {
    assert(queue_p != NULL);

    return queue_p->count == queue_p->capacity;
}

static inline VALUE_TYPE JOIN(T_queue, peek)(T_queue_type* queue_p) {
    assert(queue_p != NULL);
#define T_queue_is_empty JOIN(T_queue, is_empty)
    assert(T_queue_is_empty(queue_p) == false);
#undef T_queue_is_empty

    return queue_p->arr[queue_p->begin_index];
}

static inline VALUE_TYPE JOIN(T_queue, peek_first)(T_queue_type* queue_p) {
    return JOIN(T_queue, peek)(queue_p);
}

static inline VALUE_TYPE JOIN(T_queue, peek_last)(T_queue_type* queue_p) {
    assert(queue_p != NULL);
#define T_queue_is_empty JOIN(T_queue, is_empty)
    assert(T_queue_is_empty(queue_p) == false);
#undef T_queue_is_empty

    return queue_p->arr[(queue_p->end_index - 1) & (queue_p->capacity - 1)];
}

static inline void JOIN(T_queue, enqueue)(T_queue_type* queue_p, const VALUE_TYPE value) {
    assert(queue_p != NULL);
#define T_queue_is_full JOIN(T_queue, is_full)
    assert(T_queue_is_full(queue_p) == false);
#undef T_queue_is_full

    queue_p->arr[queue_p->end_index] = value;
    queue_p->end_index = (queue_p->end_index + 1) & (queue_p->capacity - 1);
    queue_p->count++;
}

static inline VALUE_TYPE JOIN(T_queue, dequeue)(T_queue_type* queue_p) {
    assert(queue_p != NULL);
#define T_queue_is_empty JOIN(T_queue, is_empty)
    assert(T_queue_is_empty(queue_p) == false);
#undef T_queue_is_empty

    VALUE_TYPE value = queue_p->arr[queue_p->begin_index];
    queue_p->begin_index = (queue_p->begin_index + 1) & (queue_p->capacity - 1);
    queue_p->count--;

    return value;
}

#undef T_queue
#undef T_queue_type

#undef PREFIX
#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN

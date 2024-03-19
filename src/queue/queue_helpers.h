/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with label T are:
    - queue_peek_T
    - queue_peek_first_T
    - queue_peek_last_T
    - queue_push_T
    - queue_pop_T

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#include "queue.h"
#include <stdlib.h> // NULL, size_t
#include <stdbool.h> // bool, true, false

#ifndef VALUE_LABEL
#error "Must declare VALUE_LABEL. Defaulting to 'int'."
#define VALUE_LABEL int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline const VALUE_TYPE JOIN(queue_peek, VALUE_LABEL)(const Queue* queue_p) {
    return *(VALUE_TYPE*)(queue_peek_node(queue_p)->value_p);
}

static inline const VALUE_TYPE JOIN(queue_peek_first, VALUE_LABEL)(const Queue* queue_p) {
    return *(VALUE_TYPE*)(queue_peek_first_node(queue_p)->value_p);
}

static inline const VALUE_TYPE JOIN(queue_peek_last, VALUE_LABEL)(const Queue* queue_p) {
    return *(VALUE_TYPE*)(queue_peek_last_node(queue_p)->value_p);
}

static inline bool JOIN(queue_enqueue, VALUE_LABEL)(Queue* queue_p, const VALUE_TYPE value) {
    QueueNode* node_p = queuenode_create(queue_p);
    if (node_p == NULL) {
        return false;
    }
    *(VALUE_TYPE*)node_p->value_p = value;
    queue_enqueue_node(queue_p, node_p);
    return true;
}

static inline VALUE_TYPE JOIN(queue_dequeue, VALUE_LABEL)(Queue* queue_p) {
    QueueNode* node_p = queue_dequeue_node(queue_p);
    VALUE_TYPE value = *(VALUE_TYPE*)node_p->value_p;
    queuenode_free(queue_p, node_p);
    return value;
}

#undef VALUE_TYPE
#undef VALUE_LABEL

#undef CAT
#undef PASTE
#undef JOIN

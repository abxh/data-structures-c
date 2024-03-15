/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with label T are:
    - queuenode_get_value_as_T
    - queuenode_set_value_to_T
    - queue_peek_T
    - queue_push_T
    - queue_pop_T

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#include "queue.h"
#include <assert.h> // assert
#include <stdlib.h> // NULL, size_t, malloc, free

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

#define queuenode_get_value_as_T JOIN(queuenode_get_value_as, VALUE_LABEL)
#define queuenode_set_value_to_T JOIN(queuenode_set_value_to, VALUE_LABEL)
#define queue_peek_T JOIN(queue_peek, VALUE_LABEL)
#define queue_peek_first_T JOIN(queue_peek_first, VALUE_LABEL)
#define queue_peek_last_T JOIN(queue_peek_last, VALUE_LABEL)
#define queue_enqueue_T JOIN(queue_enqueue, VALUE_LABEL)
#define queue_dequeue_T JOIN(queue_dequeue, VALUE_LABEL)

static inline VALUE_TYPE queuenode_get_value_as_T(const QueueNode* node_p) {
    assert(node_p != NULL);

    return *(VALUE_TYPE*)node_p->value_p;
}

static inline void queuenode_set_value_to_T(QueueNode* node_p, VALUE_TYPE value) {
    assert(node_p != NULL);

    *(VALUE_TYPE*)node_p->value_p = value;
}

static inline VALUE_TYPE queue_peek_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return queuenode_get_value_as_T(queue_peek_node(queue_p));
}

static inline VALUE_TYPE queue_peek_first_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return queuenode_get_value_as_T(queue_peek_first_node(queue_p));
}

static inline VALUE_TYPE queue_peek_last_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return queuenode_get_value_as_T(queue_peek_last_node(queue_p));
}

static inline bool queue_enqueue_T(Queue* queue_p, VALUE_TYPE value) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    QueueNode* node_p = queuenode_create(queue_p);
    if (node_p == NULL) {
        return false;
    }
    queuenode_set_value_to_T(node_p, value);
    queue_enqueue_node(queue_p, node_p);

    return true;
}

static inline VALUE_TYPE queue_dequeue_T(Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    QueueNode* node_p = queue_dequeue_node(queue_p);
    VALUE_TYPE value = queuenode_get_value_as_T(node_p);
    queuenode_free(queue_p, node_p);

    return value;
}

#undef queuenode_get_value_as_T
#undef queuenode_set_value_to_T
#undef queue_peek_T
#undef queue_peek_first_T
#undef queue_peek_last_T
#undef queue_enqueue_T
#undef queue_dequeue_T

#undef VALUE_TYPE
#undef VALUE_LABEL

#undef CAT
#undef PASTE
#undef JOIN

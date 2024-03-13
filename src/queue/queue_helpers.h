/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with label T are:
    - queuenode_get_value_T
    - queuenode_set_value_T
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

#define QUEUENODE_GET_VALUE_T JOIN(queuenode_get_value, VALUE_TYPE)
#define QUEUENODE_SET_VALUE_T JOIN(queuenode_set_value, VALUE_TYPE)
#define QUEUE_PEEK_T JOIN(queue_peek, VALUE_LABEL)
#define QUEUE_PEEK_FIRST_T JOIN(queue_peek_first, VALUE_LABEL)
#define QUEUE_PEEK_LAST_T JOIN(queue_peek_last, VALUE_LABEL)
#define QUEUE_ENQUEUE_T JOIN(queue_enqueue, VALUE_LABEL)
#define QUEUE_DEQUEUE_T JOIN(queue_dequeue, VALUE_LABEL)

static inline VALUE_TYPE QUEUENODE_GET_VALUE_T(const QueueNode* node_p) {
    assert(node_p != NULL);

    return *(VALUE_TYPE*)node_p->value_p;
}

static inline void QUEUENODE_SET_VALUE_T(QueueNode* node_p, VALUE_TYPE value) {
    assert(node_p != NULL);

    *(VALUE_TYPE*)node_p->value_p = value;
}

static inline VALUE_TYPE QUEUE_PEEK_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return QUEUENODE_GET_VALUE_T(queue_peek(queue_p));
}

static inline VALUE_TYPE QUEUE_PEEK_FIRST_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return QUEUENODE_GET_VALUE_T(queue_peek_first(queue_p));
}

static inline VALUE_TYPE QUEUE_PEEK_LAST_T(const Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    return QUEUENODE_GET_VALUE_T(queue_peek_last(queue_p));
}

static inline bool QUEUE_ENQUEUE_T(Queue* queue_p, VALUE_TYPE value) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    QueueNode* node_p = (QueueNode*)malloc(sizeof(QueueNode));
    if (node_p == NULL) {
        return false;
    }
    node_p->value_p = malloc(sizeof(VALUE_TYPE));
    if (node_p->value_p == NULL) {
        free(node_p);
        return false;
    }
    QUEUENODE_SET_VALUE_T(node_p, value);

    queue_enqueue(queue_p, node_p);

    return true;
}

static inline VALUE_TYPE QUEUE_DEQUEUE_T(Queue* queue_p) {
    assert(queue_p != NULL);
    assert(queue_p->value_size == sizeof(VALUE_TYPE));

    QueueNode* node_p = queue_dequeue(queue_p);
    void* value_p = node_p->value_p;

    VALUE_TYPE value = QUEUENODE_GET_VALUE_T(node_p);

    free(value_p);
    free(node_p);

    return value;
}

#undef QUEUENODE_GET_VALUE_T
#undef QUEUENODE_SET_VALUE_T
#undef QUEUE_PEEK_T
#undef QUEUE_PEEK_FIRST_T
#undef QUEUE_PEEK_LAST_T
#undef QUEUE_ENQUEUE_T
#undef QUEUE_DEQUEUE_T

#undef VALUE_TYPE
#undef VALUE_LABEL

#undef CAT
#undef PASTE
#undef JOIN

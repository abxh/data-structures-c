#include <stdbool.h>
#include <stdlib.h>

#ifndef CUSTOM_QUEUE_VALUE
typedef int q_value;
#endif

typedef struct q_element {
    q_value value;
    struct q_element *next;
} q_element;

typedef struct {
    q_element *front;
    q_element *back;
} queue;

queue *q_new() {
    queue *q = malloc(sizeof(queue));
    if (q != NULL) {
        q->front = NULL;
        q->back = NULL;
    }
    return q;
}

bool q_empty(queue *queue_p) { return queue_p->front == NULL; }

q_value q_peek(queue *queue_p) {
    return queue_p->front->value;
}

bool q_enqueue(queue* queue_p, q_value value) {
    q_element *elm_p = malloc(sizeof(q_element));
    if (elm_p != NULL) {
        elm_p->value = value;
        elm_p->next = NULL;
        if (queue_p->front == NULL) {
            queue_p->front = elm_p;
            queue_p->back = elm_p;
        } else {
            queue_p->back->next = elm_p;
            queue_p->back = elm_p;
        }
    }
    return elm_p != NULL;
}

q_value q_dequeue(queue* queue_p) {
    q_element *next = queue_p->front->next;
    q_value value = queue_p->front->value;
    free(queue_p->front);
    queue_p->front = next;
    if (queue_p->front == NULL) {
        queue_p->back = NULL;
    }
    return value;
}

void q_free(queue *queue_p) {
    q_element *l = queue_p->front;
    q_element *r = NULL;
    while (l != NULL) {
        r = l->next;
        free(l);
        l = r;
    }
    free(queue_p);
}

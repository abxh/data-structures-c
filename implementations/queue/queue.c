#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

queue *q_new() {
    queue *q = malloc(sizeof(queue));
    if (q != NULL) {
        q->front = NULL;
        q->back = NULL;
    }
    return q;
}

bool q_empty(queue *queue_p) {
    return queue_p->front == NULL;
}

Q_VALUE q_peek(queue *queue_p) {
    return queue_p->front->value;
}

bool q_enqueue(queue* queue_p, Q_VALUE value) {
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

Q_VALUE q_dequeue(queue* queue_p) {
    q_element *next = queue_p->front->next;
    Q_VALUE value = queue_p->front->value;
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

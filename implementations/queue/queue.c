#include <stdbool.h>
#include <stdlib.h>

#include "queue.h"

bool queue_enqueue(Queue* queue_p, void* value_p) {
    QElement* elm_p = malloc(sizeof(QElement));
    if (elm_p == NULL) {
        return false;
    }
    elm_p->value_p = value_p;
    elm_p->next_p = NULL;
    if (queue_p->front_p == NULL) {
        queue_p->front_p = elm_p;
        queue_p->back_p = elm_p;
    } else {
        queue_p->back_p->next_p = elm_p;
        queue_p->back_p = elm_p;
    }
    return true;
}

void* queue_dequeue(Queue* queue_p) {
    void* value_p = queue_p->front_p->value_p;
    QElement* front_p_new = queue_p->front_p->next_p;
    free(queue_p->front_p);
    queue_p->front_p = front_p_new;
    if (queue_p->front_p == NULL) {
        queue_p->back_p = NULL;
    }
    return value_p;
}

void queue_free(Queue* queue_p) {
    QElement* front_p = queue_p->front_p;
    QElement* next_p = NULL;
    while (front_p != NULL) {
        next_p = front_p->next_p;
        free(front_p->value_p);
        free(front_p);
        front_p = next_p;
    }
    free(queue_p);
}

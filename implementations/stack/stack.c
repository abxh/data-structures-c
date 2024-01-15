#include <stdbool.h>
#include <stdlib.h>
#include "stack.h"

stack *st_new() {
    stack *st_p = malloc(sizeof(stack));
    if (st_p != NULL) {
        st_p->head = NULL;
    }
    return st_p;
}

bool st_empty(stack *stack_p) { return stack_p->head == NULL; }

ST_VALUE st_peek(stack *stack_p) {
    return stack_p->head->value;
}

bool st_push(stack* stack_p, ST_VALUE value) {
    st_element *elm_p = malloc(sizeof(st_element));
    if (elm_p != NULL) {
        elm_p->value = value;
        elm_p->next = stack_p->head;
        stack_p->head = elm_p;
    }
    return elm_p != NULL;
}

ST_VALUE st_pop(stack* stack_p) {
    st_element *next_p = stack_p->head->next;
    ST_VALUE value = stack_p->head->value;
    free(stack_p->head);
    stack_p->head = next_p;
    return value;
}

void st_free(stack *stack_p) {
    st_element *l = stack_p->head;
    st_element *r = NULL;
    while (l != NULL) {
        r = l->next;
        free(l);
        l = r;
    }
    free(stack_p);
}

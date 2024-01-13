#include <stdbool.h>
#include <stdlib.h>


typedef int st_value;

typedef struct st_element {
    st_value value;
    struct st_element *next;
} st_element;

typedef struct {
    st_element *head;
} stack;

stack *st_new() {
    stack *st_p = malloc(sizeof(stack));
    if (st_p != NULL) {
        st_p->head = NULL;
    }
    return st_p;
}

bool st_empty(stack *stack_p) { return stack_p->head == NULL; }

st_value st_peek(stack *stack_p) {
    return stack_p->head->value;
}

bool st_push(stack* stack_p, st_value value) {
    st_element *elm_p = malloc(sizeof(st_element));
    if (elm_p != NULL) {
        elm_p->value = value;
        elm_p->next = stack_p->head;
        stack_p->head = elm_p;
    }
    return elm_p != NULL;
}

st_value st_pop(stack* stack_p) {
    st_element *next_p = stack_p->head->next;
    st_value value = stack_p->head->value;
    free(stack_p->head);
    stack_p->head = next_p;
    return value;
}

void st_free(stack *stack_p) {
    st_element *l = stack_p->head;
    st_element *r = NULL;
    while (l) {
        r = l->next;
        free(l);
        l = r;
    }
    free(stack_p);
}

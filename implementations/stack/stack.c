#include <stdbool.h>
#include <stdlib.h>

// Technicalities:
// - Last In [is] First Out (LIFO).

typedef int value_t;

typedef struct element {
    value_t value;
    struct element *next;
} element;

typedef struct {
    element *head;
} stack;

stack *st_new() {
    stack *st = malloc(sizeof(stack));
    if (st != NULL) {
        st->head = NULL;
    }
    return st;
}

bool st_empty(stack *stack) { return stack->head == NULL; }

value_t st_peek(stack *stack) {
    return stack->head->value;
}

element* st_push(stack* stack_p, value_t value) {
    element *elm = malloc(sizeof(element));
    if (elm != NULL) {
        elm->value = value;
        elm->next = stack_p->head;
        stack_p->head = elm;
    }
    return elm;
}

value_t st_pop(stack* stack_p) {
    element *next = stack_p->head->next;
    value_t value = stack_p->head->value;
    free(stack_p->head);
    stack_p->head = next;
    return value;
}

void st_free(stack *stack_p) {
    element *l = stack_p->head;
    element *r = NULL;
    while (l) {
        r = l->next;
        free(l);
        l = r;
    }
    free(stack_p);
}

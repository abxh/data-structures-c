#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

Stack* stack_new(void) {
    Stack* stack_p = malloc(sizeof(Stack));
    if (stack_p != NULL) {
        stack_p->next_p = NULL;
    }
    return stack_p;
}

bool stack_push(Stack* stack_p, void* value_p) {
    StackNode* elm_p = malloc(sizeof(StackNode));
    if (elm_p == NULL) {
        return false;
    }
    elm_p->value_p = value_p;
    elm_p->next_p = stack_p->next_p;
    stack_p->next_p = elm_p;
    return true;
}

void* stack_pop(Stack* stack_p) {
    void* value_p = stack_p->next_p->value_p;
    StackNode* next_p = stack_p->next_p->next_p;
    free(stack_p->next_p);
    stack_p->next_p = next_p;
    return value_p;
}

void stack_clear(Stack* stack_p) {
    StackNode* l = stack_p->next_p;
    StackNode* r = NULL;
    while (l != NULL) {
        r = l->next_p;
        free(l->value_p);
        free(l);
        l = r;
    }
    stack_p->next_p = NULL;
}

void stack_free(Stack* stack_p) {
    stack_clear(stack_p);
    free(stack_p);
}

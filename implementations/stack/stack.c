#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"

bool stack_push(Stack* stack_p, void* value_p) {
    StackNode* head_p = malloc(sizeof(StackNode));
    if (head_p == NULL) {
        return false;
    }
    head_p->value_p = value_p;
    head_p->next_p = *stack_p;
    *stack_p = head_p;
    return true;
}

void* stack_pop(Stack* stack_p) {
    void* value_p = (*stack_p)->value_p;
    StackNode* head_p_new = (*stack_p)->next_p;
    free(*stack_p);
    *stack_p = head_p_new;
    return value_p;
}

void stack_free(Stack* stack_p) {
    StackNode* head_p = *stack_p;
    StackNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }
    free(stack_p);
}

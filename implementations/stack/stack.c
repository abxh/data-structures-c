#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"

Stack* stack_new(size_t data_size) {
    Stack* stack_p = malloc(sizeof(Stack));
    if (stack_p == NULL) {
        return stack_p;
    }
    stack_p->head_p = NULL;
    stack_p->data_size = data_size;
    return stack_p;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->head_p == NULL;
}

void* stack_peek(const Stack* stack_p) {
    assert(stack_p->head_p != NULL);
    return stack_p->head_p->value_p;
}

bool stack_push(Stack* stack_p, void* value_p, size_t size) {
    assert(size == stack_p->data_size);
    StackNode* node_p = malloc(sizeof(StackNode));
    if (node_p == NULL) {
        return false;
    }
    node_p->value_p = malloc(stack_p->data_size);
    if (node_p->value_p == NULL) {
        free(node_p);
        return false;
    }
    memcpy(node_p->value_p, value_p, stack_p->data_size);
    node_p->next_p = stack_p->head_p;
    stack_p->head_p = node_p;
    return true;
}

void* stack_pop(Stack* stack_p, size_t size) {
    assert(stack_p->head_p != NULL);
    assert(size == stack_p->data_size);
    void* value_p = stack_p->head_p->value_p;
    StackNode* next_p = stack_p->head_p->next_p;
    free(stack_p->head_p);
    stack_p->head_p = next_p;
    return value_p;
}

void stack_free(Stack* stack_p) {
    StackNode* head_p = stack_p->head_p;
    StackNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }
    free(stack_p);
}

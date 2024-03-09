/*
   implementation file of a stack implemented as a linked-list.
*/

#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "stack.h"

bool stack_init(Stack** stack_pp, size_t value_size) {
    *stack_pp = (Stack*)malloc(sizeof(Stack));
    if (*stack_pp == NULL) {
        return false;
    }
    (*stack_pp)->head_p = NULL;
    (*stack_pp)->length = 0;
    (*stack_pp)->value_size = value_size;
    return true;
}

bool stack_deinit(Stack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    StackNode* head_p = (*stack_pp)->head_p;
    StackNode* next_p = NULL;
    while (head_p != NULL) {
        next_p = head_p->next_p;
        free(head_p->value_p);
        free(head_p);
        head_p = next_p;
    }
    free(*stack_pp);
    (*stack_pp) = NULL;
    return true;
}

size_t stack_length(const Stack* stack_p) {
    return stack_p->length;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->head_p == NULL;
}

StackNode* stack_peek(const Stack* stack_p) {
    assert(stack_isempty(stack_p) == false);
    return stack_p->head_p;
}

void stack_push(Stack* stack_p, StackNode* node_p) {
    node_p->next_p = stack_p->head_p;
    stack_p->head_p = node_p;
    stack_p->length++;
}

StackNode* stack_pop(Stack* stack_p) {
    assert(stack_isempty(stack_p) == false);
    StackNode* node_p = stack_p->head_p;
    stack_p->head_p = node_p->next_p;
    stack_p->length--;
    return node_p;
}

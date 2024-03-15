#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, free
#include <string.h>  // memcpy

#include "stack.h"

bool stack_init(Stack** stack_pp, const size_t value_size) {
    assert(stack_pp != NULL);
    assert(value_size != 0);

    *stack_pp = (Stack*)malloc(sizeof(Stack));
    if (*stack_pp == NULL) {
        return false;
    }
    (*stack_pp)->head_p = NULL;
    (*stack_pp)->count = 0;
    (*stack_pp)->value_size = value_size;
    (*stack_pp)->freed_nodes = NULL;

    return true;
}

bool stack_deinit(Stack** stack_pp) {
    assert(stack_pp != NULL);

    if (*stack_pp == NULL) {
        return false;
    }

    // traverse list and free nodes one by one
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

size_t stack_count(const Stack* stack_p) {
    assert(stack_p != NULL);

    return stack_p->count;
}

bool stack_isempty(const Stack* stack_p) {
    assert(stack_p != NULL);

    return stack_p->head_p == NULL;
}

const StackNode* stack_peek_node(const Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_isempty(stack_p) == false);

    return stack_p->head_p;
}

void stack_push_node(Stack* stack_p, StackNode* node_p) {
    assert(stack_p != NULL);

    node_p->next_p = stack_p->head_p;
    stack_p->head_p = node_p;
    stack_p->count++;
}

StackNode* stack_pop_node(Stack* stack_p) {
    assert(stack_p != NULL);
    assert(stack_isempty(stack_p) == false);

    StackNode* node_p = stack_p->head_p;
    stack_p->head_p = node_p->next_p;
    stack_p->count--;

    return node_p;
}

StackNode* stacknode_create(Stack* stack_p) {
    if (stack_p->freed_nodes != NULL) {
        StackNode* node_p = stack_p->freed_nodes;
        stack_p->freed_nodes = stack_p->freed_nodes->next_p;
        return node_p;
    }
    StackNode* node_p = (StackNode*)malloc(sizeof(StackNode));
    if (node_p == NULL) {
        return false;
    }
    node_p->value_p = malloc(sizeof(stack_p->value_size));
    if (node_p->value_p == NULL) {
        free(node_p);
        return false;
    }
    return node_p;
}

void stacknode_free(Stack* stack_p, StackNode* node_p) {
    node_p->next_p = stack_p->freed_nodes;
    stack_p->freed_nodes = node_p;
}

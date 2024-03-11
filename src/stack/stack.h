#pragma once

#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct StackNode {
    struct StackNode* next_p;
    void* value_p;
} StackNode;

typedef struct {
    StackNode* head_p;
    size_t value_size;
    size_t count;
} Stack;

bool stack_init(Stack** stack_pp, size_t value_size);

bool stack_deinit(Stack** stack_pp);

StackNode* stack_peek(const Stack* stack_p);

size_t stack_count(const Stack* stack_p);

bool stack_isempty(const Stack* stack_p);

void stack_push(Stack* stack_p, StackNode* node_p);

StackNode* stack_pop(Stack* stack_p);

#define stack_foreach(stack_p, node_p, value)                                            \
    for ((assert((stack_p)->value_size == sizeof(value)), (node_p) = (stack_p)->head_p); \
         (node_p) != NULL && ((value) = *(typeof(value)*)(node_p)->value_p, true); (node_p) = (node_p)->next_p)

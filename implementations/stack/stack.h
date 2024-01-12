#include <stdbool.h>
#include <stdlib.h>

#pragma once

typedef int value_t;

typedef struct element {
    value_t value;
    struct element *next;
} element;

typedef struct {
    element *head;
} stack;

/* create a new LIFO stack. can return NULL and it must be checked manually. */
stack *st_new();

/* return if the stack is empty. stack pointer cannot be NULL.*/
bool st_empty(stack *stack);

/* 
   peek at the next value to be popped. stack pointer cannot be NULL.
   must check if stack is empty beforehand. stack pointer cannot be NULL.
*/
value_t st_peek(stack *stack);

/* 
   push a value onto the stack. returns if memory was able to be allocated
   for the value. stack pointer cannot be NULL.
*/
bool st_push(stack* stack_p, value_t value);

/* 
   return the popped value from the stack. must check if stack is empty
   beforehand. stack pointer cannot be NULL.
´*/
value_t st_pop(stack* stack_p);

/* free the stack from the heap. stack pointer cannot be NULL.*/
void st_free(stack *stack_p);

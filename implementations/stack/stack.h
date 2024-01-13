#include <stdbool.h>
#include <stdlib.h>

#pragma once

typedef int st_value;

typedef struct element {
    st_value value;
    struct element *next;
} st_element;

typedef struct {
    st_element *head;
} stack;

/* create a new LIFO stack. can return NULL and it must be checked manually. */
stack *st_new();

/* return if the stack is empty. stack pointer cannot be NULL.*/
bool st_empty(stack *stack_p);

/* 
   peek at the next value to be popped. stack pointer cannot be NULL.
   must check if stack is empty beforehand. stack pointer cannot be NULL.
*/
st_value st_peek(stack *stack_p);

/* 
   push a value onto the stack. returns if memory was able to be allocated
   for the value. stack pointer cannot be NULL.
*/
bool st_push(stack* stack_p, st_value value);

/* 
   return the popped value from the stack. must check if stack is empty
   beforehand. stack pointer cannot be NULL.
´*/
st_value st_pop(stack* stack_p);

/* free the stack from the heap. stack pointer cannot be NULL.*/
void st_free(stack *stack_p);

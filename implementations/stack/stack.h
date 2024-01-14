#include <stdbool.h>
#include <stdlib.h>

#pragma once

#ifndef CUSTOM_STACK_VALUE
typedef int st_value;
#endif

typedef struct element {
    st_value value;
    struct element *next;
} st_element;

typedef struct {
    st_element *head;
} stack;

/* Create a new LIFO stack. Can return NULL and it must be checked manually. */
stack *st_new();

/* Return if the stack is empty. Stack pointer cannot be NULL.*/
bool st_empty(stack *stack_p);

/* 
   Peek at the next value to be popped. Stack pointer cannot be NULL.
   Must check if stack is empty beforehand.
*/
st_value st_peek(stack *stack_p);

/* 
   Push a value onto the stack. Returns if memory was able to be allocated
   for the value. Stack pointer cannot be NULL.
*/
bool st_push(stack* stack_p, st_value value);

/* 
   Pop the stack and return the value. Must check if stack is empty
   beforehand. Stack pointer cannot be NULL.
´*/
st_value st_pop(stack* stack_p);

/* Free the stack from the heap. Stack pointer cannot be NULL.*/
void st_free(stack *stack_p);

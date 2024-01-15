#include <stdbool.h>
#include <stdlib.h>

#pragma once

#ifndef ST_VALUE
#define ST_VALUE double
#endif

typedef struct st_element {
    ST_VALUE value;
    struct st_element *next;
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
ST_VALUE st_peek(stack *stack_p);

/* 
   Push a value onto the stack. Returns if memory was able to be allocated
   for the value. Stack pointer cannot be NULL.
*/
bool st_push(stack* stack_p, ST_VALUE value);

/* 
   Pop the stack and return the value. Must check if stack is empty
   beforehand. Stack pointer cannot be NULL.
´*/
ST_VALUE st_pop(stack* stack_p);

/* Free the stack from the heap. Stack pointer cannot be NULL.*/
void st_free(stack *stack_p);

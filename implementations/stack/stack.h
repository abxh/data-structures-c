#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct StackNode {
    void* value_p;
    struct StackNode* next_p;
} StackNode;

typedef StackNode Stack;

/* Create a new stack. Can return NULL and it must be checked manually.*/
Stack* stack_new(void);

/* Return if the stack is empty. */
static inline bool stack_empty(const Stack* stack_p) {
    return stack_p->next_p == NULL;
}

/* Peek at the next value pointer to be popped. Must check if stack is empty
 * beforehand.*/
static inline void* stack_peek(const Stack* stack_p) {
    return stack_p->next_p->value_p;
}

/* Push a value pointer onto the stack. Returns if successful. */
bool stack_push(Stack* stack_p, void* element_p);

/* Pop the stack and return the value pointer. Must check if stack is empty
 * beforehand.*/
void* stack_pop(Stack* stack_p);

/* Clear the stack of it's elements.*/
void stack_clear(Stack* stack_p);

/* Free the memory of the stack appropiately.*/
void stack_free(Stack* stack_p);

/* Create inline functions to directly work with stack values with appropiate
 * memory handling. */
#define CREATE_STACK_INLINE_FUNCTIONS(name, type)                              \
    static inline type stack_peek_##name(const Stack* stack_p) {               \
        return *(type*)stack_peek(stack_p);                                    \
    }                                                                          \
    static inline bool stack_push_##name(Stack* stack_p, type value) {         \
        void* value_p = malloc(sizeof(type));                                  \
        if (value_p == NULL) {                                                 \
            return false;                                                      \
        }                                                                      \
        memcpy(value_p, &value, sizeof(type));                                 \
        if (stack_push(stack_p, value_p)) {                                    \
            return true;                                                       \
        }                                                                      \
        free(value_p);                                                         \
        return false;                                                          \
    }                                                                          \
    static inline type stack_pop_##name(Stack* stack_p) {                      \
        void* value_t = stack_pop(stack_p);                                    \
        type value = *(type*)value_t;                                          \
        free(value_t);                                                         \
        return value;                                                          \
    }

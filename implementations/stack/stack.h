#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct StackNode {
    void* value_p;
    struct StackNode* next_p;
} StackNode;

typedef StackNode* Stack;

/* Create a new stack and return it's pointer. can return NULL and it should
 * be checked manually.*/
static inline Stack* stack_new(void) {
    return (Stack*)calloc(1, sizeof(Stack)); // also sets the value and next pointer to NULL
}

/* Return if the stack is empty. */
static inline bool stack_isempty(const Stack* stack_p) {
    return *stack_p == NULL;
}

/* Peek at the next value pointer to be popped. Must check if stack is empty
 * beforehand.*/
static inline void* stack_peek(const Stack* stack_p) {
    return (*stack_p)->value_p;
}

/* Push a heap allocated value pointer onto the stack. Returns if successful.*/
bool stack_push(Stack* stack_p, void* value_p);

/* Pop the stack and return the heap allocated value pointer. Must check if stack
 * is empty beforehand.*/
void* stack_pop(Stack* stack_p);

/* Free the memory of stack appropiately.*/
void stack_free(Stack* stack_p);

/* Create inline functions to directly work with stack values with appropiate
 * memory handling.*/
#define CREATE_STACK_INLINE_FUNCTIONS(name, type, default_)                       \
    static inline type stack_peek_##name(const Stack* stack_p) {                  \
        void* value_p = stack_peek(stack_p);                                      \
        if (value_p == NULL) {                                                    \
            return (default_);                                                    \
        }                                                                         \
        return *(type*)value_p;                                                   \
    }                                                                             \
    static inline bool stack_push_##name(Stack* stack_p, type value) {            \
        void* value_p = malloc(sizeof(type));                                     \
        if (value_p == NULL) {                                                    \
            return false;                                                         \
        }                                                                         \
        memcpy(value_p, &value, sizeof(type));                                    \
        if (stack_push(stack_p, value_p)) {                                       \
            return true;                                                          \
        }                                                                         \
        free(value_p);                                                            \
        return false;                                                             \
    }                                                                             \
    static inline type stack_pop_##name(Stack* stack_p) {                         \
        void* value_p = stack_pop(stack_p);                                       \
        if (value_p == NULL) {                                                    \
            return (default_);                                                    \
        }                                                                         \
        type value = *(type*)value_p;                                             \
        free(value_p);                                                            \
        return value;                                                             \
    }                                                                             \
    static inline type stacknode_get_##name(StackNode* stacknode_p) {             \
        void* value_p = stacknode_p->value_p;                                     \
        if (value_p == NULL) {                                                    \
            return (default_);                                                    \
        }                                                                         \
        return *(type*)value_p;                                                   \
    }                                                                             \
    static inline void stacknode_set_##name(StackNode* stacknode_p, type value) { \
        memcpy(stacknode_p->value_p, &value, sizeof(type));                       \
    }

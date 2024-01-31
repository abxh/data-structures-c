#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct StackNode {
    void* value_p;
    struct StackNode* next_p;
} StackNode;

typedef struct {
    size_t data_size;
    StackNode* head_p;
} Stack;

/* Try create a new stack and returns it's pointer. Returns NULL if OOM. */
Stack* stack_new(size_t data_size);

/* Return if the stack is empty. */
bool stack_isempty(const Stack* stack_p);

/* Peek at the next value to be popped. Should check for empty stack beforehand. */
void* stack_peek(const Stack* stack_p);

/* Push a value onto the stack and return the pointer to it. Returns if successful. */
bool stack_push(Stack* stack_p, void* value_p, size_t size);

/* Pop a value from the stack and return the pointer to it. Should check for empty stack
 * beforehand and free the returned value from heap. */
void* stack_pop(Stack* stack_p, size_t size);

/* Free the memory of the stack appropiately. */
void stack_free(Stack* stack_p);

/* Create inline functions to directly work with stack values. */
#define CREATE_STACK_INLINE_FUNCTIONS(name, type)                                   \
    static inline Stack* stack_new_##name(void) {                                   \
        return stack_new(sizeof(type));                                             \
    }                                                                               \
    static inline type stack_peek_##name(Stack* stack_p) {                          \
        return *(type*)stack_peek(stack_p);                                         \
    }                                                                               \
    static inline bool stack_push_##name(Stack* stack_p, type value) {              \
        return stack_push(stack_p, &value, sizeof(type));                           \
    }                                                                               \
    static inline type stack_pop_##name(Stack* stack_p) {                           \
        void* value_p = stack_pop(stack_p, sizeof(type));                           \
        type value = *(type*)value_p;                                               \
        free(value_p);                                                              \
        return value;                                                               \
    }                                                                               \
    static inline type stack_node_get_##name(StackNode* stack_node_p) {             \
        return *(type*)stack_node_p->value_p;                                       \
    }                                                                               \
    static inline void stack_node_set_##name(StackNode* stack_node_p, type value) { \
        memcpy(stack_node_p->value_p, &value, sizeof(type));                        \
    }

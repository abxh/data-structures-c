#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t data_size;
    size_t used_bytes;
    size_t capacity_bytes;
    uint8_t* darr;
} Stack;

/* Try create a new stack and returns it's pointer. Returns NULL if OOM. */
Stack* stack_new(size_t data_size);

/* Check if stack is empty */
bool stack_isempty(const Stack* stack_p);

/* Peek at the next value to be popped. Should check that stack is not empty beforehand. */
void* stack_peek(const Stack* stack_p);

/* Push a value onto the stack. */
bool stack_push(Stack* stack_p, void* value_p, size_t size);

/* Pop a value from the stack. Should check that stack is not empty beforehand. */
void* stack_pop(Stack* stack_p, size_t size);

/* Free the memory of the stack appropiately. */
void stack_free(Stack* stack_p);

/* Create inline functions to directly work with stack values. */
#define CREATE_STACK_INLINE_FUNCTIONS(name, type, default_value)                                     \
    static inline Stack* stack_new_##name(void) {                                                    \
        return stack_new(sizeof(type));                                                              \
    }                                                                                                \
    static inline type stack_peek_##name(Stack* stack_p) {                                           \
        return !stack_isempty(stack_p) ? (*(type*)stack_peek(stack_p)) : (default_value);            \
    }                                                                                                \
    static inline bool stack_push_##name(Stack* stack_p, type value) {                               \
        return stack_push(stack_p, &value, sizeof(type));                                            \
    }                                                                                                \
    static inline type stack_pop_##name(Stack* stack_p) {                                            \
        return !stack_isempty(stack_p) ? *(type*)stack_pop(stack_p, sizeof(type)) : (default_value); \
    }

#pragma once

#include <stdalign.h> // alignof, alignas
#include <stdbool.h>  // bool
#include <stdlib.h>   // size_t

typedef struct {
    size_t used;
    size_t capacity;
    size_t data_size;
    alignas(alignof(size_t)) unsigned char arr[];
} Stack;

/* Create a new stack of some maximum capacity for some data size. Returns NULL if OOM. */
Stack* stack_new(size_t capacity, size_t data_size);

/* Return if the stack is empty. */
bool stack_isempty(const Stack* stack_p);

/* Return if the stack is full. */
bool stack_isfull(const Stack* stack_p);

/* Peek at the next bytes. Should check for empty stack beforehand. */
unsigned char* stack_peek(Stack* stack_p);

/* Push value onto the stack as bytes. Should check for full stack beforehand. */
void stack_push(Stack* stack_p, unsigned char* value);

/* Pop a value from the stack and return the pointer to it. Should check for empty stack
 * beforehand and free the returned value from heap. */
unsigned char* stack_pop(Stack* stack_p);

/* Free the memory of the stack appropiately. */
void stack_free(Stack* stack_p);

/* Create inline functions to directly work with stack values. */
#define STACK_CREATE_INLINE_FUNCTIONS(name, type)                      \
    static inline Stack* stack_new_##name(size_t capacity) {           \
        return stack_new(capacity, sizeof(type));                      \
    }                                                                  \
    static inline type stack_peek_##name(Stack* stack_p) {             \
        return *(type*)stack_peek(stack_p);                            \
    }                                                                  \
    static inline void stack_push_##name(Stack* stack_p, type value) { \
        stack_push(stack_p, (unsigned char*)&value);                   \
    }                                                                  \
    static inline type stack_pop_##name(Stack* stack_p) {              \
        return *(type*)stack_pop(stack_p);                             \
    }

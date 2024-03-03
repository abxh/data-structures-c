#pragma once

#include <assert.h>   // static_assert
#include <stdbool.h>  // bool
#include <stdlib.h>   // size_t, typeof

typedef struct {
    size_t used;
    size_t capacity;
    size_t data_size;
    unsigned char* arr_p;
} Stack;

/* Create a new stack of some maximum capacity for some data size. Returns
   NULL if OOM or `capacity * data_size` exceeds SIZE_MAX. */
Stack* stack_new(size_t capacity, size_t data_size);

/* Return if the stack is empty. */
bool stack_isempty(const Stack* stack_p);

/* Return if the stack is full. */
bool stack_isfull(const Stack* stack_p);

/* Peek at the next popped value of a non-empty stack as bytes. */
unsigned char* stack_peek(const Stack* stack_p);

/* Push value onto a non-full stack as bytes. */
void stack_push(Stack* stack_p, const unsigned char* bytes);

/* Pop a value from a non-empty stack and return it as bytes. */
unsigned char* stack_pop(Stack* stack_p);

/* Resize stack to have a new non-zero capacity. Returns false if OOM or
   `new_capacity x data_size` exceeds SIZE_MAX. */
bool stack_resize(Stack* stack_p, size_t new_capacity);

/* Free the memory of a stack appropiately and set it's pointer to NULL.
   This may be called even after the stack pointer is NULL. */
void stack_free(Stack** stack_pp);

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

/* Iterate through the stack starting from the next popped value. */
#define STACK_FOREACH(stack_p, index, value)                                          \
    for ((static_assert(sizeof(typeof(value)) == (stack_p)->data_size), (index) = 0); \
         (index) < stack_p->used && ((value) = *(typeof(value)*)((stack_p)->arr_p + i_ * (stack_p)->data_size), true); index++)

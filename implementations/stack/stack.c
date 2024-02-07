#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stddef.h>  // offsetof
#include <stdlib.h>  // malloc, size_t, NULL

#include "stack.h"

Stack* stack_new(size_t capacity, size_t data_size) {
    assert(capacity != 0);
    assert(data_size != 0);
    Stack* stack_p = malloc(offsetof(Stack, arr) + capacity * data_size);
    if (stack_p == NULL) {
        return stack_p;
    }
    stack_p->used = 0;
    stack_p->capacity = capacity;
    stack_p->data_size = data_size;
    return stack_p;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->used == 0;
}

bool stack_isfull(const Stack* stack_p) {
    return stack_p->used == stack_p->capacity;
}

char* stack_peek(Stack* stack_p) {
    assert(stack_p->used != 0);
    return &stack_p->arr[stack_p->data_size * (stack_p->used - 1)];
}

void stack_push(Stack* stack_p, char* value) {
    assert(stack_p->used != stack_p->capacity);
    memcpy(stack_p->arr + stack_p->data_size * stack_p->used++, value, stack_p->data_size);
}

char* stack_pop(Stack* stack_p) {
    assert(stack_p->used != 0);
    return &stack_p->arr[stack_p->data_size * --stack_p->used];
}

void stack_free(Stack* stack_p) {
    free(stack_p);
}

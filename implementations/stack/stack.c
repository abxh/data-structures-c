#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

#include "stack.h"

Stack* stack_new(size_t data_size) {
    Stack* stack_p = malloc(sizeof(Stack));
    if (stack_p == NULL) {
        return NULL;
    }
    stack_p->data_size = data_size;
    stack_p->used_bytes = 0;
    stack_p->capacity_bytes = data_size;
    stack_p->darr = malloc(data_size);
    if (stack_p->darr == NULL) {
        free(stack_p);
        return NULL;
    }
    return stack_p;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->used_bytes == 0;
}

void* stack_peek(const Stack* stack_p) {
    assert(stack_p->used_bytes != 0);
    return stack_p->darr;
}

bool stack_push(Stack* stack_p, void* value_p, size_t size) {
    assert(size == stack_p->data_size);
    if (stack_p->used_bytes == stack_p->capacity_bytes) {
        void* new_darr = reallocarray(stack_p->darr, stack_p->capacity_bytes << 1, stack_p->data_size);
        if (new_darr == NULL) {
            errno = ENOMEM;
            perror("realloc stack array");
            return false;
        }
        stack_p->capacity_bytes <<= 1;
        stack_p->darr = new_darr;
    } 
    memcpy(stack_p->darr + stack_p->used_bytes, value_p, stack_p->data_size);
    stack_p->used_bytes += stack_p->data_size;
    return true;
}

void* stack_pop(Stack* stack_p, size_t size) {
    assert(stack_p->used_bytes != 0);
    assert(size == stack_p->data_size);
    if ((stack_p->used_bytes >> 2) == stack_p->capacity_bytes) {
        void* new_darr = realloc(stack_p->darr, (stack_p->capacity_bytes >> 1) * stack_p->data_size);
        if (new_darr == NULL) {
            errno = ENOMEM;
            perror("realloc stack array");
            return NULL;
        }
        stack_p->capacity_bytes >>= 1;
        stack_p->darr = new_darr;
    } 
    stack_p->used_bytes -= stack_p->data_size;
    return stack_p->darr + stack_p->used_bytes;
}

void stack_free(Stack* stack_p) {
    free(stack_p->darr);
    free(stack_p);
}


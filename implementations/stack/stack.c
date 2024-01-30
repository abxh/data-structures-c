#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

#include "stack.h"

uint64_t nextpow2(uint32_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

Stack* stack_new(size_t data_size) {
    assert(data_size != 0);
    Stack* stack_p = malloc(sizeof(Stack));
    if (stack_p == NULL) {
        return NULL;
    }
    stack_p->data_size = data_size;
    stack_p->aligned_data_size = nextpow2(data_size);
    stack_p->used_bytes = 0;
    stack_p->capacity_bytes = stack_p->aligned_data_size + data_size;
    stack_p->darr = malloc(stack_p->aligned_data_size + data_size);
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
    stack_p->used_bytes -= stack_p->data_size;
    return stack_p->darr + stack_p->used_bytes;
}

bool stack_shrink(Stack* stack_p) {
    while (stack_p->capacity_bytes != stack_p->data_size && (stack_p->used_bytes >> 2) <= stack_p->capacity_bytes) {
        void* new_darr = realloc(stack_p->darr, (stack_p->capacity_bytes >> 1) * stack_p->data_size);
        if (new_darr == NULL) {
            errno = ENOMEM;
            perror("realloc stack array");
            return false;
        }
        stack_p->capacity_bytes >>= 1;
        stack_p->darr = new_darr;
    } 
    return true;
}

void stack_clear(Stack* stack_p) {
    stack_p->used_bytes = 0;
}

void stack_free(Stack* stack_p) {
    free(stack_p->darr);
    free(stack_p);
}


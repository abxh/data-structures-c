#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, calloc, free

#define NGENERATING_MACROS
#include "astack.h"

bool astack_init(AStack** stack_pp, size_t capacity, size_t value_size) {
    assert(capacity != 0);
    *stack_pp = malloc(sizeof(AStack));
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->arr_p = calloc(capacity, sizeof(value_size));
    if ((*stack_pp)->arr_p == NULL) {
        free(*stack_pp);
        *stack_pp = NULL;
        return false;
    }
    (*stack_pp)->used_count = 0;
    (*stack_pp)->capacity = capacity;
    (*stack_pp)->value_size = value_size;
    return true;
}

bool astack_deinit(AStack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
    return true;
}

bool astack_isempty(AStack* stack_p) {
    return stack_p->used_count == 0;
}

bool astack_isfull(AStack* stack_p) {
    return stack_p->used_count == stack_p->capacity;
}

bool astack_used_count(AStack* stack_p) {
    return stack_p->used_count;
}

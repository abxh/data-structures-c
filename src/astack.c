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
    (*stack_pp)->length = 0;
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
    return stack_p->length == 0;
}

bool astack_isfull(AStack* stack_p) {
    return stack_p->length == stack_p->capacity;
}

size_t astack_length(AStack* stack_p) {
    return stack_p->length;
}

size_t astack_capacity(AStack* stack_p) {
    return stack_p->capacity;
}

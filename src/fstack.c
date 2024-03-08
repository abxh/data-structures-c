#include <assert.h>  // assert
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // NULL, size_t, malloc, calloc, reallocarray, free

#define NGENERATING_MACROS
#include "fstack.h"

bool fstack_init(FStack** stack_pp, size_t capacity, size_t value_size) {
    assert(capacity != 0);
    *stack_pp = malloc(sizeof(FStack));
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->arr_p = calloc(capacity, sizeof(value_size));
    if ((*stack_pp)->arr_p == NULL) {
        free(*stack_pp);
        *stack_pp = NULL;
        return false;
    }
    (*stack_pp)->used = 0;
    (*stack_pp)->capacity = capacity;
    (*stack_pp)->value_size = value_size;
    return true;
}

bool fstack_deinit(FStack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
    return true;
}

bool fstack_isempty(FStack* stack_p) {
    return stack_p->used == 0;
}

bool fstack_isfull(FStack* stack_p) {
    return stack_p->used == stack_p->capacity;
}

bool fstack_resize(FStack* stack_p, size_t new_capacity) {
    assert(new_capacity != 0);
    void* new_arr_p = reallocarray(stack_p->arr_p, new_capacity, stack_p->value_size);
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

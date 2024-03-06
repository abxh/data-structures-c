#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // malloc, calloc, reallocarray, NULL, size_t

#define NDEFINE_TEMPLATE_METHODS
#include "stack.h"

bool stack_init(Stack** stack_pp, size_t capacity, size_t data_size) {
    assert(capacity != 0);
    assert(data_size != 0);
    *stack_pp = malloc(sizeof(Stack));
    if ((*stack_pp) == NULL) {
        return false;
    }
    (*stack_pp)->arr_p = calloc(capacity, data_size);
    if ((*stack_pp)->arr_p == NULL) {
        free(*stack_pp);
        *stack_pp = NULL;
        return false;
    }
    (*stack_pp)->used = 0;
    (*stack_pp)->capacity = capacity;
    (*stack_pp)->data_size = data_size;
    return true;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->used == 0;
}

bool stack_isfull(const Stack* stack_p) {
    return stack_p->used == stack_p->capacity;
}

bool stack_resize(Stack* stack_p, size_t new_capacity) {
    assert(new_capacity != 0);
    void* new_arr_p = reallocarray(stack_p->arr_p, new_capacity, stack_p->data_size);
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

bool stack_free(Stack** stack_pp) {
    if (*stack_pp == NULL) {
        return false;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
    return true;
}

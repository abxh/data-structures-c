
#include <assert.h>

#define T float
#include "stack.h"

void float_stack_demo(void) {
    float_stack_type* s_ptr = float_stack_create();
    if (!s_ptr) {
        assert(false);
    }

    assert(float_stack_is_empty(s_ptr));

    float_stack_push(s_ptr, 1.F);

    assert(!float_stack_is_empty(s_ptr));
    assert(1 == float_stack_get_count(s_ptr));

    assert(1.F == float_stack_get_top(s_ptr));
    assert(1.F == float_stack_peek(s_ptr));

    float res = float_stack_pop(s_ptr);
    assert(1.F == res);

    float_stack_destroy(s_ptr);
}

#define PREFIX uint_stk
#define T unsigned long int
#include "stack.h"

// alt:
// typedef unsigned long int uint;
// #define T uint

void uint_stk_demo(void) {
    uint_stk_type* s_ptr = uint_stk_create();

    // alt:
    // uint_stk_type* s_ptr = uint_stk_create_with_initial_capacity(1e+6);

    if (!s_ptr) {
        assert(false);
    }

    for (unsigned long int i = 1; i <= 1e+6; i++) {
        assert(true == uint_stk_push(s_ptr, i));
    }

    assert(1e+6 <= uint_stk_get_capacity(s_ptr));

    uint_stk_type* s_clone_ptr = uint_stk_clone(s_ptr);
    if (!s_clone_ptr) {
        assert(false);
    }

    for (unsigned long int i = 1e+6; i >= 1; i--) {
        assert(i == uint_stk_pop(s_ptr));
    }

    size_t x = 1e+6;

    // {...} for scoped variables
    { 
        // macro is defined as:
        // T_stack_for_each(in_T_stack_ptr, out_size_t_count, out_T_value)

        size_t count;
        unsigned long int value;
        T_stack_for_each(s_clone_ptr, count, value) {
            assert(value == x--);
        }
    }

    uint_stk_destroy(s_ptr);
    uint_stk_destroy(s_clone_ptr);
}

int main(void) {
    float_stack_demo();
    uint_stk_demo();
    return 0;
}

#include <assert.h>

#define VALUE_TYPE float
#include "stack.h"

void float_stack_demo(void) {
    float_stack_type* s = float_stack_create();
    if (!s) {
        assert(false);
    }
    float_stack_is_empty(s);

    assert(float_stack_is_empty(s));

    float_stack_push(s, 1.F);
    float_stack_push(s, 2.F);
    float_stack_push(s, 3.F);

    assert(!float_stack_is_empty(s));

    assert(3 == float_stack_count(s));

    assert(3.F == float_stack_top(s));

    assert(3.F == float_stack_peek(s));

    assert(1.F == float_stack_at(s, 0));
    assert(2.F == float_stack_at(s, 1));
    assert(3.F == float_stack_at(s, 2));

    float res = float_stack_pop(s);

    assert(3.F == res);

    float_stack_destroy(s);
}

typedef unsigned int uint;
#define VALUE_TYPE uint
#include "stack.h"

void uint_stack_demo(void) {
    uint_stack_type* s = uint_stack_create();

    if (!s) {
        assert(false);
    }

    for (unsigned long int i = 1; i <= 1e+6; i++) {
        assert(true == uint_stack_push(s, i));
    }

    assert(1e+6 <= uint_stack_capacity(s));

    uint_stack_type* s_copy = uint_stack_clone(s);
    if (!s_copy) {
        assert(false);
    }

    for (unsigned long int i = 1e+6; i >= 1; i--) {
        assert(i == uint_stack_pop(s));
    }

    size_t x = 1e+6;
    size_t stack_val_count = 0;

    // {...} for scoped variables
    {
        size_t count;
        unsigned long int value;
        stack_for_each(s_copy, count, value) {
            assert(value == x--);
            stack_val_count++;
        }
    }

    assert(stack_val_count == 1e+6);

    uint_stack_destroy(s);
    uint_stack_destroy(s_copy);
}

int main(void) {
    float_stack_demo();
    uint_stack_demo();
    return 0;
}

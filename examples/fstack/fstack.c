#include <assert.h>

#include <assert.h>

#define NAME int_stack
#define VALUE_TYPE int
#include "fstack.h"

void int_stack_example(void) {
    int_stack_type* s = int_stack_create(3);

    if (!s) {
        assert(false);
    }

    int_stack_is_empty(s);

    assert(int_stack_is_empty(s));
    assert(!int_stack_is_full(s));

    int_stack_push(s, 1);
    int_stack_push(s, 2);
    int_stack_push(s, 3);

    assert(!int_stack_is_empty(s));
    assert(int_stack_is_full(s));

    assert(s->count == 3);

    assert(int_stack_get_top(s) == 3);
    assert(int_stack_get_bottom(s) == 1);

    assert(int_stack_peek(s) == 3);

    assert(int_stack_at(s, 0) == 3);
    assert(int_stack_at(s, 1) == 2);
    assert(int_stack_at(s, 2) == 1);

    const int res = int_stack_pop(s);
    assert(res == 3);

    assert(s->count == 2);

    int_stack_clear(s);

    assert(s->count == 0);

    int_stack_destroy(s);
}

#define NAME uintstk
#define VALUE_TYPE unsigned int
#include "fstack.h"

void uintstk_example(void) {
    const size_t lim = 1e+6;

    uintstk_type* s = uintstk_create(lim);
    if (!s) {
        assert(false);
    }

    for (unsigned int i = 1; i <= lim; i++) {
        uintstk_push(s, i);
    }

    uintstk_type* s_copy = uintstk_create(s->capacity);
    if (!s_copy) {
        assert(false);
    }
    uintstk_copy(s_copy, s);

    for (unsigned int i = lim; i >= 1; i--) {
        assert(i == uintstk_pop(s));
    }

    {
        size_t x = 1;
        unsigned int value;

        size_t tempi;
        fstack_for_each_reverse(s_copy, tempi, value) {
            assert(value == x++);
        }
    }

    {
        size_t cpy_count = 0;
        size_t x = lim;
        unsigned int value;

        size_t tempi;
        fstack_for_each(s_copy, tempi, value) {
            assert(value == x--);
            cpy_count++;
        }
        assert(cpy_count == lim);
    }

    uintstk_destroy(s);
    uintstk_destroy(s_copy);
}

int main(void) {
    int_stack_example();
    uintstk_example();
}

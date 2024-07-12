/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 2
    - N := 10
    - N := 1e+6

    Non-mutating operation types / properties:
    - .count
    - .capacity
    - is_empty
    - is_full
    - get_top / peek
    - get_bottom
    - at + FSTACK_FOREACH + FSTACK_FOREACH_REVERSE

    Mutating operation types:
    - push
    - pop
    - clear

    Memory operations [to also be tested with sanitizers]:
    - create
    - destroy
    - copy
*/

#define NAME i64_stk
#define VALUE_TYPE int64_t
#include "fstack.h"

static inline bool check_count_invariance(const i64_stk_type* stk_p, const size_t push_op_count, const size_t pop_op_count) {
    // push and pop operations *after* the stack is cleared / created
    return stk_p->count == (push_op_count - pop_op_count);
}

static inline bool check_capacity_invariance(const i64_stk_type* stk_p, const size_t old_capacity) {
    return stk_p->capacity >= old_capacity;
}

static inline bool check_empty_full(const i64_stk_type* stk_p, const size_t push_op_count, const size_t pop_op_count) {
    // push and pop operations *after* the stack is cleared / created
    const size_t diff = push_op_count - pop_op_count;

    if (i64_stk_is_empty(stk_p)) {
        return diff == 0;
    } else if (i64_stk_is_full(stk_p)) {
        return diff == stk_p->capacity;
    } else {
        return true;
    }
}

static inline bool check_top_bottom(const i64_stk_type* stk_p, const int64_t latest_value_pushed, const int64_t first_value_pushed) {
    // first and last value pushed *after* the stack is cleared / created
    return i64_stk_get_top(stk_p) == latest_value_pushed && i64_stk_get_bottom(stk_p) == first_value_pushed;
}

static inline bool check_ordered_values(const i64_stk_type* stk_p, const size_t n, const int64_t expected_value[n]) {
    assert(n != 0);

    bool res = true;
    for (size_t i = 0; i < n; i++) {
        res &= i64_stk_at(stk_p, i) == expected_value[i];
    }
    {
        size_t index = 0;
        int64_t value;
        FSTACK_FOREACH(stk_p, value) {
            res &= value == expected_value[index++];
        }
        assert(index == n);
    }
    {
        size_t index = n;
        int64_t value;
        FSTACK_FOREACH_REVERSE(stk_p, value) {
            res &= value == expected_value[--index];
        }
        assert(index == 0);
    }
    return res;
}

static inline bool copy_values_and_check_ordered_values(const i64_stk_type* stk_p, const size_t n, const int64_t expected_value[n]) {
    assert(n != 0);

    i64_stk_type* stk_copy_p = i64_stk_create(n);
    if (!stk_copy_p) {
        return false;
    }
    i64_stk_copy(stk_copy_p, stk_p);
    const bool res = check_ordered_values(stk_p, n, expected_value);
    i64_stk_destroy(stk_copy_p);
    return res;
}

int main(void) {
    // N = 0
    {
        i64_stk_type* stk_p = i64_stk_create(0);
        if (stk_p) {
            assert(false);
        }
    }
    // N = 1
    {
        i64_stk_type* stk_p = i64_stk_create(1);
        if (!stk_p) {
            assert(false);
        }

        assert(check_count_invariance(stk_p, 0, 0));
        assert(check_capacity_invariance(stk_p, 1));
        assert(check_empty_full(stk_p, 0, 0));

        i64_stk_destroy(stk_p);
    }
    // N = 1, push
    {
        i64_stk_type* stk_p = i64_stk_create(1);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 42);

        assert(check_count_invariance(stk_p, 1, 0));
        assert(check_capacity_invariance(stk_p, 1));
        assert(check_empty_full(stk_p, 1, 0));
        assert(check_top_bottom(stk_p, 42, 42));

        i64_stk_destroy(stk_p);
    }
    // N = 1, push -> pop
    {
        i64_stk_type* stk_p = i64_stk_create(1);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 2);

        assert(i64_stk_pop(stk_p) == 2);
        assert(check_count_invariance(stk_p, 1, 1));
        assert(check_capacity_invariance(stk_p, 1));
        assert(check_empty_full(stk_p, 1, 1));

        i64_stk_destroy(stk_p);
    }
    // N = 2, push * 2
    {
        i64_stk_type* stk_p = i64_stk_create(2);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 42);
        i64_stk_push(stk_p, 69);

        assert(check_count_invariance(stk_p, 2, 0));
        assert(check_capacity_invariance(stk_p, 2));
        assert(check_empty_full(stk_p, 2, 0));
        assert(check_top_bottom(stk_p, 69, 42));
        assert(check_ordered_values(stk_p, 2, (int64_t[2]){69, 42}));
        assert(copy_values_and_check_ordered_values(stk_p, 2, (int64_t[2]){69, 42}));

        i64_stk_destroy(stk_p);
    }
    // N = 2, push * 2 -> pop
    {
        i64_stk_type* stk_p = i64_stk_create(2);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 42);
        i64_stk_push(stk_p, 69);
        assert(i64_stk_pop(stk_p) == 69);

        assert(check_count_invariance(stk_p, 2, 1));
        assert(check_capacity_invariance(stk_p, 2));
        assert(check_empty_full(stk_p, 2, 1));
        assert(check_top_bottom(stk_p, 42, 42));

        i64_stk_destroy(stk_p);
    }
    // N = 10, push * 10
    {
        i64_stk_type* stk_p = i64_stk_create(10);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 421);
        i64_stk_push(stk_p, 422);
        i64_stk_push(stk_p, 423);
        i64_stk_push(stk_p, 424);
        i64_stk_push(stk_p, 425);
        i64_stk_push(stk_p, 426);
        i64_stk_push(stk_p, 427);
        i64_stk_push(stk_p, 428);
        i64_stk_push(stk_p, 429);
        i64_stk_push(stk_p, 430);

        assert(check_count_invariance(stk_p, 10, 0));
        assert(check_capacity_invariance(stk_p, 10));
        assert(check_empty_full(stk_p, 10, 0));
        assert(check_top_bottom(stk_p, 430, 421));
        assert(check_ordered_values(stk_p, 10, (int64_t[10]){430, 429, 428, 427, 426, 425, 424, 423, 422, 421}));
        assert(copy_values_and_check_ordered_values(stk_p, 10, (int64_t[10]){430, 429, 428, 427, 426, 425, 424, 423, 422, 421}));

        i64_stk_destroy(stk_p);
    }
    // N = 10, push * 10 -> pop * 5
    {
        i64_stk_type* stk_p = i64_stk_create(10);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 421);
        i64_stk_push(stk_p, 422);
        i64_stk_push(stk_p, 423);
        i64_stk_push(stk_p, 424);
        i64_stk_push(stk_p, 425);
        i64_stk_push(stk_p, 426);
        i64_stk_push(stk_p, 427);
        i64_stk_push(stk_p, 428);
        i64_stk_push(stk_p, 429);
        i64_stk_push(stk_p, 430);

        assert(i64_stk_pop(stk_p) == 430);
        assert(i64_stk_pop(stk_p) == 429);
        assert(i64_stk_pop(stk_p) == 428);
        assert(i64_stk_pop(stk_p) == 427);
        assert(i64_stk_pop(stk_p) == 426);

        assert(check_count_invariance(stk_p, 10, 5));
        assert(check_capacity_invariance(stk_p, 10));
        assert(check_empty_full(stk_p, 10, 5));
        assert(check_top_bottom(stk_p, 425, 421));
        assert(check_ordered_values(stk_p, 5, (int64_t[5]){425, 424, 423, 422, 421}));
        assert(copy_values_and_check_ordered_values(stk_p, 5, (int64_t[5]){425, 424, 423, 422, 421}));

        i64_stk_destroy(stk_p);
    }
    // N = 10, push * 10 -> pop * 5 -> push * 5
    {
        i64_stk_type* stk_p = i64_stk_create(10);
        if (!stk_p) {
            assert(false);
        }
        i64_stk_push(stk_p, 421);
        i64_stk_push(stk_p, 422);
        i64_stk_push(stk_p, 423);
        i64_stk_push(stk_p, 424);
        i64_stk_push(stk_p, 425);
        i64_stk_push(stk_p, 426);
        i64_stk_push(stk_p, 427);
        i64_stk_push(stk_p, 428);
        i64_stk_push(stk_p, 429);
        i64_stk_push(stk_p, 430);

        for (size_t i = 0; i < 5; i++) {
            i64_stk_pop(stk_p);
        }

        i64_stk_push(stk_p, 431);
        i64_stk_push(stk_p, 432);
        i64_stk_push(stk_p, 433);
        i64_stk_push(stk_p, 434);
        i64_stk_push(stk_p, 435);

        assert(check_count_invariance(stk_p, 15, 5));
        assert(check_capacity_invariance(stk_p, 10));
        assert(check_empty_full(stk_p, 15, 5));
        assert(check_top_bottom(stk_p, 435, 421));
        assert(check_ordered_values(stk_p, 10, (int64_t[10]){435, 434, 433, 432, 431, 425, 424, 423, 422, 421}));
        assert(copy_values_and_check_ordered_values(stk_p, 10, (int64_t[10]){435, 434, 433, 432, 431, 425, 424, 423, 422, 421}));

        i64_stk_destroy(stk_p);
    }
    // N = 10, push * 10 -> clear() -> push * 5
    {
        i64_stk_type* stk_p = i64_stk_create(10);
        if (!stk_p) {
            assert(false);
        }
        for (size_t i = 0; i < 10; i++) {
            i64_stk_push(stk_p, 420 + i);
        }
        i64_stk_clear(stk_p);

        i64_stk_push(stk_p, 431);
        i64_stk_push(stk_p, 432);
        i64_stk_push(stk_p, 433);
        i64_stk_push(stk_p, 434);
        i64_stk_push(stk_p, 435);

        assert(check_count_invariance(stk_p, 15 - 10, 0));
        assert(check_capacity_invariance(stk_p, 10));
        assert(check_empty_full(stk_p, 15 - 10, 0));
        assert(check_top_bottom(stk_p, 435, 431));
        assert(check_ordered_values(stk_p, 5, (int64_t[10]){435, 434, 433, 432, 431}));
        assert(copy_values_and_check_ordered_values(stk_p, 5, (int64_t[10]){435, 434, 433, 432, 431}));

        i64_stk_destroy(stk_p);
    }
    // N = 1e+6, push * 1e+6
    {
        i64_stk_type* stk_p = i64_stk_create(1e+6);
        if (!stk_p) {
            assert(false);
        }
        for (size_t i = 0; i < 1e+6; i++) {
            i64_stk_push(stk_p, 420 + i);
        }
        assert(check_count_invariance(stk_p, 1e+6, 0));
        assert(check_capacity_invariance(stk_p, 1e+6));
        assert(check_empty_full(stk_p, 1e+6, 0));
        assert(check_top_bottom(stk_p, 420 + 1e+6 - 1, 420));

        i64_stk_destroy(stk_p);
    }
    // N = 1e+6, push * 1e+6 -> pop * 1e+3
    {
        i64_stk_type* stk_p = i64_stk_create(1e+6);
        if (!stk_p) {
            assert(false);
        }
        for (size_t i = 0; i < 1e+6; i++) {
            i64_stk_push(stk_p, 420 + i);
        }
        for (size_t i = 0; i < 1e+3; i++) {
            assert(i64_stk_pop(stk_p) == 420 + (1e+6 - i - 1));
        }
        assert(check_count_invariance(stk_p, 1e+6, 1e+3));
        assert(check_capacity_invariance(stk_p, 1e+6));
        assert(check_empty_full(stk_p, 1e+6, 1e+3));
        assert(check_top_bottom(stk_p, 420 + (1e+6 - 1e+3 - 1), 420));

        i64_stk_destroy(stk_p);
    }
}

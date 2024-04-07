#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf

#include "test.h"

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src-alt/T_stack.h" // int_stack_*, T_stack_for_each

bool empty_test(void) {
    int_stack_type* stack_p;
    if (!int_stack_init(&stack_p, 1)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_stack_capacity(stack_p), (size_t)1);

    res &= is_true(int_stack_is_empty(stack_p));
    res &= is_equal(int_stack_count(stack_p), (size_t)0);
    res &= is_true(int_stack_deinit(&stack_p));

    return res;
}

bool one_element_test(void) {
    int_stack_type* stack_p;
    if (!int_stack_init(&stack_p, 1)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_stack_capacity(stack_p), (size_t)1);

    int value = 5;

    res &= is_equal(int_stack_count(stack_p), (size_t)0);
    int_stack_push(stack_p, value);
    res &= is_equal(int_stack_count(stack_p), (size_t)1);

    res &= is_equal(value, int_stack_peek(stack_p));
    res &= is_true(!int_stack_is_empty(stack_p));

    res &= is_equal(value, int_stack_pop(stack_p));
    res &= is_equal(int_stack_count(stack_p), (size_t)0);

    res &= is_true(int_stack_is_empty(stack_p));
    res &= is_true(!int_stack_is_full(stack_p));

    res &= is_true(int_stack_deinit(&stack_p));

    return res;
}

bool million_elements_test(void) {
    int_stack_type* stack_p;
    if (!int_stack_init(&stack_p, 1000000)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_stack_capacity(stack_p), (size_t)1000000);

    for (int i = 1; i <= 1000000; i++) {
        int_stack_push(stack_p, i);
        res &= is_equal(int_stack_count(stack_p), (size_t)i);
    }

    for (int i = 1000000; i >= 1; i--) {
        res &= is_equal(int_stack_count(stack_p), (size_t)i);
        res &= is_equal(i, int_stack_pop(stack_p));
    }

    res &= is_true(int_stack_deinit(&stack_p));
    return res;
}

bool for_each_test(void) {
    int_stack_type* stack_p;
    if (!int_stack_init(&stack_p, 50)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        int_stack_push(stack_p, i);
    }
    int value = 51;
    int var;
    size_t i;
    T_stack_for_each(stack_p, var, i) {
        res &= is_equal(value, var);
        value++;
    }
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} bool_f_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    bool_f_plus bool_f_arr[] = {{empty_test, "empty test"},
                                {one_element_test, "one element test"},
                                {million_elements_test, "million elements test"},
                                {for_each_test, "for each test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}

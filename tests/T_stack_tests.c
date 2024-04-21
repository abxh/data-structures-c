#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr

#include "test.h" // is_true, is_false, is_equal

#define VALUE_TYPE int
#include "header-only/fixed-containers/T_stack.h" // int_stack_*, T_stack_for_each

bool empty_test(void) {
    int_stack_type* stack_p = int_stack_create(1);
    if (!stack_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_stack_get_capacity(stack_p), 1UL);
    res &= is_true(int_stack_is_empty(stack_p));
    res &= is_equal(int_stack_get_count(stack_p), 0UL);

    int_stack_destroy(stack_p);

    return res;
}

bool one_element_test(void) {
    int_stack_type* stack_p = int_stack_create(1);
    if (!stack_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_stack_get_capacity(stack_p), 1UL);

    int value = 5;

    res &= is_equal(int_stack_get_count(stack_p), 0UL);
    int_stack_push(stack_p, value);
    res &= is_equal(int_stack_get_count(stack_p), 1UL);

    res &= is_equal(value, int_stack_peek(stack_p));
    res &= is_false(int_stack_is_empty(stack_p));

    res &= is_equal(value, int_stack_pop(stack_p));
    res &= is_equal(int_stack_get_count(stack_p), 0UL);

    res &= is_true(int_stack_is_empty(stack_p));
    res &= is_false(int_stack_is_full(stack_p));

    int_stack_destroy(stack_p);

    return res;
}

bool million_elements_test(void) {
    int_stack_type* stack_p = int_stack_create(1000000);
    if (!stack_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    for (int i = 1; i <= 1000000; i++) {
        int_stack_push(stack_p, i);
        res &= is_equal(int_stack_get_count(stack_p), (size_t)i);
    }

    for (int i = 1000000; i >= 1; i--) {
        res &= is_equal(int_stack_get_count(stack_p), (size_t)i);
        res &= is_equal(i, int_stack_pop(stack_p));
    }

    int_stack_destroy(stack_p);
    return res;
}

bool for_each_and_copy_test(void) {
    int_stack_type* stack_p = int_stack_create(50);
    if (!stack_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        int_stack_push(stack_p, i);
    }
    {
        int x = 100;
        int out_value;
        size_t out_index;
        T_stack_for_each(stack_p, out_index, out_value) {
            res &= is_equal(x, out_value);
            x--;
        }
    }
    int_stack_type* stack_copy_p = int_stack_clone(stack_p);
    if (!stack_copy_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    {
        int x = 100;
        int out_value;
        size_t out_index;
        T_stack_for_each(stack_copy_p, out_index, out_value) {
            res &= is_equal(x, out_value);
            x--;
        }
    }
    int_stack_destroy(stack_p);
    int_stack_destroy(stack_copy_p);

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
                                {for_each_and_copy_test, "for each and copy test"}};
    printf(__FILE_NAME__ ":\n");
    bool overall_res = true;
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        bool res = bool_f_arr[i].func();
        overall_res &= res;
        printf(" [%s] %s\n", res ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return overall_res ? 0 : 1;
}

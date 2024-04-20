#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr
#include <stdlib.h>  // NULL

#include "containers/ll_stack.h" // ll_stack_*
#include "test.h"                // is_true, is_false, is_equal

bool empty_test(void) {
    ll_stack_type* stack_p = NULL;
    if (!ll_stack_init(&stack_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(ll_stack_get_count(stack_p), 0UL);
    res &= is_true(ll_stack_is_empty(stack_p));
    res &= is_true(ll_stack_deinit(&stack_p));

    return res;
}

bool one_element_test(void) {
    ll_stack_type* stack_p = NULL;
    if (!ll_stack_init(&stack_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    int value = 5;

    bool out_rtr;
    ll_stack_push(int, stack_p, value, out_rtr);
    res &= is_true(out_rtr);
    res &= is_equal(value, ll_stack_peek(int, stack_p));
    res &= is_false(ll_stack_is_empty(stack_p));
    res &= is_equal(ll_stack_get_count(stack_p), 1UL);

    int out_value;
    ll_stack_pop(int, stack_p, out_value);
    res &= is_equal(value, out_value);
    res &= is_true(ll_stack_is_empty(stack_p));
    res &= is_equal(ll_stack_get_count(stack_p), 0UL);

    res &= is_true(ll_stack_deinit(&stack_p));

    return res;
}

bool million_elements_test(void) {
    ll_stack_type* stack_p = NULL;
    if (!ll_stack_init(&stack_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        bool out_rtr;
        ll_stack_push(int, stack_p, i, out_rtr);
        res &= is_true(out_rtr);
    }
    for (int i = 1000000; i >= 1; i--) {
        int out_value;
        ll_stack_pop(int, stack_p, out_value);
        res &= is_equal(i, out_value);
    }
    res &= is_true(ll_stack_deinit(&stack_p));
    return res;
}

bool for_each_and_copy_test(void) {
    ll_stack_type* stack_p = NULL;
    if (!ll_stack_init(&stack_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        bool out_rtr;
        ll_stack_push(int, stack_p, i, out_rtr);
        res &= is_true(out_rtr);
    }
    {
        int x = 100;
        ll_stack_node_type* node_p = NULL;
        int* value_p;
        ll_stack_for_each(stack_p, node_p, value_p) {
            int value = *value_p;
            res &= is_equal(x, value);
            x--;
        }
    }
    // return true;
    ll_stack_type* stack_copy_p = NULL;
    if (!ll_stack_copy(&stack_copy_p, stack_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        ll_stack_deinit(&stack_p);
        return false;
    }
    {
        int x = 100;
        ll_stack_node_type* node_original_p = stack_p->head_p;

        ll_stack_node_type* out_node_p = NULL;
        int* out_value_p;
        ll_stack_for_each(stack_copy_p, out_node_p, out_value_p) {
            int value = *out_value_p;
            res &= is_equal(x, value);
            res &= is_true(out_node_p != node_original_p);
            res &= is_true(out_node_p->value_p != node_original_p->value_p);
            x--;
            node_original_p = node_original_p->next_p;
        }
    }
    res &= is_equal(ll_stack_get_count(stack_p), ll_stack_get_count(stack_copy_p));
    res &= is_true(ll_stack_deinit(&stack_p));
    res &= is_true(ll_stack_deinit(&stack_copy_p));
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

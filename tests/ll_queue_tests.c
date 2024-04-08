#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr

#include "test.h" // is_true, is_false, is_equal

#define VALUE_TYPE int
#include "../src-alt/ll_queue/ll_queue_wrapper.h" // ll_queue_*

bool empty_test(void) {
    ll_queue_type* queue_p;
    if (!ll_queue_init_int(&queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(ll_queue_get_count(queue_p), (size_t)0);
    res &= is_true(ll_queue_is_empty(queue_p));
    res &= is_true(ll_queue_deinit(&queue_p));

    return res;
}

bool one_element_test(void) {
    ll_queue_type* queue_p;
    if (!ll_queue_init_int(&queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value = 5;

    res &= is_true(ll_queue_enqueue_int(queue_p, value));
    res &= is_equal(value, ll_queue_peek_int(queue_p));

    res &= is_false(ll_queue_is_empty(queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), (size_t)1);

    res &= is_equal(value, ll_queue_dequeue_int(queue_p));
    res &= is_true(ll_queue_is_empty(queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), (size_t)0);

    res &= is_true(ll_queue_deinit(&queue_p));

    return res;
}

bool two_elements_test(void) {
    ll_queue_type* queue_p;
    if (!ll_queue_init_int(&queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value1 = 1;
    int value2 = 2;

    res &= is_true(ll_queue_enqueue_int(queue_p, value1));
    res &= is_true(ll_queue_enqueue_int(queue_p, value2));
    res &= is_equal(value1, ll_queue_peek_first_int(queue_p));
    res &= is_equal(value2, ll_queue_peek_last_int(queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), (size_t)2);
    res &= is_true(ll_queue_deinit(&queue_p));

    return res;
}

bool million_elements_test(void) {
    ll_queue_type* queue_p;
    if (!ll_queue_init_int(&queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        ll_queue_enqueue_int(queue_p, i);
        res &= is_equal(ll_queue_get_count(queue_p), (size_t)i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= is_equal(i, ll_queue_dequeue_int(queue_p));
        res &= is_equal(ll_queue_get_count(queue_p), (size_t)(1000000 - i));
    }
    res &= is_true(ll_queue_deinit(&queue_p));
    return res;
}

bool for_each_and_copy_test(void) {
    ll_queue_type* queue_p;
    if (!ll_queue_init_int(&queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        ll_queue_enqueue_int(queue_p, i);
    }
    {
        int x = 51;
        ll_queue_node_type* node_p = NULL;
        int value;
        ll_queue_for_each(queue_p, node_p, value) {
            res &= is_equal(x, value);
            x++;
        }
    }
    ll_queue_type* queue_copy_p;
    if (!ll_queue_copy(&queue_copy_p, queue_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        ll_queue_deinit(&queue_p);
        return false;
    }
    {
        int x = 51;
        ll_queue_node_type* node_p = NULL;
        ll_queue_node_type* node_original_p = queue_p->head_p;
        int value;
        ll_queue_for_each(queue_copy_p, node_p, value) {
            res &= is_equal(x, value);
            res &= is_true(node_p != node_original_p);
            res &= is_true(node_p->value_p != node_original_p->value_p);
            x++;
            node_original_p = node_original_p->next_p;
        }
    }
    res &= is_equal(ll_queue_get_count(queue_p), ll_queue_get_count(queue_copy_p));
    res &= is_true(ll_queue_deinit(&queue_p));
    res &= is_true(ll_queue_deinit(&queue_copy_p));
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
                                {two_elements_test, "two elements test"},
                                {million_elements_test, "million elements test"},
                                {for_each_and_copy_test, "for each and copy test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}

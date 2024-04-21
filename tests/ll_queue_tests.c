#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr

#include "allocators/arena.h"    // arena_*
#include "containers/ll_queue.h" // ll_queue_*
#include "test.h"                // is_true, is_false, is_equal

bool empty_test(void) {
    ll_queue_type* queue_p = ll_queue_create();
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(ll_queue_get_count(queue_p), 0UL);
    res &= is_true(ll_queue_is_empty(queue_p));

    ll_queue_destroy(queue_p);

    return res;
}

bool one_element_test(void) {
    ll_queue_type* queue_p = ll_queue_create();
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value = 5;

    bool out_rtr;
    ll_queue_enqueue(int, queue_p, value, out_rtr);
    res &= is_true(out_rtr);
    res &= is_equal(value, ll_queue_peek(int, queue_p));

    res &= is_false(ll_queue_is_empty(queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), 1UL);

    int out_value;
    ll_queue_dequeue(int, queue_p, out_value);
    res &= is_equal(value, out_value);
    res &= is_true(ll_queue_is_empty(queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), 0UL);

    ll_queue_destroy(queue_p);

    return res;
}

bool two_elements_test(void) {
    ll_queue_type* queue_p = ll_queue_create();
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value1 = 1;
    int value2 = 2;

    bool out_rtr;
    ll_queue_enqueue(int, queue_p, value1, out_rtr);
    res &= is_true(out_rtr);
    ll_queue_enqueue(int, queue_p, value2, out_rtr);
    res &= is_true(out_rtr);
    res &= is_equal(value1, ll_queue_peek_first(int, queue_p));
    res &= is_equal(value2, ll_queue_peek_last(int, queue_p));
    res &= is_equal(ll_queue_get_count(queue_p), 2UL);

    ll_queue_destroy(queue_p);

    return res;
}

bool million_elements_test(void) {
    static unsigned char buf[40000 * 1024];
    arena_type arena = {0};
    arena_init(&arena, buf, sizeof(buf));

    ll_queue_type* queue_p = NULL;
    if (!ll_queue_init(&queue_p, &arena, arena_allocate, arena_deallocate)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        bool out_rtr;
        ll_queue_enqueue(int, queue_p, i, out_rtr);
        res &= is_true(out_rtr);
        res &= is_equal(ll_queue_get_count(queue_p), (size_t)i);
    }
    for (int i = 1; i <= 1000000; i++) {
        int out_value;
        ll_queue_dequeue(int, queue_p, out_value);
        res &= is_equal(i, out_value);
        res &= is_equal(ll_queue_get_count(queue_p), (size_t)(1000000 - i));
    }
    ll_queue_destroy(queue_p);
    return res;
}

bool for_each_and_copy_test(void) {
    ll_queue_type* queue_p = ll_queue_create();
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        bool out_rtr;
        ll_queue_enqueue(int, queue_p, i, out_rtr);
        res &= is_true(out_rtr);
    }
    {
        int x = 51;
        ll_queue_node_type* node_p = NULL;
        int* value_p;
        ll_queue_for_each(queue_p, node_p, value_p) {
            int value = *value_p;
            res &= is_equal(x, value);
            x++;
        }
    }
    ll_queue_type* queue_copy_p = ll_queue_clone(queue_p);
    if (!queue_copy_p) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        ll_queue_destroy(queue_p);
        return false;
    }
    {
        int x = 51;
        ll_queue_node_type* node_p = NULL;
        ll_queue_node_type* node_original_p = queue_p->head_p;
        int* value_p;
        ll_queue_for_each(queue_copy_p, node_p, value_p) {
            int value = *value_p;
            res &= is_equal(x, value);
            res &= is_true(node_p != node_original_p);
            res &= is_true(node_p->value_p != node_original_p->value_p);
            x++;
            node_original_p = node_original_p->next_p;
        }
    }
    res &= is_equal(ll_queue_get_count(queue_p), ll_queue_get_count(queue_copy_p));

    ll_queue_destroy(queue_p);
    ll_queue_destroy(queue_copy_p);

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
    bool overall_res = true;
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        bool res = bool_f_arr[i].func();
        overall_res &= res;
        printf(" [%s] %s\n", res ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return overall_res ? 0 : 1;
}

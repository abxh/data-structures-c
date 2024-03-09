#include <stdbool.h>
#include <stdio.h>

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/aqueue.h"

bool empty_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 1)) {
        return false;
    }
    bool res = true;

    res &= aqueue_isempty(q);
    res &= aqueue_deinit(&q);

    return res;
}

bool one_element_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 1)) {
        return false;
    }
    bool res = true;
    int value = 5;

    aqueue_enqueue_int(q, value);
    res &= value == aqueue_peek_int(q);
    res &= value == aqueue_peek_last_int(q);
    res &= !aqueue_isempty(q);
    res &= aqueue_length(q) == 1;

    // quirks:
    // res &= !aqueue_isfull(q);
    // res &= aqueue_capacity(q) == 2;

    res &= value == aqueue_dequeue_int(q);
    res &= aqueue_isempty(q);
    res &= !aqueue_isfull(q);
    res &= aqueue_length(q) == 0;

    res &= aqueue_deinit(&q);

    return res;
}

bool two_elements_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 2)) {
        return false;
    }
    bool res = true;
    int value1 = 1;
    int value2 = 2;

    aqueue_enqueue_int(q, value1);
    aqueue_enqueue_int(q, value2);
    res &= value1 == aqueue_peek_int(q);
    res &= value2 == aqueue_peek_last_int(q);
    res &= aqueue_length(q) == 2;
    res &= aqueue_deinit(&q);

    return res;
}

bool million_elements_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        aqueue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    res &= aqueue_deinit(&q);
    return res;
}

bool grow_and_wraparound_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        aqueue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    for (int i = 1; i <= 500000; i++) {
        aqueue_enqueue_int(q, i);
    }
    res &= aqueue_resize_int(q, 1000000);
    for (int i = 500001; i <= 1000000; i++) {
        aqueue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    res &= aqueue_deinit(&q);
    return res;
}

bool shrink_with_nonzero_start_index(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        aqueue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    for (int i = 1; i <= 250000; i++) {
        aqueue_enqueue_int(q, i);
    }
    aqueue_resize_int(q, 250000);
    for (int i = 1; i <= 250000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    aqueue_deinit(&q);
    return res;
}

bool shrink_and_wraparound_test(void) {
    AQueue* q;
    if (!aqueue_init_int(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        aqueue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    for (int i = 1; i <= 250000; i++) {
        aqueue_enqueue_int(q, i);
    }
    aqueue_resize_int(q, 250000);
    for (int i = 1; i <= 250000; i++) {
        res &= i == aqueue_dequeue_int(q);
    }
    aqueue_deinit(&q);
    return res;
}

bool foreach_test(void) {
    AQueue* s;
    if (!aqueue_init_int(&s, 50)) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        aqueue_enqueue_int(s, i);
    }
    int x = 51;
    size_t index;
    int value;
    aqueue_foreach(s, index, value) {
        res &= x == value;
        x++;
    }
    res &= aqueue_deinit(&s);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    func_plus bool_f_arr[] = {{empty_test, "empty test"},
                              {one_element_test, "one element test"},
                              {two_elements_test, "two elements test"},
                              {million_elements_test, "million elements test"},
                              {grow_and_wraparound_test, "grow and wraparound test"},
                              {shrink_and_wraparound_test, "shrink and wraparound test"},
                              {shrink_with_nonzero_start_index, "shrink with nonzero start index"},
                              {foreach_test, "foreach test"}};

    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}

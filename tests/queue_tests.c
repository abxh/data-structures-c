
#include <stdbool.h>
#include <stdio.h>

#define VALUE_NAME int
#define VALUE_TYPE int
#include "../src/queue.h"

bool empty_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 1)) {
        return false;
    }
    bool res = true;
    res &= queue_int_isempty(q);
    res &= queue_int_deinit(&q);
    return res;
}

bool one_element_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 1)) {
        return false;
    }
    bool res = true;
    int value = 5;
    res &= queue_int_isempty(q);
    res &= !queue_int_isfull(q);
    queue_int_enqueue(q, value);
    res &= !queue_int_isempty(q);
    res &= value == queue_int_dequeue(q);
    res &= queue_int_isempty(q);
    res &= !queue_int_isfull(q);
    res &= queue_int_deinit(&q);
    return res;
}

bool million_elements_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool wraparound_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 750000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 750000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    for (int i = 1; i <= 750000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 750000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool grow_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_int_enqueue(q, i);
    }
    if (!queue_int_resize(q, q->used << 1)) {
        queue_int_deinit(&q);
        return false;
    }
    for (int i = 500001; i <= 1000000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool shrink_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        queue_int_enqueue(q, i);
    }
    if (!queue_int_resize(q, q->used)) {
        queue_int_deinit(&q);
        return false;
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool grow_and_wraparound_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    for (int i = 1; i <= 500000; i++) {
        queue_int_enqueue(q, i);
    }
    if (!queue_int_resize(q, q->used << 1)) {
        queue_int_deinit(&q);
        return false;
    }
    for (int i = 500001; i <= 1000000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool shrink_with_nonzero_start_index(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    for (int i = 1; i <= 250000; i++) {
        queue_int_enqueue(q, i);
    }
    if (!queue_int_resize(q, 250000)) {
        queue_int_deinit(&q);
        return false;
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

bool shrink_and_wraparound_test(void) {
    queue_int_t* q;
    if (!queue_int_init(&q, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_int_enqueue(q, i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    for (int i = 1; i <= 250000; i++) {
        queue_int_enqueue(q, i);
    }
    if (!queue_int_resize(q, 250000)) {
        queue_int_deinit(&q);
        return false;
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == queue_int_dequeue(q);
    }
    res &= queue_int_deinit(&q);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

int main(void) {
    func_plus bool_f_arr[] = {{empty_test, "empty test"},
                              {one_element_test, "one element test"},
                              {wraparound_test, "wraparound test"},
                              {grow_test, "grow test"},
                              {shrink_test, "shrink test"},
                              {grow_and_wraparound_test, "grow and wraparound test"},
                              {shrink_with_nonzero_start_index, "shrink with nonzero start index"},
                              {shrink_and_wraparound_test, "shrink and wraparound test"}};
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}


#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#include "queue.h" // queue_*

// note:
// test using valgrind as well.

bool empty_test(void) {
    Queue* queue_p = queue_new(1, sizeof(int));
    bool res = true;
    queue_isempty(queue_p);
    queue_free(&queue_p);
    return res;
}

bool one_element_test(void) {
    Queue* queue_p = queue_new(1, sizeof(int));
    bool res = true;
    int value = 5;
    res &= queue_isempty(queue_p);
    res &= !queue_isfull(queue_p);
    queue_enqueue(queue_p, (unsigned char*)&value);
    res &= !queue_isempty(queue_p);
    res &= value == *(int*)queue_dequeue(queue_p);
    res &= queue_isempty(queue_p);
    res &= !queue_isfull(queue_p);
    queue_free(&queue_p);
    return res;
}

bool million_elements_test(void) {
    Queue* queue_p = queue_new(1000000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool wraparound_test(void) {
    Queue* queue_p = queue_new(1000000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 750000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 750000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    for (int i = 1; i <= 750000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 750000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool grow_test(void) {
    Queue* queue_p = queue_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    queue_resize(queue_p, 1000000);
    for (int i = 500001; i <= 1000000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool shrink_test(void) {
    Queue* queue_p = queue_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    queue_resize(queue_p, 250000);
    for (int i = 1; i <= 250000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool grow_and_wraparound_test(void) {
    Queue* queue_p = queue_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 250000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    for (int i = 1; i <= 500000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    queue_resize(queue_p, 1000000);
    for (int i = 500001; i <= 1000000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool shrink_with_nonzero_start_index(void) {
    Queue* queue_p = queue_new(1000000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    for (int i = 1; i <= 250000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    queue_resize(queue_p, 250000);
    for (int i = 1; i <= 250000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
    return res;
}

bool shrink_and_wraparound_test(void) {
    Queue* queue_p = queue_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    for (int i = 1; i <= 500000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    for (int i = 1; i <= 250000; i++) {
        queue_enqueue(queue_p, (unsigned char*)&i);
    }
    queue_resize(queue_p, 250000);
    for (int i = 1; i <= 250000; i++) {
        res &= i == *(int*)queue_dequeue(queue_p);
    }
    queue_free(&queue_p);
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
                              {million_elements_test, "million elements test"},
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

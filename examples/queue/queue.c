#include <assert.h>
#include <stdbool.h>

#define VALUE_TYPE char
#include "queue.h"

void char_queue_demo(void) {
    char_queue_type* q = char_queue_create();
    if (!q) {
        assert(false);
    }
    char_queue_is_empty(q);

    assert(char_queue_is_empty(q));

    char_queue_enqueue(q, 'a');
    char_queue_enqueue(q, 'b');
    char_queue_enqueue(q, 'c');

    assert(!char_queue_is_empty(q));

    assert(3 == char_queue_count(q));

    assert('a' == char_queue_front(q));
    assert('c' == char_queue_back(q));

    assert('a' == char_queue_peek(q));
    assert('a' == char_queue_peek_first(q));
    assert('c' == char_queue_peek_last(q));

    char res = char_queue_dequeue(q);

    assert('a' == res);

    char_queue_destroy(q);
}

#define VALUE_TYPE int
#include "queue.h"

#include <stdio.h>
void int_queue_demo(void) {
    int_queue_type* q = int_queue_create();

    if (!q) {
        assert(false);
    }

    for (unsigned long int i = 1; i <= 1e+3 * 2 / 3; i++) {
        assert(true == int_queue_enqueue(q, i));
        printf("start: %zu, end: %zu, count: %zu\n", q->start_index, q->end_index, q->count);
    }
    putchar('\n');
    for (unsigned long int i = 1; i <= 1e+3 * 2 / 3; i++) {
        int value = int_queue_dequeue(q);
        assert(value == i);
        printf("start: %zu, end: %zu, count: %zu\n", q->start_index, q->end_index, q->count);
    }
    putchar('\n');

    for (unsigned long int i = 1; i <= 1e+3; i++) {
        assert(true == int_queue_enqueue(q, i));
        printf("start: %zu, end: %zu, count: %zu\n", q->start_index, q->end_index, q->count);
    }

    assert(16 <= int_queue_capacity(q));

    int_queue_type* q_copy = int_queue_clone(q);
    if (!q_copy) {
        assert(false);
    }

    size_t x = 1;
    size_t queue_val_count = 0;

    // {...} for scoped variables
    {
        size_t index;
        int value;
        queue_for_each(q_copy, index, value) {
            printf("%zu, %d\n", index, q->values[index]);
            assert(value == x);
            x++;
            queue_val_count++;
        }
    }
    // assert(16 == queue_val_count);

    int_queue_destroy(q);
    int_queue_destroy(q_copy);
}

int main(void) {
    char_queue_demo();
    int_queue_demo();
    return 0;
}

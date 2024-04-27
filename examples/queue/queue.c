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

    assert('a' == char_queue_at(q, 0));
    assert('b' == char_queue_at(q, 1));
    assert('c' == char_queue_at(q, 2));

    char res = char_queue_dequeue(q);

    assert('a' == res);

    char_queue_destroy(q);
}

#define VALUE_TYPE int
#include "queue.h"

void int_queue_demo(void) {
    int_queue_type* q = int_queue_create();
    size_t lim = 1e+6;

    if (!q) {
        assert(false);
    }

    for (int i = 1; i <= lim * 2 / 3; i++) {
        assert(true == int_queue_enqueue(q, i));
    }

    for (int i = 1; i <= lim; i++) {
        assert(true == int_queue_enqueue(q, i));
    }

    for (int i = 1; i <= lim * 2 / 3; i++) {
        assert(i == int_queue_dequeue(q));
    }

    assert(lim <= int_queue_capacity(q));

    int_queue_type* q_copy = int_queue_clone(q);
    if (!q_copy) {
        assert(false);
    }

    for (int i = 1; i <= lim; i++) {
        assert(i == int_queue_dequeue(q));
    }

    int x = 1;
    size_t queue_val_count = 0;

    // {...} for scoped variables
    {
        size_t index;
        int value;
        queue_for_each(q_copy, index, value) {
            assert(value == x);
            x++;
            queue_val_count++;
        }
    }
    assert(lim == queue_val_count);

    int_queue_destroy(q);
    int_queue_destroy(q_copy);
}

int main(void) {
    char_queue_demo();
    int_queue_demo();
    return 0;
}

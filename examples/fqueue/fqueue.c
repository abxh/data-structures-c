#include <assert.h>
#include <stdbool.h>

#define NAME chque
#define VALUE_TYPE char
#include "fqueue.h"

void chque_example(void) {
    chque_type* q = chque_create(4);
    if (!q) {
        assert(false);
    }
    chque_is_empty(q);

    assert(chque_is_empty(q));
    assert(!chque_is_full(q));

    chque_enqueue(q, 'a');
    chque_enqueue(q, 'b');
    chque_enqueue(q, 'c');
    chque_enqueue(q, 'd');

    assert(!chque_is_empty(q));
    assert(chque_is_full(q)); // note: capacity is rounded up to the next power of 2.

    assert(q->count == 4);

    assert('a' == chque_get_front(q));
    assert('d' == chque_get_back(q));

    assert('a' == chque_peek(q));

    assert('a' == chque_at(q, 0));
    assert('b' == chque_at(q, 1));
    assert('c' == chque_at(q, 2));
    assert('d' == chque_at(q, 3));

    const char res = chque_dequeue(q);
    assert('a' == res);

    assert(q->count == 3);

    chque_clear(q);

    assert(q->count == 0);

    chque_destroy(q);
}

#define NAME int_queue
#define VALUE_TYPE int
#include "fqueue.h"

void int_queue_example(const bool offset) {
    const int lim = 1e+6;

    int_queue_type* q = int_queue_create(lim);

    if (!q) {
        assert(false);
    }

    if (offset) { // this is just for some light testing
        for (int i = 1; i <= lim / 3; i++) {
            int_queue_enqueue(q, i);
        }
        for (int i = 1; i <= lim / 3; i++) {
            int_queue_dequeue(q);
        }
    }

    for (int i = 1; i <= lim; i++) {
        int_queue_enqueue(q, i);
    }

    int_queue_type* q_copy = int_queue_create(lim);
    if (!q_copy) {
        assert(false);
    }
    int_queue_copy(q_copy, q);

    for (int i = 1; i <= lim; i++) {
        assert(i == int_queue_dequeue(q));
    }

    {
        int x = lim;
        size_t temp_index;
        int value;
        FQUEUE_FOREACH_REVERSE(q_copy, temp_index, value) {
            assert(value == x--);
        }
    }
    {
        int x = 1;
        int count = 0;
        size_t temp_index;
        int value;
        FQUEUE_FOREACH(q_copy, temp_index, value) {
            assert(value == x++);
            count++;
        }
    }

    int_queue_destroy(q);
    int_queue_destroy(q_copy);
}

int main(void) {
    chque_example();
    int_queue_example(true);
    int_queue_example(false);
    return 0;
}

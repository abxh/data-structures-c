#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define NAME pqueue_int
#define VALUE_TYPE int
#include "fpqueue.h"

void preorder_print_and_traverse(pqueue_int_type* pq, uint32_t index) {
    if (index >= pq->count) {
        printf(" ()");
        return;
    }
    printf(" ( %d", index);
    preorder_print_and_traverse(pq, fpqueue_left_child(index));
    preorder_print_and_traverse(pq, fpqueue_right_child(index));
    printf(" )");
}

void int_example(void) {
    pqueue_int_type* pq = pqueue_int_create(10);

    if (!pq) {
        assert(false);
    }

    assert(pq->capacity >= 10);

    const int values[10] = {3, 1, 5, 2, 4, 7, 6, 10, 9, 8};

    assert(pqueue_int_is_empty(pq));
    assert(pq->count == 0);

    for (int i = 0; i < 10; i++) {
        const uint32_t priority = values[i];

        pqueue_int_push(pq, values[i], priority);

        preorder_print_and_traverse(pq, 0);
        printf("\n");
    }

    assert(pq->count == 10);
    assert(pqueue_int_is_full(pq));

    assert(pqueue_int_peek(pq) == 10);
    assert(pqueue_int_get_max(pq) == 10);

    pqueue_int_type* pq_copy = pqueue_int_create(10);
    pqueue_int_type* pq_copy_copy = pqueue_int_create(10);
    if (!pq_copy || !pq_copy_copy) {
        assert(false);
    }
    pqueue_int_copy(pq_copy, pq);
    pqueue_int_copy(pq_copy_copy, pq_copy);

    for (int i = 0; i < 10; i++) {
        assert(pqueue_int_pop_max(pq) == 10 - i);
    }

    bool covered[11] = {true, false};
    for (int i = 0; i < 10; i++) {
        assert(!covered[pqueue_int_get_max(pq_copy)]);

        covered[pqueue_int_pop_max(pq_copy)] = true;
    }

    size_t count = 0;
    {
        int value;
        size_t tempi;

        (void)(value); // mark unused
        fpqueue_for_each(pq_copy_copy, tempi, value) {
            count++;
        }
    }
    assert(count == 10);
    pqueue_int_clear(pq_copy_copy);
    assert(pq_copy_copy->count == 0);

    pqueue_int_destroy(pq);
    pqueue_int_destroy(pq_copy);
    pqueue_int_destroy(pq_copy_copy);
}

int main(void) {
    int_example();
    return 0;
}

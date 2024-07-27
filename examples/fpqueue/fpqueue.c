#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define NAME pqueue_int
#define VALUE_TYPE int
#include "fpqueue.h"

void preorder_print_and_traverse(pqueue_int_type* pq, uint32_t index) {
    if (index >= pq->count) {
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

    const int values[10] = {3, 1, 5, 2, 4, 7, 6, 10, 9, 8};

    assert(pqueue_int_is_empty(pq));

    for (int i = 0; i < 10; i++) {
        const uint32_t priority = values[i];

        pqueue_int_push(pq, values[i], priority);

        preorder_print_and_traverse(pq, 0);
        printf("\n");
    }

    assert(pqueue_int_is_full(pq));

    for (int i = 0; i < 10; i++) {
        assert(pqueue_int_get_max(pq) == 10 - i);

        assert(pqueue_int_pop_max(pq) == 10 - i);
    }

    pqueue_int_destroy(pq);
}

int main(void) {
    int_example();
    return 0;
}

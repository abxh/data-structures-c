/*  fpqueue.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef NAME
#define NAME fpqueue
#error "Must define NAME."
#else
#define FPQUEUE_NAME NAME
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE."
#define VALUE_TYPE int
#endif

#define FPQUEUE_TYPE JOIN(FPQUEUE_NAME, type)
#define FPQUEUE_ELEMENT_TYPE JOIN(FPQUEUE_ELEMENT, type)

typedef struct {
    uint32_t priority;
    VALUE_TYPE value;
} FPQUEUE_ELEMENT_TYPE;

typedef struct {
    uint32_t count;
    uint32_t capacity;
    FPQUEUE_ELEMENT_TYPE elements[];
} FPQUEUE_TYPE;

#define FPQUEUE_LCHILD(index) (2 * (index) + 1)
#define FPQUEUE_RCHILD(index) (2 * (index) + 2)
#define FPQUEUE_PARENT(index) (((index) - 1) / 2)
#define FPQUEUE_IS_EMPTY JOIN(FPQUEUE_NAME, is_empty)
#define FPQUEUE_IS_FULL JOIN(FPQUEUE_NAME, is_full)

static inline FPQUEUE_TYPE* JOIN(FPQUEUE_NAME, create)(const uint32_t capacity) {
    if (capacity == 0 || capacity > (UINT32_MAX - offsetof(FPQUEUE_TYPE, elements)) / sizeof(FPQUEUE_ELEMENT_TYPE)) {
        return NULL;
    }
    FPQUEUE_TYPE* pqueue_ptr = (FPQUEUE_TYPE*)malloc(offsetof(FPQUEUE_TYPE, elements) + capacity * sizeof(FPQUEUE_ELEMENT_TYPE));

    if (!pqueue_ptr) {
        return NULL;
    }
    memset(pqueue_ptr, 0, offsetof(FPQUEUE_TYPE, elements) + capacity * sizeof(FPQUEUE_ELEMENT_TYPE));

    pqueue_ptr->count = 0;
    pqueue_ptr->capacity = capacity;

    return pqueue_ptr;
}

static inline void JOIN(FPQUEUE_NAME, destroy)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    free(pqueue_ptr);
}

static inline bool JOIN(FPQUEUE_NAME, is_empty)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return pqueue_ptr->count == 0;
}

static inline bool JOIN(FPQUEUE_NAME, is_full)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return pqueue_ptr->count == pqueue_ptr->capacity;
}

static inline VALUE_TYPE JOIN(FPQUEUE_NAME, get_max)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return pqueue_ptr->elements[0].value;
}

static inline VALUE_TYPE JOIN(FPQUEUE_NAME, peek)(const FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    return JOIN(FPQUEUE_NAME, get_max)(pqueue_ptr);
}

static inline void JOIN(FPQUEUE_NAME, downheap)(FPQUEUE_TYPE* pqueue_ptr, const uint32_t index) {
    assert(pqueue_ptr != NULL);
    assert(index <= pqueue_ptr->count);

    const uint32_t l = FPQUEUE_LCHILD(index);
    const uint32_t r = FPQUEUE_RCHILD(index);

    uint32_t largest = index;
    if (l < pqueue_ptr->count && pqueue_ptr->elements[l].priority > pqueue_ptr->elements[index].priority) {
        largest = l;
    }
    if (r < pqueue_ptr->count && pqueue_ptr->elements[r].priority > pqueue_ptr->elements[largest].priority) {
        largest = r;
    }

    if (largest == index) {
        return;
    }

    const FPQUEUE_ELEMENT_TYPE temp = pqueue_ptr->elements[index];
    pqueue_ptr->elements[index] = pqueue_ptr->elements[largest];
    pqueue_ptr->elements[largest] = temp;

    JOIN(FPQUEUE_NAME, downheap)(pqueue_ptr, largest);
}

static inline VALUE_TYPE JOIN(FPQUEUE_NAME, pop_max)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);
    assert(!FPQUEUE_IS_EMPTY(pqueue_ptr));

    const VALUE_TYPE max = pqueue_ptr->elements[0].value;

    pqueue_ptr->elements[0] = pqueue_ptr->elements[pqueue_ptr->count - 1];

    pqueue_ptr->count--;

    JOIN(FPQUEUE_NAME, downheap)(pqueue_ptr, 0);

    return max;
}

static inline void JOIN(FPQUEUE_NAME, upheap)(FPQUEUE_TYPE* pqueue_ptr, uint32_t index) {
    assert(pqueue_ptr != NULL);
    assert(index < pqueue_ptr->count);

    uint32_t parent;
    while (index > 0 && (parent = FPQUEUE_PARENT(index),

                         pqueue_ptr->elements[parent].priority < pqueue_ptr->elements[index].priority)) {

        const FPQUEUE_ELEMENT_TYPE temp = pqueue_ptr->elements[index];
        pqueue_ptr->elements[index] = pqueue_ptr->elements[parent];
        pqueue_ptr->elements[parent] = temp;

        index = parent;
    }
}

static inline void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE* pqueue_ptr, const VALUE_TYPE value, const uint32_t priority) {
    assert(pqueue_ptr != NULL);
    assert(!FPQUEUE_IS_FULL(pqueue_ptr));

    const uint32_t index = pqueue_ptr->count;

    pqueue_ptr->elements[index] = (FPQUEUE_ELEMENT_TYPE){.priority = priority, .value = value};

    pqueue_ptr->count++;

    JOIN(FPQUEUE_NAME, upheap)(pqueue_ptr, index);
}

static inline void JOIN(FPQUEUE_NAME, clear)(FPQUEUE_TYPE* pqueue_ptr) {
    assert(pqueue_ptr != NULL);

    pqueue_ptr->count = 0;
}

static inline void JOIN(FPQUEUE_NAME, copy)(FPQUEUE_TYPE* restrict dest_pqueue_ptr, const FPQUEUE_TYPE* restrict src_pqueue_ptr) {
    assert(src_pqueue_ptr != NULL);
    assert(dest_pqueue_ptr != NULL);
    assert(src_pqueue_ptr->count <= dest_pqueue_ptr->capacity);
    assert(FPQUEUE_IS_EMPTY(dest_pqueue_ptr));

    for (uint32_t i = 0; i < src_pqueue_ptr->count; i++) {
        dest_pqueue_ptr->elements[i] = src_pqueue_ptr->elements[i];
    }
    dest_pqueue_ptr->count = src_pqueue_ptr->count;
}

#undef NAME
#undef VALUE_TYPE

#undef FPQUEUE_NAME
#undef FPQUEUE_TYPE
#undef FPQUEUE_ELEMENT_TYPE
#undef FPQUEUE_IS_EMPTY
#undef FPQUEUE_IS_FULL

// vim: ft=c fdm=marker

// Source used:
// https://github.com/torvalds/linux/blob/master/include/linux/list.h

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct list_head_type {
    struct list_head_type* prev;
    struct list_head_type* next;
} list_head_type;

static inline void list_head_init(list_head_type* head) {
    head->prev = head;
    head->next = head;
}

static inline void list_add_between(list_head_type* new, list_head_type* prev, list_head_type* next) {
    prev->next = new;
    new->prev = prev;

    new->next = next;
    next->prev = new;
}

static inline void list_add(list_head_type* head, list_head_type* new) {
    list_add_between(new, head, head->next);
}

static inline void list_add_tail(list_head_type* head, list_head_type* new) {
    list_add_between(new, head->prev, head);
}

static inline void list_del(list_head_type* entry) {
    entry->prev = NULL;
    entry->next = NULL;
}

static inline bool list_is_first(list_head_type* head, list_head_type* entry) {
    return entry->prev == head;
}

static inline bool list_is_last(list_head_type* head, list_head_type* entry) {
    return entry->next == head;
}

static inline bool list_is_head(list_head_type* head, list_head_type* entry) {
    return entry == head;
}

static inline bool list_is_empty(list_head_type* head) {
    return head->next == head;
}

#define list_for_each_safe(pos, n, head) for (pos = (head)->next, n = pos->next; !list_is_head(pos, (head)); pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; !list_is_head(pos, (head)); pos = n, n = pos->prev)

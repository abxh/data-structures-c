/**
 * @file list.h
 * @brief Linked list.
 * @todo Create functions + fill the rest of the documentation. Maybe start work on freelist allocator.
 */

#pragma once

#define container_of(ptr, type, member)                   \
    __extensions__({                                      \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })

typedef struct ll_node {
    struct ll_node* prev_ptr;
    struct ll_node* next_ptr;
} ll_node;

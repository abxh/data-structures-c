/*  list.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file list.h
 * @brief Intrusive circular doubly linked list
 *
 * Inspired by:
 * @li https://github.com/torvalds/linux/blob/master/include/linux/list.h
 */

/**
 * @example examples/list/list_example.c
 * Example of how `list.h` header file is used in practice with `arena.h`.
 */

#pragma once

#include "container_of.h" // container_of

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Intrusive list node structure.
 */
typedef struct list_node_type {
    struct list_node_type* prev_ptr; ///< prev node pointer.
    struct list_node_type* next_ptr; ///< next node pointer.
} list_node_type;

/**
 * @def list_node_entry(node_ptr, container_type, node_member_name)
 * @brief Get the pointer to the container instance, given a pointer to
 *        to the node member instance, container type, and node member name.
 *
 * @param[in] node_ptr A pointer to the node member.
 * @param[in] container_type The container type.
 * @param[in] node_member_name The node member name.
 * @return A pointer to the container instance.
 */
#define list_node_entry(node_ptr, container_type, node_member_name) container_of(node_ptr, container_type, node_member_name)

/// @cond DO_NOT_DOCUMENT

// Add a node between two (known) nodes.
static inline void internal_list_node_add_between(list_node_type* node_ptr, list_node_type* before_ptr, list_node_type* after_ptr);

// Attach two nodes together, so anything in between is ignored.
static inline void internal_list_node_attach(list_node_type* prev_ptr, list_node_type* next_ptr);

/// @endcond

/**
 * @brief Initialize a list node
 *
 * @param[in] node_ptr The node pointer.
 */
static inline void list_node_init(list_node_type* node_ptr)
{
    assert(node_ptr != NULL);

    node_ptr->prev_ptr = node_ptr->next_ptr = node_ptr;
}

/**
 * @brief Check if a given list node is first in the list (aka after the head).
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] head_ptr The head node pointer.
 * @return Whether the list node is first in the list.
 */
static inline bool list_node_is_first(const list_node_type* node_ptr, const list_node_type* head_ptr)
{
    assert(node_ptr != NULL);
    assert(head_ptr != NULL);

    return node_ptr->prev_ptr == head_ptr;
}

/**
 * @brief Check if a given list node is the last of the list (aka before the tail).
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] tail_ptr The tail node pointer.
 * @return Whether the list node is the last in the list.
 */
static inline bool list_node_is_last(const list_node_type* node_ptr, const list_node_type* tail_ptr)
{
    assert(node_ptr != NULL);
    assert(tail_ptr != NULL);

    return node_ptr->next_ptr == tail_ptr;
}

/**
 * @brief Check if a given list node is the head of the list.
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] head_ptr The head node pointer.
 * @return Whether the list node is head of the list.
 */
static inline bool list_node_is_head(const list_node_type* node_ptr, const list_node_type* head_ptr)
{
    assert(node_ptr != NULL);
    assert(head_ptr != NULL);

    return node_ptr == head_ptr;
}

/**
 * @brief Check if a given list node is the tail of the list.
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] tail_ptr The tail node pointer.
 * @return Whether the list node is the tail of the list.
 */
static inline bool list_node_is_tail(const list_node_type* node_ptr, const list_node_type* tail_ptr)
{
    assert(node_ptr != NULL);
    assert(tail_ptr != NULL);

    return node_ptr == tail_ptr;
}

/**
 * @brief Add a node *after* the given node.
 *
 * @note This can be used to construct a stack.
 *
 * @param[in] prev_ptr The prev node pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_node_add_after(list_node_type* node_ptr, list_node_type* prev_ptr)
{
    assert(node_ptr != NULL);
    assert(prev_ptr != NULL);

    internal_list_node_add_between(node_ptr, prev_ptr, prev_ptr->next_ptr);
}

/**
 * @brief Add a node *before* the given node.
 *
 * @note This can be used to construct a queue.
 *
 * @param[in] next_ptr The next node pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_node_add_before(list_node_type* node_ptr, list_node_type* next_ptr)
{
    assert(node_ptr != NULL);
    assert(next_ptr != NULL);

    internal_list_node_add_between(node_ptr, next_ptr->prev_ptr, next_ptr);
}

/**
 * @brief Remove a node and deattach it from the list it resides in.
 *
 * Assumes:
 * @li node_ptr node is a part of a list.
 * @li node_ptr node is not the head or tail node.
 *
 * @param[in] node_ptr The node pointer.
 *
 * @return pointer to the removed node
 */
static inline list_node_type* list_node_remove(list_node_type* node_ptr)
{
    assert(node_ptr != NULL);
    assert(node_ptr->prev_ptr != node_ptr);
    assert(node_ptr->next_ptr != node_ptr);

    internal_list_node_attach(node_ptr->prev_ptr, node_ptr->next_ptr);
    list_node_init(node_ptr);

    return node_ptr;
}

/**
 * @brief Replace a given node by a new node.
 *
 * Assumes the node is a part of a list and not the head or tail node.
 *
 * @param[in] old_ptr Pointer to old node.
 * @param[in] new_ptr Pointer to new node.
 */
static inline void list_node_replace(list_node_type* old_ptr, list_node_type* new_ptr)
{
    assert(old_ptr != NULL);
    assert(new_ptr != NULL);
    assert(old_ptr != new_ptr);
    assert(old_ptr->prev_ptr != old_ptr);
    assert(old_ptr->next_ptr != old_ptr);

    internal_list_node_add_between(new_ptr, old_ptr->prev_ptr, old_ptr->next_ptr);
    list_node_init(old_ptr);
}

/// @cond DO_NOT_DOCUMENT

static inline void internal_list_node_add_between(list_node_type* node_ptr, list_node_type* before_ptr, list_node_type* after_ptr)
{
    assert(node_ptr != NULL);
    assert(before_ptr != NULL);
    assert(after_ptr != NULL);

    before_ptr->next_ptr = node_ptr;
    node_ptr->prev_ptr = before_ptr;

    after_ptr->prev_ptr = node_ptr;
    node_ptr->next_ptr = after_ptr;
}

static inline void internal_list_node_attach(list_node_type* prev_ptr, list_node_type* next_ptr)
{
    assert(prev_ptr != NULL);
    assert(next_ptr != NULL);

    prev_ptr->next_ptr = next_ptr;
    next_ptr->prev_ptr = prev_ptr;
}

/// @endcond

// vim: ft=c

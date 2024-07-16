/**
 * @file list.h
 * @brief Intrusive doubly linked list.
 *
 * Inspired by:
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
 */

/**
 * @example list/list_example.c
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

/**
 * @brief Initialize a list node
 *
 * @param[in,out] node_ptr The node pointer.
 */
static inline void list_node_init(list_node_type* node_ptr) {
    node_ptr->prev_ptr = node_ptr->next_ptr = node_ptr;
}

/**
 * @brief Check if a given list node is first in the list.
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] head_ptr The head node pointer.
 *
 * @return Whether the list node is first in the list.
 */
static inline bool list_node_is_first(const list_node_type* node_ptr, const list_node_type* head_ptr) {
    return node_ptr->prev_ptr == head_ptr;
}

/**
 * @brief Check if a given list node is the last of the list.
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] tail_ptr The tail node pointer.
 *
 * @return Whether the list node is last in the list.
 */
static inline bool list_node_is_last(const list_node_type* node_ptr, const list_node_type* tail_ptr) {
    return node_ptr->next_ptr == tail_ptr;
}

/// @cond DO_NOT_DOCUMENT

// Add a node between two (known) nodes.
static inline void internal_list_node_add_between(list_node_type* node_ptr, list_node_type* before_ptr, list_node_type* after_ptr) {
    before_ptr->next_ptr = node_ptr;
    node_ptr->prev_ptr = before_ptr;

    after_ptr->prev_ptr = node_ptr;
    node_ptr->next_ptr = after_ptr;
}
/// @endcond

/**
 * @brief Add a node *after* the given node.
 *
 * @note This can be used to construct a stack.
 *
 * @param[in,out] prev_ptr The prev node pointer.
 * @param[in,out] node_ptr The node pointer.
 */
static inline void list_node_add_after(list_node_type* node_ptr, list_node_type* prev_ptr) {
    internal_list_node_add_between(node_ptr, prev_ptr, prev_ptr->next_ptr);
}

/**
 * @brief Add a node *before* the given node.
 *
 * @note This can be used to construct a queue.
 *
 * @param[in,out] next_ptr The next node pointer.
 * @param[in,out] node_ptr The node pointer.
 */
static inline void list_node_add_before(list_node_type* node_ptr, list_node_type* next_ptr) {
    internal_list_node_add_between(node_ptr, next_ptr->prev_ptr, next_ptr);
}

/// @cond DO_NOT_DOCUMENT

// Attach two nodes together, so anything in between is ignored.
static inline void internal_list_node_attach(list_node_type* prev_ptr, list_node_type* next_ptr) {
    prev_ptr->next_ptr = next_ptr;
    next_ptr->prev_ptr = prev_ptr;
}
/// @endcond

/**
 * @brief Remove a node and deattach it from the list it resides in.
 *
 * @param[in,out] node_ptr The node pointer.
 */
static inline void list_node_remove(list_node_type* node_ptr) {
    internal_list_node_attach(node_ptr->prev_ptr, node_ptr->next_ptr);
    list_node_init(node_ptr);
}

/**
 * @brief Replace a given node by a new node.
 *
 * Assumes:
 * @li `old_ptr` and `new_ptr` are not pointing to the same node.
 * @li old_ptr node is a part of a list and not the head or tail node.
 *
 * @param[in,out] old_ptr Pointer to old node.
 * @param[in,out] new_ptr Pointer to new node.
 */
static inline void list_node_replace(list_node_type* old_ptr, list_node_type* new_ptr) {
    assert(old_ptr != new_ptr);
    assert(old_ptr->prev_ptr != old_ptr);
    assert(old_ptr->next_ptr != old_ptr);

    internal_list_node_add_between(new_ptr, old_ptr->prev_ptr, old_ptr->next_ptr);
    list_node_init(old_ptr);
}

// vim: ft=c

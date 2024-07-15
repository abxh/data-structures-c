/**
 * @file list.h
 * @brief Doubly linked list.
 *
 * Inspired by:
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
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
 * @retval A pointer to the container instance.
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
 *
 * @return Whether the list node is first in the list.
 */
static inline bool list_node_is_first(const list_node_type* node_ptr) {
    return node_ptr->prev_ptr == node_ptr;
}

/**
 * @brief Check if a given list node is last in the list.
 *
 * @param[in] node_ptr The node pointer.
 *
 * @return Whether the list node is last in the list.
 */
static inline bool list_node_is_last(const list_node_type* node_ptr) {
    return node_ptr->next_ptr == node_ptr;
}

/**
 * @brief Count number of nodes after a given node.
 *
 * This does not count the given node itself. So if the given node is the last node,
 * the function returns 0.
 *
 * @param[in] head_ptr The head node pointer.
 *
 * @return Number of nodes *after* head_ptr.
 *  @retval 0 If the given node is the last node.
 */
static inline size_t list_node_count_after(list_node_type* head_ptr) {
    list_node_type* node_ptr = head_ptr;
    size_t count = 0;

    while (!list_node_is_last(node_ptr)) {
        node_ptr = node_ptr->next_ptr;
        count++;
    }
    return count;
}

/// @cond DO_NOT_DOCUMENT
static inline void internal_list_node_add_between(list_node_type* node_ptr, list_node_type* before_ptr, list_node_type* after_ptr) {
    before_ptr->next_ptr = node_ptr;
    node_ptr->prev_ptr = before_ptr;

    after_ptr->prev_ptr = node_ptr;
    node_ptr->next_ptr = after_ptr;
}
/// @endcond

/**
 * @brief Add a node *after* the head node.
 *
 * @param[in] head_ptr The head node pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_add_after(list_node_type* node_ptr, list_node_type* head_ptr) {
    internal_list_node_add_between(node_ptr, head_ptr, head_ptr->next_ptr);
}

/**
 * @brief Add a node *before* the tail node.
 *
 * @param[in] tail_ptr The tail node pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_add_before(list_node_type* node_ptr, list_node_type* tail_ptr) {
    internal_list_node_add_between(node_ptr, tail_ptr->prev_ptr, tail_ptr);
}

// vim: ft=c fdm=marker

/**
 * @file list.h
 * @brief Doubly linked list.
 *
 * Inspired by:
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
 */

#pragma once

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
 * @brief List structure, keeping track of head nod, tail node and node count.
 */
typedef struct {
    struct list_node_type* head_ptr; ///< head node pointer.
    struct list_node_type* tail_ptr; ///< tail node pointer.
    size_t count;                    ///< Node count.
} list_type;

/**
 * @brief Initalize a list.
 * @note The list could heap / stack allocated depending on program structure.
 *
 * Assumes list_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 */
static inline void list_init(list_type* list_ptr) {
    assert(list_ptr != NULL);

    list_ptr->head_ptr = NULL;
    list_ptr->tail_ptr = NULL;
    list_ptr->count = 0;
}

/**
 * @brief Initalize a list node.
 * @note The list node could heap / stack allocated depending on program structure.
 *
 * Assumes node_ptr is not NULL.
 *
 * @param[in] node_ptr The list pointer.
 */
static inline void list_node_init(list_node_type* node_ptr) {
    assert(node_ptr != NULL);

    node_ptr->prev_ptr = NULL;
    node_ptr->next_ptr = NULL;
}

/**
 * @brief Check if the list empty.
 *
 * Assumes list_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 */
static inline bool list_is_empty(list_type* list_ptr) {
    assert(list_ptr != NULL);

    return list_ptr->head_ptr == NULL; // or list_ptr->tail_ptr == NULL
}

/**
 * @brief Check if the list node is head.
 *
 * @param[in] node_ptr The node pointer.
 * @return Whether list node is head.
 */
static inline bool list_node_is_head(list_node_type* node_ptr) {
    assert(node_ptr != NULL);

    return node_ptr->prev_ptr == NULL;
}

/**
 * @brief Check if the list node is tail.
 *
 * @param[in] node_ptr The node pointer.
 * @return Whether list node is tail.
 */
static inline bool list_node_is_tail(list_node_type* node_ptr) {
    assert(node_ptr != NULL);

    return node_ptr->next_ptr == NULL;
}

/**
 * @brief Append a node to the head of the list.
 *
 * Assumes:
 * @li list_ptr is not NULL.
 * @li node_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_appendleft(list_type* list_ptr, list_node_type* node_ptr) {
    assert(list_ptr != NULL);
    assert(node_ptr != NULL);

    list_node_init(node_ptr);

    if (list_is_empty(list_ptr)) {
        list_ptr->head_ptr = node_ptr;
        list_ptr->tail_ptr = node_ptr;
    } else {
        list_ptr->head_ptr->prev_ptr = node_ptr;
        node_ptr->next_ptr = list_ptr->head_ptr;
        list_ptr->head_ptr = node_ptr;
    }

    list_ptr->count++;
}

/**
 * @brief Append a node to the tail of the list.
 *
 * Assumes:
 * @li list_ptr is not NULL.
 * @li node_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_append(list_type* list_ptr, list_node_type* node_ptr) {
    assert(list_ptr != NULL);
    assert(node_ptr != NULL);

    list_node_init(node_ptr);

    if (list_is_empty(list_ptr)) {
        list_ptr->head_ptr = node_ptr;
        list_ptr->tail_ptr = node_ptr;
    } else {
        list_ptr->tail_ptr->next_ptr = node_ptr;
        node_ptr->prev_ptr = list_ptr->tail_ptr;
        list_ptr->tail_ptr = node_ptr;
    }

    list_ptr->count++;
}

/**
 * @brief Pop the tail of the list.
 *
 * Assumes list_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @return The node pointer.
 *  @retval NULL if the list is empty.
 */
static inline list_node_type* list_pop(list_type* list_ptr) {
    assert(list_ptr != NULL);

    if (list_is_empty(list_ptr)) {
        return NULL;
    }

    list_node_type* node_ptr = list_ptr->tail_ptr;

    list_ptr->tail_ptr = list_ptr->tail_ptr->prev_ptr;
    if (list_ptr->tail_ptr == NULL) {
        list_ptr->head_ptr = NULL;
    } else {
        list_ptr->tail_ptr->next_ptr = NULL;
    }
    list_ptr->count--;

    return node_ptr;
}

/**
 * @brief Pop the head of the list.
 *
 * Assumes:
 * @li list_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @return The node pointer.
 *  @retval NULL if the list is empty.
 */
static inline list_node_type* list_popleft(list_type* list_ptr) {
    assert(list_ptr != NULL);

    if (list_is_empty(list_ptr)) {
        return NULL;
    }

    list_node_type* node_ptr = list_ptr->head_ptr;

    list_ptr->head_ptr = list_ptr->head_ptr->next_ptr;
    if (list_ptr->head_ptr == NULL) {
        list_ptr->tail_ptr = NULL;
    } else {
        list_ptr->head_ptr->prev_ptr = NULL;
    }
    list_ptr->count--;

    return node_ptr;
}

/**
 * @brief Erase a given node from the list.
 *
 * Assumes node_ptr is not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @param[in] node_ptr The node pointer.
 */
static inline void list_erase(list_type* list_ptr, list_node_type* node_ptr) {
    assert(list_ptr != NULL);
    assert(node_ptr != NULL);

    if (list_node_is_head(node_ptr)) {
        list_popleft(list_ptr);
        return;
    } else if (list_node_is_tail(node_ptr)) {
        list_pop(list_ptr);
        return;
    }

    assert(list_ptr->count > 2);

    node_ptr->prev_ptr->next_ptr = node_ptr->next_ptr;
    node_ptr->next_ptr->prev_ptr = node_ptr->prev_ptr;

    list_node_init(node_ptr);
}

/**
 * @brief Swap a given node with another node in the list.
 *
 * Assumes both node points are not NULL.
 *
 * @param[in] list_ptr The list pointer.
 * @param[in] a_ptr Pointer to the first node.
 * @param[in] b_ptr Pointer to the second node.
 */
static inline void list_swap(list_type* list_ptr, list_node_type* a_ptr, list_node_type* b_ptr) {
    assert(a_ptr != NULL);
    assert(b_ptr != NULL);

    if (list_node_is_head(a_ptr)) {
        assert(!list_node_is_head(b_ptr));

        list_ptr->head_ptr = b_ptr;
    } else if (list_node_is_head(b_ptr)) {
        assert(!list_node_is_head(a_ptr));

        list_ptr->head_ptr = a_ptr;
    }
    if (list_node_is_tail(a_ptr)) {
        assert(!list_node_is_tail(b_ptr));
        
        list_ptr->tail_ptr = a_ptr;
    } else if (list_node_is_tail(b_ptr)) {
        assert(!list_node_is_tail(a_ptr));
        
        list_ptr->tail_ptr = b_ptr;
    }

    list_node_type* prev_ptr = a_ptr->prev_ptr;
    a_ptr->prev_ptr = b_ptr->prev_ptr;
    b_ptr->prev_ptr = prev_ptr;

    list_node_type* next_ptr = a_ptr->next_ptr;
    a_ptr->next_ptr = b_ptr->next_ptr;
    b_ptr->next_ptr = next_ptr;
}

// vim: ft=c fdm=marker

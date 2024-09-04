/*  rbtree.h
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
 * @file rbtree.h
 * @brief Intrusive red-black tree
 *
 * Duplicates are not allowed. For supporting duplicate keys, you may store a counter for each node,
 * and count up when a node key already exists.
 *
 * The memory of the nodes are expected to managed seperately.
 *
 * Inspired by:
 * @li https://github.com/torvalds/linux/blob/master/include/linux/rbtree.h
 *
 * Sources used:
 * @li https://en.wikipedia.org/wiki/Binary_search_tree#Operations
 * @li https://en.wikipedia.org/wiki/Red-black_tree#Operations
 * @li CLRS
 */

/**
 * @example examples/rbtree/rbtree.c
 * Examples of how `rbtree.h` header file is used in practice.
 */

// macro definitions: {{{

#ifndef RBTREE_H
#define RBTREE_H

#include "container_of.h" // container_of
#include "paste.h"        // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @def rbtree_node_entry(node_ptr, container_type, node_member_name)
 * @brief Get the pointer to the container instance, given a pointer to
 *        to the node member instance, container type, and node member name.
 *
 * @param[in] node_ptr A pointer to the node member.
 * @param[in] container_type The container type.
 * @param[in] node_member_name The node member name.
 *
 * @return A pointer to the container instance.
 */
#define rbtree_node_entry(node_ptr, container_type, node_member_name) container_of(node_ptr, container_type, node_member_name)

#endif

/**
 * @def NAME
 * @brief Prefix to red black tree types and operations.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef NAME
#error "Must define NAME."
#define NAME rbtree
#else
#define RBTREE_NAME NAME
#endif

/**
 * @def KEY_TYPE
 * @brief The key type. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef KEY_TYPE
#error "Must define KEY_TYPE."
#define KEY_TYPE int
#endif

/**
 * @def KEY_IS_STRICTLY_LESS(a, b)
 * @brief Used to compare two keys. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 *
 * Equality for two keys a and b is defined as !KEY_IS_STRICTLY_LESS(a,b) && !KEY_IS_STRICTLY_LESS(b,a).
 *
 * @attention This must be manually defined before including this header file.
 *
 * @retval true If key a is strictly less than b.
 * @retval false If key a is greater than or equal to b.
 */
#ifndef KEY_IS_STRICTLY_LESS
#error "Must define KEY_IS_STRICTLY_LESS."
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#endif

/// @cond DO_NOT_DOCUMENT
#define RBTREE_TYPE                             JOIN(RBTREE_NAME, type)
#define RBTREE_NODE_TYPE                        JOIN(RBTREE_NAME, node_type)
#define RBTREE_CONTAINS_KEY                     JOIN(RBTREE_NAME, contains_key)
#define RBTREE_NODE_IS_RED                      JOIN(RBTREE_NAME, node_is_red)
#define RBTREE_NODE_IS_BLACK                    JOIN(RBTREE_NAME, node_is_black)
#define RBTREE_NODE_GET_PARENT_PTR              JOIN(RBTREE_NAME, node_get_parent_ptr)

#define RBTREE_NODE_TRANSPLANT                  JOIN(internal, JOIN(RBTREE_NAME, node_transplant))
#define RBTREE_NODE_SET_COLOR_TO_RED            JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_red))
#define RBTREE_NODE_SET_COLOR_TO_BLACK          JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_black))
#define RBTREE_NODE_SET_PARENT_PTR              JOIN(internal, JOIN(RBTREE_NAME, node_set_parent_ptr))
#define RBTREE_NODE_SET_COLOR_TO_COLOR_OF_OTHER JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_color_of_other))
#define RBTREE_ROTATE_DIR                       JOIN(internal, JOIN(RBTREE_NAME, rotate_dir))

#define RBTREE_CHILD_DIR(node_ptr)              ((node_ptr) == RBTREE_NODE_GET_PARENT_PTR(node_ptr)->left_ptr ? 0 : 1)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated red-black tree node struct type for a given `KEY_TYPE`.
 */
typedef struct RBTREE_NODE_TYPE {
    uintptr_t __parent_ptr_with_color; ///< variable containing both the parent pointer and color info
    union {
        struct {
            struct RBTREE_NODE_TYPE* left_ptr;  ///< pointer to left node
            struct RBTREE_NODE_TYPE* right_ptr; ///< pointer to right node
        };
        struct RBTREE_NODE_TYPE* child_ptrs[2]; ///< array of child pointers
    };
    KEY_TYPE key; ///< node key
} RBTREE_NODE_TYPE;

// }}}

// function definitions: {{{

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_red))(RBTREE_NODE_TYPE* node_ptr);
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_black))(RBTREE_NODE_TYPE* node_ptr);
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_color_of_other))(RBTREE_NODE_TYPE* node_ptr,
                                                                                       const RBTREE_NODE_TYPE* other_ptr);
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_parent_ptr))(RBTREE_NODE_TYPE* node_ptr, RBTREE_NODE_TYPE* parent_ptr);

// rotate a subtree around a given subtree root node and direction (0: left or 1: right). returns the new subtree root
static inline RBTREE_NODE_TYPE* JOIN(internal, JOIN(RBTREE_NAME, rotate_dir))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* P,
                                                                              const uint8_t dir);

// rebalance tree after insert. see explanation in the sources linked above.
static inline void JOIN(internal, JOIN(RBTREE_NAME, insert_fixup))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* N);

// move the parent of a node (src) to another node (dest).
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_transplant))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* src_node,
                                                                      RBTREE_NODE_TYPE* dest_node);

// rebalance tree after delete. see explanation in the sources linked above. for the special case where:
// (P's child) was not root and was black and had no children
static inline void JOIN(internal, JOIN(RBTREE_NAME, delete_fixup))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* P, int dir);
/// @endcond

/**
 * @brief Initialize a red-black tree struct
 *
 * @note This should be initialized with the address to a pointer to a node.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 */
static inline void JOIN(RBTREE_NAME, init)(RBTREE_NODE_TYPE** rootptr_ptr)
{
    assert(rootptr_ptr != NULL);

    *rootptr_ptr = NULL;
}

/**
 * @brief Initialize a red-black tree node
 *
 * @param[in] node_ptr The node pointer.
 * @param[in] key The key to initialize the node with.
 */
static inline void JOIN(RBTREE_NAME, node_init)(RBTREE_NODE_TYPE* node_ptr, KEY_TYPE key)
{
    assert(node_ptr != NULL);

    node_ptr->key = key;
    node_ptr->left_ptr = node_ptr->right_ptr = NULL;
    RBTREE_NODE_SET_PARENT_PTR(node_ptr, NULL);
}

/**
 * @brief Extract the parent pointer of a node.
 *
 * @param[in] node_ptr The node pointer.
 */
static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, node_get_parent_ptr)(RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    return (RBTREE_NODE_TYPE*)(node_ptr->__parent_ptr_with_color & ~1);
}

/**
 * @brief Check if the given node is colored black.
 *
 * @param[in] node_ptr The node pointer.
 */
static inline bool JOIN(RBTREE_NAME, node_is_black)(const RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    return (node_ptr->__parent_ptr_with_color & 1);
}

/**
 * @brief Check if the given node is colored red.
 *
 * @param[in] node_ptr The node pointer.
 */
static inline bool JOIN(RBTREE_NAME, node_is_red)(const RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    return !JOIN(RBTREE_NAME, node_is_black)(node_ptr);
}

/**
 * @brief Check if the tree is empty.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 * @return Whether the tree is empty.
 */
static inline bool JOIN(RBTREE_NAME, is_empty)(RBTREE_NODE_TYPE** rootptr_ptr)
{
    assert(rootptr_ptr != NULL);

    return *rootptr_ptr == NULL;
}

/**
 * @brief Check if the tree contains a given key.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 * @param[in] key The key
 * @return Whether the tree contains the key.
 */
static inline bool JOIN(RBTREE_NAME, contains_key)(RBTREE_NODE_TYPE** rootptr_ptr, const KEY_TYPE key)
{
    assert(rootptr_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = *rootptr_ptr;

    while (node_ptr != NULL) {
        const bool is_strictly_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_strictly_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_strictly_less && !is_strictly_greater) {
            return true;
        }
        else if (is_strictly_less) {
            node_ptr = node_ptr->left_ptr;
        }
        else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return false;
}

/**
 * @brief Search for a given node and return a pointer to the node.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 * @param[in] key The key of the node being searched for.
 * @return A pointer to the searched node
 *  @retval `NULL` If a node with the given key wasn't found.
 */
static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, search_node)(RBTREE_NODE_TYPE** rootptr_ptr, const KEY_TYPE key)
{
    assert(rootptr_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = *rootptr_ptr;
    while (node_ptr != NULL) {
        const bool is_strictly_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_strictly_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_strictly_less && !is_strictly_greater) {
            return node_ptr;
        }
        else if (is_strictly_less) {
            node_ptr = node_ptr->left_ptr;
        }
        else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return NULL;
}

/**
 * @brief Insert a given node with a non-duplicate key in the tree.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 * @param[in] node_ptr The node pointer.
 */
static inline void JOIN(RBTREE_NAME, insert_node)(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* node_ptr)
{
    assert(rootptr_ptr != NULL);
    assert(node_ptr != NULL);
    assert(RBTREE_CONTAINS_KEY(rootptr_ptr, node_ptr->key) == false);

    RBTREE_NODE_TYPE* parent_ptr = NULL;
    RBTREE_NODE_TYPE* current_ptr = *rootptr_ptr;

    while (current_ptr != NULL) {
        parent_ptr = current_ptr;

        if (KEY_IS_STRICTLY_LESS(node_ptr->key, current_ptr->key)) {
            current_ptr = current_ptr->left_ptr;
        }
        else {
            current_ptr = current_ptr->right_ptr;
        }
    }

    node_ptr->left_ptr = node_ptr->right_ptr = NULL;
    RBTREE_NODE_SET_PARENT_PTR(node_ptr, parent_ptr);
    RBTREE_NODE_SET_COLOR_TO_RED(node_ptr);

    if (parent_ptr == NULL) {
        *rootptr_ptr = node_ptr;
    }
    else {
        const uint8_t dir = KEY_IS_STRICTLY_LESS(parent_ptr->key, node_ptr->key);
        parent_ptr->child_ptrs[dir] = node_ptr;

        JOIN(internal, JOIN(RBTREE_NAME, insert_fixup))(rootptr_ptr, node_ptr);
    }
}

/**
 * @brief Delete a given node from the tree.
 *
 * @param[in] rootptr_ptr A pointer to the pointer to the root node.
 * @param[in] node_ptr The node pointer.
 * @return Pointer to the deleted node reinitialized.
 */
static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, delete_node)(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* node_ptr)
{
    assert(rootptr_ptr != NULL);
    assert(node_ptr != NULL);

    if (node_ptr->left_ptr == NULL && node_ptr->right_ptr == NULL) {
        if (node_ptr == *rootptr_ptr || RBTREE_NODE_IS_RED(node_ptr)) {
            RBTREE_NODE_TRANSPLANT(rootptr_ptr, node_ptr, NULL);
        }
        else {
            RBTREE_NODE_TYPE* const parent_ptr = RBTREE_NODE_GET_PARENT_PTR(node_ptr);
            const int dir = RBTREE_CHILD_DIR(node_ptr);

            RBTREE_NODE_TRANSPLANT(rootptr_ptr, node_ptr, NULL);

            JOIN(internal, JOIN(RBTREE_NAME, delete_fixup))(rootptr_ptr, parent_ptr, dir);
        }
    }
    else if (node_ptr->left_ptr == NULL || node_ptr->right_ptr == NULL) {
        const uint8_t dir = node_ptr->left_ptr == NULL;

        assert(RBTREE_NODE_IS_BLACK(node_ptr));
        assert(RBTREE_NODE_IS_RED(node_ptr->child_ptrs[dir]));

        RBTREE_NODE_SET_COLOR_TO_BLACK(node_ptr->child_ptrs[dir]);

        RBTREE_NODE_TRANSPLANT(rootptr_ptr, node_ptr, node_ptr->child_ptrs[dir]);
    }
    else {
        RBTREE_NODE_TYPE* successor_ptr = node_ptr->right_ptr;

        while (successor_ptr->left_ptr != NULL) {
            successor_ptr = successor_ptr->left_ptr;
        }

        const bool prev_successor_black = RBTREE_NODE_IS_BLACK(successor_ptr);
        const int prev_successor_dir = RBTREE_CHILD_DIR(successor_ptr);
        RBTREE_NODE_TYPE* const prev_successor_parent_ptr = RBTREE_NODE_GET_PARENT_PTR(successor_ptr);
        RBTREE_NODE_TYPE* const prev_successor_child_ptr = successor_ptr->right_ptr;

        {
            if (RBTREE_NODE_GET_PARENT_PTR(successor_ptr) != node_ptr) {
                RBTREE_NODE_TRANSPLANT(rootptr_ptr, successor_ptr, successor_ptr->right_ptr);

                successor_ptr->right_ptr = node_ptr->right_ptr;
                RBTREE_NODE_SET_PARENT_PTR(node_ptr->right_ptr, successor_ptr);
            }
            RBTREE_NODE_TRANSPLANT(rootptr_ptr, node_ptr, successor_ptr);

            successor_ptr->left_ptr = node_ptr->left_ptr;
            RBTREE_NODE_SET_PARENT_PTR(node_ptr->left_ptr, successor_ptr);
        }

        {
            RBTREE_NODE_SET_COLOR_TO_COLOR_OF_OTHER(successor_ptr, node_ptr);

            if (prev_successor_child_ptr != NULL) {
                assert(prev_successor_black);
                assert(RBTREE_NODE_IS_RED(prev_successor_child_ptr));

                RBTREE_NODE_SET_COLOR_TO_BLACK(prev_successor_child_ptr);
            }
            else if (prev_successor_child_ptr == NULL && prev_successor_black) {
                if (prev_successor_parent_ptr == node_ptr) {
                    JOIN(internal, JOIN(RBTREE_NAME, delete_fixup))(rootptr_ptr, successor_ptr, prev_successor_dir);
                }
                else {
                    JOIN(internal, JOIN(RBTREE_NAME, delete_fixup))(rootptr_ptr, prev_successor_parent_ptr, prev_successor_dir);
                }
            }
        }
    }

    node_ptr->left_ptr = node_ptr->right_ptr = NULL;
    RBTREE_NODE_SET_PARENT_PTR(node_ptr, NULL);

    return node_ptr;
}

/// @cond DO_NOT_DOCUMENT

static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_red))(RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    node_ptr->__parent_ptr_with_color &= ~1;
}
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_black))(RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    node_ptr->__parent_ptr_with_color |= 1;
}
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_color_of_other))(RBTREE_NODE_TYPE* node_ptr,
                                                                                       const RBTREE_NODE_TYPE* other_ptr)
{
    assert(other_ptr != NULL);
    assert(node_ptr != NULL);

    const bool is_black = other_ptr->__parent_ptr_with_color & 1;

    node_ptr->__parent_ptr_with_color &= ~1;
    node_ptr->__parent_ptr_with_color += is_black;
}
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_parent_ptr))(RBTREE_NODE_TYPE* node_ptr, RBTREE_NODE_TYPE* parent_ptr)
{
    assert(node_ptr != NULL);

    const bool is_black = node_ptr->__parent_ptr_with_color & 1;

    node_ptr->__parent_ptr_with_color = (uintptr_t)parent_ptr;
    node_ptr->__parent_ptr_with_color += is_black;
}

// rotate a subtree around a given subtree root node and direction (0: left or 1: right). returns the new subtree root
static inline RBTREE_NODE_TYPE* JOIN(internal, JOIN(RBTREE_NAME, rotate_dir))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* P,
                                                                              const uint8_t dir)
{
    assert(rootptr_ptr != NULL);
    assert(P != NULL);
    assert(dir == 0 || dir == 1);

    /*  Right rotate around P:
             P                   S
           /   \                /  \
          S     T     -->      ..   P
         / \   / \     ⟳           /  \
        .. C  .. ..               C    T
          / \                   / \   / \
         .. ..                 .. .. .. ..

        Left rotate around P:
             P                   S
           /   \                /  \
          T     S              P   ..
         / \   / \     -->    /  \
        .. .. C  ..     ↺    T    C
             / \            / \  / \
            .. ..          .. .. .. ..
    */

    RBTREE_NODE_TYPE* G = RBTREE_NODE_GET_PARENT_PTR(P);
    RBTREE_NODE_TYPE* S = P->child_ptrs[1 - dir];
    assert(S != NULL);
    RBTREE_NODE_TYPE* C = S->child_ptrs[dir];

    P->child_ptrs[1 - dir] = C;
    if (C != NULL) {
        RBTREE_NODE_SET_PARENT_PTR(C, P);
    }

    S->child_ptrs[dir] = P;
    RBTREE_NODE_SET_PARENT_PTR(P, S);

    RBTREE_NODE_SET_PARENT_PTR(S, G);
    if (G != NULL) {
        G->child_ptrs[P == G->left_ptr ? 0 : 1] = S;
    }
    else {
        *rootptr_ptr = S;
    }
    return S;
}

// rebalance tree after insert. see explanation in the sources linked above.
static inline void JOIN(internal, JOIN(RBTREE_NAME, insert_fixup))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* N)
{
    assert(rootptr_ptr != NULL);
    assert(N != NULL);
    assert(RBTREE_NODE_IS_RED(N));
    assert(N != *rootptr_ptr);

    RBTREE_NODE_TYPE* P = RBTREE_NODE_GET_PARENT_PTR(N); // parent
    RBTREE_NODE_TYPE* G = NULL;                          // grandparent
    RBTREE_NODE_TYPE* U = NULL;                          // uncle

    do {
        if (RBTREE_NODE_IS_BLACK(P)) {
            return;
        }
        if ((G = RBTREE_NODE_GET_PARENT_PTR(P)) == NULL) {
            RBTREE_NODE_SET_COLOR_TO_BLACK(P);
            return;
        }
        const uint8_t dir = RBTREE_CHILD_DIR(P);
        U = G->child_ptrs[1 - dir];

        if (U == NULL || RBTREE_NODE_IS_BLACK(U)) {
            if (N == P->child_ptrs[1 - dir]) {
                RBTREE_ROTATE_DIR(rootptr_ptr, P, dir);
                N = P;
                P = G->child_ptrs[dir];
            }
            RBTREE_NODE_SET_COLOR_TO_BLACK(P);
            RBTREE_NODE_SET_COLOR_TO_RED(G);
            RBTREE_ROTATE_DIR(rootptr_ptr, G, 1 - dir);
            return;
        }

        RBTREE_NODE_SET_COLOR_TO_BLACK(P);
        RBTREE_NODE_SET_COLOR_TO_BLACK(U);
        RBTREE_NODE_SET_COLOR_TO_RED(G);

        N = G;
        P = RBTREE_NODE_GET_PARENT_PTR(N);
    } while (P != NULL);
}

// move the parent of a node (src) to another node (dest).
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_transplant))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* src_node,
                                                                      RBTREE_NODE_TYPE* dest_node)
{
    assert(rootptr_ptr != NULL);
    assert(src_node != NULL);

    RBTREE_NODE_TYPE* src_node_parent_ptr = RBTREE_NODE_GET_PARENT_PTR(src_node);

    if (src_node_parent_ptr == NULL) {
        *rootptr_ptr = dest_node;
    }
    else {
        src_node_parent_ptr->child_ptrs[RBTREE_CHILD_DIR(src_node)] = dest_node;
    }

    if (dest_node != NULL) {
        RBTREE_NODE_SET_PARENT_PTR(dest_node, src_node_parent_ptr);
    }
}

// rebalance tree after delete. see explanation in the sources linked above. for the special case where:
//      (P's child) was not root and was black and had no children
static inline void JOIN(internal, JOIN(RBTREE_NAME, delete_fixup))(RBTREE_NODE_TYPE** rootptr_ptr, RBTREE_NODE_TYPE* P, int dir)
{
    assert(rootptr_ptr != NULL);
    assert(P != NULL);
    assert(dir == 0 || dir == 1);

    RBTREE_NODE_TYPE* N = NULL; // node (temp)
    RBTREE_NODE_TYPE* S = NULL; // sibling
    RBTREE_NODE_TYPE* C = NULL; // close nephew
    RBTREE_NODE_TYPE* D = NULL; // distant nephew

    do {
        S = P->child_ptrs[1 - dir];
        D = S->child_ptrs[1 - dir];
        C = S->child_ptrs[dir];

        if (RBTREE_NODE_IS_RED(S)) {
            goto Case_3;
        }
        if (D != NULL && RBTREE_NODE_IS_RED(D)) {
            goto Case_6;
        }
        if (C != NULL && RBTREE_NODE_IS_RED(C)) {
            goto Case_5;
        }
        if (RBTREE_NODE_IS_RED(P)) {
            goto Case_4;
        }
        RBTREE_NODE_SET_COLOR_TO_RED(S);

        N = P;
        P = RBTREE_NODE_GET_PARENT_PTR(N);
    } while (P != NULL && (dir = RBTREE_CHILD_DIR(N), true));

    return;
Case_3:
    RBTREE_ROTATE_DIR(rootptr_ptr, P, dir);
    RBTREE_NODE_SET_COLOR_TO_RED(P);
    RBTREE_NODE_SET_COLOR_TO_BLACK(S);
    S = C;
    D = S->child_ptrs[1 - dir];
    if (D != NULL && RBTREE_NODE_IS_RED(D)) {
        goto Case_6;
    }
    C = S->child_ptrs[dir];
    if (C != NULL && RBTREE_NODE_IS_RED(C)) {
        goto Case_5;
    }
Case_4:
    RBTREE_NODE_SET_COLOR_TO_RED(S);
    RBTREE_NODE_SET_COLOR_TO_BLACK(P);
    return;
Case_5:
    RBTREE_ROTATE_DIR(rootptr_ptr, S, 1 - dir);
    RBTREE_NODE_SET_COLOR_TO_RED(S);
    RBTREE_NODE_SET_COLOR_TO_BLACK(C);
    D = S;
    S = C;
Case_6:
    RBTREE_ROTATE_DIR(rootptr_ptr, P, dir);
    RBTREE_NODE_SET_COLOR_TO_COLOR_OF_OTHER(S, P);
    RBTREE_NODE_SET_COLOR_TO_BLACK(P);
    RBTREE_NODE_SET_COLOR_TO_BLACK(D);
    return;
}
/// @endcond

// }}}

// macro undefs: {{{

#undef NAME
#undef KEY_TYPE
#undef VALUE_TYPE
#undef KEY_IS_STRICTLY_LESS

#undef RBTREE_TYPE
#undef RBTREE_NAME
#undef RBTREE_NODE_TYPE

#undef RBTREE_CONTAINS_KEY
#undef RBTREE_NODE_IS_RED
#undef RBTREE_NODE_IS_BLACK
#undef RBTREE_NODE_GET_PARENT_PTR
#undef RBTREE_NODE_TRANSPLANT
#undef RBTREE_NODE_SET_COLOR_TO_COLOR_OF_OTHER
#undef RBTREE_NODE_SET_COLOR_TO_RED
#undef RBTREE_NODE_SET_COLOR_TO_BLACK
#undef RBTREE_NODE_SET_PARENT_PTR
#undef RBTREE_CHILD_DIR

// }}}

// vim: ft=c fdm=marker

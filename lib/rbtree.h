
// inspiration:
// https://github.com/torvalds/linux/blob/master/include/linux/rbtree.h

// sources used:
// https://en.wikipedia.org/wiki/Binary_search_tree#Operations
// https://en.wikipedia.org/wiki/Red-black_tree#Operations
// CLRS

#ifndef RBTREE_H
#define RBTREE_H

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#endif

#ifndef NAME
#error "Must define NAME."
#define NAME rbtree
#else
#define RBTREE_NAME NAME
#endif

#ifndef KEY_TYPE
#error "Must define KEY_TYPE."
#define KEY_TYPE int
#endif

#ifndef KEY_IS_STRICTLY_LESS
#error "Must define KEY_IS_STRICTLY_LESS."
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#endif

#ifndef VALUE_TYPE
#error "Must define VALUE_TYPE."
#define VALUE_TYPE int
#endif

#define RBTREE_TYPE      JOIN(RBTREE_NAME, type)
#define RBTREE_NODE_TYPE JOIN(RBTREE_NAME, node_type)

/// @cond DO_NOT_DOCUMENT
#define RBTREE_CONTAINS_KEY            JOIN(RBTREE_NAME, contains_key)
#define RBTREE_NODE_IS_RED             JOIN(RBTREE_NAME, node_is_red)
#define RBTREE_NODE_IS_BLACK           JOIN(RBTREE_NAME, node_is_black)
#define RBTREE_NODE_GET_PARENT_PTR     JOIN(RBTREE_NAME, node_get_parent_ptr)

#define RBTREE_NODE_SHIFT              JOIN(internal, JOIN(RBTREE_NAME, node_shift))
#define RBTREE_NODE_SET_COLOR_TO_RED   JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_red))
#define RBTREE_NODE_SET_COLOR_TO_BLACK JOIN(internal, JOIN(RBTREE_NAME, node_set_color_to_black))
#define RBTREE_NODE_SET_PARENT_PTR     JOIN(internal, JOIN(RBTREE_NAME, node_set_parent_ptr))
#define RBTREE_ROTATE_DIR              JOIN(internal, JOIN(RBTREE_NAME, rotate_dir))

#define RBTREE_CHILD_DIR(node_ptr)     ((node_ptr) == RBTREE_NODE_GET_PARENT_PTR(node_ptr)->left_ptr ? 0 : 1)
/// @endcond

typedef struct RBTREE_NODE_TYPE {
    uintptr_t __parent_ptr_with_color;
    union {
        struct {
            struct RBTREE_NODE_TYPE* left_ptr;
            struct RBTREE_NODE_TYPE* right_ptr;
        };
        struct RBTREE_NODE_TYPE* child_ptrs[2];
    };
    KEY_TYPE key;
    VALUE_TYPE value;
} RBTREE_NODE_TYPE;

typedef struct RBTREE_TYPE {
    RBTREE_NODE_TYPE* root_ptr;
    size_t count;
} RBTREE_TYPE;

static inline void JOIN(RBTREE_NAME, init)(RBTREE_TYPE* rbtree_ptr)
{
    assert(rbtree_ptr != NULL);

    rbtree_ptr->root_ptr = NULL;
    rbtree_ptr->count = 0;
}

static inline void JOIN(RBTREE_NAME, node_init)(RBTREE_NODE_TYPE* node_ptr, KEY_TYPE key, VALUE_TYPE value)
{
    assert(node_ptr != NULL);

    node_ptr->key = key;
    node_ptr->value = value;
}

static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, node_get_parent_ptr)(RBTREE_NODE_TYPE* node_ptr)
{
    return (RBTREE_NODE_TYPE*)(node_ptr->__parent_ptr_with_color & ~1);
}

static inline bool JOIN(RBTREE_NAME, node_is_black)(const RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    return (node_ptr->__parent_ptr_with_color & 1);
}

static inline bool JOIN(RBTREE_NAME, node_is_red)(const RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    return !JOIN(RBTREE_NAME, node_is_black)(node_ptr);
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

static inline void JOIN(internal, JOIN(RBTREE_NAME, node_set_parent_ptr))(RBTREE_NODE_TYPE* node_ptr, RBTREE_NODE_TYPE* parent_ptr)
{
    assert(node_ptr != NULL);

    const bool is_black = node_ptr->__parent_ptr_with_color & 1;
    node_ptr->__parent_ptr_with_color = (uintptr_t)parent_ptr;
    node_ptr->__parent_ptr_with_color += is_black;
}
/// @endcond

static inline bool JOIN(RBTREE_NAME, is_empty)(const RBTREE_TYPE* rbtree_ptr)
{
    assert(rbtree_ptr != NULL);

    return rbtree_ptr->root_ptr == NULL;
}

static inline bool JOIN(RBTREE_NAME, contains_key)(const RBTREE_TYPE* rbtree_ptr, const KEY_TYPE key)
{
    assert(rbtree_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = rbtree_ptr->root_ptr;

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

static inline VALUE_TYPE JOIN(RBTREE_NAME, get_value)(const RBTREE_TYPE* rbtree_ptr, const KEY_TYPE key, VALUE_TYPE default_value)
{
    assert(rbtree_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = rbtree_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_strictly_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_strictly_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_strictly_less && !is_strictly_greater) {
            return node_ptr->value;
        }
        else if (is_strictly_less) {
            node_ptr = node_ptr->left_ptr;
        }
        else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return default_value;
}

static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, search_node)(RBTREE_TYPE* rbtree_ptr, const KEY_TYPE key)
{
    assert(rbtree_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = rbtree_ptr->root_ptr;
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

static inline VALUE_TYPE* JOIN(RBTREE_NAME, get_value_mut)(RBTREE_TYPE* rbtree_ptr, const KEY_TYPE key)
{
    assert(rbtree_ptr != NULL);

    RBTREE_NODE_TYPE* node_ptr = JOIN(RBTREE_NAME, search_node)(rbtree_ptr, key);

    return (node_ptr != NULL) ? &node_ptr->value : NULL;
}

/// @cond DO_NOT_DOCUMENT

// rotate a subtree around a given subtree root node and direction (0: left or 1: right). returns the new subtree root
static inline RBTREE_NODE_TYPE* JOIN(internal, JOIN(RBTREE_NAME, rotate_dir))(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* P,
                                                                              const uint8_t dir)
{
    assert(rbtree_ptr != NULL);
    assert(P != NULL);
    assert(dir == 0 || dir == 1);

    /* Right rotate around P:
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
        rbtree_ptr->root_ptr = S;
    }
    return S;
}

// rebalance tree after insert. see explanation in the sources linked above.
static inline void JOIN(internal, JOIN(RBTREE_NAME, insert_fixup))(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* N)
{
    assert(rbtree_ptr != NULL);
    assert(N != NULL);
    assert(RBTREE_NODE_IS_RED(N));
    assert(N != rbtree_ptr->root_ptr);

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
                RBTREE_ROTATE_DIR(rbtree_ptr, P, dir);
                N = P;
                P = G->child_ptrs[dir];
            }
            RBTREE_NODE_SET_COLOR_TO_BLACK(P);
            RBTREE_NODE_SET_COLOR_TO_RED(G);
            RBTREE_ROTATE_DIR(rbtree_ptr, G, 1 - dir);
            return;
        }

        RBTREE_NODE_SET_COLOR_TO_BLACK(P);
        RBTREE_NODE_SET_COLOR_TO_BLACK(U);
        RBTREE_NODE_SET_COLOR_TO_RED(G);

        N = G;
        P = RBTREE_NODE_GET_PARENT_PTR(N);
    } while (P != NULL);
}

/// @endcond

static inline void JOIN(RBTREE_NAME, insert_node)(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* node_ptr)
{
    assert(rbtree_ptr != NULL);
    assert(RBTREE_CONTAINS_KEY(rbtree_ptr, node_ptr->key) == false);

    RBTREE_NODE_TYPE* parent_ptr = NULL;
    RBTREE_NODE_TYPE* current_ptr = rbtree_ptr->root_ptr;

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
        rbtree_ptr->root_ptr = node_ptr;
    }
    else {
        const uint8_t dir = KEY_IS_STRICTLY_LESS(parent_ptr->key, node_ptr->key);
        parent_ptr->child_ptrs[dir] = node_ptr;

        JOIN(internal, JOIN(RBTREE_NAME, insert_fixup))(rbtree_ptr, node_ptr);
    }

    rbtree_ptr->count++;
}

/// @cond DO_NOT_DOCUMENT

// find the node with the smallest key after node_ptr key (including node_ptr key)
static inline RBTREE_NODE_TYPE* JOIN(internal, JOIN(RBTREE_NAME, node_find_min))(RBTREE_NODE_TYPE* node_ptr)
{
    assert(node_ptr != NULL);

    while (node_ptr->left_ptr != NULL) {
        node_ptr = node_ptr->left_ptr;
    }
    return node_ptr;
}

// shift a new node onto the place of a old node. new_node may be NULL. children are not moved.
static inline void JOIN(internal, JOIN(RBTREE_NAME, node_shift))(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* old_node,
                                                                 RBTREE_NODE_TYPE* new_node)
{
    assert(rbtree_ptr != NULL);
    assert(old_node != NULL);

    RBTREE_NODE_TYPE* old_node_parent_ptr = RBTREE_NODE_GET_PARENT_PTR(old_node);

    if (old_node_parent_ptr == NULL) {
        rbtree_ptr->root_ptr = new_node;
    }
    else {
        old_node_parent_ptr->child_ptrs[RBTREE_CHILD_DIR(old_node)] = new_node;
    }

    if (new_node != NULL) {
        RBTREE_NODE_SET_PARENT_PTR(new_node, old_node_parent_ptr);
    }
}

// delete special* node and rebalance. see explanation in the sources linked above.
// *: node is not root and black and has no children
static inline void JOIN(internal, JOIN(RBTREE_NAME, delete_special))(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* N)
{
    assert(rbtree_ptr != NULL);
    assert(N != NULL);
    assert(N != rbtree_ptr->root_ptr);
    assert(RBTREE_NODE_IS_BLACK(N));
    assert(N->left_ptr == NULL && N->right_ptr == NULL);

    RBTREE_NODE_TYPE* P = RBTREE_NODE_GET_PARENT_PTR(N); // parent
    RBTREE_NODE_TYPE* S = NULL;                          // sibling
    RBTREE_NODE_TYPE* C = NULL;                          // close nephew
    RBTREE_NODE_TYPE* D = NULL;                          // distant nephew

    int dir = RBTREE_CHILD_DIR(N);
    P->child_ptrs[dir] = NULL;

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
    RBTREE_ROTATE_DIR(rbtree_ptr, P, dir);
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
    RBTREE_ROTATE_DIR(rbtree_ptr, S, 1 - dir);
    RBTREE_NODE_SET_COLOR_TO_RED(S);
    RBTREE_NODE_SET_COLOR_TO_BLACK(C);
    D = S;
    S = C;
Case_6:
    RBTREE_ROTATE_DIR(rbtree_ptr, P, dir);
    if (RBTREE_NODE_IS_RED(P)) {
        RBTREE_NODE_SET_COLOR_TO_RED(S);
    }
    else {
        RBTREE_NODE_SET_COLOR_TO_BLACK(S);
    }
    RBTREE_NODE_SET_COLOR_TO_BLACK(P);
    RBTREE_NODE_SET_COLOR_TO_BLACK(D);
    return;
}

/// @endcond

static inline RBTREE_NODE_TYPE* JOIN(RBTREE_NAME, delete_node)(RBTREE_TYPE* rbtree_ptr, RBTREE_NODE_TYPE* node_ptr)
{
    assert(rbtree_ptr != NULL);
    assert(node_ptr != NULL);

    RBTREE_NODE_TYPE* ret_ptr = node_ptr;

    if (node_ptr->left_ptr == NULL && node_ptr->right_ptr == NULL) {
        if (node_ptr == rbtree_ptr->root_ptr || RBTREE_NODE_IS_RED(node_ptr)) {
            RBTREE_NODE_SHIFT(rbtree_ptr, node_ptr, NULL);
        }
        else {
            JOIN(internal, JOIN(RBTREE_NAME, delete_special))(rbtree_ptr, node_ptr);
        }
    }
    else if (node_ptr->left_ptr == NULL || node_ptr->right_ptr == NULL) {
        const uint8_t dir = node_ptr->left_ptr == NULL;

        assert(RBTREE_NODE_IS_BLACK(node_ptr));
        assert(RBTREE_NODE_IS_RED(node_ptr->child_ptrs[dir]));

        RBTREE_NODE_SHIFT(rbtree_ptr, node_ptr, node_ptr->child_ptrs[dir]);

        RBTREE_NODE_SET_COLOR_TO_BLACK(node_ptr->child_ptrs[dir]);
    }
    else {
        RBTREE_NODE_TYPE* successor_ptr = JOIN(internal, JOIN(RBTREE_NAME, node_find_min)(node_ptr->right_ptr));

        {
            const KEY_TYPE temp_key = successor_ptr->key;
            node_ptr->key = successor_ptr->key;
            successor_ptr->key = temp_key;
        }
        {
            const VALUE_TYPE temp_value = successor_ptr->value;
            node_ptr->value = successor_ptr->value;
            successor_ptr->value = temp_value;
        }

        ret_ptr = successor_ptr;

        if (successor_ptr->right_ptr == NULL && RBTREE_NODE_IS_BLACK(successor_ptr)) {
            JOIN(internal, JOIN(RBTREE_NAME, delete_special))(rbtree_ptr, successor_ptr);
        }
        else {
            if (successor_ptr->right_ptr) {
                assert(RBTREE_NODE_IS_BLACK(successor_ptr));
                assert(RBTREE_NODE_IS_RED(successor_ptr->right_ptr));

                RBTREE_NODE_SHIFT(rbtree_ptr, successor_ptr, successor_ptr->right_ptr);

                RBTREE_NODE_SET_COLOR_TO_BLACK(successor_ptr->right_ptr);
            }
            else {
                RBTREE_NODE_SHIFT(rbtree_ptr, successor_ptr, NULL);
            }
        }
    }

    ret_ptr->left_ptr = ret_ptr->right_ptr = NULL;
    RBTREE_NODE_SET_PARENT_PTR(ret_ptr, NULL);

    rbtree_ptr->count--;

    return ret_ptr;
}

static inline void JOIN(RBTREE_NAME, clear)(RBTREE_TYPE* rbtree_ptr)
{
    assert(rbtree_ptr != NULL);

    rbtree_ptr->root_ptr = NULL;
}

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
#undef RBTREE_NODE_SHIFT
#undef RBTREE_NODE_SET_COLOR_TO_RED
#undef RBTREE_NODE_SET_COLOR_TO_BLACK
#undef RBTREE_NODE_SET_PARENT_PTR
#undef RBTREE_CHILD_DIR

// vim: ft=c

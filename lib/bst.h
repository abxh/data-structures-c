
// inspiration:
// https://github.com/torvalds/linux/blob/master/include/linux/rbtree_augmented.h

// sources used:
// https://en.wikipedia.org/wiki/Binary_search_tree#Operations
// Michael Sambol - red-black tree playlist
// https://en.wikipedia.org/wiki/Red-black_tree#Operations

#include "paste.h" // PASTE, XPASTE, JOIN

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef NAME
#error "Must define NAME."
#define NAME bst
#else
#define BST_NAME NAME
#endif

#ifndef KEY_TYPE
#error "Must define KEY_TYPE."
#define KEY_TYPE int
#endif

#ifndef VALUE_TYPE
#error "Must define VALUE_TYPE."
#define VALUE_TYPE int
#endif

#ifndef KEY_IS_STRICTLY_LESS
#error "Must define KEY_IS_STRICTLY_LESS."
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#endif

static_assert(sizeof(unsigned long) == sizeof(void*), "unsigned long size is address size");

#define BST_TYPE JOIN(BST_NAME, type)
#define BST_NODE_TYPE JOIN(BST_NAME, node_type)

/// @cond DO_NOT_DOCUMENT
#define BST_CONTAINS_KEY JOIN(BST_NAME, contains_key)
#define BST_NODE_IS_RED JOIN(BST_NAME, node_is_red)
#define BST_NODE_IS_BLACK JOIN(BST_NAME, node_is_black)
#define BST_NODE_GET_PARENT_PTR JOIN(BST_NAME, node_get_parent_ptr)
#define BST_NODE_SHIFT JOIN(internal, JOIN(BST_NAME, node_shift))
#define BST_NODE_SET_COLOR_TO_RED JOIN(internal, JOIN(BST_NAME, node_set_color_to_red))
#define BST_NODE_SET_COLOR_TO_BLACK JOIN(internal, JOIN(BST_NAME, node_set_color_to_black))
#define BST_NODE_SET_PARENT_PTR JOIN(internal, JOIN(BST_NAME, node_set_parent_ptr))

#define BST_CHILD_DIR(node_ptr) ((node_ptr) == BST_NODE_GET_PARENT_PTR(node_ptr)->left_ptr ? 0 : 1)
/// @endcond

typedef struct BST_NODE_TYPE {
    unsigned long __parent_ptr_with_color;
    union {
        struct {
            struct BST_NODE_TYPE* left_ptr;
            struct BST_NODE_TYPE* right_ptr;
        };
        struct BST_NODE_TYPE* child_ptrs[2];
    };
    KEY_TYPE key;
    VALUE_TYPE value;
} BST_NODE_TYPE;

typedef struct BST_TYPE {
    BST_NODE_TYPE* root_ptr;
} BST_TYPE;

static inline void JOIN(BST_NAME, init)(BST_TYPE* bst_ptr) {
    assert(bst_ptr != NULL);

    bst_ptr->root_ptr = NULL;
}

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(BST_NAME, node_set_color_to_red))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);
    node_ptr->__parent_ptr_with_color &= ~1;
}

static inline void JOIN(internal, JOIN(BST_NAME, node_set_color_to_black))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);
    node_ptr->__parent_ptr_with_color |= 1;
}

static inline void JOIN(internal, JOIN(BST_NAME, node_set_parent_ptr))(BST_NODE_TYPE* node_ptr, BST_NODE_TYPE* parent_ptr) {
    assert(node_ptr != NULL);

    const bool is_black = node_ptr->__parent_ptr_with_color & 1;

    node_ptr->__parent_ptr_with_color = (unsigned long)parent_ptr;
    node_ptr->__parent_ptr_with_color += is_black;
}
/// @endcond

static inline void JOIN(BST_NAME, node_init)(BST_NODE_TYPE* node_ptr, KEY_TYPE key, VALUE_TYPE value) {
    assert(node_ptr != NULL);

    node_ptr->left_ptr = NULL;
    node_ptr->right_ptr = NULL;
    node_ptr->key = key;
    node_ptr->value = value;

    BST_NODE_SET_COLOR_TO_RED(node_ptr);
    BST_NODE_SET_PARENT_PTR(node_ptr, NULL);
}

static inline BST_NODE_TYPE* JOIN(BST_NAME, node_get_parent_ptr)(BST_NODE_TYPE* node_ptr) {
    return (BST_NODE_TYPE*)(node_ptr->__parent_ptr_with_color & ~1);
}

static inline bool JOIN(BST_NAME, node_is_black)(const BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    return (node_ptr->__parent_ptr_with_color & 1);
}

static inline bool JOIN(BST_NAME, node_is_red)(const BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    return !JOIN(BST_NAME, node_is_black)(node_ptr);
}

static inline bool JOIN(BST_NAME, is_empty)(const BST_TYPE* bst_ptr) {
    assert(bst_ptr != NULL);

    return bst_ptr->root_ptr == NULL;
}

static inline bool JOIN(BST_NAME, contains_key)(const BST_TYPE* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_less && !is_greater) {
            return true;
        } else if (is_less) {
            node_ptr = node_ptr->left_ptr;
        } else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return false;
}

static inline VALUE_TYPE JOIN(BST_NAME, get_value)(const BST_TYPE* bst_ptr, const KEY_TYPE key, VALUE_TYPE default_value) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_strictly_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_strictly_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_strictly_less && !is_strictly_greater) {
            return node_ptr->value;
        } else if (is_strictly_less) {
            node_ptr = node_ptr->left_ptr;
        } else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return default_value;
}

static inline BST_NODE_TYPE* JOIN(BST_NAME, search_node)(BST_TYPE* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

        if (!is_less && !is_greater) {
            return node_ptr;
        } else if (is_less) {
            node_ptr = node_ptr->left_ptr;
        } else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return NULL;
}

static inline VALUE_TYPE* JOIN(BST_NAME, get_value_mut)(BST_TYPE* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = JOIN(BST_NAME, search_node)(bst_ptr, key);

    return (node_ptr != NULL) ? &node_ptr->value : NULL;
}

/// @cond DO_NOT_DOCUMENT

// rotate a subtree around a given subtree root node and direction (0: left or 1: right). returns the new subtree root
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, rotate_dir))(BST_TYPE* bst_ptr, BST_NODE_TYPE* P, const int dir) {
    assert(bst_ptr != NULL);
    assert(P != NULL);
    assert(dir == 0 || dir == 1);

    // Rotation is allowed, when there is a "triangle pattern":
    //   P     (right)    S
    // S/ \T     ⟳         \P
    //  \C               C/ \T

    //   P     (left)     S
    // T/ \S     ↺      P/
    //   C/            T/\C

    BST_NODE_TYPE* G = BST_NODE_GET_PARENT_PTR(P);
    BST_NODE_TYPE* S = P->child_ptrs[!dir];
    assert(S != NULL);
    BST_NODE_TYPE* C = S->child_ptrs[dir];

    P->child_ptrs[!dir] = C;
    if (C != NULL) {
        BST_NODE_SET_PARENT_PTR(C, P);
    }

    S->child_ptrs[dir] = P;
    BST_NODE_SET_PARENT_PTR(P, S);

    BST_NODE_SET_PARENT_PTR(S, G);
    if (G != NULL) {
        G->child_ptrs[P == G->left_ptr ? 0 : 1] = S;
    } else {
        bst_ptr->root_ptr = S;
    }
    return S;
}

// rebalance tree after insert
static inline void JOIN(internal, JOIN(BST_NAME, insert_fixup))(BST_TYPE* bst_ptr, BST_NODE_TYPE* N, BST_NODE_TYPE* P, int dir) {
    // See following for well-explained code:
    // https://en.wikipedia.org/wiki/Red-black_tree#Operations
    // And following for a intuitive explanation:
    // https://www.youtube.com/watch?v=5IBxA-bZZH8

    assert(bst_ptr != NULL);
    assert(P->left_ptr == N || P->right_ptr == N);
    assert(dir == 0 || dir == 1);
    assert(P != NULL);

    BST_NODE_TYPE* G;
    BST_NODE_TYPE* U;

    P->child_ptrs[dir] = N;
    do {
        if (BST_NODE_IS_BLACK(P)) {
            return;
        }
        if ((G = BST_NODE_GET_PARENT_PTR(P)) == NULL) {
            BST_NODE_SET_COLOR_TO_BLACK(P);
            return;
        }
        dir = BST_CHILD_DIR(P);
        U = G->child_ptrs[!dir];
        if (U == NULL || BST_NODE_IS_BLACK(U)) {
            if (N == P->child_ptrs[!dir]) {
                JOIN(internal, JOIN(BST_NAME, rotate_dir))(bst_ptr, P, dir);
                N = P;
                P = G->child_ptrs[dir];
            }
            JOIN(internal, JOIN(BST_NAME, rotate_dir))(bst_ptr, G, !dir);
            BST_NODE_SET_COLOR_TO_BLACK(P);
            BST_NODE_SET_COLOR_TO_RED(G);
            return;
        }
        BST_NODE_SET_COLOR_TO_BLACK(P);
        BST_NODE_SET_COLOR_TO_BLACK(U);
        BST_NODE_SET_COLOR_TO_RED(G);
        N = G;
    } while ((P = BST_NODE_GET_PARENT_PTR(N)) != NULL);
}

/// @endcond

static inline void JOIN(BST_NAME, insert_node)(BST_TYPE* bst_ptr, BST_NODE_TYPE* node_ptr) {
    assert(bst_ptr != NULL);
    assert(BST_CONTAINS_KEY(bst_ptr, node_ptr->key) == false);
    assert(BST_NODE_IS_RED(node_ptr) && node_ptr->left_ptr == NULL && node_ptr->right_ptr == NULL && "must initialize node");

    BST_NODE_TYPE* parent_ptr = NULL;
    BST_NODE_TYPE* current_ptr = bst_ptr->root_ptr;

    while (current_ptr != NULL) {
        parent_ptr = current_ptr;

        if (KEY_IS_STRICTLY_LESS(node_ptr->key, current_ptr->key)) {
            current_ptr = current_ptr->left_ptr;
        } else {
            current_ptr = current_ptr->right_ptr;
        }
    }

    BST_NODE_SET_PARENT_PTR(node_ptr, parent_ptr);

    if (parent_ptr == NULL) {
        bst_ptr->root_ptr = node_ptr;

    } else if (KEY_IS_STRICTLY_LESS(node_ptr->key, parent_ptr->key)) {
        parent_ptr->left_ptr = node_ptr;
        JOIN(internal, JOIN(BST_NAME, insert_fixup))(bst_ptr, node_ptr, parent_ptr, 0);

    } else {
        parent_ptr->right_ptr = node_ptr;
        JOIN(internal, JOIN(BST_NAME, insert_fixup))(bst_ptr, node_ptr, parent_ptr, 1);
    }
}

/// @cond DO_NOT_DOCUMENT

// find the node with the smallest key after node_ptr key (including node_ptr key)
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, node_find_min))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    while (node_ptr->left_ptr != NULL) {
        node_ptr = node_ptr->left_ptr;
    }
    return node_ptr;
}

// shift a new node onto the place of a old node. new_node may be NULL. children are not moved.
static inline void JOIN(internal, JOIN(BST_NAME, node_shift))(BST_TYPE* bst_ptr, BST_NODE_TYPE* old_node, BST_NODE_TYPE* new_node) {
    assert(bst_ptr != NULL);
    assert(old_node != NULL);
    assert(new_node == NULL || new_node != NULL);

    BST_NODE_TYPE* old_node_parent_ptr = BST_NODE_GET_PARENT_PTR(old_node);

    if (old_node_parent_ptr == NULL) {
        bst_ptr->root_ptr = new_node;
    } else {
        old_node_parent_ptr->child_ptrs[BST_CHILD_DIR(old_node)] = new_node;
    }

    if (new_node != NULL) {
        BST_NODE_SET_PARENT_PTR(new_node, old_node_parent_ptr);
    }
}

/// @endcond

BST_NODE_TYPE* JOIN(BST_NAME, delete_node)(BST_TYPE* bst_ptr, BST_NODE_TYPE* node_ptr) {
    assert(bst_ptr != NULL);
    assert(node_ptr != NULL);

    if (node_ptr->left_ptr == NULL) {
        BST_NODE_SHIFT(bst_ptr, node_ptr, node_ptr->right_ptr);

    } else if (node_ptr->right_ptr == NULL) {
        BST_NODE_SHIFT(bst_ptr, node_ptr, node_ptr->left_ptr);

    } else {
        BST_NODE_TYPE* successor_ptr = JOIN(internal, JOIN(BST_NAME, node_find_min)(node_ptr->right_ptr));

        if (BST_NODE_GET_PARENT_PTR(successor_ptr) != node_ptr) {
            BST_NODE_SHIFT(bst_ptr, successor_ptr, successor_ptr->right_ptr);

            successor_ptr->right_ptr = node_ptr->right_ptr;
            BST_NODE_SET_PARENT_PTR(node_ptr->right_ptr, successor_ptr);
        }

        BST_NODE_SHIFT(bst_ptr, node_ptr, successor_ptr);

        successor_ptr->left_ptr = node_ptr->left_ptr;
        BST_NODE_SET_PARENT_PTR(node_ptr->left_ptr, successor_ptr);
    }

    node_ptr->left_ptr = node_ptr->right_ptr = NULL;
    BST_NODE_SET_PARENT_PTR(node_ptr, NULL);

    return node_ptr;
}

void JOIN(BST_NAME, clear)(BST_TYPE* bst_ptr) {
    assert(bst_ptr != NULL);

    bst_ptr->root_ptr = NULL;
}

#undef NAME
#undef KEY_TYPE
#undef VALUE_TYPE
#undef KEY_IS_STRICTLY_LESS

#undef BST_TYPE
#undef BST_NAME
#undef BST_NODE_TYPE

#undef BST_CONTAINS_KEY
#undef BST_NODE_IS_RED
#undef BST_NODE_IS_BLACK
#undef BST_NODE_GET_PARENT_PTR
#undef BST_NODE_SHIFT
#undef BST_NODE_SET_COLOR
#undef BST_NODE_SET_PARENT_PTR

#undef BST_CHILD_DIR
#undef BST_DIR_LEFT
#undef BST_DIR_RIGHT

// vim: ft=c
// Michael Sambol - red-black tree playlist

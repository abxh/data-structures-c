
// sources used:
// https://en.wikipedia.org/wiki/Binary_search_tree#Operations

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef NAME
#define NAME bst
#error "Must define NAME."
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

#ifndef KEY_IS_LESS
#error "Must define KEY_IS_LESS."
#define KEY_IS_LESS(a, b) ((a) < (b))
#endif

static_assert(sizeof(unsigned long) == sizeof(void*), "unsigned long size is address size");

typedef struct bst_node_type {
    unsigned long __parent_ptr_color;
    struct bst_node_type* left_ptr;
    struct bst_node_type* right_ptr;
    KEY_TYPE key;
    VALUE_TYPE value;
} bst_node_type;

typedef struct bst_type {
    bst_node_type* root_ptr;
} bst_type;

static inline void bst_init(bst_type* bst_ptr) {
    assert(bst_ptr != NULL);

    bst_ptr->root_ptr = NULL;
}

/// @cond DO_NOT_DOCUMENT
// set the color of a node
static inline void internal_bst_node_set_color(bst_node_type* node_ptr, const bool is_black) {
    assert(node_ptr != NULL);

    node_ptr->__parent_ptr_color &= ~1;
    node_ptr->__parent_ptr_color += is_black;
}

static inline void internal_bst_node_set_parent(bst_node_type* node_ptr, bst_node_type* parent_ptr) {
    assert(node_ptr != NULL);

    const bool is_black = ((unsigned long)node_ptr->__parent_ptr_color) & 1;
    node_ptr->__parent_ptr_color = (unsigned long)parent_ptr;
    node_ptr->__parent_ptr_color += is_black;
}
/// @endcond

static inline void bst_node_init(bst_node_type* node_ptr, KEY_TYPE key, VALUE_TYPE value) {
    assert(node_ptr != NULL);

    node_ptr->left_ptr = NULL;
    node_ptr->right_ptr = NULL;
    node_ptr->key = key;
    node_ptr->value = value;
    internal_bst_node_set_color(node_ptr, false);
    internal_bst_node_set_parent(node_ptr, NULL);
}

static inline bst_node_type* bst_node_parent_ptr(bst_node_type* node_ptr) {
    return (bst_node_type*)(node_ptr->__parent_ptr_color & ~1);
}

static inline bool bst_node_is_black(const bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    return (bool)(node_ptr->__parent_ptr_color & 1);
}

static inline bool bst_node_is_red(const bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    return !bst_node_is_black(node_ptr);
}

static inline bool bst_is_empty(const bst_type* bst_ptr) {
    assert(bst_ptr != NULL);

    return bst_ptr->root_ptr == NULL;
}

static inline bool bst_contains_key(const bst_type* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    bst_node_type* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const KEY_TYPE current_key = node_ptr->key;
        const bool is_less = KEY_IS_LESS(key, current_key);
        const bool is_greater = KEY_IS_LESS(current_key, key);

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

static inline VALUE_TYPE bst_get_value(const bst_type* bst_ptr, const KEY_TYPE key, VALUE_TYPE default_value) {
    assert(bst_ptr != NULL);

    bst_node_type* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const KEY_TYPE current_key = node_ptr->key;
        const bool is_less = KEY_IS_LESS(key, current_key);
        const bool is_greater = KEY_IS_LESS(current_key, key);

        if (!is_less && !is_greater) {
            return node_ptr->value;
        } else if (is_less) {
            node_ptr = node_ptr->left_ptr;
        } else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return default_value;
}

static inline VALUE_TYPE* bst_get_value_mut(bst_type* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    bst_node_type* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_less = KEY_IS_LESS(key, node_ptr->key);
        const bool is_greater = KEY_IS_LESS(node_ptr->key, key);

        if (!is_less && !is_greater) {
            return &node_ptr->value;
        } else if (is_less) {
            node_ptr = node_ptr->left_ptr;
        } else {
            node_ptr = node_ptr->right_ptr;
        }
    }
    return NULL;
}

static inline bst_node_type* bst_search_node(bst_type* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    bst_node_type* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_less = KEY_IS_LESS(key, node_ptr->key);
        const bool is_greater = KEY_IS_LESS(node_ptr->key, key);

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

static inline void bst_insert_node(bst_type* bst_ptr, bst_node_type* node_ptr) {
    assert(bst_ptr != NULL);
    assert(bst_contains_key(bst_ptr, node_ptr->key) == false);
    assert(node_ptr->left_ptr == NULL && node_ptr->right_ptr == NULL && "must initialize node");

    bst_node_type* parent_ptr = NULL;
    bst_node_type* current_ptr = bst_ptr->root_ptr;

    while (current_ptr != NULL) {
        parent_ptr = current_ptr;

        if (KEY_IS_LESS(node_ptr->key, current_ptr->key)) {
            current_ptr = current_ptr->left_ptr;
        } else {
            current_ptr = current_ptr->right_ptr;
        }
    }

    internal_bst_node_set_parent(node_ptr, parent_ptr);

    if (parent_ptr == NULL) {
        bst_ptr->root_ptr = node_ptr;
    } else if (KEY_IS_LESS(node_ptr->key, parent_ptr->key)) {
        parent_ptr->left_ptr = node_ptr;
    } else {
        parent_ptr->right_ptr = node_ptr;
    }
}

/// @cond DO_NOT_DOCUMENT

// find the node with the largest key after node_ptr
static inline bst_node_type* internal_bst_node_find_max_node(bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    while (node_ptr->right_ptr != NULL) {
        node_ptr = node_ptr->right_ptr;
    }
    return node_ptr;
}

// find the node with the smallest key after node_ptr
static inline bst_node_type* internal_bst_node_find_min_node(bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    while (node_ptr->left_ptr != NULL) {
        node_ptr = node_ptr->left_ptr;
    }
    return node_ptr;
}

// find the node with the smallest key larger than node_ptr key
static inline bst_node_type* internal_bst_node_successor(bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    if (node_ptr->right_ptr != NULL) {
        return internal_bst_node_find_min_node(node_ptr->right_ptr);
    }
    bst_node_type* parent_ptr = bst_node_parent_ptr(node_ptr);
    while (parent_ptr != NULL && node_ptr == parent_ptr->right_ptr) {
        node_ptr = parent_ptr;
        parent_ptr = bst_node_parent_ptr(parent_ptr);
    }
    return parent_ptr;
}

// find the node with the largest key smaller than node_ptr key
static inline bst_node_type* internal_bst_node_predecessor(bst_node_type* node_ptr) {
    assert(node_ptr != NULL);

    if (node_ptr->left_ptr != NULL) {
        return internal_bst_node_find_max_node(node_ptr->left_ptr);
    }
    bst_node_type* parent_ptr = bst_node_parent_ptr(node_ptr);
    while (parent_ptr != NULL && node_ptr == parent_ptr->left_ptr) {
        node_ptr = parent_ptr;
        parent_ptr = bst_node_parent_ptr(parent_ptr);
    }
    return parent_ptr;
}

static inline void internal_bst_node_shift_nodes(bst_type* bst_ptr, bst_node_type* u_ptr, bst_node_type* v_ptr) {
    assert(bst_ptr != NULL);
    assert(u_ptr != NULL);
    assert(v_ptr != NULL);

    if (bst_node_parent_ptr(u_ptr) == NULL) {
        bst_ptr->root_ptr = u_ptr;
    } else if (u_ptr == bst_node_parent_ptr(u_ptr)->left_ptr) {
        bst_node_parent_ptr(u_ptr)->left_ptr = v_ptr;
    } else if (u_ptr == bst_node_parent_ptr(u_ptr)->right_ptr) {
        bst_node_parent_ptr(u_ptr)->right_ptr = v_ptr;
    }
    if (v_ptr != NULL) {
        internal_bst_node_set_parent(v_ptr, bst_node_parent_ptr(u_ptr));
    }
}
/// @endcond

void bst_delete(bst_type* bst_ptr, bst_node_type* node_ptr) {
    if (node_ptr->left_ptr == NULL) {
        internal_bst_node_shift_nodes(bst_ptr, node_ptr, node_ptr->right_ptr);
    } else if (node_ptr->left_ptr == NULL) {
        internal_bst_node_shift_nodes(bst_ptr, node_ptr, node_ptr->left_ptr);
    } else {
        bst_node_type* successor_ptr = internal_bst_node_successor(node_ptr);

        if (bst_node_parent_ptr(successor_ptr) != node_ptr) {
            internal_bst_node_shift_nodes(bst_ptr, successor_ptr, successor_ptr->right_ptr);

            successor_ptr->right_ptr = node_ptr->right_ptr;
            internal_bst_node_set_parent(node_ptr->right_ptr, successor_ptr);
        }

        internal_bst_node_shift_nodes(bst_ptr, node_ptr, successor_ptr);

        successor_ptr->left_ptr = node_ptr->left_ptr;
        internal_bst_node_set_parent(node_ptr->left_ptr, successor_ptr);
    }
}

// vim: ft=c

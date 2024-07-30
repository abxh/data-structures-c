
// inspiration:
// https://github.com/torvalds/linux/blob/master/include/linux/rbtree_augmented.h

// sources used:
// https://en.wikipedia.org/wiki/Binary_search_tree#Operations

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
#define BST_NODE_SET_COLOR JOIN(internal, JOIN(BST_NAME, node_set_color))
#define BST_NODE_SET_PARENT JOIN(internal, JOIN(BST_NAME, node_set_parent))
#define BST_NODE_PARENT_PTR JOIN(BST_NAME, node_parent_ptr)
#define BST_NODE_REPLACE_NODE JOIN(internal, JOIN(BST_NAME, node_replace_node))
#define BST_NODE_FIND_MIN JOIN(internal, JOIN(BST_NAME, node_find_min))
#define BST_NODE_FIND_MAX JOIN(internal, JOIN(BST_NAME, node_find_max))
#define BST_NODE_SUCCESSOR JOIN(internal, JOIN(BST_NAME, node_successor))
/// @endcond

typedef struct BST_NODE_TYPE {
    unsigned long __parent_ptr_color;
    struct BST_NODE_TYPE* left_ptr;
    struct BST_NODE_TYPE* right_ptr;
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

// set the color of a node
static inline void JOIN(internal, JOIN(BST_NAME, node_set_color))(BST_NODE_TYPE* node_ptr, const bool is_black) {
    assert(node_ptr != NULL);

    node_ptr->__parent_ptr_color &= ~1;
    node_ptr->__parent_ptr_color += is_black;
}

// set the parent of a node
static inline void JOIN(internal, JOIN(BST_NAME, node_set_parent))(BST_NODE_TYPE* node_ptr, BST_NODE_TYPE* parent_ptr) {
    assert(node_ptr != NULL);

    const bool is_black = ((unsigned long)node_ptr->__parent_ptr_color) & 1;
    node_ptr->__parent_ptr_color = (unsigned long)parent_ptr;
    node_ptr->__parent_ptr_color += is_black;
}
/// @endcond

static inline void JOIN(BST_NAME, node_init)(BST_NODE_TYPE* node_ptr, KEY_TYPE key, VALUE_TYPE value) {
    assert(node_ptr != NULL);

    node_ptr->left_ptr = NULL;
    node_ptr->right_ptr = NULL;
    node_ptr->key = key;
    node_ptr->value = value;
    BST_NODE_SET_COLOR(node_ptr, false);
    BST_NODE_SET_PARENT(node_ptr, NULL);
}

static inline BST_NODE_TYPE* JOIN(BST_NAME, node_parent_ptr)(BST_NODE_TYPE* node_ptr) {
    return (BST_NODE_TYPE*)(node_ptr->__parent_ptr_color & ~1);
}

static inline bool JOIN(BST_NAME, node_is_black)(const BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    return (node_ptr->__parent_ptr_color & 1);
}

static inline bool JOIN(BST_NAME, node_is_red)(const BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    return !(node_ptr->__parent_ptr_color & 1);
}

static inline bool JOIN(BST_NAME, is_empty)(const BST_TYPE* bst_ptr) {
    assert(bst_ptr != NULL);

    return bst_ptr->root_ptr == NULL;
}

static inline bool JOIN(BST_NAME, contains_key)(const BST_TYPE* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const KEY_TYPE current_key = node_ptr->key;
        const bool is_less = KEY_IS_STRICTLY_LESS(key, current_key);
        const bool is_greater = KEY_IS_STRICTLY_LESS(current_key, key);

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
        const KEY_TYPE current_key = node_ptr->key;
        const bool is_strictly_less = KEY_IS_STRICTLY_LESS(key, current_key);
        const bool is_strictly_greater = KEY_IS_STRICTLY_LESS(current_key, key);

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

static inline VALUE_TYPE* JOIN(BST_NAME, get_value_mut)(BST_TYPE* bst_ptr, const KEY_TYPE key) {
    assert(bst_ptr != NULL);

    BST_NODE_TYPE* node_ptr = bst_ptr->root_ptr;
    while (node_ptr != NULL) {
        const bool is_less = KEY_IS_STRICTLY_LESS(key, node_ptr->key);
        const bool is_greater = KEY_IS_STRICTLY_LESS(node_ptr->key, key);

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

static inline void JOIN(BST_NAME, insert_node)(BST_TYPE* bst_ptr, BST_NODE_TYPE* node_ptr) {
    assert(bst_ptr != NULL);
    assert(BST_CONTAINS_KEY(bst_ptr, node_ptr->key) == false);
    assert(node_ptr->left_ptr == NULL && node_ptr->right_ptr == NULL && "must initialize node");

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

    BST_NODE_SET_PARENT(node_ptr, parent_ptr);

    if (parent_ptr == NULL) {
        bst_ptr->root_ptr = node_ptr;
    } else if (KEY_IS_STRICTLY_LESS(node_ptr->key, parent_ptr->key)) {
        parent_ptr->left_ptr = node_ptr;
    } else {
        parent_ptr->right_ptr = node_ptr;
    }
}

/// @cond DO_NOT_DOCUMENT

// find the node with the largest key after node_ptr
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, node_find_max))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    while (node_ptr->right_ptr != NULL) {
        node_ptr = node_ptr->right_ptr;
    }
    return node_ptr;
}

// find the node with the smallest key after node_ptr
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, node_find_min))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    while (node_ptr->left_ptr != NULL) {
        node_ptr = node_ptr->left_ptr;
    }
    return node_ptr;
}

// find the node with the smallest key larger than node_ptr key
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, node_successor))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    if (node_ptr->right_ptr != NULL) {
        return BST_NODE_FIND_MIN(node_ptr->right_ptr);
    }
    BST_NODE_TYPE* parent_ptr = BST_NODE_PARENT_PTR(node_ptr);
    while (parent_ptr != NULL && node_ptr == parent_ptr->right_ptr) {
        node_ptr = parent_ptr;
        parent_ptr = BST_NODE_PARENT_PTR(parent_ptr);
    }
    return parent_ptr;
}

// find the node with the largest key smaller than node_ptr key
static inline BST_NODE_TYPE* JOIN(internal, JOIN(BST_NAME, node_predecessor))(BST_NODE_TYPE* node_ptr) {
    assert(node_ptr != NULL);

    if (node_ptr->left_ptr != NULL) {
        return BST_NODE_FIND_MAX(node_ptr->left_ptr);
    }
    BST_NODE_TYPE* parent_ptr = BST_NODE_PARENT_PTR(node_ptr);
    while (parent_ptr != NULL && node_ptr == parent_ptr->left_ptr) {
        node_ptr = parent_ptr;
        parent_ptr = BST_NODE_PARENT_PTR(parent_ptr);
    }
    return parent_ptr;
}

// replace a given node with a new node (new NULL node is allowed)
static inline void JOIN(internal, JOIN(BST_NAME, node_replace_node))(BST_TYPE* bst_ptr, BST_NODE_TYPE* old_node,
                                                                     BST_NODE_TYPE* new_node) {
    assert(bst_ptr != NULL);
    assert(old_node != NULL);

    if (BST_NODE_PARENT_PTR(old_node) == NULL) {
        bst_ptr->root_ptr = old_node;

    } else if (old_node == BST_NODE_PARENT_PTR(old_node)->left_ptr) {
        BST_NODE_PARENT_PTR(old_node)->left_ptr = new_node;

    } else if (old_node == BST_NODE_PARENT_PTR(old_node)->right_ptr) {
        BST_NODE_PARENT_PTR(old_node)->right_ptr = new_node;
    }
    if (new_node != NULL) {
        BST_NODE_SET_PARENT(new_node, BST_NODE_PARENT_PTR(old_node));
    }
}
/// @endcond

BST_NODE_TYPE* JOIN(BST_NAME, delete_node)(BST_TYPE* bst_ptr, BST_NODE_TYPE* node_ptr) {
    assert(bst_ptr != NULL);
    assert(node_ptr != NULL);

    if (node_ptr->left_ptr == NULL) {
        BST_NODE_REPLACE_NODE(bst_ptr, node_ptr, node_ptr->right_ptr); // NULL right_ptr is allowed

    } else if (node_ptr->left_ptr == NULL) {
        BST_NODE_REPLACE_NODE(bst_ptr, node_ptr, node_ptr->left_ptr); // NULL left_ptr is allowed

    } else {
        BST_NODE_TYPE* successor_ptr = BST_NODE_SUCCESSOR(node_ptr);

        // note: the successor is the node with the smallest key larger than node key
        //       that means successor does *not* have a left node.

        if (BST_NODE_PARENT_PTR(successor_ptr) != node_ptr) {
            BST_NODE_REPLACE_NODE(bst_ptr, successor_ptr, successor_ptr->right_ptr); // NULL right_ptr is allowed

            successor_ptr->right_ptr = node_ptr->right_ptr;
            BST_NODE_SET_PARENT(node_ptr->right_ptr, successor_ptr);
        }

        assert(successor_ptr != NULL);

        BST_NODE_REPLACE_NODE(bst_ptr, node_ptr, successor_ptr);

        successor_ptr->left_ptr = node_ptr->left_ptr;
        BST_NODE_SET_PARENT(node_ptr->left_ptr, successor_ptr);
    }

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
#undef BST_NODE_SET_COLOR
#undef BST_NODE_SET_PARENT
#undef BST_NODE_PARENT_PTR
#undef BST_NODE_REPLACE_NODE
#undef BST_NODE_FIND_MIN
#undef BST_NODE_FIND_MAX
#undef BST_NODE_SUCCESSOR

// vim: ft=c

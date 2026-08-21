// Copyright (c) 2026 abxh
// SPDX-License-Identifier: MIT

/**
 * @file list_template.h
 * @brief Intrusive circular doubly linked list
 *
 * Inspired by:
 * @li https://github.com/torvalds/linux/blob/master/include/linux/list.h
 */

/**
 * @example list_example.c
 * Example of how `list_template.h` header file is used in practice.
 */

#ifdef __cplusplus
#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

// macro definitions: {{{

/**
 * @def PASTE(a,b)
 * @brief Paste two tokens together.
 */
#ifndef PASTE
#define PASTE(a, b) a##b
#endif

/**
 * @def XPASTE(a,b)
 * @brief First expand tokens, then paste them together.
 */
#ifndef XPASTE
#define XPASTE(a, b) PASTE(a, b)
#endif

/**
 * @def JOIN(a,b)
 * @brief First expand tokens, then paste them together with a _ in between.
 */
#ifndef JOIN
#define JOIN(a, b) XPASTE(a, XPASTE(_, b))
#endif

/**
 * @def NAME
 * @brief Prefix to list node types and operations. This must be manually
 *        defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#error "Must define NAME."
#define FUNCTION_DEFINITIONS
#define TYPE_DEFINITIONS
#else
#define LIST_NAME NAME
#endif

/**
 * @def FUNCTION_LINKAGE
 * @brief Specify function linkage e.g. static inline
 */
#ifndef FUNCTION_LINKAGE
#define FUNCTION_LINKAGE
#endif

/// @cond DO_NOT_DOCUMENT
#define LIST_NODE_TYPE        struct JOIN(LIST_NAME, node)
#define LIST_NODE_ADD_BETWEEN JOIN(JOIN(internal, LIST_NAME), node_add_between)
#define LIST_NODE_ATTACH      JOIN(JOIN(internal, LIST_NAME), node_attach)
#define LIST_NODE_INIT        JOIN(LIST_NAME, node_init)
#define LIST_NODE_IS_SINGULAR JOIN(LIST_NAME, node_is_singular)
/// @endcond

/**
 * @def LIST_FOR_EACH
 * @brief Iterate over a list.
 *
 * @param node_ptr              Current node pointer.
 * @param head_ptr              List head pointer.
 */
#ifndef LIST_FOR_EACH
#define LIST_FOR_EACH(node_ptr, head_ptr) \
    for ((node_ptr) = (head_ptr)->next_ptr; (node_ptr) != (head_ptr); (node_ptr) = (node_ptr)->next_ptr)
#endif

/**
 * @def LIST_FOR_EACH_REVERSE
 * @brief Iterate over a list in reverse.
 *
 * @param node_ptr              Current node pointer.
 * @param head_ptr              List head pointer.
 */
#ifndef LIST_FOR_EACH_REVERSE
#define LIST_FOR_EACH_REVERSE(node_ptr, head_ptr) \
    for ((node_ptr) = (head_ptr)->prev_ptr; (node_ptr) != (head_ptr); (node_ptr) = (node_ptr)->prev_ptr)
#endif

/**
 * @def LIST_FOR_EACH_SAFE(node_ptr, next_ptr, head_ptr)
 * @brief Iterate over a list while allowing the current node to be modified.
 *
 * @param node_ptr              Current node pointer.
 * @param next_ptr              Temporary pointer to the next node.
 * @param head_ptr              List head pointer.
 */
#ifndef LIST_FOR_EACH_SAFE
#define LIST_FOR_EACH_SAFE(node_ptr, next_ptr, head_ptr)                                                 \
    for ((node_ptr) = (head_ptr)->next_ptr, (next_ptr) = (node_ptr)->next_ptr; (node_ptr) != (head_ptr); \
         (node_ptr) = (next_ptr), (next_ptr) = (node_ptr)->next_ptr)
#endif

/**
 * @def LIST_FOR_EACH_REVERSE_SAFE(node_ptr, prev_ptr, head_ptr)
 * @brief Iterate over a list in reverse while allowing the current node to be modified.
 *
 * @param node_ptr              Current node pointer.
 * @param prev_ptr              Temporary pointer to the previous node.
 * @param head_ptr              List head pointer.
 */
#ifndef LIST_FOR_EACH_REVERSE_SAFE
#define LIST_FOR_EACH_REVERSE_SAFE(node_ptr, prev_ptr, head_ptr)                                         \
    for ((node_ptr) = (head_ptr)->prev_ptr, (prev_ptr) = (node_ptr)->prev_ptr; (node_ptr) != (head_ptr); \
         (node_ptr) = (prev_ptr), (prev_ptr) = (node_ptr)->prev_ptr)
#endif

// }}}

// type definitions: {{{

struct JOIN(LIST_NAME, node);

/**
 * @def TYPE_DEFINITIONS
 * @brief Define the types
 */
#ifdef TYPE_DEFINITIONS

/**
 * @brief Intrusive list node structure.
 */
struct JOIN(LIST_NAME, node) {
    LIST_NODE_TYPE *prev_ptr; ///< prev node pointer.
    LIST_NODE_TYPE *next_ptr; ///< next node pointer.
};

#endif

// }}}

// function declarations: {{{

/**
 * @brief Initialize a list node
 *
 * @param[in] node_ptr          The node pointer.
 */
FUNCTION_LINKAGE void JOIN(LIST_NAME, node_init)(LIST_NODE_TYPE *node_ptr);

/**
 * @brief Check if a given list node is first in the list (aka after the head).
 *
 * @pre both nodes are part of the same list.
 *
 * @param[in] head_ptr          The head node pointer.
 * @param[in] node_ptr          The node pointer.
 *
 * @return                      Whether the list node is first in the list.
 */
FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_first)(const LIST_NODE_TYPE *head_ptr, const LIST_NODE_TYPE *node_ptr);

/**
 * @brief Check if a given list node is the last of the list (aka before the
 *        head).
 *
 * @pre both nodes are part of the same list.
 *
 * @param[in] head_ptr          The head node pointer.
 * @param[in] node_ptr          The node pointer.
 *
 * @return                      Whether the list node is the last in the list.
 */
FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_last)(const LIST_NODE_TYPE *head_ptr, const LIST_NODE_TYPE *node_ptr);

/**
 * @brief Check if a given list node is singular (and initialized).
 *
 * @param[in] node_ptr          The node pointer.
 *
 * @return                      Whether the list node is singular.
 */
FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_singular)(const LIST_NODE_TYPE *node_ptr);

/**
 * @brief Add a node *after* the given node.
 *
 * @note This can be used to construct a stack.
 *
 * @param[in] node_ptr          The node pointer.
 * @param[in] prev_ptr          The prev node pointer.
 */
FUNCTION_LINKAGE void JOIN(LIST_NAME, node_add_after)(LIST_NODE_TYPE *prev_ptr, LIST_NODE_TYPE *node_ptr);

/**
 * @brief Add a node *before* the given node.
 *
 * @note This can be used to construct a queue.
 *
 * @param[in] node_ptr          The node pointer.
 * @param[in] next_ptr          The next node pointer.
 */
FUNCTION_LINKAGE void JOIN(LIST_NAME, node_add_before)(LIST_NODE_TYPE *next_ptr, LIST_NODE_TYPE *node_ptr);

/**
 * @brief Remove a node from the list it resides in.
 *
 * @pre The node is a part of a list.
 *
 * @param[in] node_ptr          The node pointer.
 *
 * @return The removed node, now initialized as a singular node.
 */
FUNCTION_LINKAGE LIST_NODE_TYPE *JOIN(LIST_NAME, node_remove)(LIST_NODE_TYPE *node_ptr);

/**
 * @brief Replace a given node by a new node.
 *
 * @pre The old node is a part of a list while the new node is not.
 *
 * @param[in] old_ptr           Pointer to old node.
 * @param[in] new_ptr           Pointer to new node.
 */
FUNCTION_LINKAGE void JOIN(LIST_NAME, node_replace)(LIST_NODE_TYPE *restrict old_ptr, LIST_NODE_TYPE *restrict new_ptr);

// }}}

// function definitions: {{{

/**
 * @def FUNCTION_DEFINITIONS
 * @brief Define the functions
 */
#ifdef FUNCTION_DEFINITIONS

#include <assert.h>

/// @cond DO_NOT_DOCUMENT

// Add a node between two (known) nodes.
static inline void JOIN(JOIN(internal, LIST_NAME), node_add_between)(LIST_NODE_TYPE *node_ptr,
                                                                     LIST_NODE_TYPE *before_ptr,
                                                                     LIST_NODE_TYPE *after_ptr)
{
    before_ptr->next_ptr = node_ptr;
    node_ptr->prev_ptr = before_ptr;

    after_ptr->prev_ptr = node_ptr;
    node_ptr->next_ptr = after_ptr;
}

// Attach two nodes together, so anything in between is ignored.
static inline void JOIN(JOIN(internal, LIST_NAME), node_attach)(LIST_NODE_TYPE *prev_ptr, LIST_NODE_TYPE *next_ptr)
{
    prev_ptr->next_ptr = next_ptr;
    next_ptr->prev_ptr = prev_ptr;
}

/// @endcond

FUNCTION_LINKAGE void JOIN(LIST_NAME, node_init)(LIST_NODE_TYPE *node_ptr)
{
    assert(node_ptr != NULL);

    node_ptr->prev_ptr = node_ptr->next_ptr = node_ptr;
}

FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_first)(const LIST_NODE_TYPE *head_ptr, const LIST_NODE_TYPE *node_ptr)
{
    assert(head_ptr != NULL);
    assert(node_ptr != NULL);

    return node_ptr->prev_ptr == head_ptr;
}

FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_last)(const LIST_NODE_TYPE *head_ptr, const LIST_NODE_TYPE *node_ptr)
{
    assert(head_ptr != NULL);
    assert(node_ptr != NULL);

    return node_ptr->next_ptr == head_ptr;
}

FUNCTION_LINKAGE bool JOIN(LIST_NAME, node_is_singular)(const LIST_NODE_TYPE *node_ptr)
{
    assert(node_ptr != NULL);

    return node_ptr->prev_ptr == node_ptr && node_ptr->next_ptr == node_ptr;
}

FUNCTION_LINKAGE void JOIN(LIST_NAME, node_add_after)(LIST_NODE_TYPE *prev_ptr, LIST_NODE_TYPE *node_ptr)
{
    assert(prev_ptr != NULL);
    assert(node_ptr != NULL);
    assert(LIST_NODE_IS_SINGULAR(node_ptr));

    LIST_NODE_ADD_BETWEEN(node_ptr, prev_ptr, prev_ptr->next_ptr);
}

FUNCTION_LINKAGE void JOIN(LIST_NAME, node_add_before)(LIST_NODE_TYPE *next_ptr, LIST_NODE_TYPE *node_ptr)
{
    assert(next_ptr != NULL);
    assert(node_ptr != NULL);
    assert(LIST_NODE_IS_SINGULAR(node_ptr));

    LIST_NODE_ADD_BETWEEN(node_ptr, next_ptr->prev_ptr, next_ptr);
}

FUNCTION_LINKAGE LIST_NODE_TYPE *JOIN(LIST_NAME, node_remove)(LIST_NODE_TYPE *node_ptr)
{
    assert(node_ptr != NULL);
    assert(!LIST_NODE_IS_SINGULAR(node_ptr));

    LIST_NODE_ATTACH(node_ptr->prev_ptr, node_ptr->next_ptr);
    LIST_NODE_INIT(node_ptr);

    return node_ptr;
}

FUNCTION_LINKAGE void JOIN(LIST_NAME, node_replace)(LIST_NODE_TYPE *restrict old_ptr, LIST_NODE_TYPE *restrict new_ptr)
{
    assert(old_ptr != NULL);
    assert(new_ptr != NULL);
    assert(!LIST_NODE_IS_SINGULAR(old_ptr));
    assert(LIST_NODE_IS_SINGULAR(new_ptr));

    LIST_NODE_ADD_BETWEEN(new_ptr, old_ptr->prev_ptr, old_ptr->next_ptr);
    LIST_NODE_INIT(old_ptr);
}

#endif

// }}}

// macro undefs: {{{
#undef NAME
#undef FUNCTION_LINKAGE
#undef FUNCTION_DEFINITIONS
#undef TYPE_DEFINITIONS

#undef LIST_NAME
#undef LIST_NODE_TYPE
#undef LIST_NODE_ADD_BETWEEN
#undef LIST_NODE_ATTACH
#undef LIST_NODE_INIT
#undef LIST_NODE_IS_SINGULAR
// }}}

#ifdef __cplusplus
}
#endif

// vim: ft=c fdm=marker

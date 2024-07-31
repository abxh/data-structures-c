/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 16
    - N := 4096

    Non-mutating operation types / properties:
    - .root_ptr
    - node_parent_ptr
    - node_is_black
    - node_is_red
    - is_empty
    - contains_key
    - get_value / get_value_mut
    - search_node

    Mutating operation types:
    - init
    - node_init
    - insert_node
    - delete_node
    - clear
*/

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define NAME bst
#define KEY_TYPE int
#define VALUE_TYPE int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#include "bst.h"

static int count_nodes(bst_node_type* root_ptr) {
    if (root_ptr == NULL) {
        return 0;
    }

    int count = 1;
    if (root_ptr->left_ptr != NULL) {
        count += count_nodes(root_ptr->left_ptr);
    }
    if (root_ptr->right_ptr != NULL) {
        count += count_nodes(root_ptr->right_ptr);
    }
    return count;
}

static bool is_valid_binary_search_tree(bst_node_type* root_ptr) {
    // no duplicates allowed (however the value can be used as a counter)

    if (root_ptr == NULL) {
        return true;
    }

    bool ret = true;
    if (root_ptr->left_ptr != NULL) {
        ret &= root_ptr->left_ptr->key < root_ptr->key;
        ret &= is_valid_binary_search_tree(root_ptr->left_ptr);
    }
    if (root_ptr->right_ptr != NULL) {
        ret &= root_ptr->key < root_ptr->right_ptr->key;
        ret &= is_valid_binary_search_tree(root_ptr->right_ptr);
    }
    return ret;
}

static void preorder_print_and_traverse(bst_node_type* root_ptr) {
    if (root_ptr == NULL) {
        printf(" ()");
        return;
    }
    printf(" ( %d", root_ptr->key);
    preorder_print_and_traverse(root_ptr->left_ptr);
    preorder_print_and_traverse(root_ptr->right_ptr);
    printf(" )");
}

int main(void) {
    // N = 0
    {
        bst_type bst;
        bst_init(&bst);
        assert(is_valid_binary_search_tree(bst.root_ptr));
        assert(count_nodes(bst.root_ptr) == 0);
    }
    // N = 1
    {
        bst_type bst;
        bst_init(&bst);

        assert(!bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));
        assert(bst_get_value(&bst, 42, -1) == -1);
        assert(bst_get_value(&bst, 69, -1) == -1);
        assert(bst_get_value_mut(&bst, 42) == NULL);
        assert(bst_get_value_mut(&bst, 69) == NULL);

        assert(bst.root_ptr == NULL);
        assert(bst_is_empty(&bst));

        assert(is_valid_binary_search_tree(bst.root_ptr));
        assert(count_nodes(bst.root_ptr) == 0);
    }
    // N = 1, insert_node
    {
        bst_type bst;
        bst_init(&bst);

        bst_node_type node;
        bst_node_init(&node, 42, 69);
        bst_insert_node(&bst, &node);

        assert(bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));
        assert(bst_get_value(&bst, 42, -1) == 69);
        assert(bst_get_value(&bst, 69, -1) == -1);
        assert(*bst_get_value_mut(&bst, 42) == 69);
        assert(bst_get_value_mut(&bst, 69) == NULL);

        assert(bst.root_ptr == &node);
        assert(!bst_is_empty(&bst));

        assert(is_valid_binary_search_tree(bst.root_ptr));
        assert(count_nodes(bst.root_ptr) == 1);
    }

    // N = 16, insert_node * 8, delete_node * 4, insert_node * 12
    {
        bst_type bst;
        bst_init(&bst);

        bst_node_type node[8];
        const int values[8] = {3, 1, 5, 2, 4, 7, 6, 8};
        for (int i = 0; i < 8; i++) {
            bst_node_init(&node[i], values[i], values[i]);
            bst_insert_node(&bst, &node[i]);

            assert(count_nodes(bst.root_ptr) == i + 1);
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }

        const int deleted_values[4] = {3, 2, 4, 8};
        for (int i = 0; i < 4; i++) {
            bst_delete_node(&bst, bst_search_node(&bst, deleted_values[i]));

            assert(count_nodes(bst.root_ptr) == 8 - i - 1);
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }

        bst_node_type new_node[12];
        const int new_values[12] = {4, 12, 9, 10, 11, 16, 13, 14, 17, 16, 15, 8};
        for (int i = 0; i < 6; i++) {
            bst_node_init(&new_node[i], new_values[i], new_values[i]);
            bst_insert_node(&bst, &new_node[i]);

            assert(count_nodes(bst.root_ptr) == 4 + i + 1);
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }
    }

    // N = 4096, insert_node * 4096, delete_node * 4096, insert_node * 4096
    {
        bst_type bst;
        bst_init(&bst);

        srand(time(NULL));

        bst_node_type* node_buf = calloc(8192, sizeof(bst_node_type));
        size_t count = 0;

        for (int i = 0; i < 4096; i++) {
            int val = i % 2048;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (!node_ptr) {
                bst_node_init(&node_buf[count], val, 0);
                bst_insert_node(&bst, &node_buf[count]);
                count++;
            } else {
                node_ptr->value++;
            }
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }

        for (int i = 0; i < 4096; i++) {
            int val = i % 2048;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (node_ptr) {
                bst_delete_node(&bst, node_ptr);
            }
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }

        for (int i = 0; i < 4096; i++) {
            int val = i % 2048;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (!node_ptr) {
                bst_node_init(&node_buf[count], val, 0);
                bst_insert_node(&bst, &node_buf[count]);
                count++;
            } else {
                node_ptr->value++;
            }
            assert(is_valid_binary_search_tree(bst.root_ptr));
        }

        free(node_buf);
    }
}

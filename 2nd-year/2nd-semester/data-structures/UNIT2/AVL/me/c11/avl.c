#include "avl.h"

#include <stdlib.h>

struct AVLNode {
    int value;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
};

static int node_height(const AVLNode *node)
{
    return node == NULL ? -1 : node->height;
}

static int maximum_int(int lhs, int rhs)
{
    return lhs > rhs ? lhs : rhs;
}

static void update_height(AVLNode *node)
{
    node->height = 1 + maximum_int(node_height(node->left),
                                    node_height(node->right));
}

static AVLNode *rotate_right(AVLNode *root)
{
    AVLNode *new_root = root->left;
    AVLNode *middle = new_root->right;

    new_root->right = root;
    root->left = middle;
    update_height(root);
    update_height(new_root);
    return new_root;
}

static AVLNode *rotate_left(AVLNode *root)
{
    AVLNode *new_root = root->right;
    AVLNode *middle = new_root->left;

    new_root->left = root;
    root->right = middle;
    update_height(root);
    update_height(new_root);
    return new_root;
}

static AVLNode *rebalance(AVLNode *root)
{
    int balance;

    update_height(root);
    balance = node_height(root->right) - node_height(root->left);

    if (balance > 1) {
        if (node_height(root->right->right) < node_height(root->right->left)) {
            root->right = rotate_right(root->right);
        }
        return rotate_left(root);
    }
    if (balance < -1) {
        if (node_height(root->left->left) < node_height(root->left->right)) {
            root->left = rotate_left(root->left);
        }
        return rotate_right(root);
    }
    return root;
}

static AVLNode *insert_node(AVLNode *root, int value, int *inserted)
{
    if (root == NULL) {
        AVLNode *node = malloc(sizeof(*node));

        if (node == NULL) {
            *inserted = 0;
            return NULL;
        }
        node->value = value;
        node->height = 0;
        node->left = NULL;
        node->right = NULL;
        *inserted = 1;
        return node;
    }

    if (value <= root->value) {
        AVLNode *left = insert_node(root->left, value, inserted);

        if (!*inserted) {
            return root;
        }
        root->left = left;
    } else {
        AVLNode *right = insert_node(root->right, value, inserted);

        if (!*inserted) {
            return root;
        }
        root->right = right;
    }
    return rebalance(root);
}

static AVLNode *minimum_node(AVLNode *root)
{
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

static AVLNode *erase_node(AVLNode *root, int value, int *erased)
{
    if (root == NULL) {
        return NULL;
    }

    if (value < root->value) {
        root->left = erase_node(root->left, value, erased);
    } else if (value > root->value) {
        root->right = erase_node(root->right, value, erased);
    } else {
        AVLNode *replacement;

        *erased = 1;
        if (root->left == NULL) {
            replacement = root->right;
            free(root);
            return replacement;
        }
        if (root->right == NULL) {
            replacement = root->left;
            free(root);
            return replacement;
        }

        replacement = minimum_node(root->right);
        root->value = replacement->value;
        root->right = erase_node(root->right, replacement->value, erased);
    }
    return rebalance(root);
}

static void destroy_nodes(AVLNode *root)
{
    if (root != NULL) {
        destroy_nodes(root->left);
        destroy_nodes(root->right);
        free(root);
    }
}

void avl_init(AVL *tree)
{
    tree->root = NULL;
    tree->size = 0;
}

void avl_clear(AVL *tree)
{
    destroy_nodes(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

void avl_destroy(AVL *tree)
{
    avl_clear(tree);
}

int avl_insert(AVL *tree, int value)
{
    int inserted = 0;

    tree->root = insert_node(tree->root, value, &inserted);
    if (inserted) {
        ++tree->size;
    }
    return inserted;
}

int avl_erase(AVL *tree, int value)
{
    int erased = 0;

    tree->root = erase_node(tree->root, value, &erased);
    if (erased) {
        --tree->size;
    }
    return erased;
}

int avl_contains(const AVL *tree, int value)
{
    const AVLNode *current = tree->root;

    while (current != NULL) {
        if (value < current->value) {
            current = current->left;
        } else if (value > current->value) {
            current = current->right;
        } else {
            return 1;
        }
    }
    return 0;
}

size_t avl_size(const AVL *tree)
{
    return tree->size;
}

int avl_height(const AVL *tree)
{
    return node_height(tree->root);
}

int avl_empty(const AVL *tree)
{
    return tree->size == 0;
}

int avl_minimum(const AVL *tree, int *out_value)
{
    const AVLNode *current;

    if (tree->root == NULL || out_value == NULL) {
        return 0;
    }
    current = tree->root;
    while (current->left != NULL) {
        current = current->left;
    }
    *out_value = current->value;
    return 1;
}

int avl_maximum(const AVL *tree, int *out_value)
{
    const AVLNode *current;

    if (tree->root == NULL || out_value == NULL) {
        return 0;
    }
    current = tree->root;
    while (current->right != NULL) {
        current = current->right;
    }
    *out_value = current->value;
    return 1;
}

typedef struct {
    int valid;
    int height;
    size_t count;
    int minimum;
    int maximum;
    int has_value;
} Validation;

static Validation validate_node(const AVLNode *root)
{
    Validation result = {1, -1, 0, 0, 0, 0};
    Validation left;
    Validation right;
    int balance;

    if (root == NULL) {
        return result;
    }

    left = validate_node(root->left);
    right = validate_node(root->right);
    result.height = 1 + maximum_int(left.height, right.height);
    result.count = 1 + left.count + right.count;
    result.minimum = left.has_value ? left.minimum : root->value;
    result.maximum = right.has_value ? right.maximum : root->value;
    result.has_value = 1;
    balance = right.height - left.height;

    result.valid = left.valid && right.valid
        && (!left.has_value || left.maximum <= root->value)
        && (!right.has_value || right.minimum >= root->value)
        && root->height == result.height
        && balance >= -1 && balance <= 1;
    return result;
}

int avl_valid(const AVL *tree)
{
    Validation result = validate_node(tree->root);

    return result.valid && result.count == tree->size;
}

static size_t inorder_node(const AVLNode *root, int *buffer,
                           size_t capacity, size_t offset)
{
    if (root == NULL) {
        return offset;
    }
    offset = inorder_node(root->left, buffer, capacity, offset);
    if (buffer != NULL && offset < capacity) {
        buffer[offset] = root->value;
    }
    ++offset;
    return inorder_node(root->right, buffer, capacity, offset);
}

size_t avl_inorder(const AVL *tree, int *buffer, size_t capacity)
{
    return inorder_node(tree->root, buffer, capacity, 0);
}

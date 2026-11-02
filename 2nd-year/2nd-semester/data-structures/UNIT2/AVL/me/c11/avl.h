#ifndef AVL_H
#define AVL_H

#include <stddef.h>

typedef struct AVLNode AVLNode;

typedef struct {
    AVLNode *root;
    size_t size;
} AVL;

void avl_init(AVL *tree);
void avl_destroy(AVL *tree);
void avl_clear(AVL *tree);

/* Mutations and lookup are O(log n) for a valid AVL tree. */
int avl_insert(AVL *tree, int value);
int avl_erase(AVL *tree, int value);
int avl_contains(const AVL *tree, int value);

/* Metadata is O(1). Empty height is -1; leaf height is 0. */
size_t avl_size(const AVL *tree);
int avl_height(const AVL *tree);
int avl_empty(const AVL *tree);

/* Extrema are O(log n); they return 0 for an empty tree or null output. */
int avl_minimum(const AVL *tree, int *out_value);
int avl_maximum(const AVL *tree, int *out_value);

/* Validation and traversal are O(n). Inorder returns the total item count. */
int avl_valid(const AVL *tree);
size_t avl_inorder(const AVL *tree, int *buffer, size_t capacity);

#endif

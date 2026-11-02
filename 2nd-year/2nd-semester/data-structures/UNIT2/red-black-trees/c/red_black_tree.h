#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct RBTree RBTree;

/* Returns a new empty tree, or NULL if allocation fails. */
RBTree *rb_tree_create(void);
void rb_tree_destroy(RBTree *tree);

/* Returns 1 on success, 0 for a duplicate, and -1 for an invalid tree or
 * allocation failure. */
int rb_tree_insert(RBTree *tree, int key);
bool rb_tree_erase(RBTree *tree, int key);
bool rb_tree_contains(const RBTree *tree, int key);

size_t rb_tree_size(const RBTree *tree);
bool rb_tree_empty(const RBTree *tree);
int rb_tree_height(const RBTree *tree);

/* Writes at most capacity keys and returns the total number of keys. */
size_t rb_tree_inorder(const RBTree *tree, int *output, size_t capacity);

bool rb_tree_valid(const RBTree *tree);
void rb_tree_print(const RBTree *tree, FILE *output);

#endif /* RED_BLACK_TREE_H */

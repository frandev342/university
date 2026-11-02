#ifndef RB_RECURSIVE_TREE_H
#define RB_RECURSIVE_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct RBTree RBTree;

RBTree *rb_tree_create(void);
void rb_tree_destroy(RBTree *tree);
/* 1: inserted, 0: duplicate, -1: NULL tree or allocation failure. */
int rb_tree_insert(RBTree *tree, int key);
bool rb_tree_erase(RBTree *tree, int key);
bool rb_tree_contains(const RBTree *tree, int key);
size_t rb_tree_size(const RBTree *tree);
bool rb_tree_empty(const RBTree *tree);
int rb_tree_height(const RBTree *tree);
/* Returns total size; writes at most capacity keys if output is not NULL. */
size_t rb_tree_inorder(const RBTree *tree, int *output, size_t capacity);
bool rb_tree_valid(const RBTree *tree);
void rb_tree_print(const RBTree *tree, FILE *output);

#endif

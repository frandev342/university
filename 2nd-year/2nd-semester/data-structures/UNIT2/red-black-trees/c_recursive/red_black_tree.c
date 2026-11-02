#include "red_black_tree.h"

#include <limits.h>
#include <stdlib.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
  int key;
  Color color;
  struct Node *left;
  struct Node *right;
  struct Node *parent;
} Node;

struct RBTree {
  Node nil; /* All missing children share this black sentinel. */
  Node *root;
  size_t size;
};

RBTree *rb_tree_create(void) {
  RBTree *tree = malloc(sizeof *tree);
  if (tree == NULL)
    return NULL;
  tree->nil.key = 0;
  tree->nil.color = BLACK;
  tree->nil.left = &tree->nil;
  tree->nil.right = &tree->nil;
  tree->nil.parent = &tree->nil;
  tree->root = &tree->nil;
  tree->size = 0U;
  return tree;
}

static void destroy_nodes(RBTree *tree, Node *node) {
  if (node == &tree->nil)
    return;
  destroy_nodes(tree, node->left);
  destroy_nodes(tree, node->right);
  free(node);
}

void rb_tree_destroy(RBTree *tree) {
  if (tree == NULL)
    return;
  destroy_nodes(tree, tree->root);
  free(tree);
}

/* One recursive call for each level of the BST search path. */
static Node *search(const RBTree *tree, Node *node, int key) {
  if (node == &tree->nil || node->key == key)
    return node;
  return search(tree, key < node->key ? node->left : node->right, key);
}

bool rb_tree_contains(const RBTree *tree, int key) {
  return tree != NULL && search(tree, tree->root, key) != &tree->nil;
}

static Node *minimum(const RBTree *tree, Node *node) {
  return node->left == &tree->nil ? node : minimum(tree, node->left);
}

static void rotate_left(RBTree *tree, Node *node) {
  Node *pivot = node->right;
  node->right = pivot->left;
  if (pivot->left != &tree->nil)
    pivot->left->parent = node;
  pivot->parent = node->parent;
  if (node->parent == &tree->nil)
    tree->root = pivot;
  else if (node == node->parent->left)
    node->parent->left = pivot;
  else
    node->parent->right = pivot;
  pivot->left = node;
  node->parent = pivot;
}

static void rotate_right(RBTree *tree, Node *node) {
  Node *pivot = node->left;
  node->left = pivot->right;
  if (pivot->right != &tree->nil)
    pivot->right->parent = node;
  pivot->parent = node->parent;
  if (node->parent == &tree->nil)
    tree->root = pivot;
  else if (node == node->parent->right)
    node->parent->right = pivot;
  else
    node->parent->left = pivot;
  pivot->right = node;
  node->parent = pivot;
}

/* Case 1 recurses on the grandparent; cases 2/3 finish locally. */
static void insert_fixup(RBTree *tree, Node *node) {
  Node *uncle;
  if (node->parent->color != RED)
    return;

  if (node->parent == node->parent->parent->left) {
    uncle = node->parent->parent->right;
    if (uncle->color == RED) {
      node->parent->color = BLACK;
      uncle->color = BLACK;
      node->parent->parent->color = RED;
      insert_fixup(tree, node->parent->parent);
    } else {
      if (node == node->parent->right) {
        node = node->parent;
        rotate_left(tree, node);
      }
      node->parent->color = BLACK;
      node->parent->parent->color = RED;
      rotate_right(tree, node->parent->parent);
    }
  } else {
    uncle = node->parent->parent->left;
    if (uncle->color == RED) {
      node->parent->color = BLACK;
      uncle->color = BLACK;
      node->parent->parent->color = RED;
      insert_fixup(tree, node->parent->parent);
    } else {
      if (node == node->parent->left) {
        node = node->parent;
        rotate_right(tree, node);
      }
      node->parent->color = BLACK;
      node->parent->parent->color = RED;
      rotate_left(tree, node->parent->parent);
    }
  }
}

/* A link points to the root slot or to its parent's left/right slot. */
static int insert_at(RBTree *tree, Node **link, Node *parent, int key,
                     Node **inserted) {
  Node *node = *link;
  if (node == &tree->nil) {
    node = malloc(sizeof *node);
    if (node == NULL)
      return -1;
    node->key = key;
    node->color = RED;
    node->left = &tree->nil;
    node->right = &tree->nil;
    node->parent = parent;
    *link = node;
    *inserted = node;
    return 1;
  }
  if (key == node->key)
    return 0;
  if (key < node->key)
    return insert_at(tree, &node->left, node, key, inserted);
  return insert_at(tree, &node->right, node, key, inserted);
}

int rb_tree_insert(RBTree *tree, int key) {
  Node *inserted = NULL;
  int result;
  if (tree == NULL)
    return -1;
  result = insert_at(tree, &tree->root, &tree->nil, key, &inserted);
  if (result == 1) {
    ++tree->size;
    insert_fixup(tree, inserted);
    tree->root->color = BLACK;
  }
  return result;
}

static void transplant(RBTree *tree, Node *old, Node *replacement) {
  if (old->parent == &tree->nil)
    tree->root = replacement;
  else if (old == old->parent->left)
    old->parent->left = replacement;
  else
    old->parent->right = replacement;
  /* Even NIL needs a temporary parent for deletion fixup. */
  replacement->parent = old->parent;
}

/* Only case 2 propagates the missing black level upward recursively. */
static void erase_fixup(RBTree *tree, Node *node) {
  Node *sibling;
  if (node == tree->root || node->color == RED) {
    node->color = BLACK;
    return;
  }
  if (node == node->parent->left) {
    sibling = node->parent->right;
    if (sibling->color == RED) {
      sibling->color = BLACK;
      node->parent->color = RED;
      rotate_left(tree, node->parent);
      sibling = node->parent->right;
    }
    if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
      sibling->color = RED;
      erase_fixup(tree, node->parent);
      return;
    }
    if (sibling->right->color == BLACK) {
      sibling->left->color = BLACK;
      sibling->color = RED;
      rotate_right(tree, sibling);
      sibling = node->parent->right;
    }
    sibling->color = node->parent->color;
    node->parent->color = BLACK;
    sibling->right->color = BLACK;
    rotate_left(tree, node->parent);
  } else {
    sibling = node->parent->left;
    if (sibling->color == RED) {
      sibling->color = BLACK;
      node->parent->color = RED;
      rotate_right(tree, node->parent);
      sibling = node->parent->left;
    }
    if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
      sibling->color = RED;
      erase_fixup(tree, node->parent);
      return;
    }
    if (sibling->left->color == BLACK) {
      sibling->right->color = BLACK;
      sibling->color = RED;
      rotate_left(tree, sibling);
      sibling = node->parent->left;
    }
    sibling->color = node->parent->color;
    node->parent->color = BLACK;
    sibling->left->color = BLACK;
    rotate_right(tree, node->parent);
  }
  tree->root->color = BLACK;
}

bool rb_tree_erase(RBTree *tree, int key) {
  Node *target;
  Node *moved;
  Node *replacement;
  Color removed_color;
  if (tree == NULL)
    return false;
  target = search(tree, tree->root, key);
  if (target == &tree->nil)
    return false;

  moved = target;
  removed_color = moved->color;
  if (target->left == &tree->nil) {
    replacement = target->right;
    transplant(tree, target, replacement);
  } else if (target->right == &tree->nil) {
    replacement = target->left;
    transplant(tree, target, replacement);
  } else {
    moved = minimum(tree, target->right);
    removed_color = moved->color;
    replacement = moved->right;
    if (moved->parent == target) {
      replacement->parent = moved;
    } else {
      transplant(tree, moved, replacement);
      moved->right = target->right;
      moved->right->parent = moved;
    }
    transplant(tree, target, moved);
    moved->left = target->left;
    moved->left->parent = moved;
    moved->color = target->color;
  }
  free(target);
  --tree->size;
  if (removed_color == BLACK)
    erase_fixup(tree, replacement);
  if (tree->root != &tree->nil)
    tree->root->parent = &tree->nil;
  tree->nil.parent = &tree->nil;
  tree->nil.color = BLACK;
  return true;
}

size_t rb_tree_size(const RBTree *tree) {
  return tree == NULL ? 0U : tree->size;
}

bool rb_tree_empty(const RBTree *tree) {
  return tree == NULL || tree->root == &tree->nil;
}

static int node_height(const RBTree *tree, const Node *node) {
  int left;
  int right;
  if (node == &tree->nil)
    return -1;
  left = node_height(tree, node->left);
  right = node_height(tree, node->right);
  return 1 + (left > right ? left : right);
}

int rb_tree_height(const RBTree *tree) {
  return tree == NULL ? -1 : node_height(tree, tree->root);
}

static void inorder_nodes(const RBTree *tree, const Node *node, int *output,
                          size_t capacity, size_t *count) {
  if (node == &tree->nil)
    return;
  inorder_nodes(tree, node->left, output, capacity, count);
  if (*count < capacity && output != NULL)
    output[*count] = node->key;
  ++*count;
  inorder_nodes(tree, node->right, output, capacity, count);
}

size_t rb_tree_inorder(const RBTree *tree, int *output, size_t capacity) {
  size_t count = 0U;
  if (tree != NULL)
    inorder_nodes(tree, tree->root, output, capacity, &count);
  return count;
}

static bool validate(const RBTree *tree, const Node *node, const Node *parent,
                     long long lower, long long upper, size_t *count, int *blacks) {
  int left;
  int right;
  if (node == &tree->nil) {
    *blacks = 1;
    return true;
  }
  if (node == NULL || node->parent != parent ||
      (long long)node->key <= lower || (long long)node->key >= upper ||
      node->left == NULL || node->right == NULL)
    return false;
  if (node->color == RED &&
      (node->left->color == RED || node->right->color == RED))
    return false;
  if (!validate(tree, node->left, node, lower, node->key, count, &left) ||
      !validate(tree, node->right, node, node->key, upper, count, &right) ||
      left != right)
    return false;
  ++*count;
  *blacks = left + (node->color == BLACK ? 1 : 0);
  return true;
}

bool rb_tree_valid(const RBTree *tree) {
  size_t count = 0U;
  int blacks = 0;
  if (tree == NULL || tree->root == NULL)
    return false;
  if (tree->nil.color != BLACK || tree->nil.left != &tree->nil ||
      tree->nil.right != &tree->nil || tree->nil.parent != &tree->nil)
    return false;
  if (tree->root == &tree->nil)
    return tree->size == 0U;
  return tree->root->color == BLACK && tree->root->parent == &tree->nil &&
         validate(tree, tree->root, &tree->nil, LLONG_MIN, LLONG_MAX,
                  &count, &blacks) && count == tree->size;
}

static void print_nodes(const RBTree *tree, const Node *node, FILE *output,
                        size_t depth) {
  size_t i;
  if (node == &tree->nil)
    return;
  print_nodes(tree, node->right, output, depth + 1U);
  for (i = 0U; i < depth * 4U; ++i)
    fputc(' ', output);
  fprintf(output, "%c:%d\n", node->color == BLACK ? 'B' : 'R', node->key);
  print_nodes(tree, node->left, output, depth + 1U);
}

void rb_tree_print(const RBTree *tree, FILE *output) {
  if (tree == NULL || output == NULL)
    return;
  if (rb_tree_empty(tree))
    fputs("(empty)\n", output);
  else
    print_nodes(tree, tree->root, output, 0U);
}

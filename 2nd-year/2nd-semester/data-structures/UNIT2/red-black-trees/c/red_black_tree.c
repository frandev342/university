#include "red_black_tree.h"

#include <limits.h>
#include <stdlib.h>

typedef enum { RB_RED, RB_BLACK } RBColor;

typedef struct RBNode {
  int key;
  RBColor color;
  struct RBNode *left;
  struct RBNode *right;
  struct RBNode *parent;
} RBNode;

struct RBTree {
  RBNode nil;
  RBNode *root;
  size_t size;
};

RBTree *rb_tree_create(void) {
  RBTree *tree = malloc(sizeof *tree);

  if (tree == NULL)
    return NULL;

  tree->nil.key = 0;
  tree->nil.color = RB_BLACK;
  tree->nil.left = &tree->nil;
  tree->nil.right = &tree->nil;
  tree->nil.parent = &tree->nil;
  tree->root = &tree->nil;
  tree->size = 0U;
  return tree;
}

static RBNode *newNode(RBTree *tree, RBNode *parent, int key) {
  RBNode *node = malloc(sizeof *node);
  if (node == NULL)
    return NULL;

  node->key = key;
  node->color = RB_RED;
  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = parent;
  return node;
}

static RBNode *find_node(const RBTree *tree, int key) {
  RBNode *current = tree->root;

  while (current != &tree->nil && current->key != key)
    current = key < current->key ? current->left : current->right;
  return current;
}

static RBNode *minimum(const RBTree *tree, RBNode *node) {
  while (node->left != &tree->nil)
    node = node->left;
  return node;
}

bool rb_tree_contains(const RBTree *tree, int key) {
  return tree != NULL && find_node(tree, key) != &tree->nil;
}

static void rotate_left(RBTree *tree, RBNode *node) {
  RBNode *pivot = node->right;
  node->right = pivot->left;

  if (pivot->left != &tree->nil)
    pivot->left->parent = node;

  pivot->parent = node->parent;
  // Si era la raíz, ectualizar a donde apunta la raíz
  if (node->parent == &tree->nil)
    tree->root = pivot;
  // Sí no era raíz, entonces era hijo de algún nodo, debemos verificar si era
  // hizo izq o derecho
  else if (node == node->parent->left)
    node->parent->left = pivot;
  else
    node->parent->right = pivot;

  pivot->left = node;
  node->parent = pivot;
}

static void rotate_right(RBTree *tree, RBNode *node) {
  RBNode *pivot = node->left;
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

static void insert_fixup(RBTree *tree, RBNode *node) {
  while (node->parent->color == RB_RED) {
    if (node->parent == node->parent->parent->left) {
      RBNode *uncle = node->parent->parent->right;

      if (uncle->color == RB_RED) {
        node->parent->color = RB_BLACK;
        uncle->color = RB_BLACK;
        node->parent->parent->color = RB_RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          rotate_left(tree, node);
        }
        node->parent->color = RB_BLACK;
        node->parent->parent->color = RB_RED;
        rotate_right(tree, node->parent->parent);
      }
    } else {
      RBNode *uncle = node->parent->parent->left;

      if (uncle->color == RB_RED) {
        node->parent->color = RB_BLACK;
        uncle->color = RB_BLACK;
        node->parent->parent->color = RB_RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->left) {
          node = node->parent;
          rotate_right(tree, node);
        }
        node->parent->color = RB_BLACK;
        node->parent->parent->color = RB_RED;
        rotate_left(tree, node->parent->parent);
      }
    }
  }

  tree->root->color = RB_BLACK;
  tree->root->parent = &tree->nil;
}

int rb_tree_insert(RBTree *tree, int key) {
  RBNode *parent;
  RBNode *current;
  RBNode *inserted;

  if (tree == NULL)
    return -1;

  parent = &tree->nil;
  current = tree->root;
  while (current != &tree->nil) {
    parent = current;
    if (key == current->key)
      return 0;
    current = key < current->key ? current->left : current->right;
  }

  inserted = newNode(tree, parent, key);
  if (inserted == NULL)
    return -1;

  if (parent == &tree->nil)
    tree->root = inserted;
  else if (key < parent->key)
    parent->left = inserted;
  else
    parent->right = inserted;

  ++tree->size;
  insert_fixup(tree, inserted);
  return 1;
}

static void transplant(RBTree *tree, RBNode *old_root, RBNode *new_root) {
  if (old_root->parent == &tree->nil)
    tree->root = new_root;
  else if (old_root == old_root->parent->left)
    old_root->parent->left = new_root;
  else
    old_root->parent->right = new_root;

  /* erase_fixup needs this parent even when new_root is the sentinel. */
  new_root->parent = old_root->parent;
}

static void erase_fixup(RBTree *tree, RBNode *node) {
  while (node != tree->root && node->color == RB_BLACK) {
    if (node == node->parent->left) {
      RBNode *sibling = node->parent->right;

      if (sibling->color == RB_RED) {
        sibling->color = RB_BLACK;
        node->parent->color = RB_RED;
        rotate_left(tree, node->parent);
        sibling = node->parent->right;
      }

      if (sibling->left->color == RB_BLACK &&
          sibling->right->color == RB_BLACK) {
        sibling->color = RB_RED;
        node = node->parent;
      } else {
        if (sibling->right->color == RB_BLACK) {
          sibling->left->color = RB_BLACK;
          sibling->color = RB_RED;
          rotate_right(tree, sibling);
          sibling = node->parent->right;
        }

        sibling->color = node->parent->color;
        node->parent->color = RB_BLACK;
        sibling->right->color = RB_BLACK;
        rotate_left(tree, node->parent);
        node = tree->root;
      }
    } else {
      RBNode *sibling = node->parent->left;

      if (sibling->color == RB_RED) {
        sibling->color = RB_BLACK;
        node->parent->color = RB_RED;
        rotate_right(tree, node->parent);
        sibling = node->parent->left;
      }

      if (sibling->right->color == RB_BLACK &&
          sibling->left->color == RB_BLACK) {
        sibling->color = RB_RED;
        node = node->parent;
      } else {
        if (sibling->left->color == RB_BLACK) {
          sibling->right->color = RB_BLACK;
          sibling->color = RB_RED;
          rotate_left(tree, sibling);
          sibling = node->parent->left;
        }

        sibling->color = node->parent->color;
        node->parent->color = RB_BLACK;
        sibling->left->color = RB_BLACK;
        rotate_right(tree, node->parent);
        node = tree->root;
      }
    }
  }

  node->color = RB_BLACK;
}

bool rb_tree_erase(RBTree *tree, int key) {
  // Nodo cuya clave pidió borrar el usuario. Este es el nodo que se libera con
  // free.
  RBNode *target;
  // Nodo que ocupará la posición de target. Si hay dos hijos, será el sucesor.
  RBNode *moved;
  // Nodo que ocupa la posición que moved dejó vacía; puede ser &tree->nil. Es
  // el punto desde el que se inicia la reparación.
  RBNode *replacement;
  // Color que se retiró de la posición original de moved. Determina si hay que
  // llamar a erase_fixup.
  RBColor removed_color;

  if (tree == NULL)
    return false;

  target = find_node(tree, key);
  if (target == &tree->nil)
    return false;

  moved = target;
  removed_color = moved->color;
  replacement = &tree->nil;

  if (target->left == &tree->nil) {
    replacement = target->right;
    transplant(tree, target, target->right);
  } else if (target->right == &tree->nil) {
    replacement = target->left;
    transplant(tree, target, target->left);
  } else {
    moved = minimum(tree, target->right);
    removed_color = moved->color;
    replacement = moved->right;

    if (moved->parent == target) {
      replacement->parent = moved;
    } else {
      transplant(tree, moved, moved->right);
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

  if (removed_color == RB_BLACK)
    erase_fixup(tree, replacement);

  if (tree->root != &tree->nil)
    tree->root->parent = &tree->nil;
  tree->nil.parent = &tree->nil;
  tree->nil.color = RB_BLACK;
  return true;
}

size_t rb_tree_size(const RBTree *tree) {
  return tree == NULL ? 0U : tree->size;
}

bool rb_tree_empty(const RBTree *tree) {
  return tree == NULL || tree->root == &tree->nil;
}

static int node_height(const RBTree *tree, const RBNode *node) {
  int left_height;
  int right_height;

  if (node == &tree->nil)
    return -1;

  left_height = node_height(tree, node->left);
  right_height = node_height(tree, node->right);
  return 1 + (left_height > right_height ? left_height : right_height);
}

int rb_tree_height(const RBTree *tree) {
  return tree == NULL ? -1 : node_height(tree, tree->root);
}

static void inorder_nodes(const RBTree *tree, const RBNode *node, int *output,
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

static bool validate_node(const RBTree *tree, const RBNode *node,
                          const RBNode *expected_parent, long long lower,
                          long long upper, size_t *node_count,
                          int *black_height) {
  int left_black_height;
  int right_black_height;

  if (node == &tree->nil) {
    *black_height = 1;
    return true;
  }

  if (node == NULL || node->parent != expected_parent)
    return false;
  if ((long long)node->key <= lower || (long long)node->key >= upper)
    return false;
  if (node->left == NULL || node->right == NULL)
    return false;
  if (node->color == RB_RED &&
      (node->left->color == RB_RED || node->right->color == RB_RED))
    return false;

  if (!validate_node(tree, node->left, node, lower, (long long)node->key,
                     node_count, &left_black_height))
    return false;
  if (!validate_node(tree, node->right, node, (long long)node->key, upper,
                     node_count, &right_black_height))
    return false;
  if (left_black_height != right_black_height)
    return false;

  ++*node_count;
  *black_height = left_black_height + (node->color == RB_BLACK ? 1 : 0);
  return true;
}

bool rb_tree_valid(const RBTree *tree) {
  size_t node_count = 0U;
  int black_height = 0;
  bool structure_is_valid;

  if (tree == NULL)
    return false;
  if (tree->nil.color != RB_BLACK || tree->nil.left != &tree->nil ||
      tree->nil.right != &tree->nil || tree->nil.parent != &tree->nil)
    return false;
  if (tree->root == NULL)
    return false;
  if (tree->root == &tree->nil)
    return tree->size == 0U;
  if (tree->root->parent != &tree->nil || tree->root->color != RB_BLACK)
    return false;

  structure_is_valid = validate_node(tree, tree->root, &tree->nil, LLONG_MIN,
                                     LLONG_MAX, &node_count, &black_height);
  return structure_is_valid && node_count == tree->size;
}

static void print_node(const RBTree *tree, const RBNode *node, FILE *output,
                       size_t depth) {
  size_t index;

  if (node == &tree->nil)
    return;

  print_node(tree, node->right, output, depth + 1U);
  for (index = 0U; index < depth * 4U; ++index)
    fputc(' ', output);
  fprintf(output, "%c:%d\n", node->color == RB_BLACK ? 'B' : 'R', node->key);
  print_node(tree, node->left, output, depth + 1U);
}

void rb_tree_print(const RBTree *tree, FILE *output) {
  if (tree == NULL || output == NULL)
    return;
  if (rb_tree_empty(tree)) {
    fputs("(empty)\n", output);
    return;
  }
  print_node(tree, tree->root, output, 0U);
}

static void destroy_nodes(RBTree *tree, RBNode *node) {
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

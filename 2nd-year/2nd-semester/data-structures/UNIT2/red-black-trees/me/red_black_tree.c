#include "red_black_tree.h"
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

// Crea un árbol vacío con un centinela negro compartido. Tiempo O(1).
Tree *rb_tree_create(void) {
  Tree *tree = malloc(sizeof *tree);
  if (!tree)
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

// Libera recursivamente los nodos reales en postorden. Tiempo O(n), memoria
// O(h).
static void destroy_nodes(Tree *tree, Node *node) {
  if (node == &tree->nil)
    return;
  destroy_nodes(tree, node->left);
  destroy_nodes(tree, node->right);
  free(node);
}

// Libera los nodos y la estructura Tree; acepta NULL. Tiempo O(n), memoria
// O(h).
void rb_tree_destroy(Tree *tree) {
  if (!tree)
    return;
  destroy_nodes(tree, tree->root);
  free(tree);
}

// Crea un nodo rojo enlazado al centinela. Tiempo y memoria auxiliar O(1).
static Node *newNode(Tree *tree, Node *parent, int key) {
  Node *node = malloc(sizeof(*node));
  if (!node)
    return NULL;
  node->color = RED;
  node->key = key;
  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = parent;
  return node;
}

// Busca una clave y devuelve su nodo o el centinela. Tiempo O(log n), memoria
// O(1).
static Node *find_node(const Tree *tree, int key) {
  Node *current = tree->root;
  while (current != &tree->nil && current->key != key)
    current = (key < current->key) ? current->left : current->right;
  // SI no lo encuentra retorna &tree->nil y si si retorna el nodo con el key
  // indicado
  return current;
}

// Consulta una clave sin modificar el árbol. Tiempo O(log n), memoria O(1).
bool rb_tree_contains(const Tree *tree, int key) {
  if (!tree)
    return false;
  Node *node = find_node(tree, key);
  if (node == &tree->nil)
    return false;
  return true;
}

// Obtiene el mínimo de un subárbol no vacío. Tiempo O(log n), memoria O(1).
static Node *minimum(const Tree *tree, Node *node) {
  while (node->left != &tree->nil) {
    node = node->left;
  }
  return node;
}

// Rota a la izquierda y conserva parent y root. Tiempo y memoria O(1).
static void rotate_left(Tree *tree, Node *node) {
  Node *y = node->right;
  node->right = y->left;

  if (y->left != &tree->nil)
    y->left->parent = node;
  y->parent = node->parent;
  if (node->parent == &tree->nil)
    tree->root = y;
  else if (node == node->parent->left) {
    node->parent->left = y;
  } else {
    node->parent->right = y;
  }
  y->left = node;
  node->parent = y;
}

// Rota a la derecha y conserva parent y root. Tiempo y memoria O(1).
static void rotate_right(Tree *tree, Node *node) {
  Node *y = node->left;
  node->left = y->right;
  if (y->right != &tree->nil)
    y->right->parent = node;
  y->parent = node->parent;
  if (node->parent == &tree->nil)
    tree->root = y;
  else if (node == node->parent->left)
    node->parent->left = y;
  else
    node->parent->right = y;
  y->right = node;
  node->parent = y;
}

// Consulta el contador almacenado. Tiempo y memoria auxiliar O(1).
size_t rb_tree_size(const Tree *tree) {
  if (!tree)
    return 0U;
  return tree->size;
}

// Indica si el árbol no existe o su raíz es el centinela. Tiempo O(1).
bool rb_tree_empty(const Tree *tree) {
  return !tree || tree->root == &tree->nil;
}

// Calcula altura en aristas; el centinela vale -1. Tiempo O(n), memoria O(h).
static int node_height(const Tree *tree, const Node *node) {
  if (node == &tree->nil)
    return -1;
  int leftHeight = node_height(tree, node->left);
  int rightHeight = node_height(tree, node->right);
  return 1 + (rightHeight >= leftHeight ? rightHeight : leftHeight);
}

// Devuelve la altura del árbol; NULL y vacío valen -1. Tiempo O(n), memoria
// O(h).
int rb_tree_height(const Tree *tree) {
  if (!tree)
    return -1;
  return node_height(tree, tree->root);
}

static void insert_fixup(Tree *tree, Node *node) {
  while (node->parent->color == RED) {
    if (node->parent->parent->left == node->parent) {
      Node *uncle = node->parent->parent->right;
      if (uncle->color == RED) {
        uncle->color = BLACK;
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node->parent->right == node) {
          // Actualizamos para volver a ser hijos luego de la rotación
          // Y tener a nuestro padre y abuelo
          node = node->parent;
          rotate_left(tree, node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_right(tree, node->parent->parent);
      }
    } else {
      Node *uncle = node->parent->parent->left;
      if (uncle->color == RED) {
        uncle->color = BLACK;
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node->parent->left == node) {
          node = node->parent;
          rotate_right(tree, node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_left(tree, node->parent->parent);
      }
    }
  }
  // Colorear de negro en caso se use el 1er caso y quedemos en la raíz o
  // el nodo insertado sea la raíz
  tree->root->color = BLACK;
  // Realmente rotate ya hace esta tarea, pero por seguridad lo ponemos.
  // Sin embargo se puede omitir esta línea.
  tree->root->parent = &tree->nil;
}

int rb_tree_insert(Tree *tree, int key) {
  if (!tree)
    return -1;
  Node *parent = &tree->nil;
  Node *current = tree->root;
  // Buscamos al futuro padre del nuevo nodo a agregar
  while (current != &tree->nil) {
    parent = current;
    if (key == current->key)
      return 0;
    else if (key < current->key)
      current = current->left;
    else
      current = current->right;
  }
  Node *inserted = newNode(tree, parent, key);
  if (!inserted)
    return -1;
  if (parent == &tree->nil)
    tree->root = inserted;
  else if (key < parent->key)
    parent->left = inserted;
  else
    parent->right = inserted;

  tree->size++;
  insert_fixup(tree, inserted);
  return 1;
}

static void transplant(Tree *tree, Node *old_root, Node *new_root) {
  if (old_root->parent == &tree->nil)
    tree->root = new_root;
  else if (old_root == old_root->parent->left)
    old_root->parent->left = new_root;
  else
    old_root->parent->right = new_root;

  new_root->parent = old_root->parent;
}

static void erase_fixup(Tree *tree, Node *node) {
  while (node != tree->root && node->color == BLACK) {
    if (node == node->parent->left) {
      Node *sibling = node->parent->right;
      if (sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        rotate_left(tree, node->parent);
        sibling = node->parent->right;
      }
      if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
        sibling->color = RED;
        node = node->parent;
      } else {
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
        node = tree->root;
      }
    } else {
      Node *sibling = node->parent->left;
      if (sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        rotate_right(tree, node->parent);
        sibling = node->parent->left;
      }
      if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
        sibling->color = RED;
        node = node->parent;
      } else {
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
        node = tree->root;
      }
    }
  }
  node->color = BLACK;
}

bool rb_tree_erase(Tree *tree, int key) {
  Node *target;
  Node *moved;
  Node *replacement;
  Color removed_color;
  if (!tree)
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
  if (removed_color == BLACK)
    erase_fixup(tree, replacement);
  if (tree->root != &tree->nil)
    tree->root->parent = &tree->nil;
  tree->nil.parent = &tree->nil;
  tree->nil.color = BLACK;
  return true;
}

static bool validate_node(const Tree *tree, const Node *node,
                          const Node *expected_parent, long long lower,
                          long long upper, size_t *node_count,
                          int *black_height) {
  if (node == &tree->nil) {
    *black_height = 1;
    return true;
  }
  if (!node || node->parent != expected_parent || !node->left || !node->right)
    return false;
  if ((long long)node->key <= lower || (long long)node->key >= upper)
    return false;
  if (node->color == RED &&
      (node->left->color == RED || node->right->color == RED))
    return false;

  int left_black_height;
  int right_black_height;
  if (!validate_node(tree, node->left, node, lower, (long long)node->key,
                     node_count, &left_black_height))
    return false;
  if (!validate_node(tree, node->right, node, (long long)node->key, upper,
                     node_count, &right_black_height))
    return false;
  if (left_black_height != right_black_height)
    return false;

  ++*node_count;
  *black_height = left_black_height + (node->color == BLACK ? 1 : 0);
  return true;
}

bool rb_tree_valid(const Tree *tree) {
  if (!tree || tree->nil.color != BLACK || tree->nil.left != &tree->nil ||
      tree->nil.right != &tree->nil || tree->nil.parent != &tree->nil ||
      !tree->root)
    return false;
  if (tree->root == &tree->nil)
    return tree->size == 0U;
  if (tree->root->parent != &tree->nil || tree->root->color != BLACK)
    return false;

  size_t node_count = 0U;
  int black_height;
  return validate_node(tree, tree->root, &tree->nil, LLONG_MIN, LLONG_MAX,
                       &node_count, &black_height) &&
         node_count == tree->size;
}

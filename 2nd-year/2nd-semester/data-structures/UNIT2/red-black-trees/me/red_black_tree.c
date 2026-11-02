#include "red_black_tree.h"
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

// Libera recursivamente los nodos reales en postorden. Tiempo O(n), memoria O(h).
static void destroy_nodes(Tree *tree, Node *node) {
  if (node == &tree->nil)
    return;
  destroy_nodes(tree, node->left);
  destroy_nodes(tree, node->right);
  free(node);
}

// Libera los nodos y la estructura Tree; acepta NULL. Tiempo O(n), memoria O(h).
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

// Busca una clave y devuelve su nodo o el centinela. Tiempo O(log n), memoria O(1).
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

#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
  int key;
  Color color;
  struct Node *left;
  struct Node *right;
  struct Node *parent;
} Node;

typedef struct Tree {
  Node nil;
  Node *root;
  size_t size;
} Tree;

// Crea un árbol vacío con centinela negro. Tiempo y memoria auxiliar O(1).
Tree *rb_tree_create(void);

// Libera todos los nodos y el árbol. Tiempo O(n), memoria auxiliar O(h).
void rb_tree_destroy(Tree *tree);

// Inserta una clave: 1 éxito, 0 repetida, -1 error. Tiempo O(log n).
int rb_tree_insert(Tree *tree, int key);

// Elimina una clave si existe. Tiempo O(log n), memoria auxiliar O(1).
bool rb_tree_erase(Tree *tree, int key);

// Comprueba si una clave existe. Tiempo O(log n), memoria auxiliar O(1).
bool rb_tree_contains(const Tree *tree, int key);

// Verifica orden BST y propiedades rojinegras. Tiempo O(n), memoria O(h).
bool rb_tree_valid(const Tree *tree);

#endif

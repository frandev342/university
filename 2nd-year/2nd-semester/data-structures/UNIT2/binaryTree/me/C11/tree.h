#ifndef TREE_H
#define TREE_H

typedef struct Node {
  int data;
  struct Node *left;
  struct Node *right;
} Node;

// Crea un nodo sin hijos. Tiempo O(1), memoria O(1).
Node *newNode(int data);

// Inserta en el BST. Tiempo O(h), memoria auxiliar O(h) por recursión.
Node *insert(Node *root, int data);

// Libera todos los nodos en postorden. Tiempo O(n), memoria auxiliar O(h).
void freeTree(Node *root);

#endif

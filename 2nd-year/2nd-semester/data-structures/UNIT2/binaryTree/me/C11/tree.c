#include "tree.h"
#include <stdlib.h>

// Crea un nodo sin hijos. Tiempo O(1), memoria O(1).
Node *newNode(int data) {
  Node *p = malloc(sizeof(*p));
  if (!p)
    return NULL;
  p->left = NULL;
  p->right = NULL;
  p->data = data;
  return p;
}

// Inserta en el BST. Tiempo O(h), memoria auxiliar O(h) por recursión.
Node *insert(Node *root, int data) {
  if (!root)
    return newNode(data);
  if (data <= root->data) {
    Node *p = insert(root->left, data);
    if (p != NULL)
      root->left = p;
  } else {
    Node *p = insert(root->right, data);
    if (p != NULL)
      root->right = p;
  }
  return root;
}

// Libera todos los nodos en postorden. Tiempo O(n), memoria auxiliar O(h).
void freeTree(Node *root) {
  if (!root)
    return;
  freeTree(root->left);
  freeTree(root->right);
  free(root);
}

/*
Resumen de complejidades (n = nodos, h = altura):

Función    Qué hace                              Tiempo   Memoria auxiliar
newNode    Reserva e inicializa un nodo          O(1)     O(1)
insert     Inserta respetando el orden del BST   O(h)     O(h)
freeTree   Libera todos los nodos en postorden   O(n)     O(h)

En un BST balanceado h = O(log n); en uno degenerado h = O(n).
*/

#ifndef TREE_H
#define TREE_H

#include <stddef.h>

typedef struct node {
  int data;
  struct node *left;
  struct node *right;
} Node;

/*
 * Crea un nodo aislado.
 * Retorna NULL si no se pudo reservar memoria.
 */
Node *newNode(int data);

/*
 * Inserta data en un arbol binario de busqueda.
 *
 * Convencion:
 *   data <= root->data  -> subarbol izquierdo
 *   data >  root->data  -> subarbol derecho
 *
 * Retorna la nueva raiz del arbol.
 * Si la reserva de memoria falla, el arbol original se conserva y
 * se retorna NULL solo cuando se intento insertar en un arbol vacio.
 */
Node *insert(Node *root, int data);

/*
 * Libera todos los nodos del arbol.
 */
void freeTree(Node *root);

#endif

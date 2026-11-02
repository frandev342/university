#include "treelist.h"
#include <stdlib.h>

Node *newNode(int data) {
  Node *p = malloc(sizeof *p);
  if (p == NULL)
    return NULL;
  p->data = data;
  p->small = NULL;
  p->large = NULL;
  return p;
}

/* EJERCICIO 1. Inserte usando una referencia local al puntero
   que enlazara el nuevo nodo. Devuelva 1 si tuvo exito; 0
   si malloc falla. Los repetidos van al subarbol izquierdo. */
int treeInsert(Node **root, int data) {
  /* TODO 1 */
  if (*root == NULL) {
    *root = newNode(data);
    if (!*root)
      return 0;
    return 1;
  }
  Node *current = *root;
  if (data <= current->data)
    return (treeInsert(&(*root)->small, data));
  else
    return treeInsert(&(*root)->large, data);
}
// GDB para dibujar

/* EJERCICIO 2. Despues de join(a,b), b sigue a a, y a
   precede a b. Se asume a != NULL y b != NULL. */
void join(Node *a, Node *b) { /* TODO 2 */
  a->large = b;
  b->small = a;
}

/* EJERCICIO 3. Concatene dos listas DOBLEMENTE CIRCULARES.
   Devuelva el primer nodo de a, o de b si a esta vacia.
   Debe ejecutarse en O(1) y no reservar nodos. */
Node *append(Node *a, Node *b) {
  /* TODO 3 */
  if (!a && b)
    return b;
  // Esta segunda condicional es importante, porque se asume para Join que a y b
  // no son nulos
  if (a && !b)
    return a;
  if (!a && !b)
    return NULL;
  Node *endA = a->small;
  Node *endB = b->small;
  join(endA, b);
  join(endB, a);
  // endA->large = b;
  // b->small = endA;
  // endB->large = a;
  // a->small = endB;
  return a;
}

/* EJERCICIO 4. Convierta el ABB en lista circular ordenada,
   reusando exactamente los mismos nodos, en tiempo O(n).
   Devuelva el nodo de menor valor o NULL si no hay nodos. */
Node *treeToList(Node *root) {
  /* TODO 4 */
  if (!root)
    return NULL;
  // Piensa en lo más mínimo para resolver, si lo lees lo entiendes
  // La razón es porque está bien implementado, piensa un árbol pequeño de un
  // nodo Luego un de tres nodos. Todo se resuelve luego de eso.
  Node *leftList = treeToList(root->small);
  Node *rightList = treeToList(root->large);
  root->small = root;
  root->large = root;
  return append(append(leftList, root), rightList);
}

/* Son utilidades de infraestructura: no modificarlas. */
void freeTree(Node *root) {
  if (root == NULL)
    return;
  freeTree(root->small);
  freeTree(root->large);
  free(root);
}

void freeList(Node *head) {
  if (head == NULL)
    return;
  Node *p = head->large;
  while (p != head) {
    Node *next = p->large;
    free(p);
    p = next;
  }
  free(head);
}

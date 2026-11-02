#include "tree_algorithms.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

// Cuenta todos los nodos del árbol. Tiempo O(n), memoria auxiliar O(h).
int size(const Node *root) {
  if (!root)
    return 0;
  return 1 + size(root->left) + size(root->right);
}

// Calcula la altura en aristas; el árbol vacío vale -1. Tiempo O(n), memoria
// O(h).
int maxDepth(const Node *root) {
  if (!root)
    return -1;
  int leftHeight = maxDepth(root->left);
  int rightHeight = maxDepth(root->right);
  int maxHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
  return 1 + maxHeight;
}

// TODO: Para min y max, como precondición el árbol no está vacío y es un BST
// Obtiene el mínimo de un BST no vacío. Tiempo O(h), memoria auxiliar O(1).
int minValue(const Node *root) {
  while (root->left)
    root = root->left;
  return root->data;
}

// Obtiene recursivamente el mínimo de un BST no vacío. Tiempo y memoria O(h).
int minValueRec(const Node *root) {
  if (!root->left)
    return root->data;
  return minValueRec(root->left);
}

// Obtiene el máximo de un BST no vacío. Tiempo O(h), memoria auxiliar O(1).
int maxValue(const Node *root) {
  while (root->right)
    root = root->right;
  return root->data;
}

// Obtiene recursivamente el máximo de un BST no vacío. Tiempo y memoria O(h).
int maxValueRec(const Node *root) {
  if (!root->right)
    return root->data;
  return maxValueRec(root->right);
}

// Imprime raíz, izquierda y derecha. Tiempo O(n), memoria auxiliar O(h).
void printPreorder(const Node *root) {
  if (!root)
    return;
  printf("%d ", root->data);
  printPreorder(root->left);
  printPreorder(root->right);
}

// Imprime izquierda, raíz y derecha. Tiempo O(n), memoria auxiliar O(h).
void printInorder(const Node *root) {
  if (!root)
    return;
  printInorder(root->left);
  printf("%d ", root->data);
  printInorder(root->right);
}

// Imprime izquierda, derecha y raíz. Tiempo O(n), memoria auxiliar O(h).
void printPostorder(const Node *root) {
  if (!root)
    return;
  printPostorder(root->left);
  printPostorder(root->right);
  printf("%d ", root->data);
}

// Intercambia recursivamente ambos hijos. Tiempo O(n), memoria auxiliar O(h).
void mirror(Node *root) {
  if (!root)
    return;
  Node *temp = root->left;
  root->left = root->right;
  root->right = temp;
  mirror(root->left);
  mirror(root->right);
}

// Comprueba valores y estructura iguales. Tiempo O(n), memoria auxiliar O(h).
int sameTree(const Node *a, const Node *b) {
  // esta 1ra condición cumple que si son NULL a y b o son el mismo árbol desde
  // el comienzo
  if (a == b)
    return 1;
  if (!a || !b)
    return 0;
  return (a->data == b->data) && sameTree(a->left, b->left) &&
         sameTree(a->right, b->right);
}

// Cuenta los BST distintos con n claves. Tiempo O(n), memoria auxiliar O(n).
long long countTrees(int numKeys) {
  if (numKeys == 0)
    return 1;
  if (numKeys == 1)
    return 1;
  long long previous = countTrees(numKeys - 1);
  return previous * 2 * (2 * numKeys - 1) / (numKeys + 1);
}

// Valida cada subárbol con sus extremos. Tiempo O(nh), peor O(n^2); memoria
// O(h).
int isBST(const Node *root) {
  if (!root)
    return 1;
  if (root->left && maxValue(root->left) > root->data)
    return 0;
  if (root->right && minValue(root->right) <= root->data)
    return 0;
  return isBST(root->left) && isBST(root->right);
}

// Valida el BST propagando límites. Tiempo O(n), memoria auxiliar O(h).
static int isBSTRec(const Node *root, long long min, long long max) {
  if (!root)
    return 1;
  if (min >= root->data || max < root->data)
    return 0;
  return isBSTRec(root->left, min, root->data) &&
         isBSTRec(root->right, root->data, max);
}

// Inicia la validación por límites. Tiempo O(n), memoria auxiliar O(h).
int isBST2(const Node *root) { return isBSTRec(root, LLONG_MIN, LLONG_MAX); }

// Busca una suma en una ruta raíz-hoja. Tiempo O(n), memoria auxiliar O(h).
int hasPathSum(const Node *root, int sum) {
  if (!root)
    return (sum == 0);
  else {
    int newSum = sum - root->data;
    return hasPathSum(root->left, newSum) || hasPathSum(root->right, newSum);
  }
}

// Imprime una ruta de n valores. Tiempo O(n), memoria auxiliar O(1).
static void printArr(int arr[], int n) {
  for (int i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

// Recorre y acumula rutas raíz-hoja. Tiempo O(n + salida), memoria O(h).
static void printPathsRec(const Node *root, int paths[], int n) {
  if (!root) {
    return;
  }
  paths[n++] = root->data;
  if (root->left || root->right) {
    printPathsRec(root->left, paths, n);
    printPathsRec(root->right, paths, n);
  } else {
    printArr(paths, n);
  }
}

// Imprime todas las rutas raíz-hoja. Tiempo O(n + salida), memoria O(h).
void printPaths(const Node *root) {
  int paths[1000];
  printPathsRec(root, paths, 0);
}

// Duplica cada nodo como hijo izquierdo. Tiempo O(n), memoria auxiliar O(h).
void doubleTree(Node *root) {
  Node *oldLeft;
  if (!root)
    return;
  doubleTree(root->left);
  doubleTree(root->right);
  oldLeft = root->left;
  root->left = newNode(root->data);
  root->left->left = oldLeft;
}

/*
Resumen de complejidades (n = nodos, h = altura, s = salida impresa):

Función          Qué hace                                  Tiempo       Memoria auxiliar
size             Cuenta los nodos                          O(n)         O(h)
maxDepth         Calcula la altura en aristas               O(n)         O(h)
minValue         Obtiene el mínimo iterativamente           O(h)         O(1)
minValueRec      Obtiene el mínimo recursivamente           O(h)         O(h)
maxValue         Obtiene el máximo iterativamente           O(h)         O(1)
maxValueRec      Obtiene el máximo recursivamente           O(h)         O(h)
printPreorder    Imprime raíz, izquierda y derecha          O(n)         O(h)
printInorder     Imprime izquierda, raíz y derecha          O(n)         O(h)
printPostorder   Imprime izquierda, derecha y raíz          O(n)         O(h)
mirror           Intercambia los hijos de cada nodo         O(n)         O(h)
sameTree         Compara estructura y valores               O(n)         O(h)
countTrees       Calcula el número de Catalan                O(n)         O(n)
isBST            Valida usando extremos de subárboles       O(nh)        O(h)
isBSTRec/isBST2  Valida propagando límites                  O(n)         O(h)
hasPathSum       Busca una suma en una ruta raíz-hoja       O(n)         O(h)
printArr         Imprime una ruta                           O(h)         O(1)
printPaths       Imprime todas las rutas raíz-hoja          O(n + s)     O(h)
doubleTree       Duplica cada nodo como hijo izquierdo      O(n)         O(h) + O(n) creada

isBST puede llegar a O(n^2) en un árbol degenerado.
printPaths usa actualmente un arreglo fijo de 1000 posiciones.
*/

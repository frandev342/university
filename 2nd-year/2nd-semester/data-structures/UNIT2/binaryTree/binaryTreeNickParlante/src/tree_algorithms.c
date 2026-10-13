#include "../include/tree_algorithms.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Este archivo contiene los ejercicios de la practica.
 *
 * Complete cada funcion sin modificar su firma.
 * Las implementaciones iniciales permiten compilar el proyecto, pero
 * NO resuelven los ejercicios.
 */

// h: altura del árbol

size_t treeSize(const Node *root) {
  if (root == NULL)
    return 0;
  else
    return treeSize(root->left) + 1 + treeSize(root->right);
}

int maxDepth(const Node *root) {
  if (root == NULL)
    return 0;
  int leftDepth = maxDepth(root->left);
  int rightDepth = maxDepth(root->right);
  if (leftDepth > rightDepth)
    return 1 + leftDepth;
  else
    return 1 + rightDepth;
}

// RECURSIVE VERSION Tiempo O(h) y memoria auxiliar O(H)
// int minValue(const Node *root) {
//   if (root->left == NULL)
//     return root->data;
//   else
//     return minValue(root->left);
// }
// ITERATIVE VERSION TIempo O(h) y memoria auxiliar O(1)
int minValue(const Node *root) {
  while (root->left != NULL)
    root = root->left;
  return root->data;
}

int maxValue(const Node *root) {
  while (root->right != NULL)
    root = root->right;
  return root->data;
}
// Tiempo O(n) Memoria recursiva O(h) recorrido inorder
void printTree(const Node *root) {
  if (root == NULL)
    return;
  printTree(root->left);
  printf("%d ", root->data);
  printTree(root->right);
}
// printTree(2)
// ├── printTree(1)
// │   ├── izquierda: NULL
// │   ├── imprime 1
// │   └── derecha: NULL
// ├── imprime 2
// └── printTree(3)
//     ├── izquierda: NULL
//     ├── imprime 3
//     └── derecha: NULL

// Tiempo O(n) Memoria recursiva O(h)
void printPostorder(const Node *root) {
  if (root == NULL)
    return;
  printPostorder(root->left);
  printPostorder(root->right);
  printf("%d ", root->data);
}

// Tiempo O(n) y memoria euxiliar O(h)
int hasPathSum(const Node *root, int sum) {
  if (root == NULL)
    return 0;

  int remaining = sum - root->data;
  if (root->left == NULL && root->right == NULL)
    return remaining == 0;
  return hasPathSum(root->left, remaining) ||
         hasPathSum(root->right, remaining);
}

static void printArray(int arr[], int n) {
  for (int i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

static void printPathsRecur(const Node *root, int path[], int pathLen) {
  if (root == NULL)
    return;
  path[pathLen] = root->data;
  pathLen++;
  if (root->left != NULL || root->right != NULL) {
    printPathsRecur(root->left, path, pathLen);
    printPathsRecur(root->right, path, pathLen);
  } else {
    printArray(path, pathLen);
  }
}

void printPaths(const Node *root) {
  if (root == NULL)
    return;
  int path[1000];
  printPathsRecur(root, path, 0);
}

// Tiempo O(n) y memoria auxiliar O(h)
void mirror(Node *root) {
  if (root == NULL)
    return;
  Node *temp = root->left;
  root->left = root->right;
  root->right = temp;
  mirror(root->left);
  mirror(root->right);
}

int sameTree(const Node *a, const Node *b) {
  if (a == b)
    return 1;
  if (a == NULL || b == NULL)
    return 0;
  return (a->data == b->data) && sameTree(a->left, b->left) &&
         sameTree(a->right, b->right);
}

// TIEMPO O(n^2) y memoria auxiliar O(h)
int isBST(const Node *root) {
  if (root == NULL)
    return 1;
  if (root->left != NULL && maxValue(root->left) > root->data)
    return 0;
  if (root->right != NULL && minValue(root->right) <= root->data)
    return 0;
  if (!isBST(root->left) || !isBST(root->right))
    return 0;
  return 1;
}

// FUNCIÓN AUXILIAR, Tiempo O(n), memoria recursiva O(h)
static int isBSTUtil(const Node *root, long long lowerExclusive,
                     long long upperInclusive) {
  if (root == NULL)
    return 1;
  if (root->data <= lowerExclusive || root->data > upperInclusive)
    return 0;
  return isBSTUtil(root->left, lowerExclusive, root->data) &&
         isBSTUtil(root->right, root->data, upperInclusive);
}

// - El límite inferior es exclusivo.
// - El límite superior es inclusivo.
int isBST2(const Node *root) { return isBSTUtil(root, LLONG_MIN, LLONG_MAX); }

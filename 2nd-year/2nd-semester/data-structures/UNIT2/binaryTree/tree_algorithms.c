#include "tree_algorithms.h"
#include "tree.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

void printInorder(const Node *root) {
  if (root == NULL)
    return;
  printInorder(root->left);
  printf("%d ", root->data);
  printInorder(root->right);
}

void printPostorder(const Node *root) {
  if (root == NULL)
    return;
  printPostorder(root->left);
  printPostorder(root->right);
  printf("%d ", root->data);
}

int size(const Node *root) {
  if (root == NULL)
    return 0;
  return 1 + size(root->left) + size(root->right);
}

// Tiempo O(n) Memoria auxiliar O(h)
int maxDepth(const Node *root) {
  if (root == NULL)
    return -1;
  int leftDepth = maxDepth(root->left);
  int rightDepth = maxDepth(root->right);
  if (leftDepth > rightDepth)
    return 1 + leftDepth;
  else
    return 1 + rightDepth;
}

// Dado un árbol no vacío obtenemos un Node
// Tiempo O(h) Memoria auxiliar O(1)
int minValue(const Node *root) {
  while (root->left != NULL)
    root = root->left;
  return root->data;
}
// Tiempo O(h) y Memoria auxiliar O(h)
int minValueRec(const Node *root) {
  if (root->left == NULL)
    return root->data;
  return minValueRec(root->left);
}

int maxValue(const Node *root) {
  while (root->right != NULL)
    root = root->right;
  return root->data;
}

// El árbol tiene una ruta de raíz a una hoja donde sus nodos den como suma sum?
// Tiempo O(h) Memoria Auxiliar (h)
int hasPathSum(const Node *root, int sum) {
  if (root == NULL)
    return sum == 0;
  int newSum = sum - root->data;
  return hasPathSum(root->left, newSum) || hasPathSum(root->right, newSum);
}

static void printArr(const int arr[], int n) {
  for (int i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

static void printPathsRec(const Node *root, int arr[], int size) {
  if (root == NULL)
    return;
  arr[size++] = root->data;
  if (root->left != NULL || root->right != NULL) {
    printPathsRec(root->left, arr, size);
    printPathsRec(root->right, arr, size);
  } else {
    printArr(arr, size);
  }
}

// Tiempo O(n) Pila recursiva O(h)
void printPaths(const Node *root) {
  int arr[1000];
  printPathsRec(root, arr, 0);
}

// Tiempo O(n) y memoria auxiliar O(h)
// Es un reverse o para árboles un espejo
// Recorrido Preorden
void mirror(Node *root) {
  if (root == NULL)
    return;
  Node *temp = root->left;
  root->left = root->right;
  root->right = temp;
  mirror(root->left);
  mirror(root->right);
}

// Tiempo O(n) Pila recursiva O(h)
int sameTree(const Node *a, const Node *b) {
  if (a == b)
    return 1;
  if (a == NULL || b == NULL)
    return 0;
  return (a->data == b->data) && sameTree(a->left, b->left) &&
         sameTree(a->right, b->right);
}

int countTrees(int numKeys) {
  if (numKeys < 0)
    return 0;
  if (numKeys <= 1)
    return 1;
  else {
    int sum = 0;
    int left, right, root;
    for (root = 1; root <= numKeys; root++) {
      left = countTrees(root - 1);
      right = countTrees(numKeys - root);
      sum += left * right;
    }
    return sum;
  }
}

// Tiempo O(n) Pila recursiva O(n)
long long countTrees2(int numKeys) {
  if (numKeys == 0)
    return 1;
  if (numKeys == 1)
    return 1;
  long long previous = countTrees2(numKeys - 1);
  return previous * 2 * (2 * numKeys - 1) / (numKeys + 1);
}

static int isBSTRec(const Node *root, long long min, long long max) {
  if (root == NULL)
    return 1;
  if (root->data <= min || root->data > max)
    return 0;
  return isBSTRec(root->left, min, root->data) &&
         isBSTRec(root->right, root->data, max);
}
int isBST(const Node *root) { return isBSTRec(root, LLONG_MIN, LLONG_MAX); }

// Tiempo O(n^2) y Pila recurisva O(h)
int isBSTv2(const Node *root) {
  if (root == NULL)
    return 1;
  if (root->right != NULL && minValue(root->right) <= root->data)
    return 0;
  if (root->left != NULL && maxValue(root->left) > root->data)
    return 0;
  return isBSTv2(root->left) && isBSTv2(root->right);
}

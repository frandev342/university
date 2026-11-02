#include "tree.h"
#include "tree_algorithms.h"
#include <stdio.h>
#include <stdlib.h>

static Node *buildExample(void) {
  const int values[] = {5, 3, 9, 1, 4, 6};
  const size_t n = sizeof values / sizeof values[0];
  Node *root = NULL;

  for (size_t i = 0; i < n; i++) {
    Node *newRoot = insert(root, values[i]);
    if (newRoot == NULL) {
      freeTree(root);
      return NULL;
    }
    root = newRoot;
  }
  return root;
}

// Devuelve 1 si encuentra el target, si no 0
// Tiempo O(h) Memoria recursiva O(h) Árbol balanceado (log n) Árbol degenerado
// (n)
static int lookup(const Node *root, int target) {
  if (root == NULL)
    return 0;
  if (root->data > target)
    return lookup(root->left, target);
  else if (root->data < target)
    return lookup(root->right, target);
  return 1;
}
// Devuelve 1 si encuentra el target, si no 0
// Tiempo O(h) Memoria recursiva O(1) Árbol balanceado (log n) Árbol degenerado
// (n)
static int lookup2(const Node *root, int target) {
  while (root != NULL) {
    if (target == root->data)
      return 1;
    if (target < root->data)
      root = root->left;
    else {
      root = root->right;
    }
  }
  return 0;
}

int main() {
  Node *root = buildExample();
  Node *root1 = buildExample();

  printInorder(root);
  printf("\n");
  printf("LookUp recursive: %d\n", lookup(root, 6));
  printf("LookUp iterative: %d\n", lookup2(root, 6));
  insert(root, 10);
  printf("Size: %d\n", size(root));
  printf("MaxDepth: %d\n", maxDepth(root));
  printf("MinValue: %d\n", minValueRec(root));
  printf("HasPathSum: %d\n", hasPathSum(root, 11));
  printf("Print paths: \n");
  printPaths(root);
  // MIRROR -> más ejercicios
  mirror(root);
  printInorder(root);
  printf("\n");

  printf("SameTree: %d\n", sameTree(root, root1));
  insert(root1, 10);
  mirror(root1);
  printf("SameTree: %d\n", sameTree(root, root1));

  printf("countTrees: %lld\n", countTrees2(size(root)));
  mirror(root1);
  printf("isBST: %d\n", isBSTv2(root));
  freeTree(root);
  freeTree(root1);
  return 0;
}

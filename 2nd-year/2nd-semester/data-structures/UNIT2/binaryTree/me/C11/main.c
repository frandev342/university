#include "tree.h"
#include "tree_algorithms.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Construye un BST con n valores. Tiempo O(nh), peor caso O(n^2); memoria O(n).
static Node *buildTree(void) {
  int values[] = {5, 9, 15, 3, 2, 6, 1};
  int n = sizeof(values) / sizeof(values[0]);
  Node *root = NULL;
  for (int i = 0; i < n; ++i) {
    Node *newRoot = insert(root, values[i]);
    if (!newRoot) {
      freeTree(root);
      return NULL;
    }
    root = newRoot;
  }
  return root;
}

// Busca recursivamente en el BST. Tiempo O(h), memoria auxiliar O(h).
static int lookup(const Node *root, int target) {
  if (!root)
    return 0;
  if (target < root->data)
    return lookup(root->left, target);
  else if (target > root->data)
    return lookup(root->right, target);
  return 1;
}

// Busca iterativamente en el BST. Tiempo O(h), memoria auxiliar O(1).
static int lookup2(const Node *root, int target) {
  while (root != NULL) {
    if (target == root->data)
      return 1;
    if (target <= root->data)
      root = root->left;
    else
      root = root->right;
  }
  return 0;
}

// Construye, consulta y libera el ejemplo. Tiempo O(nh), memoria O(n).
int main(void) {
  Node *root = buildTree();
  Node *root1 = buildTree();
  Node *root2 = buildTree();
  printf("Recursive lookup: %d\n", lookup(root, 2));
  printf("Iterative lookup: %d\n", lookup2(root, 2));
  printf("Size root: %d\n", size(root));
  printf("MaxDepth: %d\n", maxDepth(root));
  printf("Preorder: ");
  printPreorder(root);
  printf("\n");
  printf("Inorder: ");
  printInorder(root);
  printf("\n");
  printf("Postorder: ");
  printPostorder(root);
  printf("\n");

  // Mirror
  mirror(root);
  printf("Inorder: ");
  printInorder(root);
  printf("\n");

  // SameTree
  printf("SameTree: %d\n", sameTree(root, root1));
  printf("SameTree: %d\n", sameTree(root1, root2));

  // isBST
  printf("isBST: %d\n", isBST(root));
  printf("isBST: %d\n", isBST(root1));
  printf("isBST: %d\n", isBST2(root1));

  // countTrees
  printf("Count Trees: %lld\n", countTrees(size(root1)));

  // hastPathSumn
  printf("Has Path Sum: %d\n", hasPathSum(root, 11));
  // printPaths
  printf("Print all paths: \n");
  printPaths(root1);

  // doubleTree
  doubleTree(root1);
  printInorder(root1);

  freeTree(root);
  freeTree(root1);
  freeTree(root2);
  return 0;
}

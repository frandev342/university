#include "../include/tree.h"
#include "../include/tree_algorithms.h"
#include <stdio.h>
#include <stdlib.h>

static Node *buildExample(void) {
  const int values[] = {5, 3, 9, 1, 4, 6};
  const size_t n = sizeof values / sizeof values[0];

  Node *root = NULL;

  for (size_t i = 0; i < n; ++i) {
    Node *newRoot = insert(root, values[i]);

    if (newRoot == NULL) {
      freeTree(root);
      return NULL;
    }

    root = newRoot;
  }

  return root;
}

static Node *build123(void) {
  const int values[] = {2, 3, 1};
  const size_t n = sizeof values / sizeof values[0];
  Node *root = NULL;

  for (size_t i = 0; i < n; ++i) {
    Node *newRoot = insert(root, values[i]);

    if (newRoot == NULL) {
      freeTree(root);
      return NULL;
    }

    root = newRoot;
  }

  return root;
}

int main(void) {
  Node *root = build123();
  Node *root1 = build123();
  Node *root2 = build123();
  Node *root3 = buildExample();

  if (root == NULL) {
    fprintf(stderr, "No se pudo construir el arbol.\n");
    return EXIT_FAILURE;
  }

  //
  // Active estas pruebas a medida que implemente las funciones.

  printf("treeSize = %zu\n", treeSize(root));
  printf("maxDepth = %d\n", maxDepth(root));
  printf("minValue = %d\n", minValue(root));
  printf("inorder: ");
  printTree(root);
  printf("\n");

  printf("postorder: ");
  printPostorder(root);
  printf("\n");
  printf("hasPathSum: %d\n", hasPathSum(root, 1));
  printf("inorder Mirror: ");
  mirror(root);
  printTree(root);
  printf("\n");

  printf("Are they the same trees?: %d\n", sameTree(root, root1));
  printf("Are they the same trees?: %d\n", sameTree(root1, root2));
  printf("Print all tree paths: \n");
  printPaths(root3);
  printf("Is the tree a BST?: %d\n", isBST2(root1));

  freeTree(root);
  freeTree(root1);
  freeTree(root2);
  freeTree(root3);

  return EXIT_SUCCESS;
}

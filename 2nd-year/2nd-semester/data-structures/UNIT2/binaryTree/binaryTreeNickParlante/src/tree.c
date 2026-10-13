#include "../include/tree.h"
#include <stdlib.h>

Node *newNode(int data) {
  Node *p = malloc(sizeof *p);
  if (p == NULL)
    return NULL;

  p->data = data;
  p->left = NULL;
  p->right = NULL;
  return p;
}

Node *insert(Node *root, int data) {
  if (root == NULL)
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

void freeTree(Node *root) {
  if (root == NULL)
    return;

  freeTree(root->left);
  freeTree(root->right);
  free(root);
}

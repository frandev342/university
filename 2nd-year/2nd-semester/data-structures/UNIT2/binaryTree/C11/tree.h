#ifndef TREE_H
#define TREE_H

typedef struct node {
  int data;
  struct node *left;
  struct node *right;
} Node;

Node *newNode(int data);

Node *insert(Node *root, int data);

void freeTree(Node *root);

#endif

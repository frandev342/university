#ifndef TREELIST_H
#define TREELIST_H

typedef struct node {
  int data;
  struct node *small;
  struct node *large;
} Node;

/* Arbol: small=izquierdo, large=derecho.
   Lista: small=anterior, large=siguiente. */
Node *newNode(int data);
int treeInsert(Node **root, int data);
void join(Node *a, Node *b);
Node *append(Node *a, Node *b);
Node *treeToList(Node *root);
void freeTree(Node *root);
void freeList(Node *head);

#endif

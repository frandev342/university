#ifndef _NODE_
#define _NODE_

struct Node {
  int data;
  struct Node *next;
};

struct Node *buildOneTwoThree();
int count(struct Node *, int);

int getNth(struct Node *, int);

void DeleteList(struct Node **);
#endif

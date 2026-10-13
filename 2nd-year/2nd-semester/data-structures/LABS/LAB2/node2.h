#ifndef _NODE_
#define _NODE_

struct Node {
  int data;
  struct Node *next;
};

int length(struct Node *head);
void printL(struct Node *head);
struct Node *range(int start, int stop, int step);
int getNth(struct Node *head, int index);
int popFront(struct Node **head);
void pushFront(struct Node **head, int value);
void reverse(struct Node **head);
#endif // !

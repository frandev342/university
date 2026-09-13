#ifndef _NODE_
#define _NODE_

struct Node {
  int data;
  struct Node *next;
};

int length(struct Node *);
struct Node *BuildOneTwoThree();
void Push(struct Node **, int);
void ChangeToNull(struct Node **);
struct Node *AddAtHead();
int count(struct Node *, int);
int getNth(struct Node *, int);
void DeleteList(struct Node **);
int pop(struct Node **);
void InsertNth(struct Node **, int, int);
void printList(struct Node *);
struct Node *Range(int, int, int);
void pushBack(struct Node **, int);
void SortedInsert(struct Node **, struct Node *);
void InsertSort(struct Node **);
#endif // !

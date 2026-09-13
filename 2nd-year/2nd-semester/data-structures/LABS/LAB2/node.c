#include "node.h"
#include <assert.h>
#include <stdlib.h>

struct Node *buildOneTwoThree() {
  struct Node *head = malloc(sizeof(struct Node));
  head->data = 1;
  head->next = malloc(sizeof(struct Node));
  head->next->data = 2;
  head->next->next = malloc(sizeof(struct Node));
  head->next->data = 3;
  head->next->next = NULL;
  return head;
}

int count(struct Node *head, int searchFor) {
  int count = 0;
  struct Node *curr = head;
  while (curr) {
    if (curr->data == searchFor) {
      count++;
    }
    curr = curr->next;
  }
  return count;
}

int getNth(struct Node *head, int idx) {
  // assert sirve para acabar el programa si no cumple está condición
  assert(idx < length(head));
  struct Node *curr = head;
  int i = 0;
  while (i < idx) {
    curr = curr->next;
    i++;
  }
  return curr->data;
}

void DeleteList(struct Node **headRef) {
  struct Node *curr = *headRef;
  struct Node *next = NULL;
  while (curr) {
    next = (*headRef)->next;
    free(curr);
    curr = (*headRef)->next;
  }
  *headRef = NULL;
}

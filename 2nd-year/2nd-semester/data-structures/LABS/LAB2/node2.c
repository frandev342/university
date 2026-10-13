#include "node2.h"
#include "node_.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int length(struct Node *head) {
  struct Node *current = head;
  int c = 0;
  while (current != NULL) {
    current = current->next;
    c++;
  }
  return c;
}

void printL(struct Node *head) {
  while (head != NULL) {
    printf("%d ", head->data);
    head = head->next;
  }
  printf("\n");
}

struct Node *range(int start, int stop, int step) {
  struct Node dummy = {0, NULL};
  struct Node *current = &dummy;
  for (int i = start; i < stop; i += step) {
    current->next = malloc(sizeof(struct Node));
    current->next->data = i;
    current = current->next;
  }
  current->next = NULL;
  return dummy.next;
}

int getNth(struct Node *head, int index) {
  while (index-- > 0)
    head = head->next;
  return head->data;
}

int popFront(struct Node **head) {
  struct Node *victim = *head;
  int value = victim->data;
  *head = (*head)->next;
  free(victim);
  return value;
}

void pushFront(struct Node **head, int value) {
  struct Node *newNode = malloc(sizeof(struct Node));
  newNode->data = value;
  newNode->next = *head;
  *head = newNode;
}

void reverse(struct Node **head) {
  assert(*head != NULL);
  struct Node *prev = *head;
  struct Node *current = (*head)->next;
  struct Node *next = NULL;
  prev->next = NULL;
  while (current != NULL) {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }
  current = prev;
  *head = current;
}

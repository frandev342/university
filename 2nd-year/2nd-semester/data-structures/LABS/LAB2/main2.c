#include "node2.h"
#include <stdio.h>

int main() {
  struct Node *list1 = range(10, 30, 1);
  printL(list1);
  printf("%d\n", length(list1));
  printf("%d\n", getNth(list1, 10));
  printf("%d\n", getNth(list1, 0));
  printf("%d\n", popFront(&list1));
  printL(list1);
  pushFront(&list1, 40);
  printL(list1);
  reverse(&list1);
  printL(list1);
  return 0;
}

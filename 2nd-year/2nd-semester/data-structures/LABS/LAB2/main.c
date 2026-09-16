#include "node_.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Importante inicializar en NULL, porque eso indica que no apunta a nada
  // Y cuando haga un push, se insertará correctamente copiando el NULL en el
  // nuevo Node
  struct Node *LinkedList = NULL; //=(struct Node *)malloc(sizeof(struct Node));
  Push(&LinkedList, 2);
  Push(&LinkedList, 1);
  printf("%d\n", LinkedList->data);
  printf("%d\n", LinkedList->next->data);
  // printf("%d\n", length(LinkedList));
  //  pop(&LinkedList);
  // printf("%d\n", length(LinkedList));
  InsertNth(&LinkedList, 1, 5);
  printf("%d\n", LinkedList->next->data);
  printf("%d\n", length(LinkedList));

  printList(LinkedList);
  InsertNth(&LinkedList, 1, 10);
  printList(LinkedList);
  pushBack(&LinkedList, 20);
  printList(LinkedList);

  // TODO: 2ND LIST
  struct Node *LinkedList2 = Range(4, 20, 2);
  // InsertNth(&LinkedList2, 4, 100);
  printList(LinkedList2);

  struct Node *newN = malloc(sizeof(struct Node));
  newN->data = 1;
  newN->next = NULL;
  printf("%d\n", length(newN));
  printf("%p\n", newN->next);
  SortedInsert(&LinkedList2, newN);
  printList(LinkedList2);

  // TODO: 3RD LIST
  struct Node *LinkedList3 = NULL;
  Push(&LinkedList3, 5);
  Push(&LinkedList3, 3);
  Push(&LinkedList3, 1);
  Push(&LinkedList3, 10);
  Push(&LinkedList3, 9);
  printList(LinkedList3);
  InsertSort(&LinkedList3);
  printList(LinkedList3);

  // Añadir una lista dentro de otra
  Append(&LinkedList2, &LinkedList3);
  printList(LinkedList2);
  InsertSort(&LinkedList2);
  printList(LinkedList2);

  RemoveDuplicates(LinkedList2);
  printList(LinkedList2);
  printList(LinkedList);
  // TODO: Copiar lista dentro de otras
  // Prueba cubriendo el único caso particular
  struct Node *LinkedList4 = CopyList(LinkedList2);
  printList(LinkedList4);

  // Utilizando otra implementación de SortedInsert con dummy
  printf("---------\n");
  struct Node *node4 = malloc(sizeof(struct Node));
  node4->data = -12;
  node4->next = NULL;
  SortedInsert(&LinkedList4, node4);
  printList(LinkedList4);
  printList(LinkedList2);
  MoveNode(&LinkedList4, &LinkedList2);
  printList(LinkedList4);
  printList(LinkedList2);

  return 0;
}

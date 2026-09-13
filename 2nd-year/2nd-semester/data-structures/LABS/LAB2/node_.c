#include "node_.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Obtener el tamaño de la lista enlazada
int length(struct Node *head) {
  int count = 0;
  struct Node *current = head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

// CAMBIAMOS A NULL el puntero que apunta a un Node
// Es necesario usar ** para usar un puntero para modificar el puntero.
// Si no modificaríamos a una copia del puntero
void ChangeToNull(struct Node **headRef) { *headRef = NULL; }

struct Node *BuildOneTwoThree() { return NULL; }

struct Node *AddAtHead() {
  struct Node *head = NULL;
  int i = 0;
  for (i = 1; i < 6; i++)
    // push(&head, i);
    return head;
  // head == {5, 4, 3, 2, 1}
  return head;
}

int count(struct Node *head, int x) {
  int count = 0;
  struct Node *current = head;

  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

int getNth(struct Node *head, int idx) {
  // TODO: assert en C++ es una macro de depuración que verifica
  // si una condición es verdadera y detiene el programa si resulta falsa.
  assert(idx < length(head) && idx >= 0);
  struct Node *current = head;
  int i = 0;
  while (i < idx) {
    current = current->next;
    i++;
  }
  return current->data;
}

void DeleteList(struct Node **headRef) {
  // headRef -> head -> [node1] -> [node2] -> [node3] -> NULL
  struct Node *current = *headRef;
  struct Node *next = NULL;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  // Liberamos la memoria de node1, node2 y node3, pero
  // head sigue accediando al primer nodo, a memoria prohibida, asi que lo
  // establecemos en NULL Si no lo eliminaramos, length mostraría 1 y head daría
  // segmentation fault por acceder a memoria prohibida.
  *headRef = NULL;
}

int pop(struct Node **headRef) {
  if (headRef != NULL && *headRef != NULL) {
    struct Node *current = *headRef;
    int value = current->data;
    *headRef = (*headRef)->next;
    free(current);
    return value;
  }
  return -1;
}

void Push(struct Node **headRef, int x) {
  struct Node *newNode = malloc(sizeof(struct Node));
  newNode->data = x;
  newNode->next = *headRef;
  *headRef = newNode;
}

// Insertar nodo en posición idx
// void InsertNth(struct Node **headRef, int idx, int x) {
//   // Verificamos que el idx no sea superior a length
//   assert(idx < length(*headRef) && idx >= 0);
//   // Nodo que captará el nodo anterior a la posición pedida
//   struct Node *beforeNode = *headRef;
//   struct Node *currentNode = NULL;
//   struct Node *newNode = malloc(sizeof(struct Node));
//   int i = 0;
//   while (i < idx - 1) {
//     i++;
//     beforeNode = beforeNode->next;
//   }
//   // el nodo actual se moverá adelante permitiendo que el nodo nuevo
//   // ocupe su espacio
//   currentNode = beforeNode->next;
//   beforeNode->next = newNode;
//
//   newNode->data = x;
//   newNode->next = currentNode;
// }

void InsertNth(struct Node **headRef, int idx, int x) {
  if (idx == 0)
    Push(headRef, x);
  else {
    struct Node *current = *headRef;
    for (int i = 0; i < idx - 1; i++) {
      // Necesario usar esta forma y no current->next
      // para evitar los casos de no poder insertar en idx=2 en
      // una lista de dos elementos.
      assert(current != NULL);
      current = current->next;
    }
    assert(current != NULL);
    Push(&(current->next), x);
  }
}

void printList(struct Node *head) {
  struct Node *current = head;
  while (current != NULL) {
    printf("%d ", current->data);
    current = current->next;
  }
  printf("\n");
}

// No podemos usar pushBack
// Aumentaría el costo empleado
struct Node *Range(int start, int stop, int step) {
  struct Node *head = NULL;
  struct Node *tail;
  int i;
  // Cubrimos el único caso diferente a los demás
  Push(&head, start);
  tail = head;

  for (i = start + step; i < stop; i += step) {
    Push(&(tail->next), i);
    tail = tail->next;
  }
  return head;
}

// Agregar un elemento al final
void pushBack(struct Node **headRef, int x) {
  struct Node *newNode = malloc(sizeof(struct Node));
  newNode->data = x;
  newNode->next = NULL;
  if (*headRef == NULL) {
    *headRef = newNode;
    return;
  } else {
    struct Node *current = *headRef;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNode;
  }
}

void SortedInsert(struct Node **headRef, struct Node *newNode) {
  if (*headRef == NULL || ((*headRef)->data >= newNode->data)) {
    newNode->next = *headRef;
    *headRef = newNode;
    return;
  } else {
    struct Node *current = *headRef;
    while (current->next != NULL && current->next->data < newNode->data) {
      current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
  }
}

void InsertSort(struct Node **headRef) {
  struct Node *result = NULL;
  struct Node *current = *headRef;
  struct Node *next = current;
  while (current != NULL) {
    next = current->next;
    SortedInsert(&result, current);
    current = next;
  }
  *headRef = result;
}

// Añadir una lista dentro de otra, dejando la 2da lista en NULL
void Append(struct Node **aRef, struct Node **bRef) {
  struct Node *aCurrent = *aRef;
  // Avanzamos hasta llegar al últimmo elemento de A
  while (aCurrent->next != NULL) {
    aCurrent = aCurrent->next;
  }
  // Continuamos la lista A apuntando al primer elemento de B
  aCurrent->next = *bRef;
  // Dejamos en nulo a B
  *bRef = NULL;
}

void RemoveDuplicates(struct Node *head) {
  if (head == NULL)
    return;
  struct Node *current = head->next;
  struct Node *previousNode = head;
  while (current != NULL) {
    if (current->data != previousNode->data) {
      previousNode = current;
      current = current->next;
    } else {
      previousNode->next = current->next;
      free(current);
      current = previousNode->next;
    }
  }
}

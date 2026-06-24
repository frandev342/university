#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  char customer[40];
  char dish[20];
  int arrivalOrder;
} Order;

typedef int (*Comparator)(const void *x, const void *y);

int compareOrders(const void *x, const void *y);
void insertSort(void *base, int n, int size, Comparator cmp);
void swapBytes(void *x, void *y, int size);

int main() {
  int n;
  do {
    scanf("%d", &n);
  } while (!(n >= 1 && n <= 1000));

  Order *arr;
  arr = (Order *)malloc(n * sizeof(Order));

  for (int i = 0; i < n; i++) {
    scanf("%s %s", arr[i].customer, arr[i].dish);
    for (int j = 0; (arr[i].dish)[j] != '\0'; j++)
      (arr[i].dish)[j] = toupper((arr[i].dish)[j]);
    arr[i].arrivalOrder = i;
  }
  insertSort(arr, n, sizeof(Order), compareOrders);

  // Imprimir el orden de servicio
  for (int i = 0; i < n; i++) {
    printf("%s %s %d\n", arr[i].customer, arr[i].dish, arr[i].arrivalOrder);
  }

  return 0;
}

int compareOrders(const void *x, const void *y) {
  const Order *a = (const Order *)x;
  const Order *b = (const Order *)y;

  char *c[] = {"MEAT", "FISH", "DESSERT", "DRINK"};
  // BUSCAMOS ORDEN POR TIPO DE PLATO
  int index_a = -1, index_b = -1, i;
  for (i = 0; i < 4; i++) {
    if (strcmp(a->dish, c[i]) == 0)
      index_a = i;
  }
  for (i = 0; i < 4; i++) {
    if (strcmp(b->dish, c[i]) == 0)
      index_b = i;
  }

  if (index_a != index_b)
    return index_a - index_b;

  return a->arrivalOrder - b->arrivalOrder;
}

void insertSort(void *base, int n, int size, Comparator cmp) {
  unsigned char *a = (unsigned char *)base;
  for (int i = 1; i < n; i++) {
    int j = i;
    while (j > 0 && cmp(a + j * size, a + (j - 1) * size) < 0) {
      swapBytes(a + j * size, a + (j - 1) * size, size);
      j--;
    }
  }
}

void swapBytes(void *x, void *y, int size) {
  unsigned char *a = x;
  unsigned char *b = y;
  for (int i = 0; i < size; i++) {
    unsigned char temp = *(a + i);
    *(a + i) = *(b + i);
    *(b + i) = temp;
  }
}

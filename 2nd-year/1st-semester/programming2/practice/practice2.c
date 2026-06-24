#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*Comparator)(const void *x, const void *y);

typedef struct {
  char name[40];
  long long value;
  long long weigth;
  long long rareness;
  // ratio value/peso, mientras más alto la taza que sea primero
} Treasure;

void swapbytes(void *x, void *y, int size);
void insertionSort(void *base, int n, int size, Comparator cmp);

// CRITERIOS PARA ORDENAR
int compareByValue(const void *x, const void *y) {
  const Treasure *a = x;
  const Treasure *b = y;
  return b->value - a->value;
}
int compareByWeigth(const void *x, const void *y) {
  const Treasure *a = x;
  const Treasure *b = y;
  return a->weigth - b->value;
}

int compareByRareness(const void *x, const void *y) {
  const Treasure *a = x;
  const Treasure *b = y;
  return b->rareness - a->rareness;
}
int compareByRatio(const void *x, const void *y) {
  const Treasure *a = x;
  const Treasure *b = y;
  return b->value * a->weigth - a->value * b->weigth;
}

void printArr(const void *x, int n) {
  const Treasure *arr = (const Treasure *)x;
  for (int i = 0; i < n; i++)
    printf("%s %lld %lld %lld\n", arr[i].name, arr[i].value, arr[i].weigth,
           arr[i].rareness);
}
// MAIN PRINCIPAL
int main() {
  int n;
  do {
    scanf("%d", &n);
  } while (!(n >= 1 && n <= 1000));

  Treasure *arr;
  arr = (Treasure *)malloc(n * sizeof(Treasure));

  for (int i = 0; i < n; i++)
    scanf("%s %lld  %lld %lld", arr[i].name, &arr[i].value, &arr[i].weigth,
          &arr[i].rareness);

  // ORDENAR POR...
  char strategy[30];
  scanf("%s", strategy);
  for (int i = 0; strategy[i] != '\0'; i++)
    strategy[i] = toupper(strategy[i]);

  if (strcmp(strategy, "VALUE") == 0)
    insertionSort(arr, n, sizeof(Treasure), compareByValue);
  else if (strcmp(strategy, "WEIGTH") == 0)
    insertionSort(arr, n, sizeof(Treasure), compareByRareness);
  else if (strcmp(strategy, "RARENESS") == 0)
    insertionSort(arr, n, sizeof(Treasure), compareByRareness);
  else if (strcmp(strategy, "RATIO") == 0)
    insertionSort(arr, n, sizeof(Treasure), compareByRatio);

  printArr(arr, n);

  return 0;
}

void swapbytes(void *x, void *y, int size) {
  unsigned char *a = (unsigned char *)x;
  unsigned char *b = (unsigned char *)y;
  for (int i = 0; i < size; i++) {
    unsigned char temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}

void insertionSort(void *base, int n, int size, Comparator cmp) {
  unsigned char *a = (unsigned char *)base;
  for (int i = 1; i < n; i++) {
    int j = i;
    while (j > 0 && cmp(a + j * size, a + (j - 1) * size) < 0) {
      swapbytes(a + j * size, a + (j - 1) * size, size);
      j--;
    }
  }
}

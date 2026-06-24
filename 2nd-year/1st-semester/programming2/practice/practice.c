#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*Comparator)(const void *x, const void *y);

typedef struct {
  char name[40];
  long long bounty;
  int age;
  char crew[40];
} Pirate;

void swapBytes(void *x, void *y, int size);
int comparePirates(const void *x, const void *y);
void insertionSort(void *base, int n, int size, Comparator cmp);

int main() {
  Pirate *arr;
  int n;
  scanf("%d", &n);
  arr = (Pirate *)malloc(sizeof(Pirate) * n);

  for (int i = 0; i < n; i++) {
    scanf("%s %lld %d %s", arr[i].name, &arr[i].bounty, &arr[i].age,
          arr[i].crew);
  }
  insertionSort(arr, n, sizeof(Pirate), comparePirates);
  for (int i = 0; i < n; i++) {
    printf("%s %lld %d %s\n", arr[i].name, arr[i].bounty, arr[i].age,
           arr[i].crew);
  }

  return 0;
}

void swapBytes(void *x, void *y, int size) {
  unsigned char *a = x;
  unsigned char *b = y;
  for (int i = 0; i < size; i++) {
    unsigned char temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}

int comparePirates(const void *x, const void *y) {
  const Pirate *a = (const Pirate *)x;
  const Pirate *b = (const Pirate *)y;

  // Ordenamiento por recompensa
  if (a->bounty !=
      b->bounty) // Verificar si son iguales, para ordenar de otra manera
    return (a->bounty < b->bounty) ? 1 : -1; // Ordenar por mayor

  // Ordenamiento por edad
  if (a->age != b->age)
    return a->age - b->age;

  // Ordenamiento por nombre
  if (strcmp(a->name, b->name) != 0)
    return strcmp(a->name, b->name);

  // SI no se puede ordenar por ninguno, bueno... queda dejarlo por tripulación
  return strcmp(a->crew, b->crew);
}

void insertionSort(void *base, int n, int size, Comparator cmp) {
  unsigned char *a = base;
  for (int i = 1; i < n; i++) {
    int j = i;
    while (j > 0 && cmp(a + j * size, a + (j - 1) * size) < 0) {
      swapBytes(a + j * size, a + (j - 1) * size, size);
      j--;
    }
  }
}

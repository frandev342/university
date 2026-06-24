#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ORDENAR ARREGLO DE ESTUDIANTES
// por código, nombre, edad y promedio
// ordenamiento utilizando punteros genéricos y typedef

typedef struct {
  int code;
  char name[50];
  int age;
  float average;
} Student;

typedef int (*Comparator)(const void *x, const void *y);

// INTERCAMBIAR BITS DE DOS ESPACIOS
void swapBytes(void *x, void *y, int size) {
  unsigned char *a = (unsigned char *)x;
  unsigned char *b = (unsigned char *)y;
  for (int i = 0; i < size; i++) {
    unsigned char temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}
// ORDENAR POR CODE (el menor código primero)
int codecmp_(const void *x, const void *y) {
  const Student *a = (const Student *)x;
  const Student *b = (const Student *)y;
  return a->code - b->code;
}

// ORDENAR POR NAME (orden lexicográfico)
int namecmp_(const void *x, const void *y) {
  const Student *a = (const Student *)x;
  const Student *b = (const Student *)y;
  return strcmp(a->name, b->name);
}

// ORDENAR POR EDAD (el menor primero)
int agecmp_(const void *x, const void *y) {
  const Student *a = (const Student *)x;
  const Student *b = (const Student *)y;
  return a->age - b->age;
}

// ORDENAR POR PROMEDIO (el promedio más alto primero)
int averagecmp_(const void *x, const void *y) {
  const Student *a = (const Student *)x;
  const Student *b = (const Student *)y;
  return b->average - a->average;
}

// INSERTION-SORT
void inserSort(void *base, int n, int size, Comparator cmp) {
  unsigned char *a = (unsigned char *)base;
  for (int i = 1; i < n; i++) {
    int j = i - 1;
    while (j >= 0 && cmp(a + j * size, a + (j + 1) * size) > 0) {
      swapBytes(a + j * size, a + (j + 1) * size, size);
      j--;
    }
  }
}

void printArr(Student *arr, int n) {
  for (int i = 0; i < n; i++) {
    printf("%d, %s, %d, %.2f\n", arr[i].code, arr[i].name, arr[i].age,
           arr[i].average);
  }
  printf("\n");
}

/*
20251450 Luffy 19 16.5
20241230 Anakin 19 18.5
20124130 Doctor 29 18.0
20231304 Johan 22 20.0
20241032 Christina 18 20.0
20241036 Aqua 19 6.5
*/
int main() {
  int n;
  scanf("%d", &n);

  Student *school;
  school = (Student *)malloc(n * sizeof(Student));

  for (int i = 0; i < n; i++) {
    scanf("%d %s %d %f", &school[i].code, school[i].name, &school[i].age,
          &school[i].average);
  }

  // IMPRIMIMOS ANTES DE ORDENAR
  printf("\nANTES DE ORDENAR\n");
  for (int i = 0; i < n; i++) {
    printf("%d %s %d %.2f\n", school[i].code, school[i].name, school[i].age,
           school[i].average);
  }

  printf("\n");

  printf("Orden por code:\n");
  inserSort(school, n, sizeof(Student), codecmp_);
  printArr(school, n);

  printf("Orden por name:\n");
  inserSort(school, n, sizeof(Student), namecmp_);
  printArr(school, n);

  printf("Orden por age:\n");
  inserSort(school, n, sizeof(Student), agecmp_);
  printArr(school, n);

  printf("Orden por average:\n");
  inserSort(school, n, sizeof(Student), averagecmp_);
  printArr(school, n);

  return 0;
}

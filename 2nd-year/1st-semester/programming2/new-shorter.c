#include <stdio.h>

void shortArr(int A[], int n);

void printArr(int A[], int n);

int main() {
  // int arr[] = {5, 9, 1, 8, 3};
  int arr[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  shortArr(arr, 10);
  printArr(arr, 10);
}

void shortArr(int A[], int n) {
  int temp, j, k;
  // pivote
  j = 0;
  // Cambio de while por for y solucionando el problema de nuevo de iteraciones
  // insuficiente para recorrer todo el arreglo
  while (j < n - 1) {
    // puntero i niciando desde el pivote
    k = j;
    // Comparar cada elemento del arreglo con el pivote
    while (k + 1 < n && A[j] > A[k + 1])
      k++;
    // Si el intercambio es con el mismo, se define el nuevo pivote desde el
    // siguiente a él
    if (j == k)
      j++;
    // intercambio e iniciar el pivote desde 0
    else {
      temp = A[k];
      A[k] = A[j];
      A[j] = temp;
      j = 0;
    }
  }
}

void printArr(int A[], int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", A[i]);
  }
  printf("\n");
}

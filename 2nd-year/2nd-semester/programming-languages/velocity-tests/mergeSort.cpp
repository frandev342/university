#include <cstdlib>
#include <ctime>
#include <iostream>

void merge(int array[], int low, int mid, int high) {
  int temp[high - low + 1];
  int i = low;
  int j = mid + 1;
  int k = 0;

  while (i <= mid && j <= high) {
    if (array[i] <= array[j])
      temp[k++] = array[i++];
    else
      temp[k++] = array[j++];
  }

  while (i <= mid)
    temp[k++] = array[i++];

  while (j <= high)
    temp[k++] = array[j++];

  for (int i = 0; i < k; i++)
    array[low + i] = temp[i];
}

void mergeSort(int array[], int low, int high) {
  if (low < high) {
    int mid = (low + high) / 2;

    mergeSort(array, low, mid);
    mergeSort(array, mid + 1, high);
    merge(array, low, mid, high);
  }
}

int main() {
  int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
  int len = sizeof(sizes) / sizeof(sizes[0]);

  srand(time(NULL));

  for (int i = 0; i < len; i++) {
    int n = sizes[i];

    int *array = new int[n];

    for (int j = 0; j < n; j++)
      array[j] = rand();

    clock_t start = clock();

    mergeSort(array, 0, n - 1);

    clock_t end = clock();

    double time_ = (double)(end - start) / CLOCKS_PER_SEC;

    std::cout << "Tiempo de ejecucion (n=" << n << "): " << time_ << " segundos"
              << std::endl;

    delete[] array;
  }

  return 0;
}

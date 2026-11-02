#include "heap.h"
#include <stdio.h>

int main() {
  const Process input[] = {
      {0, 4}, {1, 1},  {2, 3},  {3, 2}, {4, 16},
      {5, 9}, {6, 10}, {7, 14}, {8, 8}, {9, 7},
  };
  const size_t size = sizeof(input) / sizeof(input[0]);
  HeapC heap;
  if (!heap_init(&heap, size * 2)) {
    printf("No se inicilizo correctamente el heap");
    return -1;
  }

  if (!heap_build(&heap, input, size)) {
    fprintf(stderr, "No se pudo construir el heap.\n");
    heap_destroy(&heap);
    return -1;
  }

  printf("%d \n", heap.data[0].priority);
  // Heap_push
  Process newP = {10, 11};
  if (!heap_push(&heap, newP)) {
    fprintf(stderr, "No se pudo pushear en el heap.\n");
    heap_destroy(&heap);
    return -1;
  }
  printf("%zu \n", heap.size);
  // Heap_pop;
  Process myP;
  if (!heap_pop(&heap, &myP)) {
    fprintf(stderr, "No se pudo extrar de heap.\n");
    heap_destroy(&heap);
    return -1;
  }
  printf("%d \n", myP.priority);
  printf("%zu \n", heap.size);

  print_priority(&heap);
  printf("\n");
  heap_destroy(&heap);
  return 0;
}

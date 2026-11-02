#include "heap.h"
#include <stdio.h>

int main() {
  const Process input[] = {
      {0, 4}, {1, 1},  {2, 3},  {3, 2}, {4, 16},
      {5, 9}, {6, 10}, {7, 14}, {8, 8}, {9, 7},
  };
  const size_t size = sizeof(input) / sizeof(input[0]);
  HeapC heap;
  heap_init(&heap, size * 2U);
  heap_build(&heap, input, size);
  Process p1 = {10, 23};
  heap_push(&heap, p1);
  printf("Process 1 priority: %d\n", heap.data[0].priority);

  heap_destroy(&heap);
  return 0;
}

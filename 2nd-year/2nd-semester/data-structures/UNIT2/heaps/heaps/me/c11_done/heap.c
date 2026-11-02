#include "heap.h"
#include "indices.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool heap_init(HeapC *h, size_t capacity) {
  if (h == NULL)
    return false;
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
  if (capacity > SIZE_MAX / sizeof(Process))
    return false;
  if (capacity != 0U) {
    h->data = malloc(capacity * sizeof(Process));
    if (h->data == NULL)
      return false;
  }
  h->capacity = capacity;
  return true;
}

void heap_destroy(HeapC *h) {
  if (h == NULL)
    return;
  free(h->data);
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
}

bool heap_valid(const HeapC *h) {
  if (h == NULL || h->size > h->capacity ||
      (h->capacity != 0U && h->data == NULL))
    return false;
  for (size_t i = 1U; i < h->size; ++i) {
    if (h->data[HEAP_PARENT(i)].priority < h->data[i].priority)
      return false;
  }
  return true;
}

static void sift_down(HeapC *h, size_t i) {
  while (i < h->size / 2U) {
    size_t left = HEAP_LEFT(i);
    size_t right = HEAP_RIGHT(i);
    size_t best = left;
    if (right < h->size && h->data[right].priority > h->data[left].priority) {
      best = right;
    }
    if (h->data[i].priority >= h->data[best].priority) {
      return;
    }
    Process temp = h->data[best];
    h->data[best] = h->data[i];
    h->data[i] = temp;
    i = best;
  }
}

bool heap_build(HeapC *h, const Process *src, size_t n) {
  if (h == NULL || n > h->capacity)
    return false;
  if (n != 0U && src == NULL)
    return false;
  for (size_t i = 0U; i < n; i++) {
    h->data[i] = src[i];
  }
  h->size = n;
  // NUnca usar j>=OU, porque cuando j = 0U y llegue a --j, se volverá positivo,
  // volviendolo un bucle infinito, por eso funciona como contador size_t, por
  // esa razón j - 1U en sift_down
  for (size_t j = n / 2U; j > 0u; --j) {
    sift_down(h, j - 1U);
  }
  return true;
}
// Tiempo: O(log n)
// Memoria auxiliar: O(1)
bool heap_push(HeapC *h, Process x) {
  if (h == NULL || h->size == h->capacity)
    return false;
  size_t i = h->size;
  h->data[i] = x;
  h->size++;
  while (i > 0U) {
    size_t parent = HEAP_PARENT(i);
    if (h->data[parent].priority >= h->data[i].priority)
      break;
    // EL intercambio se puede realizar así o por un temp
    h->data[i] = h->data[parent];
    h->data[parent] = x;

    i = parent;
  }
  return true;
}

bool heap_pop(HeapC *h, Process *out) {
  if (h == NULL || out == NULL || h->size == 0U)
    return false;
  *out = h->data[0];
  h->data[0] = h->data[h->size - 1];
  h->size--;
  if (h->size != 0U)
    sift_down(h, 0U);
  return true;
}

void print_priority(const HeapC *h) {
  for (size_t i = 0; i < h->size; ++i) {
    printf("%d ", h->data[i].priority);
  }
  printf("\n");
}

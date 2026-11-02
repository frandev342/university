#include "heap.h"
#include "indices.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// Repara el max-heap intercambiando con el mejor hijo. Tiempo O(log n), memoria
// O(1).
#if 0
static void sift_down_swaps(HeapC *h, size_t i) {
  while (i < h->size / 2) {
    size_t leftChild = HEAP_LEFT(i);
    size_t rightChild = leftChild + 1U;
    size_t best = leftChild;
    if (rightChild < h->size &&
        h->data[rightChild].priority > h->data[leftChild].priority)
      best = rightChild;
    if (h->data[i].priority >= h->data[best].priority)
      return;
    Process temp = h->data[i];
    h->data[i] = h->data[best];
    h->data[best] = temp;

    i = best;
  }
}
#endif

// Repara el max-heap desplazando el hueco hacia abajo. Tiempo O(log n), memoria
// O(1).
static void sift_down(HeapC *h, size_t i) {
  Process x = h->data[i];
  while (i < h->size / 2) {
    size_t leftChild = HEAP_LEFT(i);
    size_t rightChild = leftChild + 1U;
    size_t best = leftChild;
    if (rightChild < h->size &&
        h->data[rightChild].priority > h->data[leftChild].priority)
      best = rightChild;
    if (x.priority >= h->data[best].priority)
      break;
    h->data[i] = h->data[best];
    i = best;
  }
  h->data[i] = x;
}

// Inicializa un heap vacío con capacidad fija. Tiempo O(1), reserva
// O(capacity).
int heap_init(HeapC *h, size_t capacity) {
  if (!h)
    return 0;
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
  if (capacity > SIZE_MAX / sizeof(Process))
    return 0;
  if (capacity != 0) {
    h->data = malloc(sizeof(Process) * capacity);
    if (!h->data)
      return 0;
  }
  h->capacity = capacity;
  return 1;
}

// Libera el arreglo y reinicia el heap. Tiempo O(1), memoria auxiliar O(1).
void heap_destroy(HeapC *h) {
  if (!h)
    return;
  free(h->data);
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
}

// Construye el max-heap desde n procesos. Tiempo O(n), memoria auxiliar O(1).
int heap_build(HeapC *h, const Process *src, size_t n) {
  if (!h || h->capacity < n)
    return 0;
  if (n != 0U && !src)
    return 0;
  for (size_t i = 0U; i < n; ++i)
    h->data[i] = src[i];
  h->size = n;
  for (size_t i = n / 2; i > 0U; --i) {
    sift_down(h, i - 1U);
  }
  return 1;
}

// Inserta un proceso y lo asciende hasta restaurar el max-heap. Tiempo O(log
// n), memoria O(1).
int heap_push(HeapC *h, Process x) {
  if (!h || h->size == h->capacity)
    return 0;
  size_t i = h->size;
  h->data[i] = x;
  ++h->size;
  while (i > 0U) {
    size_t parent = HEAP_PARENT(i);
    if (h->data[parent].priority >= x.priority)
      break;
    h->data[i] = h->data[parent];
    i = parent;
  }
  h->data[i] = x;
  return 1;
}

// Extrae la raíz y hunde el último proceso. Tiempo O(log n), memoria O(1).
int heap_pop(HeapC *h, Process *out) {
  if (!h || !out || h->size == 0U)
    return 0;
  *out = h->data[0];
  h->data[0] = h->data[h->size - 1];
  --h->size;
  if (h->size != 0U)
    sift_down(h, 0U);
  return 1;
}

// Comprueba estructura y orden padre-hijo. Tiempo O(n), memoria O(1).
int heap_valid(const HeapC *h) {
  if (!h || h->size > h->capacity || (h->capacity != 0U && !h->data))
    return 0;
  for (size_t i = 1U; i < h->size; ++i) {
    if (h->data[HEAP_PARENT(i)].priority < h->data[i].priority)
      return 0;
  }
  return 1;
}

// Imprime las prioridades por niveles. Tiempo O(n), memoria auxiliar O(1).
void print_priority(const HeapC *h) {
  if (!h)
    return;
  for (size_t i = 0U; i < h->size; ++i)
    printf("%d ", h->data[i].priority);
  printf("\n");
}

/*
Resumen de complejidades (n = cantidad de procesos):

Función             Qué hace                                  Tiempo         Memoria auxiliar
sift_down_swaps     Repara hacia abajo con intercambios       O(log n)      O(1)
sift_down           Repara hacia abajo usando un hueco        O(log n)      O(1)
heap_init           Inicializa y reserva capacity elementos   O(1)          O(capacity) reservada
heap_destroy        Libera y reinicia el heap                  O(1)          O(1)
heap_build          Construye el heap de abajo hacia arriba    O(n)          O(1)
heap_push           Inserta y asciende un proceso              O(log n)      O(1)
heap_pop            Extrae el máximo y repara hacia abajo      O(log n)      O(1)
heap_valid          Verifica representación y orden            O(n)          O(1)
print_priority      Imprime el arreglo por niveles             O(n)          O(1)
*/

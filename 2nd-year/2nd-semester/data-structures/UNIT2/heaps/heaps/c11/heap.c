#include "heap.h"
#include "indices.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Max-heap: se compara priority; en un empate cualquiera es valido.
// Tiempo O(log n) Espacio auxiliar O(1)
static void sift_down(HeapC *h, size_t i) {
  /* TODO C1: reparar hacia abajo, en O(log n) tiempo y O(1) espacio.
     Precondicion: los subarboles hijos de i ya son heaps.
     Comprobar existencia ANTES de calcular/acceder a los hijos. */
  // Heap repara desde i mirando sus hijos quién es mayor e intercambiando con
  // quién, Para luego enfocarnos con quién cambiamos y seguir continuando hasta
  // abajo, acabará hasta que encontremos que no tenemos un hijo izquierdo o la
  // prioridad de i es mayor que sus hijos

  // Mientras que i sea un nodo interno y tenga hijo izquierdo
  while (i < h->size / 2U) {
    size_t left = HEAP_LEFT(i);
    size_t right = HEAP_RIGHT(i);
    // SUponemos que best, el mejor es el izquierdo
    size_t best = left;
    // Verificar si existe el hijo derecho y es mayor que best
    if (right < h->size && h->data[right].priority > h->data[left].priority) {
      best = right;
    }
    // Si ningún hijo es mayor que i, entonces acaba
    if (h->data[i].priority >= h->data[best].priority) {
      return;
    }
    // Intercambio de nodos
    Process temp = h->data[best];
    h->data[best] = h->data[i];
    h->data[i] = temp;
    // Continar con el siguiente de abajo
    i = best;
  }
}

// h == NULL, es para
/*
Para comprobar que el objeto existe, por ejemplo estos están disponible
- HeapC heap;
- HeapC* heap = malloc(sizeof(*heap));

En caso sea
- HeapC* heap = NULL;
- O malloc falló y regreso NULL, se debe verificar dentro de INIT
*/
bool heap_init(HeapC *h, size_t capacity) {
  if (h == NULL)
    return false;
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
  // SIZE_MAX indica un tamaño máximo teórico en bytes de cualquier objeto o
  // arreglo} SIZE_MAX es un tipo sin signo al igual que size_t, y su valor
  // depende de la arquitectura En sistema de 64 bits, su valor es 2^64 - 1.
  if (capacity >
      SIZE_MAX / sizeof(Process)) // Si la capacity dada supera la máxima
                                  // disponible a dar, retorno false
    return false;
  if (capacity != 0U) {
    h->data = malloc(capacity * sizeof(Process));
    if (h->data == NULL)
      return false;
  }
  h->capacity = capacity;
  return true;
}

// Tiempo O(1)
void heap_destroy(HeapC *h) {
  if (h == NULL)
    return;
  free(h->data);
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
}

// Tiempo O(n), Espacio auxiliar O (1)
bool heap_build(HeapC *h, const Process *src, size_t n) {
  if (h == NULL || n > h->capacity || (n != 0U && src == NULL))
    return false;
  /* TODO C2: copiar src, actualizar size y construir bottom-up.
     Utilizar: for(size_t k = n / 2U; k > 0U; --k) ...
     El codigo tambien debe aceptar n == 0. */
  if (n != 0U)
    memcpy(h->data, src, n * sizeof(Process));
  h->size = n;
  for (size_t k = n / 2U; k > 0U; --k)
    sift_down(h, k - 1U);
  return true;
}

// Tiempo O(log n), Espacio auxiliar O(1)
bool heap_push(HeapC *h, Process x) {
  if (h == NULL || h->size == h->capacity)
    return false;
  /* TODO C3: colocar el elemento al final y subirlo por prioridad.
     No llamar HEAP_PARENT si i == 0. */
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

// Tiempo O(log n), Espacio auxiliar O(1)
bool heap_pop(HeapC *h, Process *out) {
  if (h == NULL || out == NULL || h->size == 0U)
    return false;
  /* TODO C4: extraer maximo, sustituir raiz por ultimo y reparar.
     Para heap de 1 elemento no llamar sift_down con heap vacio. */
  *out = h->data[0];
  h->data[0] = h->data[h->size - 1];
  h->size--;
  if (h->size != 0U)
    sift_down(h, 0U);
  return true;
}

// Tiempo O(n) Espacio auxiliar O(!)
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

#ifndef ME_HEAP_H
#define ME_HEAP_H

#include <stddef.h>
typedef struct {
  int pid;
  int priority;
} Process;

// max-heap
typedef struct {
  Process *data;
  size_t size;
  size_t capacity;
} HeapC;

// Inicializa un heap vacío con capacidad fija. Tiempo O(1), reserva O(capacity).
int heap_init(HeapC *h, size_t capacity);

// Libera el arreglo y reinicia el heap. Tiempo O(1), memoria auxiliar O(1).
void heap_destroy(HeapC *h);

// Construye el max-heap desde n procesos. Tiempo O(n), memoria auxiliar O(1).
int heap_build(HeapC *h, const Process *src, size_t n);

// Inserta un proceso y restaura el orden. Tiempo O(log n), memoria O(1).
int heap_push(HeapC *h, Process x);

// Extrae el proceso de mayor prioridad. Tiempo O(log n), memoria O(1).
int heap_pop(HeapC *h, Process *out);

// Comprueba la representación y el orden del max-heap. Tiempo O(n), memoria O(1).
int heap_valid(const HeapC *h);

// Imprime las prioridades por niveles. Tiempo O(n), memoria auxiliar O(1).
void print_priority(const HeapC *h);

#endif

#ifndef HEAP_INDICES_C11_H
#define HEAP_INDICES_C11_H

#include <stddef.h>

/* Precondicion: i > 0. Macros con parentesis y un solo uso de i. */
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
#define HEAP_LEFT(i)   (2U * (i) + 1U)
#define HEAP_RIGHT(i)  (2U * (i) + 2U)

/* Alternativa C11 para comparar en el laboratorio. */
static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}

#endif

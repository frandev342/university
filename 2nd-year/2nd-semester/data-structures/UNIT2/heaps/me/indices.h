#ifndef ME_HEAP_INDICES_H
#define ME_HEAP_INDICES_H
#include <stddef.h>

// Precondiciones, acceder mediante indices y obtener otro indices
// Es valido para todo i > 0
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
#define HEAP_LEFT(i) (2 * (i) + 1U)
#define HEAP_RIGHT(i) (2 * (i) + 2U)

// Obtiene el padre de un índice i > 0. Tiempo y memoria O(1).
static inline size_t parent_inline(size_t i) { return ((i - 1U) / 2U); }

#endif

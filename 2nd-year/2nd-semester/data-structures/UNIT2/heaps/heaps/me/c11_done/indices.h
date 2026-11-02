#ifndef ME_HEAP_INDICES_H
#define ME_HEAP_INDICES_H

#include <stddef.h>

// Precondiciones, acceder mediante índices y obtener otro índice
// Válido para todo i > 0
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
#define HEAP_LEFT(i) (2U * (i) + 1U)
#define HEAP_RIGHT(i) (2U * (i) + 2U)

// Alternativa para acceder al padre
static inline size_t parent_inline(size_t i) { return (i - 1U) / 2U; }

#endif // !

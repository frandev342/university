#include <stddef.h>
#define PARENT(i) (((i) - 1U) / 2U)
static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}
size_t con_macro(size_t i) { return PARENT(i); }
size_t con_inline(size_t i) { return parent_inline(i); }

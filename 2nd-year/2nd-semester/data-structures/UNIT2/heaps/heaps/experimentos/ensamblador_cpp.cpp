#include <cstddef>
inline constexpr std::size_t parent(std::size_t i) {
  return (i - 1U) / 2U;
}
std::size_t con_constexpr(std::size_t i) { return parent(i); }

#ifndef ME_HEAP_INDICES_HPP
#define ME_HEAP_INDICES_HPP

#include <cstddef>
namespace heap11 {
inline constexpr std::size_t parent(std::size_t i) { return (i - 1U) / 2U; }
inline constexpr std::size_t left(std::size_t i) { return 2U * i + 1U; }
inline constexpr std::size_t right(std::size_t i) { return 2U * i + 2U; }
static_assert(parent(9U) == 4U, "incorrect parent");
static_assert(left(4U) == 9U, "incorrect left child");
static_assert(right(4U) == 10U, "incorrect right child");
} // namespace heap11

#endif // !HEAP_INDICES_CPP11_HPP

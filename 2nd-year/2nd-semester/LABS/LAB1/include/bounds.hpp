#pragma once

#include <cstddef>
#include <vector>

template <typename T>
std::size_t my_lower_bound(const std::vector<T> &a, const T &x) {
  (void)x;
  // TODO: primera posición i tal que a[i] >= x.
  // Debe usar O(log n) comparaciones y puede asumir que a está ordenado.
  std::size_t start = 0;
  std::size_t end = a.size(); // no se incluye
  while (start < end) {
    std::size_t mid = (end - start) / 2 + start;
    if (a[mid] >= x) {
      end = mid;
    } else {
      start = mid + 1;
    }
  }
  return start;
}

template <typename T>
std::size_t my_upper_bound(const std::vector<T> &a, const T &x) {
  (void)x;
  // TODO: primera posición i tal que a[i] > x.
  // Debe usar O(log n) comparaciones y puede asumir que a está ordenado.
  std::size_t start = 0;
  std::size_t end = a.size();
  while (start < end) {
    std::size_t mid = (end - start) / 2 + start;
    if (a[mid] > x) {
      end = mid;
    } else {
      start = mid + 1;
    }
  }
  return start;
}

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "bounds.hpp"
#include "queue_circular.hpp"
#include "queue_shift.hpp"

struct StdQueueAdapter {
  std::queue<int> q;

  void push(int x) { q.push(x); }
  void pop() { q.pop(); }
  int front() const { return q.front(); }
  bool empty() const { return q.empty(); }
  std::size_t moves() const noexcept { return 0; }
};

struct Result {
  double ms = 0.0;
  std::size_t moves = 0;
  std::uint64_t checksum = 0;
};

template<class Queue>
Result benchmark_queue(std::size_t n) {
  Queue q;
  std::uint32_t x = 0x12345678u;

  const auto begin = std::chrono::steady_clock::now();

  for(std::size_t i = 0; i < n; ++i) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    q.push(static_cast<int>(x & 0x7fffffffU));
  }

  std::uint64_t checksum = 0;
  while(!q.empty()) {
    checksum += static_cast<std::uint32_t>(q.front());
    q.pop();
  }

  const auto end = std::chrono::steady_clock::now();
  return {
    std::chrono::duration<double, std::milli>(end - begin).count(),
    q.moves(),
    checksum
  };
}

std::size_t linear_lower_bound(const std::vector<int> &a, int x) {
  std::size_t i = 0;
  while(i < a.size() && a[i] < x)
    ++i;
  return i;
}

template<class F>
double benchmark_queries(const std::vector<int> &a,
                         const std::vector<int> &queries,
                         F f,
                         std::uint64_t &checksum) {
  const auto begin = std::chrono::steady_clock::now();

  std::uint64_t local = 0;
  for(int x : queries)
    local += f(a, x);

  const auto end = std::chrono::steady_clock::now();
  checksum = local;
  return std::chrono::duration<double, std::milli>(end - begin).count();
}

int main() {
  std::cout << "=== Queue: misma especificación, distinta representación ===\n";
  std::cout << std::left
            << std::setw(10) << "n"
            << std::setw(18) << "Shift ms"
            << std::setw(20) << "Shift moves"
            << std::setw(18) << "Circular ms"
            << std::setw(20) << "Circular moves"
            << std::setw(18) << "std::queue ms" << '\n';

  for(std::size_t n : {1000UL, 5000UL, 10000UL, 20000UL, 40000UL}) {
    const Result shift = benchmark_queue<QueueShift<int>>(n);
    const Result circular = benchmark_queue<QueueCircular<int>>(n);
    const Result stl = benchmark_queue<StdQueueAdapter>(n);

    if(shift.checksum != circular.checksum || shift.checksum != stl.checksum) {
      std::cerr << "ERROR: las implementaciones no produjeron el mismo resultado\n";
      return 1;
    }

    std::cout << std::setw(10) << n
              << std::setw(18) << shift.ms
              << std::setw(20) << shift.moves
              << std::setw(18) << circular.ms
              << std::setw(20) << circular.moves
              << std::setw(18) << stl.ms << '\n';
  }

  std::cout << "\n=== Búsqueda: lineal vs. binaria ===\n";

  constexpr std::size_t n = 10000;
  constexpr std::size_t q = 20000;
  std::vector<int> a(n);
  for(std::size_t i = 0; i < n; ++i)
    a[i] = static_cast<int>(i * 2);

  std::mt19937 rng(20260901);
  std::vector<int> queries(q);
  for(int &xq : queries)
    xq = static_cast<int>(rng() % (2 * n + 1000));

  std::uint64_t c1 = 0, c2 = 0, c3 = 0;

  const double linear_ms = benchmark_queries(
    a, queries,
    [](const auto &v, int xq) { return linear_lower_bound(v, xq); },
    c1);

  const double mine_ms = benchmark_queries(
    a, queries,
    [](const auto &v, int xq) { return my_lower_bound(v, xq); },
    c2);

  const double stl_ms = benchmark_queries(
    a, queries,
    [](const auto &v, int xq) {
      return static_cast<std::size_t>(
        std::lower_bound(v.begin(), v.end(), xq) - v.begin());
    },
    c3);

  if(c1 != c2 || c1 != c3) {
    std::cerr << "ERROR: las búsquedas no produjeron el mismo resultado\n";
    return 1;
  }

  std::cout << "n=" << n << ", consultas=" << q << '\n'
            << "lineal       : " << linear_ms << " ms\n"
            << "my_lower_bound: " << mine_ms << " ms\n"
            << "std::lower_bound: " << stl_ms << " ms\n";
}

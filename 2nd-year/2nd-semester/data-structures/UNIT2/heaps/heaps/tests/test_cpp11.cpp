#include "../cpp11/heap.hpp"
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

int main() {
  heap11::Heap h(10);
  assert(h.valid());
  heap11::Process out{-1,-1};
  assert(!h.pop(out));
  std::vector<heap11::Process> input{
    {0,4}, {1,1}, {2,3}, {3,2}, {4,16},
    {5,9}, {6,10}, {7,14}, {8,8}, {9,7}
  };
  assert(h.build(input) && h.valid());
  int prev = INT_MAX;
  for(int i = 0; i < 10; ++i) {
    assert(h.pop(out));
    assert(out.priority <= prev);
    prev = out.priority;
    assert(h.valid());
  }
  assert(!h.pop(out));
  assert(h.push({30, INT_MIN}));
  assert(h.push({31, INT_MAX}));
  assert(h.push({32, INT_MAX}));
  assert(h.valid());
  assert(h.pop(out) && out.priority == INT_MAX);
  assert(h.pop(out) && out.priority == INT_MAX);
  assert(h.pop(out) && out.priority == INT_MIN);
  heap11::Heap zero(0);
  assert(!zero.push({0, 0}));
  heap11::Heap stress(64);
  for(std::size_t n = 0U; n <= 64U; ++n) {
    std::vector<heap11::Process> sample;
    for(std::size_t i = 0U; i < n; ++i)
      sample.push_back({static_cast<int>(i),
        static_cast<int>((i * 29U + n * 17U) % 41U) - 20});
    assert(stress.build(sample) && stress.valid());
    int previous_priority = INT_MAX;
    for(std::size_t i = 0U; i < n; ++i) {
      assert(stress.pop(out));
      assert(out.priority <= previous_priority);
      previous_priority = out.priority;
      assert(stress.valid());
    }
  }
  std::cout << "C++11: OK\n";
}

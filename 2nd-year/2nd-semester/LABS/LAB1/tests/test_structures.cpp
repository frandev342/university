#include <algorithm>
#include <cstddef>
#include <queue>
#include <random>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include "../include/bounds.hpp"
#include "../include/queue_circular.hpp"
#include "../include/queue_shift.hpp"
#include "../include/stack_raw.hpp"
#include "../include/stack_vector.hpp"

template <class Stack> void check_stack_axioms() {
  Stack s;
  CHECK(s.empty());
  CHECK(s.size() == 0);

  s.push(10);
  CHECK_FALSE(s.empty());
  CHECK(s.size() == 1);
  CHECK(s.top() == 10);

  s.push(20);
  const auto n = s.size();
  s.push(42);
  CHECK(s.top() == 42);
  CHECK(s.size() == n + 1);

  s.pop();
  CHECK(s.top() == 20);
}

template <class Stack>
void random_stack_test(unsigned seed = 12345, int operations = 50000) {
  std::mt19937 rng(seed);
  Stack mine;
  std::stack<int> oracle;

  for (int i = 0; i < operations; ++i) {
    if (oracle.empty() || (rng() & 1U) == 0) {
      int x = static_cast<int>(rng() % 100000);
      mine.push(x);
      oracle.push(x);
    } else {
      mine.pop();
      oracle.pop();
    }

    REQUIRE(mine.size() == oracle.size());
    REQUIRE(mine.empty() == oracle.empty());
    if (!oracle.empty())
      REQUIRE(mine.top() == oracle.top());
  }
}

template <class Queue> void check_queue_axioms() {
  Queue q;
  CHECK(q.empty());
  CHECK(q.size() == 0);

  q.push(10);
  CHECK_FALSE(q.empty());
  CHECK(q.front() == 10);
  CHECK(q.size() == 1);

  q.push(20);
  q.push(30);
  CHECK(q.front() == 10);

  q.pop();
  CHECK(q.front() == 20);
  CHECK(q.size() == 2);
}

template <class Queue>
void random_queue_test(unsigned seed = 54321, int operations = 50000) {
  std::mt19937 rng(seed);
  Queue mine;
  std::queue<int> oracle;

  for (int i = 0; i < operations; ++i) {
    if (oracle.empty() || (rng() & 1U) == 0) {
      int x = static_cast<int>(rng() % 100000);
      mine.push(x);
      oracle.push(x);
    } else {
      mine.pop();
      oracle.pop();
    }

    REQUIRE(mine.size() == oracle.size());
    REQUIRE(mine.empty() == oracle.empty());
    if (!oracle.empty())
      REQUIRE(mine.front() == oracle.front());
  }
}

TEST_CASE("StackVector satisface las propiedades básicas") {
  check_stack_axioms<StackVector<int>>();
}

TEST_CASE("StackRaw satisface las propiedades básicas") {
  check_stack_axioms<StackRaw<int>>();
}

TEST_CASE("StackRaw tiene copia independiente") {
  StackRaw<int> a;
  a.push(10);
  a.push(20);

  StackRaw<int> b = a;
  b.pop();
  b.push(99);

  CHECK(a.top() == 20);
  CHECK(b.top() == 99);
}

TEST_CASE("StackRaw transfiere ownership al mover") {
  StackRaw<int> a;
  a.push(10);
  a.push(20);

  StackRaw<int> b = std::move(a);

  CHECK(a.empty());
  CHECK(a.size() == 0);
  CHECK(b.size() == 2);
  CHECK(b.top() == 20);
}

TEST_CASE("top y pop lanzan sobre pila vacía") {
  StackRaw<int> a;
  CHECK_THROWS_AS(a.top(), std::out_of_range);
  CHECK_THROWS_AS(a.pop(), std::out_of_range);
}

TEST_CASE("Stacks coinciden con std::stack en operaciones aleatorias") {
  random_stack_test<StackRaw<int>>();
  random_stack_test<StackVector<int>>();
}

TEST_CASE("QueueShift satisface las propiedades básicas") {
  check_queue_axioms<QueueShift<int>>();
}

TEST_CASE("QueueCircular satisface las propiedades básicas") {
  check_queue_axioms<QueueCircular<int>>();
}

TEST_CASE(
    "QueueCircular conserva el orden después de wrap-around y crecimiento") {
  QueueCircular<int> q;
  for (int i = 0; i < 16; ++i)
    q.push(i);
  for (int i = 0; i < 7; ++i)
    q.pop();
  for (int i = 16; i < 40; ++i)
    q.push(i);

  for (int expected = 7; expected < 40; ++expected) {
    REQUIRE(q.front() == expected);
    q.pop();
  }
  CHECK(q.empty());
}

TEST_CASE("front y pop lanzan sobre cola vacía") {
  QueueShift<int> a;
  QueueCircular<int> b;

  CHECK_THROWS_AS(a.front(), std::out_of_range);
  CHECK_THROWS_AS(a.pop(), std::out_of_range);
  CHECK_THROWS_AS(b.front(), std::out_of_range);
  CHECK_THROWS_AS(b.pop(), std::out_of_range);
}

TEST_CASE("Queues coinciden con std::queue en operaciones aleatorias") {
  random_queue_test<QueueShift<int>>();
  random_queue_test<QueueCircular<int>>();
}

TEST_CASE("my_lower_bound coincide con STL") {
  std::mt19937 rng(20260901);

  for (int trial = 0; trial < 300; ++trial) {
    const int n = static_cast<int>(rng() % 1000);
    std::vector<int> a(n);
    for (int &x : a)
      x = static_cast<int>(rng() % 200) - 100;
    std::sort(a.begin(), a.end());

    for (int k = 0; k < 30; ++k) {
      int x = static_cast<int>(rng() % 300) - 150;
      auto expected = static_cast<std::size_t>(
          std::lower_bound(a.begin(), a.end(), x) - a.begin());
      CHECK(my_lower_bound(a, x) == expected);
    }
  }
}

TEST_CASE("my_upper_bound coincide con STL") {
  std::mt19937 rng(20260901);

  for (int trial = 0; trial < 300; ++trial) {
    const int n = static_cast<int>(rng() % 1000);
    std::vector<int> a(n);
    for (int &x : a)
      x = static_cast<int>(rng() % 200) - 100;
    std::sort(a.begin(), a.end());

    for (int k = 0; k < 30; ++k) {
      int x = static_cast<int>(rng() % 300) - 150;
      auto expected = static_cast<std::size_t>(
          std::upper_bound(a.begin(), a.end(), x) - a.begin());
      CHECK(my_upper_bound(a, x) == expected);
    }
  }
}

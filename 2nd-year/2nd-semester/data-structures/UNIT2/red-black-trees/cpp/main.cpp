#include "red_black_tree.hpp"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <vector>

using red_black::RedBlackTree;

static void check(bool condition, const char *expression, int line) {
  if (condition)
    return;

  std::cerr << "Check failed at line " << line << ": " << expression << '\n';
  std::abort();
}

#define CHECK(expression) check((expression), #expression, __LINE__)

static void test_empty_tree() {
  RedBlackTree tree;
  CHECK(tree.empty());
  CHECK(tree.size() == 0U);
  CHECK(tree.height() == -1);
  CHECK(!tree.contains(10));
  CHECK(!tree.erase(10));
  CHECK(tree.inorder().empty());
  CHECK(tree.valid());
}

static void test_cormen_sequence() {
  RedBlackTree tree;
  const int keys[] = {41, 38, 31, 12, 19, 8};

  for (int key : keys) {
    CHECK(tree.insert(key));
    CHECK(tree.contains(key));
    CHECK(tree.valid());
  }

  const std::vector<int> expected{8, 12, 19, 31, 38, 41};
  CHECK(tree.inorder() == expected);
  CHECK(tree.size() == expected.size());
  CHECK(!tree.insert(19));
  CHECK(tree.size() == expected.size());
  CHECK(tree.valid());

  for (int key : keys) {
    CHECK(tree.erase(key));
    CHECK(!tree.contains(key));
    CHECK(tree.valid());
  }
  CHECK(tree.empty());
}

static void test_boundaries_and_deletion_shapes() {
  RedBlackTree tree;
  const int keys[] = {20, 10, 30, 5, 15, 25, 40, 22, 27};

  for (int key : keys) {
    CHECK(tree.insert(key));
    CHECK(tree.valid());
  }

  CHECK(tree.erase(5));  // Leaf.
  CHECK(tree.valid());
  CHECK(tree.erase(25)); // Node with two children.
  CHECK(tree.valid());
  CHECK(tree.erase(20)); // Root with two children.
  CHECK(tree.valid());

  CHECK(tree.insert(INT_MIN));
  CHECK(tree.valid());
  CHECK(tree.insert(INT_MAX));
  CHECK(tree.valid());
  CHECK(tree.erase(INT_MIN));
  CHECK(tree.valid());
  CHECK(tree.erase(INT_MAX));
  CHECK(tree.valid());
  CHECK(!tree.erase(123456));
  CHECK(tree.valid());

  const std::vector<int> expected{10, 15, 22, 27, 30, 40};
  CHECK(tree.inorder() == expected);
}

static void test_ordered_stress() {
  RedBlackTree tree;
  const int limit = 512;

  for (int key = 1; key <= limit; ++key) {
    CHECK(tree.insert(key));
    CHECK(tree.valid());
  }

  CHECK(tree.size() == static_cast<std::size_t>(limit));
  CHECK(tree.height() <= 18); // h <= 2 log2(n + 1) for a red-black tree.

  for (int key = 2; key <= limit; key += 2) {
    CHECK(tree.erase(key));
    CHECK(tree.valid());
  }
  for (int key = 1; key <= limit; key += 2) {
    CHECK(tree.erase(key));
    CHECK(tree.valid());
  }

  CHECK(tree.empty());
}

static void test_deterministic_random_orders() {
  const int limit = 160;
  std::vector<int> keys(static_cast<std::size_t>(limit));
  std::iota(keys.begin(), keys.end(), -80);
  std::mt19937 generator(2026U);

  for (int round = 0; round < 8; ++round) {
    RedBlackTree tree;
    std::shuffle(keys.begin(), keys.end(), generator);

    for (int key : keys) {
      CHECK(tree.insert(key));
      CHECK(tree.valid());
    }

    std::shuffle(keys.begin(), keys.end(), generator);
    for (int key : keys) {
      CHECK(tree.erase(key));
      CHECK(tree.valid());
    }
    CHECK(tree.empty());
  }
}

static void test_against_std_set() {
  RedBlackTree tree;
  std::set<int> reference;
  std::mt19937 generator(417U);
  std::uniform_int_distribution<int> key_distribution(-100, 100);
  std::uniform_int_distribution<int> operation_distribution(0, 2);

  for (int step = 0; step < 5000; ++step) {
    const int key = key_distribution(generator);
    const int operation = operation_distribution(generator);

    if (operation == 0) {
      const bool expected = reference.insert(key).second;
      CHECK(tree.insert(key) == expected);
    } else if (operation == 1) {
      const bool expected = reference.erase(key) == 1U;
      CHECK(tree.erase(key) == expected);
    } else {
      const bool expected = reference.count(key) == 1U;
      CHECK(tree.contains(key) == expected);
    }

    const std::vector<int> expected(reference.begin(), reference.end());
    CHECK(tree.inorder() == expected);
    CHECK(tree.size() == reference.size());
    CHECK(tree.valid());
  }
}

static void print_example() {
  RedBlackTree tree;
  const int keys[] = {41, 38, 31, 12, 19, 8};
  for (int key : keys)
    CHECK(tree.insert(key));

  std::cout << "Cormen insertion example (right subtree is above):\n";
  tree.print(std::cout);
}

int main() {
  test_empty_tree();
  test_cormen_sequence();
  test_boundaries_and_deletion_shapes();
  test_ordered_stress();
  test_deterministic_random_orders();
  test_against_std_set();
  print_example();

  std::cout << "Red-black tree: OK\n";
}

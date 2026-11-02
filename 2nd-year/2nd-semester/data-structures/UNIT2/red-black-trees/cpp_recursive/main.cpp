#include "red_black_tree.hpp"

#include <climits>
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <vector>

using red_black_recursive::RedBlackTree;

static void check(bool condition, int line) {
  if (!condition) {
    std::cerr << "C++ recursive check failed at line " << line << '\n';
    std::abort();
  }
}
#define CHECK(x) check((x), __LINE__)

static void test_cormen() {
  RedBlackTree tree;
  CHECK(tree.empty() && tree.valid() && tree.height() == -1);
  const int keys[] = {41, 38, 31, 12, 19, 8};
  for (int key : keys) {
    CHECK(tree.insert(key));
    CHECK(tree.contains(key));
    CHECK(tree.valid());
  }
  CHECK(tree.inorder() == (std::vector<int>{8, 12, 19, 31, 38, 41}));
  CHECK(!tree.insert(19));
  CHECK(tree.size() == 6U);
  std::cout << "C++ recursive, Cormen example:\n";
  tree.print(std::cout);
  for (int key : keys) {
    CHECK(tree.erase(key));
    CHECK(tree.valid());
  }
  CHECK(tree.empty());
}

static void test_stress() {
  RedBlackTree tree;
  CHECK(tree.insert(INT_MIN));
  CHECK(tree.insert(INT_MAX));
  CHECK(tree.valid());
  CHECK(tree.erase(INT_MIN));
  CHECK(tree.erase(INT_MAX));
  for (int i = 0; i < 512; ++i) {
    CHECK(tree.insert(i));
    CHECK(tree.valid());
  }
  CHECK(tree.height() <= 18);
  for (int i = 511; i >= 0; --i) {
    CHECK(tree.erase(i));
    CHECK(tree.valid());
  }
  CHECK(!tree.erase(100));
}

static void test_mixed() {
  RedBlackTree tree;
  std::set<int> reference;
  std::mt19937 engine(2026U);
  std::uniform_int_distribution<int> keys(-100, 100);
  std::uniform_int_distribution<int> actions(0, 2);
  for (int step = 0; step < 6000; ++step) {
    int key = keys(engine);
    int action = actions(engine);
    if (action == 0)
      CHECK(tree.insert(key) == reference.insert(key).second);
    else if (action == 1)
      CHECK(tree.erase(key) == (reference.erase(key) == 1U));
    else
      CHECK(tree.contains(key) == (reference.count(key) == 1U));

    CHECK(tree.inorder() == std::vector<int>(reference.begin(), reference.end()));
    CHECK(tree.size() == reference.size());
    CHECK(tree.valid());
  }
}

int main() {
  test_cormen();
  test_stress();
  test_mixed();
  std::cout << "Red-black tree C++ recursive: OK\n";
}

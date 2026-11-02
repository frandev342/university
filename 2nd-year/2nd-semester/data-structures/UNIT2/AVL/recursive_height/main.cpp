#include "avl.hpp"

#include <cassert>
#include <climits>
#include <iostream>

using namespace avl_recursive;

static void check_rotation(int first, int second, int third) {
  Tree tree;
  assert(avl_insert(tree, first) == 1);
  assert(avl_insert(tree, second) == 1);
  assert(avl_insert(tree, third) == 1);

  assert(tree.root != nullptr);
  assert(tree.root->key == 20);
  assert(tree.root->left != nullptr && tree.root->left->key == 10);
  assert(tree.root->right != nullptr && tree.root->right->key == 30);
  assert(avl_valid(tree));
  destroy(tree);
}

static void test_rotations() {
  check_rotation(30, 20, 10); // LL
  check_rotation(10, 20, 30); // RR
  check_rotation(30, 10, 20); // LR
  check_rotation(10, 30, 20); // RL
}

static void test_queries() {
  Tree tree;
  const int keys[] = {40, 20, 60, 10, 30, 50, 70, 25, 35};
  const std::size_t count = sizeof(keys) / sizeof(keys[0]);

  for (std::size_t i = 0U; i < count; ++i)
    assert(avl_insert(tree, keys[i]) == 1);

  assert(avl_valid(tree));
  assert(tree_size(tree.root) == count);
  assert(minimum(tree.root)->key == 10);
  assert(maximum(tree.root)->key == 70);
  assert(search(tree.root, 25) != nullptr);
  assert(search(tree.root, 99) == nullptr);
  assert(successor(search(tree.root, 25))->key == 30);
  assert(successor(maximum(tree.root)) == nullptr);
  assert(lower_bound(tree.root, 26)->key == 30);
  assert(upper_bound(tree.root, 30)->key == 35);
  assert(lower_bound(tree.root, 71) == nullptr);
  assert(upper_bound(tree.root, 70) == nullptr);

  destroy(tree);
  assert(tree.root == nullptr);
}

static void test_boundaries() {
  Tree tree;
  assert(avl_insert(tree, 0) == 1);
  assert(avl_insert(tree, INT_MIN) == 1);
  assert(avl_insert(tree, INT_MAX) == 1);
  assert(avl_insert(tree, 0) == 1);
  assert(tree_size(tree.root) == 4U);
  assert(avl_valid(tree));
  destroy(tree);
}

static void test_duplicates() {
  Tree tree;
  const int repetitions = 64;

  for (int i = 0; i < repetitions; ++i) {
    assert(avl_insert(tree, 7) == 1);
    assert(avl_valid(tree));
  }

  assert(tree_size(tree.root) == static_cast<std::size_t>(repetitions));
  assert(height(tree.root) <= 7);

  for (int remaining = repetitions; remaining > 0; --remaining) {
    assert(avl_erase(tree, 7));
    assert(tree_size(tree.root) == static_cast<std::size_t>(remaining - 1));
    assert(avl_valid(tree));
  }

  assert(!avl_erase(tree, 7));
  destroy(tree);
}

static void test_stress_and_erase() {
  Tree tree;
  const int limit = 512;

  for (int key = 1; key <= limit; ++key) {
    assert(avl_insert(tree, key) == 1);
    assert(avl_valid(tree));
  }

  assert(tree_size(tree.root) == static_cast<std::size_t>(limit));
  assert(height(tree.root) <= 10);

  for (int key = 2; key <= limit; key += 2) {
    assert(avl_erase(tree, key));
    assert(avl_valid(tree));
  }

  for (int key = 1; key <= limit; key += 2) {
    assert(avl_erase(tree, key));
    assert(avl_valid(tree));
  }

  assert(tree.root == nullptr);
  assert(!avl_erase(tree, 1));
  destroy(tree);
}

int main() {
  Tree empty;
  assert(avl_valid(empty));
  assert(height(empty.root) == -1);
  assert(minimum(empty.root) == nullptr);
  assert(maximum(empty.root) == nullptr);

  test_rotations();
  test_queries();
  test_boundaries();
  test_duplicates();
  test_stress_and_erase();

  std::cout << "AVL recursive height: OK\n";
}

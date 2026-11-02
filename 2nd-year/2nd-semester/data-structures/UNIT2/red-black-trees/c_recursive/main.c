#include "red_black_tree.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, int line) {
  if (!condition) {
    fprintf(stderr, "C recursive check failed at line %d\n", line);
    abort();
  }
}
#define CHECK(x) check((x), __LINE__)

static unsigned next_random(unsigned *state) {
  *state = *state * 1664525U + 1013904223U;
  return *state;
}

static void test_cormen(void) {
  RBTree *tree = rb_tree_create();
  const int keys[] = {41, 38, 31, 12, 19, 8};
  const int sorted[] = {8, 12, 19, 31, 38, 41};
  int result[6];
  size_t i;
  CHECK(tree != NULL);
  CHECK(rb_tree_valid(tree));
  CHECK(rb_tree_height(tree) == -1);
  for (i = 0U; i < 6U; ++i) {
    CHECK(rb_tree_insert(tree, keys[i]) == 1);
    CHECK(rb_tree_valid(tree));
    CHECK(rb_tree_contains(tree, keys[i]));
  }
  CHECK(rb_tree_insert(tree, 19) == 0);
  CHECK(rb_tree_size(tree) == 6U);
  CHECK(rb_tree_inorder(tree, result, 6U) == 6U);
  for (i = 0U; i < 6U; ++i)
    CHECK(result[i] == sorted[i]);
  CHECK(rb_tree_inorder(tree, result, 2U) == 6U);
  CHECK(result[0] == 8 && result[1] == 12);
  puts("C recursive, Cormen example:");
  rb_tree_print(tree, stdout);
  for (i = 0U; i < 6U; ++i) {
    CHECK(rb_tree_erase(tree, keys[i]));
    CHECK(rb_tree_valid(tree));
  }
  CHECK(rb_tree_empty(tree));
  rb_tree_destroy(tree);
}

static void test_stress(void) {
  RBTree *tree = rb_tree_create();
  int i;
  CHECK(tree != NULL);
  CHECK(rb_tree_insert(tree, INT_MIN) == 1);
  CHECK(rb_tree_insert(tree, INT_MAX) == 1);
  CHECK(rb_tree_valid(tree));
  CHECK(rb_tree_erase(tree, INT_MIN));
  CHECK(rb_tree_erase(tree, INT_MAX));
  for (i = 0; i < 512; ++i) {
    CHECK(rb_tree_insert(tree, i) == 1);
    CHECK(rb_tree_valid(tree));
  }
  CHECK(rb_tree_height(tree) <= 18);
  for (i = 511; i >= 0; --i) {
    CHECK(rb_tree_erase(tree, i));
    CHECK(rb_tree_valid(tree));
  }
  CHECK(!rb_tree_erase(tree, 100));
  rb_tree_destroy(tree);
}

static void test_mixed(void) {
  enum { RANGE = 201, STEPS = 6000 };
  bool present[RANGE] = {false};
  int actual[RANGE];
  unsigned state = 2026U;
  RBTree *tree = rb_tree_create();
  int step;
  CHECK(tree != NULL);
  for (step = 0; step < STEPS; ++step) {
    int key = (int)(next_random(&state) % RANGE) - 100;
    int action = (int)(next_random(&state) % 3U);
    size_t index = (size_t)(key + 100);
    size_t count = 0U;
    int value;
    if (action == 0) {
      int status = rb_tree_insert(tree, key);
      CHECK(status == (present[index] ? 0 : 1));
      present[index] = true;
    } else if (action == 1) {
      CHECK(rb_tree_erase(tree, key) == present[index]);
      present[index] = false;
    } else {
      CHECK(rb_tree_contains(tree, key) == present[index]);
    }
    CHECK(rb_tree_valid(tree));
    CHECK(rb_tree_inorder(tree, actual, RANGE) == rb_tree_size(tree));
    for (value = -100; value <= 100; ++value) {
      if (present[(size_t)(value + 100)])
        CHECK(actual[count++] == value);
    }
    CHECK(rb_tree_size(tree) == count);
  }
  rb_tree_destroy(tree);
}

int main(void) {
  test_cormen();
  test_stress();
  test_mixed();
  puts("Red-black tree C recursive: OK");
  return 0;
}

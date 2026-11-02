#include "red_black_tree.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, const char *expression, int line) {
  if (condition)
    return;

  fprintf(stderr, "Check failed at line %d: %s\n", line, expression);
  abort();
}

#define CHECK(expression) check((expression), #expression, __LINE__)

static void check_inorder(const RBTree *tree, const int *expected,
                          size_t expected_size) {
  int output[512];
  size_t index;
  size_t actual_size;

  CHECK(expected_size <= sizeof output / sizeof output[0]);
  actual_size = rb_tree_inorder(tree, output, sizeof output / sizeof output[0]);
  CHECK(actual_size == expected_size);

  for (index = 0U; index < expected_size; ++index)
    CHECK(output[index] == expected[index]);
}

static void test_empty_tree(void) {
  RBTree *tree = rb_tree_create();

  CHECK(tree != NULL);
  CHECK(rb_tree_empty(tree));
  CHECK(rb_tree_size(tree) == 0U);
  CHECK(rb_tree_height(tree) == -1);
  CHECK(!rb_tree_contains(tree, 10));
  CHECK(!rb_tree_erase(tree, 10));
  CHECK(rb_tree_inorder(tree, NULL, 0U) == 0U);
  CHECK(rb_tree_valid(tree));
  rb_tree_destroy(tree);
}

static void test_api_contracts(void) {
  RBTree *tree;
  int output = 0;

  CHECK(rb_tree_insert(NULL, 10) == -1);
  CHECK(!rb_tree_erase(NULL, 10));
  CHECK(!rb_tree_contains(NULL, 10));
  CHECK(rb_tree_size(NULL) == 0U);
  CHECK(rb_tree_empty(NULL));
  CHECK(rb_tree_height(NULL) == -1);
  CHECK(rb_tree_inorder(NULL, &output, 1U) == 0U);
  CHECK(!rb_tree_valid(NULL));
  rb_tree_print(NULL, stdout);
  rb_tree_destroy(NULL);

  tree = rb_tree_create();
  CHECK(tree != NULL);
  CHECK(rb_tree_insert(tree, 2) == 1);
  CHECK(rb_tree_insert(tree, 1) == 1);
  CHECK(rb_tree_insert(tree, 3) == 1);
  CHECK(rb_tree_inorder(tree, &output, 1U) == 3U);
  CHECK(output == 1);
  rb_tree_destroy(tree);
}

static void test_cormen_sequence(void) {
  RBTree *tree = rb_tree_create();
  const int keys[] = {41, 38, 31, 12, 19, 8};
  const int expected[] = {8, 12, 19, 31, 38, 41};
  size_t index;

  CHECK(tree != NULL);
  for (index = 0U; index < sizeof keys / sizeof keys[0]; ++index) {
    CHECK(rb_tree_insert(tree, keys[index]) == 1);
    CHECK(rb_tree_contains(tree, keys[index]));
    CHECK(rb_tree_valid(tree));
  }

  check_inorder(tree, expected, sizeof expected / sizeof expected[0]);
  CHECK(rb_tree_size(tree) == sizeof expected / sizeof expected[0]);
  CHECK(rb_tree_insert(tree, 19) == 0);
  CHECK(rb_tree_valid(tree));

  for (index = 0U; index < sizeof keys / sizeof keys[0]; ++index) {
    CHECK(rb_tree_erase(tree, keys[index]));
    CHECK(!rb_tree_contains(tree, keys[index]));
    CHECK(rb_tree_valid(tree));
  }
  CHECK(rb_tree_empty(tree));
  rb_tree_destroy(tree);
}

static void test_boundaries_and_deletion_shapes(void) {
  RBTree *tree = rb_tree_create();
  const int keys[] = {20, 10, 30, 5, 15, 25, 40, 22, 27};
  const int expected[] = {10, 15, 22, 27, 30, 40};
  size_t index;

  CHECK(tree != NULL);
  for (index = 0U; index < sizeof keys / sizeof keys[0]; ++index) {
    CHECK(rb_tree_insert(tree, keys[index]) == 1);
    CHECK(rb_tree_valid(tree));
  }

  CHECK(rb_tree_erase(tree, 5));
  CHECK(rb_tree_valid(tree));
  CHECK(rb_tree_erase(tree, 25));
  CHECK(rb_tree_valid(tree));
  CHECK(rb_tree_erase(tree, 20));
  CHECK(rb_tree_valid(tree));

  CHECK(rb_tree_insert(tree, INT_MIN) == 1);
  CHECK(rb_tree_insert(tree, INT_MAX) == 1);
  CHECK(rb_tree_valid(tree));
  CHECK(rb_tree_erase(tree, INT_MIN));
  CHECK(rb_tree_erase(tree, INT_MAX));
  CHECK(!rb_tree_erase(tree, 123456));
  CHECK(rb_tree_valid(tree));
  check_inorder(tree, expected, sizeof expected / sizeof expected[0]);
  rb_tree_destroy(tree);
}

static void test_ordered_stress(void) {
  RBTree *tree = rb_tree_create();
  const int limit = 512;
  int key;

  CHECK(tree != NULL);
  for (key = 1; key <= limit; ++key) {
    CHECK(rb_tree_insert(tree, key) == 1);
    CHECK(rb_tree_valid(tree));
  }

  CHECK(rb_tree_size(tree) == (size_t)limit);
  CHECK(rb_tree_height(tree) <= 18);

  for (key = 2; key <= limit; key += 2) {
    CHECK(rb_tree_erase(tree, key));
    CHECK(rb_tree_valid(tree));
  }
  for (key = 1; key <= limit; key += 2) {
    CHECK(rb_tree_erase(tree, key));
    CHECK(rb_tree_valid(tree));
  }

  CHECK(rb_tree_empty(tree));
  rb_tree_destroy(tree);
}

static unsigned next_random(unsigned *state) {
  *state = *state * 1664525U + 1013904223U;
  return *state;
}

static void shuffle(int *values, size_t count, unsigned *state) {
  size_t index;

  for (index = count; index > 1U; --index) {
    size_t other = (size_t)next_random(state) % index;
    int temporary = values[index - 1U];
    values[index - 1U] = values[other];
    values[other] = temporary;
  }
}

static void test_deterministic_random_orders(void) {
  enum { LIMIT = 160 };
  int keys[LIMIT];
  unsigned state = 2026U;
  int round;
  int index;

  for (index = 0; index < LIMIT; ++index)
    keys[index] = index - 80;

  for (round = 0; round < 8; ++round) {
    RBTree *tree = rb_tree_create();
    CHECK(tree != NULL);
    shuffle(keys, LIMIT, &state);

    for (index = 0; index < LIMIT; ++index) {
      CHECK(rb_tree_insert(tree, keys[index]) == 1);
      CHECK(rb_tree_valid(tree));
    }

    shuffle(keys, LIMIT, &state);
    for (index = 0; index < LIMIT; ++index) {
      CHECK(rb_tree_erase(tree, keys[index]));
      CHECK(rb_tree_valid(tree));
    }
    CHECK(rb_tree_empty(tree));
    rb_tree_destroy(tree);
  }
}

static void test_mixed_operations(void) {
  enum { KEY_COUNT = 201, STEPS = 5000 };
  RBTree *tree = rb_tree_create();
  bool present[KEY_COUNT] = {false};
  int actual[KEY_COUNT];
  int expected[KEY_COUNT];
  unsigned state = 417U;
  int step;

  CHECK(tree != NULL);
  for (step = 0; step < STEPS; ++step) {
    int key = (int)(next_random(&state) % KEY_COUNT) - 100;
    unsigned operation = next_random(&state) % 3U;
    size_t position = (size_t)(key + 100);
    bool expected_result;
    size_t expected_size = 0U;
    size_t actual_size;
    int value;

    if (operation == 0U) {
      int result = rb_tree_insert(tree, key);
      expected_result = !present[position];
      CHECK(result != -1);
      CHECK((result == 1) == expected_result);
      present[position] = true;
    } else if (operation == 1U) {
      expected_result = present[position];
      CHECK(rb_tree_erase(tree, key) == expected_result);
      present[position] = false;
    } else {
      CHECK(rb_tree_contains(tree, key) == present[position]);
    }

    for (value = -100; value <= 100; ++value) {
      if (present[(size_t)(value + 100)])
        expected[expected_size++] = value;
    }

    actual_size = rb_tree_inorder(tree, actual, KEY_COUNT);
    CHECK(actual_size == expected_size);
    CHECK(rb_tree_size(tree) == expected_size);
    for (position = 0U; position < expected_size; ++position)
      CHECK(actual[position] == expected[position]);
    CHECK(rb_tree_valid(tree));
  }

  rb_tree_destroy(tree);
}

/* Enumerate all insertion and deletion orders for five distinct keys. */
static bool next_permutation(int *values, size_t count) {
  size_t i = count - 1U;
  size_t j;

  while (i > 0U && values[i - 1U] >= values[i])
    --i;
  if (i == 0U)
    return false;

  j = count - 1U;
  while (values[j] <= values[i - 1U])
    --j;
  {
    int temp = values[i - 1U];
    values[i - 1U] = values[j];
    values[j] = temp;
  }
  j = count - 1U;
  while (i < j) {
    int temp = values[i];
    values[i] = values[j];
    values[j] = temp;
    ++i;
    --j;
  }
  return true;
}

static void test_all_small_orders(void) {
  int insert_order[5] = {1, 2, 3, 4, 5};

  do {
    int erase_order[5] = {1, 2, 3, 4, 5};
    do {
      RBTree *tree = rb_tree_create();
      bool present[6] = {false};
      size_t i;
      CHECK(tree != NULL);
      for (i = 0U; i < 5U; ++i) {
        CHECK(rb_tree_insert(tree, insert_order[i]) == 1);
        present[(size_t)insert_order[i]] = true;
        CHECK(rb_tree_valid(tree));
      }
      for (i = 0U; i < 5U; ++i) {
        int expected[5];
        size_t count = 0U;
        int key;
        CHECK(rb_tree_erase(tree, erase_order[i]));
        present[(size_t)erase_order[i]] = false;
        CHECK(rb_tree_valid(tree));
        for (key = 1; key <= 5; ++key) {
          if (present[(size_t)key])
            expected[count++] = key;
        }
        CHECK(rb_tree_size(tree) == count);
        check_inorder(tree, expected, count);
      }
      CHECK(rb_tree_empty(tree));
      rb_tree_destroy(tree);
    } while (next_permutation(erase_order, 5U));
  } while (next_permutation(insert_order, 5U));
}

static void print_example(void) {
  RBTree *tree = rb_tree_create();
  const int keys[] = {41, 38, 31, 12, 19, 8};
  size_t index;

  CHECK(tree != NULL);
  for (index = 0U; index < sizeof keys / sizeof keys[0]; ++index)
    CHECK(rb_tree_insert(tree, keys[index]) == 1);

  puts("Cormen insertion example in C (right subtree is above):");
  rb_tree_print(tree, stdout);
  rb_tree_destroy(tree);
}

int main(void) {
  test_empty_tree();
  test_api_contracts();
  test_cormen_sequence();
  test_boundaries_and_deletion_shapes();
  test_ordered_stress();
  test_deterministic_random_orders();
  test_mixed_operations();
  test_all_small_orders();
  print_example();

  puts("Red-black tree C11: OK");
  return EXIT_SUCCESS;
}

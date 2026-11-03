#include "red_black_tree.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main(void) {
  Tree *tree = rb_tree_create();
  assert(tree);
  assert(rb_tree_empty(tree));
  assert(rb_tree_size(tree) == 0U);
  assert(rb_tree_height(tree) == -1);
  assert(rb_tree_valid(tree));

  assert(rb_tree_insert(tree, INT_MIN) == 1);
  assert(rb_tree_insert(tree, INT_MAX) == 1);
  assert(rb_tree_insert(tree, INT_MIN) == 0);
  assert(rb_tree_valid(tree));
  assert(rb_tree_erase(tree, INT_MIN));
  assert(rb_tree_erase(tree, INT_MAX));
  assert(rb_tree_empty(tree));
  assert(rb_tree_valid(tree));

  for (int i = 0; i < 257; ++i) {
    int key = (i * 73) % 257;
    assert(rb_tree_insert(tree, key) == 1);
    assert(rb_tree_contains(tree, key));
    assert(rb_tree_valid(tree));
  }
  assert(rb_tree_size(tree) == 257U);
  assert(rb_tree_height(tree) >= 0);

  for (int i = 0; i < 257; ++i) {
    int key = (i * 97) % 257;
    assert(rb_tree_erase(tree, key));
    assert(!rb_tree_contains(tree, key));
    assert(rb_tree_valid(tree));
  }
  assert(!rb_tree_erase(tree, 42));
  assert(rb_tree_empty(tree));
  assert(rb_tree_size(tree) == 0U);
  assert(rb_tree_valid(tree));

  rb_tree_destroy(tree);
  puts("red-black tree: OK");
  return 0;
}

#include "avl.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static void assert_sorted(const AVL *tree)
{
    size_t count = avl_inorder(tree, NULL, 0);
    int *values = count == 0 ? NULL : malloc(count * sizeof(*values));
    size_t i;

    assert(count == avl_size(tree));
    assert(count == 0 || values != NULL);
    assert(avl_inorder(tree, values, count) == count);
    for (i = 1; i < count; ++i) {
        assert(values[i - 1] <= values[i]);
    }
    free(values);
}

static void test_empty_state(void)
{
    AVL tree;
    int value = 123;

    avl_init(&tree);
    assert(avl_empty(&tree));
    assert(avl_size(&tree) == 0);
    assert(avl_height(&tree) == -1);
    assert(!avl_contains(&tree, 1));
    assert(!avl_erase(&tree, 1));
    assert(!avl_minimum(&tree, &value));
    assert(!avl_maximum(&tree, &value));
    assert(!avl_minimum(&tree, NULL));
    assert(avl_inorder(&tree, NULL, 0) == 0);
    assert(avl_valid(&tree));
    avl_destroy(&tree);
}

static void test_rotation(const int values[3])
{
    AVL tree;
    size_t i;

    avl_init(&tree);
    for (i = 0; i < 3; ++i) {
        assert(avl_insert(&tree, values[i]));
        assert(avl_valid(&tree));
    }
    assert(avl_height(&tree) == 1);
    assert_sorted(&tree);
    avl_destroy(&tree);
}

static void test_rotations(void)
{
    const int ll[3] = {30, 20, 10};
    const int rr[3] = {10, 20, 30};
    const int lr[3] = {30, 10, 20};
    const int rl[3] = {10, 30, 20};

    test_rotation(ll);
    test_rotation(rr);
    test_rotation(lr);
    test_rotation(rl);
}

static void test_duplicates_and_extrema(void)
{
    const int input[] = {5, 5, 5, 3, 7, 5, INT_MIN, INT_MAX};
    const int expected[] = {INT_MIN, 3, 5, 5, 5, 5, 7, INT_MAX};
    AVL tree;
    int output[sizeof(input) / sizeof(input[0])];
    int value;
    size_t i;

    avl_init(&tree);
    for (i = 0; i < sizeof(input) / sizeof(input[0]); ++i) {
        assert(avl_insert(&tree, input[i]));
        assert(avl_valid(&tree));
        assert(avl_size(&tree) == i + 1);
    }
    assert(avl_minimum(&tree, &value) && value == INT_MIN);
    assert(avl_maximum(&tree, &value) && value == INT_MAX);
    assert(!avl_maximum(&tree, NULL));
    assert(avl_inorder(&tree, output, 3) == avl_size(&tree));
    for (i = 0; i < 3; ++i) {
        assert(output[i] == expected[i]);
    }
    assert(avl_inorder(&tree, output, avl_size(&tree)) == avl_size(&tree));
    for (i = 0; i < avl_size(&tree); ++i) {
        assert(output[i] == expected[i]);
    }
    for (i = 0; i < 4; ++i) {
        assert(avl_erase(&tree, 5));
        assert(avl_valid(&tree));
    }
    assert(!avl_contains(&tree, 5));
    assert(!avl_erase(&tree, 5));
    assert(avl_valid(&tree));
    avl_destroy(&tree);
}

static void test_erase_cases(void)
{
    const int input[] = {20, 10, 30, 5, 15, 25, 40, 27};
    AVL tree;
    size_t i;

    avl_init(&tree);
    for (i = 0; i < sizeof(input) / sizeof(input[0]); ++i) {
        assert(avl_insert(&tree, input[i]));
        assert(avl_valid(&tree));
    }

    assert(avl_erase(&tree, 5));       /* leaf */
    assert(avl_valid(&tree));
    assert(avl_erase(&tree, 25));      /* one child */
    assert(avl_valid(&tree));
    assert(avl_erase(&tree, 20));      /* two children */
    assert(avl_valid(&tree));
    assert(!avl_erase(&tree, 999));
    assert(avl_size(&tree) == 5);
    assert(!avl_contains(&tree, 5));
    assert(!avl_contains(&tree, 25));
    assert(!avl_contains(&tree, 20));
    assert_sorted(&tree);
    avl_destroy(&tree);
}

static void test_sorted_stress_and_clear(void)
{
    enum { ITEM_COUNT = 2000 };
    AVL tree;
    int i;

    avl_init(&tree);
    for (i = 0; i < ITEM_COUNT; ++i) {
        assert(avl_insert(&tree, i));
        assert(avl_valid(&tree));
    }
    assert(avl_size(&tree) == ITEM_COUNT);
    assert(avl_height(&tree) < 20);
    assert_sorted(&tree);

    for (i = 0; i < ITEM_COUNT; i += 2) {
        assert(avl_erase(&tree, i));
        assert(avl_valid(&tree));
    }
    for (i = 0; i < ITEM_COUNT; ++i) {
        assert(avl_contains(&tree, i) == (i % 2 != 0));
    }
    assert_sorted(&tree);

    avl_clear(&tree);
    assert(avl_empty(&tree));
    assert(avl_size(&tree) == 0);
    assert(avl_height(&tree) == -1);
    assert(avl_valid(&tree));
    avl_clear(&tree);
    assert(avl_valid(&tree));
    assert(avl_insert(&tree, 42));
    assert(avl_valid(&tree));
    avl_destroy(&tree);
    assert(avl_empty(&tree));
}

int main(void)
{
    test_empty_state();
    test_rotations();
    test_duplicates_and_extrema();
    test_erase_cases();
    test_sorted_stress_and_clear();
    puts("all AVL tests passed");
    return 0;
}

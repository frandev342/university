#include "avl.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

namespace {

void assertState(const AVLTree& tree, const std::vector<int>& expected) {
    assert(tree.valid());
    assert(tree.size() == expected.size());
    assert(tree.empty() == expected.empty());
    assert(tree.inorder() == expected);
}

void testEmptyState() {
    AVLTree tree;
    int value = 99;
    assertState(tree, std::vector<int>());
    assert(tree.height() == -1);
    assert(!tree.contains(99));
    assert(!tree.erase(99));
    assert(tree.valid());
    assert(!tree.minimum(value));
    assert(value == 99);
    assert(!tree.maximum(value));
    assert(value == 99);
}

void testRotation(const int first, const int second, const int third) {
    AVLTree tree;
    assert(tree.insert(first));
    assert(tree.valid());
    assert(tree.insert(second));
    assert(tree.valid());
    assert(tree.insert(third));
    assertState(tree, std::vector<int>({10, 20, 30}));
    assert(tree.height() == 1);
}

void testRotations() {
    testRotation(30, 20, 10);  // LL
    testRotation(10, 20, 30);  // RR
    testRotation(30, 10, 20);  // LR
    testRotation(10, 30, 20);  // RL
}

void testDuplicates() {
    AVLTree tree;
    const std::size_t duplicate_count = 8;
    for (std::size_t i = 0; i < duplicate_count; ++i) {
        assert(tree.insert(5));
        assert(tree.valid());
    }
    assertState(tree, std::vector<int>(duplicate_count, 5));
    assert(tree.contains(5));

    for (std::size_t remaining = duplicate_count; remaining > 0; --remaining) {
        assert(tree.erase(5));
        assert(tree.valid());
        assert(tree.size() == remaining - 1);
    }
    assertState(tree, std::vector<int>());
    assert(!tree.erase(5));
    assert(tree.valid());
}

void testEraseCases() {
    {
        AVLTree tree;
        assert(tree.insert(20));
        assert(tree.valid());
        assert(tree.insert(10));
        assert(tree.valid());
        assert(tree.insert(30));
        assert(tree.valid());
        assert(tree.erase(10));  // leaf
        assertState(tree, std::vector<int>({20, 30}));
    }
    {
        AVLTree tree;
        const int values[] = {20, 10, 30, 25};
        for (std::size_t i = 0; i < 4; ++i) {
            assert(tree.insert(values[i]));
            assert(tree.valid());
        }
        assert(tree.erase(30));  // one child
        assertState(tree, std::vector<int>({10, 20, 25}));
    }
    {
        AVLTree tree;
        const int values[] = {20, 10, 30, 25, 35};
        for (std::size_t i = 0; i < 5; ++i) {
            assert(tree.insert(values[i]));
            assert(tree.valid());
        }
        assert(tree.erase(30));  // two children
        assertState(tree, std::vector<int>({10, 20, 25, 35}));
    }
    {
        AVLTree tree;
        assert(tree.insert(20));
        assert(tree.valid());
        assert(tree.insert(10));
        assert(tree.valid());
        assert(tree.insert(30));
        assert(tree.valid());
        assert(tree.erase(20));  // root
        assertState(tree, std::vector<int>({10, 30}));
        assert(!tree.contains(20));
    }
}

void testExtremaAndInorder() {
    AVLTree tree;
    const int values[] = {4, -2, 9, 1, 4, 7, 12};
    for (std::size_t i = 0; i < 7; ++i) {
        assert(tree.insert(values[i]));
        assert(tree.valid());
    }

    int minimum = 0;
    int maximum = 0;
    assert(tree.minimum(minimum));
    assert(tree.maximum(maximum));
    assert(minimum == -2);
    assert(maximum == 12);
    assertState(tree, std::vector<int>({-2, 1, 4, 4, 7, 9, 12}));
}

void testSortedStressAndClear() {
    AVLTree tree;
    const int count = 1000;
    for (int value = 0; value < count; ++value) {
        assert(tree.insert(value));
        assert(tree.valid());
    }
    assert(tree.size() == static_cast<std::size_t>(count));
    const std::vector<int> ordered = tree.inorder();
    assert(ordered.size() == static_cast<std::size_t>(count));
    assert(std::is_sorted(ordered.begin(), ordered.end()));
    assert(ordered.front() == 0);
    assert(ordered.back() == count - 1);

    for (int value = 0; value < count; value += 2) {
        assert(tree.erase(value));
        assert(tree.valid());
    }
    for (int value = 0; value < count; ++value) {
        assert(tree.contains(value) == (value % 2 != 0));
    }

    tree.clear();
    assertState(tree, std::vector<int>());
    assert(tree.height() == -1);
    tree.clear();
    assert(tree.valid());
}

}  // namespace

int main() {
    testEmptyState();
    testRotations();
    testDuplicates();
    testEraseCases();
    testExtremaAndInorder();
    testSortedStressAndClear();
    std::cout << "All AVL tree tests passed.\n";
    return 0;
}

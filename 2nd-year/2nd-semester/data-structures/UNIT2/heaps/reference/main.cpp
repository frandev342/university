#include "max_heap.hpp"

#include <cassert>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

template <typename Exception, typename Function>
void assertThrows(Function&& function) {
    bool caught = false;
    try {
        std::forward<Function>(function)();
    } catch (const Exception&) {
        caught = true;
    } catch (...) {
        assert(false && "unexpected exception type");
    }
    assert(caught);
}

std::vector<int> extractAll(MaxHeap heap) {
    std::vector<int> result;
    while (!heap.empty()) {
        result.push_back(heap.extractMax());
        assert(heap.isValid());
    }
    return result;
}

void testClrsBuild() {
    MaxHeap heap({4, 1, 3, 2, 16, 9, 10, 14, 8, 7});
    const std::vector<int> expected{16, 14, 10, 8, 7, 9, 3, 2, 4, 1};

    assert(heap.values() == expected);
    assert(heap.size() == expected.size());
    assert(heap.maximum() == 16);
    assert(heap.isValid());
}

void testInsertAndIncreaseKey() {
    MaxHeap inserted({4, 1, 3, 2, 16, 9, 10, 14, 8, 7});
    inserted.insert(15);
    const std::vector<int> afterInsert{16, 15, 10, 8, 14, 9, 3, 2, 4, 1, 7};
    assert(inserted.values() == afterInsert);
    assert(inserted.isValid());

    MaxHeap increased({4, 1, 3, 2, 16, 9, 10, 14, 8, 7});
    increased.increaseKey(8, 15);
    const std::vector<int> afterIncrease{16, 15, 10, 14, 7, 9, 3, 2, 8, 1};
    assert(increased.values() == afterIncrease);
    assert(increased.isValid());

    increased.increaseKey(0, 16); // Equality is a valid non-decreasing update.
    assertThrows<std::invalid_argument>([&increased] { increased.increaseKey(1, 0); });
    assertThrows<std::out_of_range>([&increased] { increased.increaseKey(99, 100); });
}

void testExtractionOrder() {
    MaxHeap heap({3, -5, 3, 10, 0, -5, 10, 2});
    const std::vector<int> expected{10, 10, 3, 3, 2, 0, -5, -5};
    assert(extractAll(heap) == expected);

    MaxHeap negatives({-8, -1, -12, -3});
    const std::vector<int> negativeOrder{-1, -3, -8, -12};
    assert(extractAll(negatives) == negativeOrder);
}

void testBoundaryCases() {
    MaxHeap empty;
    assert(empty.empty());
    assert(empty.size() == 0);
    assert(empty.values().empty());
    assert(empty.isValid());
    assertThrows<std::underflow_error>([&empty] { (void)empty.maximum(); });
    assertThrows<std::underflow_error>([&empty] { (void)empty.extractMax(); });
    assertThrows<std::out_of_range>([&empty] { empty.increaseKey(0, 1); });

    empty.insert(-7);
    assert(!empty.empty());
    assert(empty.size() == 1);
    assert(empty.maximum() == -7);
    assert(empty.isValid());
    assert(empty.extractMax() == -7);
    assert(empty.empty());

    MaxHeap one({42});
    assert(one.values() == std::vector<int>{42});
    one.increaseKey(0, 50);
    assert(one.maximum() == 50);
    assert(one.extractMax() == 50);
    assert(one.empty());
}

} // namespace

int main() {
    testClrsBuild();
    testInsertAndIncreaseKey();
    testExtractionOrder();
    testBoundaryCases();
    std::cout << "All MaxHeap tests passed.\n";
}

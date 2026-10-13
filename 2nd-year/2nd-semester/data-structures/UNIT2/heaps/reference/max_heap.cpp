#include "max_heap.hpp"

#include <stdexcept>
#include <utility>

MaxHeap::MaxHeap(std::vector<int> values) : data_(std::move(values)) {
    buildHeap();
}

bool MaxHeap::empty() const noexcept {
    return data_.empty();
}

std::size_t MaxHeap::size() const noexcept {
    return data_.size();
}

int MaxHeap::maximum() const {
    if (empty()) {
        throw std::underflow_error("maximum of an empty heap");
    }
    return data_.front();
}

int MaxHeap::extractMax() {
    if (empty()) {
        throw std::underflow_error("extractMax from an empty heap");
    }

    const int result = data_.front();
    data_.front() = data_.back();
    data_.pop_back();

    // After replacing the root, only its downward path can violate the heap.
    if (!empty()) {
        siftDown(0);
    }
    return result;
}

void MaxHeap::insert(int value) {
    data_.push_back(value);
    siftUp(data_.size() - 1);
}

void MaxHeap::increaseKey(std::size_t index, int newValue) {
    if (index >= data_.size()) {
        throw std::out_of_range("heap index out of range");
    }
    if (newValue < data_[index]) {
        throw std::invalid_argument("increaseKey cannot decrease a key");
    }

    data_[index] = newValue;
    // Increasing a key cannot break either of its child relationships.
    siftUp(index);
}

const std::vector<int>& MaxHeap::values() const noexcept {
    return data_;
}

bool MaxHeap::isValid() const noexcept {
    // Every non-root node has one parent, so checking these edges is sufficient.
    for (std::size_t index = 1; index < data_.size(); ++index) {
        if (data_[parent(index)] < data_[index]) {
            return false;
        }
    }
    return true;
}

std::size_t MaxHeap::parent(std::size_t index) noexcept {
    return (index - 1) / 2;
}

std::size_t MaxHeap::left(std::size_t index) noexcept {
    return 2 * index + 1;
}

std::size_t MaxHeap::right(std::size_t index) noexcept {
    return 2 * index + 2;
}

void MaxHeap::siftDown(std::size_t index) noexcept {
    while (left(index) < data_.size()) {
        std::size_t largerChild = left(index);
        const std::size_t rightChild = right(index);
        if (rightChild < data_.size() && data_[rightChild] > data_[largerChild]) {
            largerChild = rightChild;
        }

        if (data_[index] >= data_[largerChild]) {
            return;
        }
        std::swap(data_[index], data_[largerChild]);
        index = largerChild;
    }
}

void MaxHeap::siftUp(std::size_t index) noexcept {
    // The index > 0 guard also prevents unsigned underflow in parent().
    while (index > 0 && data_[parent(index)] < data_[index]) {
        const std::size_t parentIndex = parent(index);
        std::swap(data_[index], data_[parentIndex]);
        index = parentIndex;
    }
}

void MaxHeap::buildHeap() noexcept {
    // Leaves already satisfy the invariant; process internal nodes in reverse.
    for (std::size_t count = data_.size() / 2; count > 0; --count) {
        siftDown(count - 1);
    }
}

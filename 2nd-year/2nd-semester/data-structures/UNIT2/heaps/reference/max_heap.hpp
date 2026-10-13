#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include <cstddef>
#include <vector>

class MaxHeap {
public:
    MaxHeap() = default;
    explicit MaxHeap(std::vector<int> values);

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] int maximum() const;
    int extractMax();
    void insert(int value);
    void increaseKey(std::size_t index, int newValue);
    [[nodiscard]] const std::vector<int>& values() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;

private:
    std::vector<int> data_;

    [[nodiscard]] static std::size_t parent(std::size_t index) noexcept;
    [[nodiscard]] static std::size_t left(std::size_t index) noexcept;
    [[nodiscard]] static std::size_t right(std::size_t index) noexcept;
    void siftDown(std::size_t index) noexcept;
    void siftUp(std::size_t index) noexcept;
    void buildHeap() noexcept;
};

#endif

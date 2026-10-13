#ifndef MAX_HEAP_PRACTICE_HPP
#define MAX_HEAP_PRACTICE_HPP

#include <cstddef>
#include <vector>

class PracticeMaxHeap {
public:
    PracticeMaxHeap() = default;
    explicit PracticeMaxHeap(std::vector<int> values);

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] int maximum() const;
    int extractMax();
    void insert(int value);
    void increaseKey(std::size_t index, int newValue);
    [[nodiscard]] const std::vector<int>& values() const noexcept;
    [[nodiscard]] bool isValid() const;

private:
    std::vector<int> data_;

    [[nodiscard]] static std::size_t parent(std::size_t index);
    [[nodiscard]] static std::size_t left(std::size_t index);
    [[nodiscard]] static std::size_t right(std::size_t index);
    void siftDown(std::size_t index);
    void siftUp(std::size_t index);
    void buildHeap();
};

#endif

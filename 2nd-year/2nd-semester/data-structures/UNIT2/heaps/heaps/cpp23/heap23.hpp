#ifndef HEAP_CPP23_HPP
#define HEAP_CPP23_HPP
#include <array>
#include <cstddef>
#include <expected>
#include <utility>

namespace heap23 {
struct Process {
  int pid;
  int priority;
};
enum class Error { empty, full };

/* Capacidad conocida en compilacion; no reserva memoria dinamica.
   Esta es una alternativa de almacenamiento, no una nueva clase de heap. */
template <std::size_t Capacity> class Heap {
public:
  constexpr bool push(Process value) {
    if (size_ == Capacity)
      return false;
    std::size_t i = size_++;
    while (i > 0U) {
      const std::size_t p = (i - 1U) / 2U;
      if (data_[p].priority >= value.priority)
        break;
      data_[i] = data_[p];
      i = p;
    }
    data_[i] = value;
    return true;
  }
  constexpr std::expected<Process, Error> pop() {
    if (size_ == 0U)
      return std::unexpected(Error::empty);
    const Process answer = data_[0];
    const Process last = data_[--size_];
    if (size_ != 0U) {
      std::size_t i = 0U;
      while (i < size_ / 2U) { // garantiza existencia de hijo izquierdo
        const std::size_t l = 2U * i + 1U;
        const std::size_t r = l + 1U;
        std::size_t best = l;
        if (r < size_ && data_[r].priority > data_[l].priority)
          best = r;
        if (data_[best].priority <= last.priority)
          break;
        data_[i] = data_[best];
        i = best;
      }
      data_[i] = last;
    }
    return answer;
  }
  constexpr bool valid() const {
    for (std::size_t i = 1U; i < size_; ++i) {
      if (data_[(i - 1U) / 2U].priority < data_[i].priority)
        return false;
    }
    return true;
  }
  constexpr std::size_t size() const { return size_; }

private:
  std::array<Process, Capacity> data_{};
  std::size_t size_ = 0U;
};
} // namespace heap23
#endif

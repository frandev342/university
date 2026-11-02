#include "heap.hpp"
#include "indices.hpp"
#include <cstddef>

namespace heap11 {
Heap::Heap(std::size_t capacity) : capacity_(capacity) {
  data_.reserve(capacity_);
}
// void Heap::sift_down(std::size_t i) {
//   while (i < size() / 2U) {
//     const std::size_t left_ = left(i);
//     const std::size_t right_ = right(i);
//     std::size_t best = left_;
//     if (right_ < size() && data_[right_].priority > data_[left_].priority)
//       best = right_;
//     if (data_[i].priority >= data_[best].priority)
//       return;
//     Process temp = data_[best];
//     data_[best] = data_[i];
//     data_[i] = temp;
//     i = best;
//   }
// }
void Heap::sift_down(std::size_t i) {
  Process value = data_[i];
  while (i < size() / 2U) {
    const std::size_t left_ = left(i);
    const std::size_t right_ = right(i);
    std::size_t best = left_;
    if (right_ < size() && data_[right_].priority > data_[left_].priority)
      best = right_;
    if (value.priority >= data_[best].priority)
      break;
    data_[i] = data_[best];
    // i representa la posición actual del hueco, para extraer sus hijos, osea
    // una casilla hueca. La verdadera representación del dato que "viaja" es
    // value El cual al final es colocado en el hueco final
    i = best;
  }
  data_[i] = value;
}

bool Heap::valid() const {
  for (std::size_t i = 1U; i < size(); i++) {
    if (data_[parent(i)].priority < data_[i].priority)
      return false;
  }
  return true;
}

bool Heap::build(const std::vector<Process> &input) {
  if (input.size() > capacity_)
    return false;
  // Utilizando la asignación por copia todo se realiza solo.
  // SI vamos a usar for, preferible usar .resize() para cambiar el tamaño de
  // data_
  // Otra alternativa es limpiar data_ con .clear() y luego usar push_back
  data_ = input;
  for (std::size_t i = data_.size() / 2; i > 0U; --i)
    sift_down(i - 1);
  return true;
}

bool Heap::push(Process x) {
  if (data_.size() == capacity_)
    return false;
  data_.push_back(x);
  std::size_t i = data_.size() - 1U;
  while (i > 0U) {
    std::size_t parent_ = parent(i);
    if (data_[parent_].priority >= x.priority)
      break;
    data_[i] = data_[parent_];
    i = parent_;
  }
  data_[i] = x;
  return true;
}

bool Heap::pop(Process &out) {
  if (data_.empty())
    return false;
  out = data_.front();
  data_[0] = data_.back();
  data_.pop_back();
  if (!data_.empty())
    sift_down(0U);
  return true;
}

} // namespace heap11
//

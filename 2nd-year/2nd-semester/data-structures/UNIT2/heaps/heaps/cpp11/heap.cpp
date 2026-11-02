#include "heap.hpp"
#include "indices.hpp"
#include <utility>
namespace heap11 {
Heap::Heap(std::size_t capacity) : capacity_(capacity) {
  // .reserve: asigna previamente suficiente memoria en el sistema para
  // almacenar al menos n elementos, sin modificar el tamaño real del vector Su
  // diferencia con un arreglo simple es que si puede aumentar su capacidad.
  // Osea mantiene su comportamiento normal, solo asegura que las operaciones
  // sean más eficientes y rápidas
  data_.reserve(capacity_); // puede lanzar std::bad_alloc
}
// Tiempo: O(log n)
// Memoria auxiliar: O(1)
void Heap::sift_down(std::size_t i) {
  Process value = data_[i];
  while (i < size() / 2U) {
    const std::size_t left_ = left(i);
    const std::size_t right_ = right(i);
    std::size_t best = left_;
    if (right_ < size() && data_[right_].priority > data_[left_].priority)
      best = right_;
    // De est forma no se pierde el dato que debe bajar
    if (value.priority >= data_[best].priority)
      break;
    data_[i] = data_[best];
    // i no representa nada, sola la posición para extraer sus hijos, osea una
    // casilla hueca
    // La verdadera representación del dato que "viaja" es value
    // El cual al final es colocado en el hueco final
    i = best;
  }
  data_[i] = value;
}

// Tiempo O(n) Espacio auxiliar O(1)
bool Heap::build(const std::vector<Process> &input) {
  if (input.size() > capacity_)
    return false;
  data_ = input;
  for (std::size_t k = data_.size() / 2U; k > 0U; --k)
    sift_down(k - 1U);
  return true;
}

// Complejidad O(log n)
bool Heap::push(Process x) {
  if (data_.size() == capacity_)
    return false;
  data_.push_back(x);
  std::size_t i = data_.size() - 1U;
  while (i > 0U && data_[parent(i)].priority < data_[i].priority) {
    std::swap(data_[i], data_[parent(i)]);
    i = parent(i);
  }
  return true;
}
// Comprobar vacío: O(1)
// Obtener la raíz: O(1)
// Reemplazar y eliminar: O(1)
// sift_down: O(log n)
// Complejidad total:
// O(log n)
bool Heap::pop(Process &out) {
  if (data_.empty())
    return false;
  out = data_.front();
  data_.front() = data_.back();
  data_.pop_back();
  if (!data_.empty())
    sift_down(0U);
  return true;
}

// Tiempo O(n)
bool Heap::valid() const {
  for (std::size_t i = 1U; i < data_.size(); ++i) {
    if (data_[parent(i)].priority < data_[i].priority)
      return false;
  }
  return true;
}
} // namespace heap11

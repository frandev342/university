#pragma once

// Ejercicio #3 — QueueShift
// Esta es una cola donde el frente siempre está en data_[0]. Al hacer pop(),
// todos los elementos se desplazan a la izquierda.

// Entiendo perfectamente. En un Queue Shift, la regla de diseño obliga a que el
// frente de la cola siempre esté en la posición 0. Por lo tanto, desplazar
// todos los elementos con un bucle es estructuralmente necesario para este tipo
// específico de cola.Bajo esta restricción exacta, tu método pop() ya está
// implementado de la forma más eficiente posible. No se puede bajar de O(n)
// si el requisito inamovible es mantener el frente en el índice cero en un
// arreglo contiguo,

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T> class QueueShift {
private:
  T *data_ = nullptr;
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;
  std::size_t moves_ = 0;

  void grow();

public:
  QueueShift() = default;
  QueueShift(const QueueShift &other);
  QueueShift(QueueShift &&other) noexcept;
  QueueShift &operator=(const QueueShift &other);
  QueueShift &operator=(QueueShift &&other) noexcept;
  ~QueueShift();

  void push(const T &x);
  void push(T &&x);
  void pop();

  T &front();
  const T &front() const;

  bool empty() const noexcept { return size_ == 0; }
  std::size_t size() const noexcept { return size_; }
  std::size_t capacity() const noexcept { return capacity_; }

  std::size_t moves() const noexcept { return moves_; }
  void reset_counters() noexcept { moves_ = 0; }
};

// Restricción didáctica: T debe ser construible por defecto y asignable.
// moves_ cuenta transferencias debidas a redimensionamiento o desplazamiento,
// no la asignación normal del elemento que se inserta.

template <typename T> void QueueShift<T>::grow() {
  std::size_t new_capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
  T *new_data = new T[new_capacity_];
  for (std::size_t i = 0; i < size_; i++) {
    new_data[i] = data_[i];
  }
  delete[] data_;
  data_ = new_data;
  capacity_ = new_capacity_;
  moves_ += size_;
}

template <typename T> QueueShift<T>::QueueShift(const QueueShift &other) {
  data_ = new T[other.capacity_];
  size_ = other.size_;
  capacity_ = other.capacity_;
  moves_ = other.moves_;
  for (std::size_t i = 0; i < other.size_; i++) {
    data_[i] = other.data_[i];
  }
}

template <typename T> QueueShift<T>::QueueShift(QueueShift &&other) noexcept {
  // TODO: transferir ownership y dejar el origen vacío.
  data_ = other.data_;
  capacity_ = other.capacity_;
  size_ = other.size_;
  moves_ = other.moves_;
  other.data_ = nullptr;
  other.moves_ = 0;
  other.capacity_ = 0;
  other.size_ = 0;
}

template <typename T>
QueueShift<T> &QueueShift<T>::operator=(const QueueShift &other) {
  if (this != &other) {
    delete[] data_;
    data_ = new T[other.capacity_];
    size_ = other.size_;
    capacity_ = other.capacity_;
    moves_ = other.moves_;
    for (std::size_t i = 0; i < other.size_; i++) {
      data_[i] = other.data_[i];
    }
  }
  return *this;
}

template <typename T>
QueueShift<T> &QueueShift<T>::operator=(QueueShift &&other) noexcept {
  if (this != &other) {
    delete[] data_;
    data_ = other.data_;
    capacity_ = other.capacity_;
    size_ = other.size_;
    moves_ = other.moves_;
    other.data_ = nullptr;
    other.moves_ = 0;
    other.capacity_ = 0;
    other.size_ = 0;
  }
  return *this;
}

template <typename T> QueueShift<T>::~QueueShift() { delete[] data_; }

template <typename T> void QueueShift<T>::push(const T &x) {
  if (size_ == capacity_)
    grow();
  data_[size_] = x;
  size_++;
}

template <typename T> void QueueShift<T>::push(T &&x) {
  if (size_ == capacity_)
    grow();
  data_[size_] = std::move(x);
  size_++;
}

template <typename T> void QueueShift<T>::pop() {
  if (size_ == 0)
    throw std::out_of_range("pop on empty size");
  for (std::size_t i = 0; i < size_ - 1; i++) {
    data_[i] = data_[i + 1];
  }
  moves_ += size_ - 1;
  size_--;
}

template <typename T> T &QueueShift<T>::front() {
  if (size_ == 0)
    throw std::out_of_range("front on empty size");
  return data_[0];
}

template <typename T> const T &QueueShift<T>::front() const {
  if (size_ == 0)
    throw std::out_of_range("front on empty size");
  return data_[0];
}

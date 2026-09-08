#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T> class QueueCircular {
private:
  T *data_ = nullptr;
  std::size_t head_ = 0;
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;
  std::size_t moves_ = 0;

  void grow();
  std::size_t physical_index(std::size_t logical) const noexcept;

public:
  QueueCircular() = default;
  QueueCircular(const QueueCircular &other);
  QueueCircular(QueueCircular &&other) noexcept;
  QueueCircular &operator=(const QueueCircular &other);
  QueueCircular &operator=(QueueCircular &&other) noexcept;
  ~QueueCircular();

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
// pop() NO debe desplazar los elementos restantes.

template <typename T>
std::size_t
QueueCircular<T>::physical_index(std::size_t logical) const noexcept {
  return capacity_ == 0 ? 0 : (head_ + logical) % capacity_;
}

template <typename T> void QueueCircular<T>::grow() {
  std::size_t new_capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
  T *new_data = new T[new_capacity_];
  for (std::size_t i = 0; i < size_; i++) {
    new_data[i] = data_[physical_index(i)];
  }
  delete[] data_;
  data_ = new_data;
  head_ = 0;
  capacity_ = new_capacity_;
  moves_ += size_;
}

template <typename T>
QueueCircular<T>::QueueCircular(const QueueCircular &other) {
  data_ = new T[other.capacity_];
  size_ = other.size_;
  capacity_ = other.capacity_;
  moves_ = other.moves_;
  // Copía sus elementos organizandolos y reinicciando el head_ en la nueva
  // organización, pero con orden igual
  head_ = 0;
  for (std::size_t i = 0; i < other.size_; i++) {
    data_[i] = other.data_[other.physical_index(i)];
  }
}

template <typename T>
QueueCircular<T>::QueueCircular(QueueCircular &&other) noexcept {
  // TODO: transferir ownership y dejar el origen vacío.
  data_ = other.data_;
  capacity_ = other.capacity_;
  size_ = other.size_;
  moves_ = other.moves_;
  head_ = other.head_;
  other.data_ = nullptr;
  other.moves_ = 0;
  other.capacity_ = 0;
  other.size_ = 0;
  other.head_ = 0;
}

template <typename T>
QueueCircular<T> &QueueCircular<T>::operator=(const QueueCircular &other) {
  if (this != &other) {
    delete[] data_;
    data_ = new T[other.capacity_];
    size_ = other.size_;
    capacity_ = other.capacity_;
    moves_ = other.moves_;
    // Copía sus elementos organizandolos y reinicciando el head_ en la nueva
    // organización, pero con orden igual
    head_ = 0;
    for (std::size_t i = 0; i < other.size_; i++) {
      data_[i] = other.data_[other.physical_index(i)];
    }
  }
  return *this;
}

template <typename T>
QueueCircular<T> &QueueCircular<T>::operator=(QueueCircular &&other) noexcept {
  // TODO: liberar el recurso actual, transferir ownership y vaciar el origen.
  if (this != &other) {
    delete[] data_;
    data_ = other.data_;
    capacity_ = other.capacity_;
    size_ = other.size_;
    moves_ = other.moves_;
    head_ = other.head_;
    other.data_ = nullptr;
    other.moves_ = 0;
    other.capacity_ = 0;
    other.size_ = 0;
    other.head_ = 0;
  }
  return *this;
}

template <typename T> QueueCircular<T>::~QueueCircular() { delete[] data_; }

template <typename T> void QueueCircular<T>::push(const T &x) {
  if (size_ == capacity_)
    grow();
  data_[physical_index(size_)] = x;
  size_++;
}

template <typename T> void QueueCircular<T>::push(T &&x) {
  if (size_ == capacity_)
    grow();
  data_[physical_index(size_)] = std::move(x);
  size_++;
}

template <typename T> void QueueCircular<T>::pop() {
  if (size_ == 0)
    throw std::out_of_range("pop on empty size");
  head_++;
  size_--;
}

template <typename T> T &QueueCircular<T>::front() {
  if (size_ == 0)
    throw std::out_of_range("front on empty size");
  return data_[physical_index(0)];
}

template <typename T> const T &QueueCircular<T>::front() const {
  if (size_ == 0)
    throw std::out_of_range("front on empty size");
  // Acceder al 1er elemento correctamente, sin usar head_
  return data_[physical_index(0)];
}

/* 5. Resultados de make bench
❯ make bench
./benchmark
=== Queue: misma especificación, distinta representación ===
n         Shift ms          Shift moves         Circular ms       Circular moves
std::queue ms 1000      0.047644          500523              0.007627 1023
0.003561 5000      0.786912          12505691            0.039745          8191
0.013754 10000     2.9601            50011383            0.088242          16383
0.019533 20000     8.493             200022767           0.202216          32767
0.036038 40000     41.1316           800045535           0.415004          65535
0.074237

QueueShift es ~100x más lento que QueueCircular en n=40,000.
¿Por qué?
- QueueShift: Cada pop() desplaza todos los elementos → O(n) por pop → O(n²)
total
- QueueCircular: Cada pop() solo hace head_++ → O(1) por pop → O(n) total (solo
grow() cuenta moves)

Mismo TDA + distinta representación = costos radicalmente diferentes
*/

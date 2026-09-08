#ifndef _STACK_RAW_
#define _STACK_RAW_

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T> class StackRaw {
private:
  T *data_ = nullptr;
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;

  void grow();

public:
  StackRaw() = default;
  StackRaw(const StackRaw &other);
  StackRaw(StackRaw &&other) noexcept;
  StackRaw &operator=(const StackRaw &other);
  StackRaw &operator=(StackRaw &&other) noexcept;
  ~StackRaw();

  void push(const T &x);
  void push(T &&x);
  void pop();

  T &top();
  const T &top() const;

  bool empty() const noexcept { return size_ == 0; }
  std::size_t size() const noexcept { return size_; }
  std::size_t capacity() const noexcept { return capacity_; }
};

template <typename T> void StackRaw<T>::grow() {
  std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
  T *new_data = new T[new_capacity];
  for (std::size_t i = 0; i < size_; i++)
    new_data[i] = data_[i];
  delete[] data_;
  data_ = new_data;
  capacity_ = new_capacity;
}

template <typename T> StackRaw<T>::StackRaw(const StackRaw &other) {
  data_ = new T[other.capacity_];
  size_ = other.size_;
  capacity_ = other.capacity_;
  for (std::size_t i = 0; i < other.size_; i++)
    data_[i] = other.data_[i];
}

template <typename T> StackRaw<T>::StackRaw(StackRaw &&other) noexcept {
  size_ = other.size_;
  data_ = other.data_;
  capacity_ = other.capacity_;
  other.size_ = 0;
  other.data_ = nullptr;
  other.capacity_ = 0;
}

template <typename T>
StackRaw<T> &StackRaw<T>::operator=(const StackRaw &other) {
  if (this != &other) {
    delete[] data_;
    size_ = other.size_;
    data_ = new T[other.capacity_];
    capacity_ = other.capacity_;
    for (std::size_t i = 0; i < other.size_; i++)
      data_[i] = other.data_[i];
  }
  return *this;
}

template <typename T>
StackRaw<T> &StackRaw<T>::operator=(StackRaw &&other) noexcept {
  if (this != &other) {
    delete[] data_;
    size_ = other.size_;
    data_ = other.data_;
    capacity_ = other.capacity_;
    other.size_ = 0;
    other.data_ = nullptr;
    other.capacity_ = 0;
  }
  return *this;
}

template <typename T> StackRaw<T>::~StackRaw() { delete[] data_; }

// push, añade un valor a la fila
template <typename T> void StackRaw<T>::push(const T &x) {
  if (size_ == capacity_)
    grow();
  data_[size_] = x;
  size_++;
}

// Al pasar x a la función, dentro vuelve a ser un lvalue, por eso se usa
// std::move, para que cumpla su rol como movimiento e inhabilite a x
template <typename T> void StackRaw<T>::push(T &&x) {
  if (size_ == capacity_)
    grow();
  data_[size_] = std::move(x);
  size_++;
}

template <typename T> void StackRaw<T>::pop() {
  if (empty())
    throw std::out_of_range("pop on empty stack");
  --size_;
}

// Para uso general, modificación y objetos normales.
template <typename T> T &StackRaw<T>::top() {
  if (empty())
    throw std::out_of_range("top on empty stack");
  return data_[size_ - 1];
}

// Permitir el uso de objetos constantes y entrada a funciones(const Object &
// obj) y variables Es necesario const T &, porque si no habría una
// contradicción de promesas. Si fuera T & permitiría la modificación, pero con
// un const delante que prohibe eso, hay una contradicción. La función del const
// de delante es en el uso de OBjetos constantes
//
// METAFORA: Le das la promesa de qué no modificará nada, pero le estás
// entregando una llave a cualquiera
//
template <typename T> const T &StackRaw<T>::top() const {
  if (empty())
    throw std::out_of_range("top on empty stack");
  return data_[size_ - 1];
}

#endif // _STACK_RAW_

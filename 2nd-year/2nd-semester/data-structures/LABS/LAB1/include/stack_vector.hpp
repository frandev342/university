#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T> class StackVector {
private:
  std::vector<T> data_;

public:
  StackVector() = default;

  void push(const T &x) { data_.push_back(x); }
  void push(T &&x) { data_.push_back(std::move(x)); }

  void pop() {
    if (data_.empty())
      throw std::out_of_range("pop on empty stack");
    data_.pop_back();
  }

  T &top() {
    if (data_.empty())
      throw std::out_of_range("top on empty stack");
    return data_.back();
  }

  const T &top() const {
    if (data_.empty())
      throw std::out_of_range("top on empty stack");
    return data_.back();
  }

  bool empty() const noexcept { return data_.empty(); }
  std::size_t size() const noexcept { return data_.size(); }
  std::size_t capacity() const noexcept { return data_.capacity(); }
};

/*
1. ¿qué responsabilidad ha sido delegada a std::vector?;
Que maneje la memoria, copias y movimiento. Por eso no hay constructores mas que
solo el constructor por defecto. Además vector maneja memoria dinámica, por lo
que no habrá problema con grow, ya que el método push_back de vector realiza la
misma función de crecimiento geométrico.

La administración de memoria: reservar bloque, redimensionar cuando se
llena (crecimiento geométrico), liberar memoria. Todo lo que tú hiciste
manualmente con new[], delete[] y grow() en StackRaw.

2. ¿qué parte del TDA permanece idéntica?
El constructor por defecto, de por sí dejaba a los valores como venían
asignados, sobre el puntero data_ el cuál se tenía que manejar luego, pero
vector lo hace todo, asi que vector se encargará de manejarlo a pesar de que no
está inicializado aún

Además los métodos que si debemos manejar para que respeten un nombre y
comportamiento. Se manejará utilizando los métodos de la clase vector a nuestra
conveniencia. push, pop, top, empty, size y capacity, para que los podamos
llamar desde la clase

La interfaz pública: push, pop, top, empty, size. El comportamiento observable
de la pila (LIFO, misma semántica, mismas excepciones) no cambia.

3. ¿Por qué usar STL no elimina la necesidad de entender el costo de las
operaciones? Porque std::vector tiene sus propios costos ocultos. Por ejemplo,
push_back es O(1) amortizado pero a veces O(n) cuando redimensiona. Si no
entiendes eso, podrías escribir código ineficiente sin saberlo.

En este caso no hay diferencia de costos, ya que por dentro realizan los mismo.
Vector realiza las mismas operaciones que las que implementamos en el
stack_raw.hpp
*/

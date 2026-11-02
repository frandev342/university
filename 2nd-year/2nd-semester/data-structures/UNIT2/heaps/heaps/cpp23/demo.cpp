#include "heap23.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

/* TODO CPP23: Anada pruebas adicionales a esta funcion.
   Todas las comprobaciones de static_assert ocurren al compilar. */
constexpr bool probar_heap_en_compilacion() {
  heap23::Heap<5> h;
  if (!h.push({1, 3}) || !h.push({2, 9}) || !h.push({3, 1}))
    return false;
  if (!h.valid())
    return false;
  auto x = h.pop();
  return x.has_value() && x->pid == 2 && x->priority == 9 && h.valid() &&
         h.size() == 2U;
}
static_assert(probar_heap_en_compilacion(), "heap incorrecto");

/* Los algoritmos std::ranges::make_heap e is_heap son de C++20. */
constexpr bool prueba_ranges() {
  std::array<int, 6> a{{4, 1, 3, 2, 16, 9}};
  std::ranges::make_heap(a);
  return std::ranges::is_heap(a) && a.front() == 16;
}
static_assert(prueba_ranges(), "biblioteca estandar: heap incorrecto");

/* if consteval si es una novedad de C++23. */
constexpr bool evaluacion_constante() {
  if consteval {
    return true;
  } else {
    return false;
  }
}
static_assert(evaluacion_constante(), "debe evaluarse en compilacion");

int main() {
  assert(probar_heap_en_compilacion());
  bool runtime = evaluacion_constante();
  assert(!runtime);
  std::cout << "C++23: static_assert y pruebas en ejecucion OK\n";
}

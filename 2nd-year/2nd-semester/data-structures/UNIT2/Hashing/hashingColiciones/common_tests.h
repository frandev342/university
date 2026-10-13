#ifndef COMMON_TESTS_HPP
#define COMMON_TESTS_HPP
#include <cstdlib>
#include <iostream>

inline void check(bool condition, const char *message) {
  if (!condition) {
    std::cerr << "FALLO: " << message << '\n';
    std::exit(EXIT_FAILURE);
  }
}
// invariante: condición que debe ser correcta o cierta en toda la vida de un
// objeto

// Este bloque se entrega listo: D es la clase que se quiere probar.
// Agregar aqui los casos comunes; usar solo la interfaz publica.
template <class D> void common_tests() {
  D d;
  int value = -123;
  // batchs
  check(d.size() == 0, "El diccionario debe comenzar vacio");
  check(!d.get(10, value), "La clave 10 aun no existe");
  check(value == -123, "Una consulta fallida conserva la salida");

  check(d.put(10, 80), "Primera insercion");
  check(d.size() == 1, "Una asociacion");
  check(d.get(10, value) && value == 80, "Consultar clave 10");
  check(d.put(10, 95), "Actualizar clave 10");
  check(d.size() == 1, "Actualizar no aumenta el contador");
  check(d.get(10, value) && value == 95, "Valor actualizado");

  // TODO: agregar colisiones, ausencia, clave/valor 0 y reutilizacion.
  d.clear();
  check(d.size() == 0, "Vaciar el diccionario");
  std::cout << "Pruebas comunes superadas\n";
}
#endif

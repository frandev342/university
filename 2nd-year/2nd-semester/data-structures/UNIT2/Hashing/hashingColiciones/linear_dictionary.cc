#include "linear_dictionary.h"
#include <cstddef>

bool LinearDictionary::get(unsigned key, int &value) const {
  // TODO: recorrer como maximo M posiciones, usando probe.
  // Detenerse si encuentra la clave o una casilla vacia.
  // Si no encuentra la clave, conservar value y retornar false.
  if (count == 0)
    return false;
  const Slot s = slot[key % M];
  value = s.value;
  return true;
}

bool LinearDictionary::put(unsigned key, int value) {
  // TODO: actualizar una clave existente o insertar una nueva.
  // Incrementar count solo cuando se ocupa una casilla nueva.
  // Tabla llena + clave ausente: false, sin modificar el estado.
  // Tabla llena + clave presente: actualizar y retornar true.
  (void)key;
  (void)value;
  return false;
}

void LinearDictionary::clear() {
  for (Slot &s : slot) {
    s.occupied = false;
  }
  count = 0;
}

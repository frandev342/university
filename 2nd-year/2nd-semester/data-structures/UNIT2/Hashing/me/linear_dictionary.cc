#include "linear_dictionary.h"
#include <cstddef>

bool LinearDictionary::get(unsigned key, int &value) const {
  for (std::size_t step = 0; step < M; step++) {
    std::size_t i = probe(key, step);
    // Sí está desocupado, entonces no hay nada que conseguir del slot
    if (!slot[i].occupied)
      return false;
    // Sí encuentro termina y paso el valor a value
    if (slot[i].key == key) {
      value = slot[i].value;
      return true;
    }
  }
  // En caso de recorrer y no encontrar nada retornar false
  return false;
}

bool LinearDictionary::put(unsigned key, int value) {
  for (std::size_t step = 0; step < M; step++) {
    std::size_t i = probe(key, step);
    if (!slot[i].occupied) {
      slot[i].value = value;
      slot[i].key = key;
      slot[i].occupied = true;
      count++;
      return true;
    }
    if (slot[i].key == key) {
      slot[i].value = value;
      return true;
    }
  }
  return false;
}

bool LinearDictionary::contains(unsigned key) const {
  for (std::size_t step = 0; step < M; ++step) {
    std::size_t i = probe(key, step);
    if (!slot[i].occupied)
      return false;
    if (slot[i].key == key)
      return true;
  }
  return false;
}

void LinearDictionary::clear() {
  for (Slot &s : slot) {
    s.occupied = false;
    s.value = 0;
    s.key = 0;
  }
  count = 0;
}

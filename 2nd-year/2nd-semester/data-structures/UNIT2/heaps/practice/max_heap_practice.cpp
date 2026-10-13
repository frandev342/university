#include "max_heap_practice.hpp"

#include <stdexcept>
#include <utility>

PracticeMaxHeap::PracticeMaxHeap(std::vector<int> values) : data_(std::move(values)) {
    // Se conserva la entrada para poder practicar y observarla con values().
}

bool PracticeMaxHeap::empty() const noexcept {
    return data_.empty();
}

std::size_t PracticeMaxHeap::size() const noexcept {
    return data_.size();
}

const std::vector<int>& PracticeMaxHeap::values() const noexcept {
    return data_;
}

// TODO: devolver la raiz, que contiene el maximo. Precondicion: el heap no
// puede estar vacio. Comprobar ese caso y acceder a la posicion cero. O(1).
int PracticeMaxHeap::maximum() const {
    throw std::logic_error("TODO: implementar maximum");
}

// TODO: eliminar y devolver el maximo manteniendo el invariante. Precondicion:
// heap no vacio. Mover el ultimo valor a la raiz, borrarlo y hundir la raiz.
// Complejidad esperada: O(log n).
int PracticeMaxHeap::extractMax() {
    throw std::logic_error("TODO: implementar extractMax");
}

// TODO: insertar value sin romper el invariante. No requiere precondiciones.
// Anadirlo al final y hacerlo subir mientras supere a su padre. O(log n).
void PracticeMaxHeap::insert(int) {
    throw std::logic_error("TODO: implementar insert");
}

// TODO: sustituir la clave por newValue y restaurar el invariante hacia arriba.
// Precondiciones: index valido y newValue no menor que la clave actual. Validar
// ambas condiciones antes de asignar. Complejidad esperada: O(log n).
void PracticeMaxHeap::increaseKey(std::size_t, int) {
    throw std::logic_error("TODO: implementar increaseKey");
}

// TODO: comprobar el invariante de max-heap. No tiene precondiciones. Recorrer
// cada nodo no raiz y verificar que su padre no sea menor. O(n).
bool PracticeMaxHeap::isValid() const {
    throw std::logic_error("TODO: implementar isValid");
}

// TODO: calcular el indice del padre con indexacion desde cero. Precondicion:
// index debe ser mayor que cero para evitar desbordamiento sin signo. O(1).
std::size_t PracticeMaxHeap::parent(std::size_t) {
    throw std::logic_error("TODO: implementar parent");
}

// TODO: calcular el indice del hijo izquierdo con indexacion desde cero.
// Precondicion: el calculo debe caber en size_t; usar 2 * index + 1. O(1).
std::size_t PracticeMaxHeap::left(std::size_t) {
    throw std::logic_error("TODO: implementar left");
}

// TODO: calcular el indice del hijo derecho con indexacion desde cero.
// Precondicion: el calculo debe caber en size_t; usar 2 * index + 2. O(1).
std::size_t PracticeMaxHeap::right(std::size_t) {
    throw std::logic_error("TODO: implementar right");
}

// TODO: restaurar el invariante desde index hacia abajo. Precondicion: index
// valido y subarboles hijos ya validos. Elegir iterativamente el hijo mayor,
// intercambiar si hace falta y continuar. O(log n).
void PracticeMaxHeap::siftDown(std::size_t) {
    throw std::logic_error("TODO: implementar siftDown");
}

// TODO: restaurar el invariante desde index hacia arriba. Precondicion: index
// valido. Comparar iterativamente con el padre e intercambiar mientras la clave
// sea mayor, deteniendose en la raiz. O(log n).
void PracticeMaxHeap::siftUp(std::size_t) {
    throw std::logic_error("TODO: implementar siftUp");
}

// TODO: convertir data_ en max-heap. No requiere precondiciones. Recorrer los
// nodos internos desde el ultimo hasta la raiz y aplicar siftDown. O(n).
void PracticeMaxHeap::buildHeap() {
    throw std::logic_error("TODO: implementar buildHeap");
}

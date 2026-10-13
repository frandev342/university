#include "dictionary.h"
#include <new>

Dictionary::~Dictionary() { clear(); }

bool Dictionary::get(unsigned key, int &value) const {
  const Node *p = bucket[key % M];
  while (p != nullptr && p->key != key) {
    p = p->next;
  }
  if (p == nullptr)
    return false;
  value = p->value;
  return true;
}

bool Dictionary::put(unsigned key, int value) {
  std::size_t i = key % M;
  Node *p = bucket[i];
  while (p != nullptr && p->key != key) {
    p = p->next;
  }
  if (p != nullptr) {
    p->value = value;
    return true;
  }
  // new (argumentos_adicionales) Tipo(inicializadores)
  // nothrow evita lanzar errores en caso de fallar la reserva de memoria (no
  // hay memoria disponible)
  Node *q = new (std::nothrow) Node{key, value, bucket[i]};
  if (q == nullptr)
    return false;
  bucket[i] = q;
  ++count;
  return true;
}

void Dictionary::clear() {
  for (Node *&head : bucket) {
    while (head != nullptr) {
      Node *p = head;
      head = head->next;
      delete p;
    }
  }
  count = 0;
}

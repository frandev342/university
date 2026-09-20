#include "SinglyLinkedList.hpp"
#include <iostream>

// Método prinvado copy_nodes
SinglyLinkedList::Node *SinglyLinkedList::copy_nodes(const Node *source) {
  Node dummy{0, nullptr};
  Node *tail_ = &dummy;
  while (source != nullptr) {
    tail_->next = new Node{source->data, nullptr};
    tail_ = tail_->next;
    source = source->next;
  }
  return dummy.next;
}

// Constructor copia
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList &other)
    : head(copy_nodes(other.head)), tail(nullptr) {
  // Actualizar cola
  if (head != nullptr) {
    Node *current = head;
    while (current->next != nullptr) {
      current = current->next;
    }
    tail = current;
  }
}

// Asignación por copia
SinglyLinkedList &SinglyLinkedList::operator=(const SinglyLinkedList &other) {
  if (this != &other) {
    clear();
    head = copy_nodes(other.head);
    // Actualizar cola o tail
    if (head != nullptr) {
      Node *current = head;
      while (current->next != nullptr) {
        current = current->next;
      }
      tail = current;
    } else {
      tail = nullptr;
    }
  }
  return *this;
}

SinglyLinkedList::SinglyLinkedList(SinglyLinkedList &&other) noexcept
    : head(other.head), tail(other.tail) {
  other.head = nullptr;
  other.tail = nullptr;
}
SinglyLinkedList &
SinglyLinkedList::operator=(SinglyLinkedList &&other) noexcept {
  if (this != &other) {
    clear();
    head = other.head;
    tail = other.tail;
    other.head = nullptr;
    other.tail = nullptr;
  }
  return *this;
}

// Destructor (OwnerShip de los nodos) responsabilidad de liberar la memoria
SinglyLinkedList::~SinglyLinkedList() { clear(); }

// push_front: Pushear un value en la cabeeza de la lista O(1)
void SinglyLinkedList::push_front(int value) {
  Node *newNode = new Node{value, head};
  if (head == nullptr)
    tail = newNode;
  head = newNode;
}

// empty: verificar si está vacía O(1)
bool SinglyLinkedList::empty() const { return head == nullptr; }

// size: tamaño de la lista O(n)
int SinglyLinkedList::size() const {
  int count = 0;
  for (Node *current = head; current != nullptr; current = current->next)
    count++;
  return count;
}

// Contains: busca si contiene value dentro de la lista O(n)
bool SinglyLinkedList::contains(int value) const {
  for (Node *current = head; current != nullptr; current = current->next) {
    if (current->data == value)
      return true;
  }
  return false;
}

// clear: limpiar elementos del arreglo
void SinglyLinkedList::clear() {
  while (head != nullptr) {
    Node *nextNode = head->next;
    delete head;
    head = nextNode;
  }
  // head ya vale nullptr, solo es por seguridad y claridad
  head = nullptr;
  tail = nullptr;
}

// PushBack sin tail O(n)
/*
void SinglyLinkedList::push_back(int value) {
  Node **lastPtrRef = &head;
  while (*lastPtrRef != nullptr)
    lastPtrRef = &((*lastPtrRef)->next);
  *lastPtrRef = new Node{value, nullptr};
}
*/

// PushBack con tail O(1)
/*
void SinglyLinkedList::push_back(int value) {
  Node *newNode = new Node{value, nullptr};
  if (head == nullptr) {
    head = newNode;
    tail = newNode;
    return;
  }
  tail->next = newNode;
  tail = newNode;
}*/

// Eliminar el primer nodo O(1)
/*
bool SinglyLinkedList::pop_front() {
  if (head == nullptr)
    return false;
  Node *victim = head;
  head = head->next;
  delete victim;
  if (head == nullptr)
    tail = nullptr;
  return true;
}*/

// eraseAfter: Borrar el Nodo siguiente O(1)
bool SinglyLinkedList::eraseAfter(Node *prev) {
  if (prev == nullptr || prev->next == nullptr)
    return false;
  Node *victim = prev->next;
  prev->next = victim->next;
  if (victim == tail)
    tail = prev;
  delete victim;
  return true;
}

// eraseFirst: Eliminar la primera coincidencia O(n);
bool SinglyLinkedList::eraseFirst(int value) {
  if (head == nullptr)
    return false;
  if (head->data == value)
    return pop_front();
  Node *prev = head;
  while (prev->next && prev->next->data != value)
    prev = prev->next;

  if (prev == tail)
    return false;
  return eraseAfter(prev);
}

// Función para imprimir
void SinglyLinkedList::printL() {
  for (Node *current = head; current != nullptr; current = current->next) {
    std::cout << current->data << " ";
  }
  std::cout << std::endl;
}

// push_back: Agregar un nodo al final con sentinel
void SinglyLinkedList::push_back(int value) {
  Node *NewNode = new Node{value, nullptr};
  tail->next = NewNode;
  tail = NewNode;
}

// pop_front: Eliminar el primer elemento con sentinel
bool SinglyLinkedList::pop_front() { return eraseAfter(&sentinel); }

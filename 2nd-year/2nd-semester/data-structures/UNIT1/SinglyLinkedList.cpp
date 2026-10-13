#include "SinglyLinkedList.hpp"
#include <iostream>

// Método privado copy_nodes
SinglyLinkedList::Node *SinglyLinkedList::copy_nodes(const Node *source) {
  Node *newHead = nullptr;
  Node *newTail = nullptr;

  while (source != nullptr) {
    Node *node = new Node{source->data, newTail, nullptr};
    if (newTail != nullptr)
      newTail->next = node;
    else
      newHead = node;

    newTail = node;
    source = source->next;
  }

  tail = newTail;
  return newHead;
}

// Constructor copia
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList &other) {
  head = copy_nodes(other.head);
}

// Asignación por copia
SinglyLinkedList &SinglyLinkedList::operator=(const SinglyLinkedList &other) {
  if (this != &other) {
    clear();
    head = copy_nodes(other.head);
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
  Node *newNode = new Node{value, nullptr, head};
  if (head != nullptr)
    head->prev = newNode;
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

// eraseAfter: Borrar el Nodo siguiente O(1)
bool SinglyLinkedList::eraseAfter(Node *prev) {
  if (prev == nullptr || prev->next == nullptr)
    return false;
  eraseNode(prev->next);
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

// push_back: Agregar un nodo al final en O(1)
void SinglyLinkedList::push_back(int value) {
  Node *newNode = new Node{value, tail, nullptr};
  if (tail != nullptr)
    tail->next = newNode;
  else
    head = newNode;
  tail = newNode;
}

// pop_front: Eliminar el primer elemento en O(1)
bool SinglyLinkedList::pop_front() {
  if (head == nullptr)
    return false;
  eraseNode(head);
  return true;
}

// eraseNode: Borrar nodo O(1)
void SinglyLinkedList::eraseNode(Node *p) {
  if (p->prev != nullptr)
    p->prev->next = p->next;
  else
    head = p->next;

  if (p->next != nullptr)
    p->next->prev = p->prev;
  else
    tail = p->prev;

  delete p;
}

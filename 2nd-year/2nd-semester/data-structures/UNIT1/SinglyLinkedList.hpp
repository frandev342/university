#ifndef SINGLY_LINKED_LIST_HPP
#define SINGLY_LINKED_LIST_HPP

class SinglyLinkedList {

  struct Node {
    int data;
    Node *prev;
    Node *next;
  };
  Node *head = nullptr;
  Node *tail = nullptr;

  Node *copy_nodes(const Node *source);
  bool eraseAfter(Node *prev);
  void eraseNode(Node *p);

public:
  // Destructor
  ~SinglyLinkedList();
  // Constructor por defecto
  SinglyLinkedList() = default;
  // Constructor copia
  SinglyLinkedList(const SinglyLinkedList &other);
  // Contructor por movimiento
  SinglyLinkedList(SinglyLinkedList &&other) noexcept;

  // Asignación por copia
  SinglyLinkedList &operator=(const SinglyLinkedList &other);
  // Asignación por movimiento
  SinglyLinkedList &operator=(SinglyLinkedList &&other) noexcept;

  // Métodos (Interfaz) -> representación
  void clear();
  void push_front(int value);     // Node
  bool empty() const;             // head
  int size() const;               // next
  bool contains(int value) const; // Dynamic memory
  bool eraseFirst(int value);
  void printL();

  void push_back(int value);
  bool pop_front();
};

#endif

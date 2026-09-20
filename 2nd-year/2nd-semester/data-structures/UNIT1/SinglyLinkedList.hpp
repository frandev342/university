#ifndef _SINGLY_LINKED_LIST_
#define _SINGLY_LINKED_LIST_

class SinglyLinkedList {

  struct Node {
    int data;
    Node *next;
  };
  Node *head = nullptr;
  Node sentinel = Node{0, head};
  Node *tail = &sentinel;

  Node *copy_nodes(const Node *source);
  bool eraseAfter(Node *prev);

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
  //
  // void push_back(int value); // sin sentinel
  // bool pop_front(); // sin sentinel
  bool eraseFirst(int value);
  void printL();

  //
  void push_back(int value); // con sentinel
  bool pop_front();          // con sentinel
};

#endif // !

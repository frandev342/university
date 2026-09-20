#include "SinglyLinkedList.hpp"
#include <iostream>

using namespace std;

void range(SinglyLinkedList &list, int start, int stop, int step = 1) {
  for (int i = start; i < stop; i += step) {
    list.push_back(i);
  }
}

int main() {
  SinglyLinkedList list1;
  range(list1, 4, 20, 2);
  cout << list1.size() << endl;

  cout << list1.contains(20) << endl;
  list1.eraseFirst(10);
  list1.eraseFirst(16);
  cout << list1.size() << endl;
  list1.printL();
  return 0;
}

#include "heap.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
  const vector<heap11::Process> input = {
      {0, 4}, {1, 1},  {2, 3},  {3, 2}, {4, 16},
      {5, 9}, {6, 10}, {7, 14}, {8, 8}, {9, 7},
  };
  heap11::Heap heap(20);
  cout << "Size: " << heap.size() << endl;
  heap.build(input);
  cout << "Size: " << heap.size() << endl;
  heap11::Process process{10, 12};
  heap.push(process);
  cout << "Size: " << heap.size() << endl;

  heap11::Process process1;
  heap.pop(process1);
  cout << "Size: " << heap.size() << endl;

  return 0;
}

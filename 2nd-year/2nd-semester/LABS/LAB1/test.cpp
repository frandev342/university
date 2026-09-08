#include <iostream>
#include <vector>

void show(const std::vector<int> &v) {
  for (int i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::vector<int> v(4);
  for (int i = 0; i < 4; i++) {
    v[i] = i + 1;
  }
  show(v);
  std::cout << v.size() << " " << v.capacity() << std::endl;
  v.push_back(3);
  std::cout << v.size() << " " << v.capacity() << std::endl;

  return 0;
}

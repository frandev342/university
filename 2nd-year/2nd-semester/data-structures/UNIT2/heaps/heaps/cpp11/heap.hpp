#ifndef HEAP_CPP11_HPP
#define HEAP_CPP11_HPP
#include <cstddef>
#include <vector>
// Un namespace sirve para agrupar nombres relacionados y evitar conflictos.
// Un namespace puede repartirse entre varios archivos. indices.hpp y heap.hpp
// usan heap11 para indicar que sus funciones, tipos y clases pertenecen a la
// misma implementación del heap.
namespace heap11 {
struct Process {
  int pid;
  int priority;
};

class Heap {
public:
  explicit Heap(std::size_t capacity);
  bool build(const std::vector<Process> &input);
  bool push(Process x);
  bool pop(Process &out);
  bool valid() const;
  std::size_t size() const { return data_.size(); }
  bool empty() const { return data_.empty(); }

private:
  void sift_down(std::size_t i);
  std::vector<Process> data_;
  std::size_t capacity_;
};
} // namespace heap11
#endif

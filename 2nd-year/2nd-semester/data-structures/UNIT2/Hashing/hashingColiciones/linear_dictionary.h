#ifndef LINEAR_DICTIONARY_HPP
#define LINEAR_DICTIONARY_HPP
#include <array>
#include <cstddef>

class LinearDictionary {
  struct Slot {
    unsigned key = 0;
    int value = 0;
    bool occupied = false;
  };
  static constexpr std::size_t M = 7;
  std::array<Slot, M> slot{};
  std::size_t count = 0;
  static std::size_t probe(unsigned key, std::size_t step) {
    return (key % M + step) % M;
  }

public:
  LinearDictionary() = default;
  LinearDictionary(const LinearDictionary &) = delete;
  LinearDictionary &operator=(const LinearDictionary &) = delete;
  ~LinearDictionary() = default;

  bool get(unsigned key, int &value) const;
  bool put(unsigned key, int value);
  std::size_t size() const { return count; }
  void clear();
};
#endif

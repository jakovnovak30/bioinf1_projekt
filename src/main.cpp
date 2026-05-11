#include "CuckooFilter.hpp"
#include <functional>

class SimpleHashF : public HashFunction<int> {
  virtual int hash(int val) {
   return val % 11;
  }
  virtual size_t max_res() {
    return 11;
  }
};

/**
 * Entry point of program
 * TODO: add gtest unit tests for classes
 */
int main() {
  SimpleHashF hf;
  std::function<int(int)> ff = [](int x) { return x; };
  CuckooFilter<int> filter(hf, ff);

  filter.insert(5);
  filter.insert(6);
  filter.lookup(1);
  filter.del(6);

  return 0;
}

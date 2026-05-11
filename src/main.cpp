#include "CuckooFilter.hpp"

/**
 * Entry point of program
 * TODO: add gtest unit tests for classes
 */
int main() {
  auto hf = [&](int x) { return x; };
  CuckooFilter<int> filter(hf);

  filter.insert(5);
  filter.insert(6);
  filter.lookup(1);
  filter.del(1);

  return 0;
}

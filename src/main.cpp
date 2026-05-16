#include "CuckooFilter.hpp"
#include "SHA1HashFunction.hpp"
#include "LDCF.hpp"

#include <functional>
#include <iostream>

class SimpleHashF : public HashFunction<int> {
  virtual uint64_t hash(const int &val) const {
   return val % 11;
  }
  virtual size_t max_res() const {
    return 11;
  }
};

/**
 * Entry point of program
 * TODO: add gtest unit tests for class
 * TODO: add support for buckets with multiple entries
 */
int main() {
  SimpleHashF hf;
  std::function<int(int)> ff = [](int x) { return x; };
  CuckooFilter<int> filter(hf, ff);

  filter.insert(5);
  filter.insert(6);
  filter.lookup(1);
  filter.del(6);

  LDCF<int> ldcf(ff);

  ldcf.insert(5);
  std::cout << ldcf.lookup(5);

  // sha1 function
  SHA1HashFunction sha1(1);
  sha1.hash("ATTC");

  return 0;
}

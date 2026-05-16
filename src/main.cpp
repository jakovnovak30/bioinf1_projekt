#include "CuckooFilter.hpp"
#include "SHA1HashFunction.hpp"

#include <print>

class SimpleHashF : public HashFunction<uint32_t> {
  virtual uint64_t hash(const uint32_t &val) const {
   return val % 11;
  }
  virtual size_t max_res() const {
    return 11;
  }
  virtual uint32_t convert_back(uint64_t val) const { return val; }
};

/**
 * Entry point of program
 * TODO: add gtest unit tests for class
 * TODO: add support for buckets with multiple entries
 */
int main() {
  SimpleHashF hf;
  CuckooFilter<uint32_t> filter(hf, 10, 100);

  filter.insert(5);
  filter.insert(6);
  filter.lookup(1);
  filter.del(6);


  // // sha1 function
  SHA1HashFunction sha1 = SHA1HashFunction();
  sha1.hash("ATTC");
  CuckooFilter<std::string> str_filter(sha1, 10, 20);

  str_filter.insert("ATTC");
  str_filter.insert("GCCT");

  std::println("ATTT lookup: {}, ATTC lookup: {}",
      str_filter.lookup("ATTT"), str_filter.lookup("ATTC"));

  return 0;
}

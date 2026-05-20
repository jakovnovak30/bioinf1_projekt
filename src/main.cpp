#include "CuckooFilter.hpp"
#include "SHA1HashFunction.hpp"
#include "LDCF.hpp"
#include "FASTAReader.hpp"

#include <print>
#include <functional>

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
 * TODO: add cli arguments and proper main.cpp
 * TODO: more doxygen comments
 * TODO: more tests?
 */
int main() {
  SimpleHashF hf;
  CuckooFilter<uint32_t> filter(hf, 10, 100);

  filter.insert(5);
  filter.insert(6);
  filter.lookup(1);
  filter.del(6);

  std::function<uint32_t(uint32_t)> ff = [](uint32_t x) { return x; };
  LDCF<uint32_t> ldcf(hf, ff);

  ldcf.insert(6);

  // sha1 function
  SHA1HashFunction sha1 = SHA1HashFunction();
  sha1.hash("ATTC");
  CuckooFilter<std::string> str_filter(sha1, 10, 20);

  str_filter.insert("ATTC");
  str_filter.insert("GCCT");

  std::println("ATTT lookup: {}, ATTC lookup: {}",
      str_filter.lookup("ATTT"), str_filter.lookup("ATTC"));

  // FASTA reader
  FASTAReader reader ("datasets/artificial/test_read.fasta");

  std::println("Entire file is: {}", reader.read_all());
  srand(time(NULL));
  std::println(
      "Trying to get random subsequence of k=5: {}",
      reader.read_random(5));

  return 0;
}

#include "CuckooHashTable.hpp"

/**
 * Entry point of program
 * TODO: add gtest unit tests for classes
 */
int main() {
  CuckooHashTable<int> ht;

  ht.insert(5);
  ht.insert(6);
  ht.lookup(1);
  ht.del(1);

  return 0;
}

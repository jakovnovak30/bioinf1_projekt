#include "CuckooHashTable.hpp"

int main() {
  CuckooHashTable<int> ht;

  ht.insert(5);
  ht.insert(6);
  ht.lookup(1);
  ht.del(1);

  return 0;
}

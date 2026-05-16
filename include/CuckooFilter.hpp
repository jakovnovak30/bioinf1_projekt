#pragma once

#include "logging.hpp"
#include "HashFunction.hpp"

#include <cassert>
#include <cstdint>
#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>

/**
 * Implementation of CuckooFilter
 *
 * @author Jakov Novak
 */
template <typename T> class CuckooFilter {
public:
	CuckooFilter(HashFunction<T> &hash_function,
               uint8_t fingerprint_bits,
               uint32_t num_buckets = 1024,
               uint8_t max_num_kicks = 3)
    : fingerprint_bits(fingerprint_bits),
      num_buckets(num_buckets),
      max_num_kicks(max_num_kicks),
      hash_function(hash_function)
  {
    assert(fingerprint_bits < 32);

    LOG("Setting max bucket size of {}", num_buckets);
    this->buckets.resize(num_buckets);

    for(size_t i=0;i < this->buckets.size();i++) {
      this->buckets[i] = std::nullopt;
    }
  }
	~CuckooFilter() = default;

  /**
   * This function inserts an item into the hash table.
   *
   * @param x - any hashable type that is to be stored in the filter
   * @return nothing
   * @throws std::runtime_error - if filter hashtable is full
   *
   * @author Jakov Novak
   */
	void insert(T x) {
    size_t fingerprint =
      this->get_fingerprint(x);
    auto [i1, i2] = get_indices(x, fingerprint);

    /*
     * If either i1 or i2 is empty, just fill them
     */
    if (!this->buckets[i1].has_value()) {
      this->buckets[i1] = std::make_optional(fingerprint);
      LOG("Added fingerprint to {}", i1);
      return;
    }
    else if (!this->buckets[i2].has_value()) {
      this->buckets[i2] = std::make_optional(fingerprint);
      LOG("Added fingerprint to {}", i2);
      return;
    }

    /*
     * Otherwise, we do main loop for cuckoo hashing
     * TODO: buckets *can* have multiple entries
     */
    size_t i;
    if (rand() % 2)
      i = i1;
    else
      i = i2;
    for (size_t n=0;n < this->max_num_kicks;n++) {
      // get random entry from bucket[i]
      T e = this->buckets[i].value();
      // swap fingerprint with e
      this->buckets[i] = std::make_optional(fingerprint);
      fingerprint = e;
      // calculate next i
      i = i ^ this->hash_function.hash(fingerprint);
      // check if bucket[i] has empty entry
      if (!this->buckets[i].has_value()) {
        this->buckets[i] = std::make_optional(fingerprint);
        LOG("Added fingerprint to {}", i);
        return;
      }
    }

    WARN("Hash table is full!");
    throw std::runtime_error("Hash table is full!");
	}

  /**
   * This function checks if an item is already in the hash table.
   *
   * @param entry - value we want to look up
   * @return boolean, true if entry is *possibly* contained in table
   *
   * @author Jakov Novak
   */
	bool lookup(T x) const {
    uint32_t fingerprint = this->get_fingerprint(x);
    auto [i1, i2] = get_indices(x, fingerprint);

    if (x == 1) {
      std::cout << "fingerprint: " << fingerprint << std::endl;

      std::cout << "i1: " << i1 << " i2: " << i2 << std::endl;
      if (this->buckets[i1].has_value())
        std::cout << "i1 value: " << this->buckets[i1].value() << std::endl;
      if (this->buckets[i2].has_value())
        std::cout << "i2 value: " << this->buckets[i2].value() << std::endl;
    }
    else {
      std::cout << "x: " << x << " fingerprint: " << fingerprint << std::endl;
    }

    return 
      (this->buckets[i1].has_value()
       && this->buckets[i1].value() == fingerprint)
        ||
      (this->buckets[i2].has_value()
       && this->buckets[i2].value() == fingerprint);
  }

  /**
   * Function that deletes an entry from filter
   *
   * @param entry - value that needs to be deleted from filter
   * @return nothing
   * @throws std::runtime_error - if entry is not in filter
   *
   * @author Jakov Novak
   */
	void del(T x) {
    // upper 32 bits used as fingerprint
    uint32_t fingerprint = this->get_fingerprint(x);
    auto [i1, i2] = get_indices(x, fingerprint);

    /**
     * TODO: support for multiple entries in buckets
     */
    if (this->buckets[i1].has_value() &&
        this->buckets[i1].value() == fingerprint)
    {
      this->buckets[i1] = std::nullopt;
      return;
    }
    else if (this->buckets[i2].has_value() &&
        this->buckets[i2].value() == fingerprint)
    {
      this->buckets[i2] = std::nullopt;
      return;
    }

    WARN("Not found in {} nor {}", i1, i2);
    throw std::runtime_error("Entry is not in filter");
  }

private:
  uint32_t get_fingerprint(T x) const {
    return
      this->hash_function(x) & ((1<<this->fingerprint_bits)-1);
  }

  std::pair<uint32_t, uint32_t> get_indices(T x, uint32_t fingerprint) const {
    uint32_t i1, i2;
    i1 = this->hash_function(x) >> 32;
    i2 = i1 ^ this->hash_function(fingerprint);
    return { i1, i2 };
  }

  uint8_t fingerprint_bits;
  size_t num_buckets;
  size_t max_num_kicks;
  HashFunction<T> &hash_function;
  std::vector<std::optional<T>> buckets;
};

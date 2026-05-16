#pragma once

#include "logging.hpp"
#include "HashFunction.hpp"

#include <cassert>
#include <functional>
#include <optional>
#include <stdexcept>

/**
 * Implementation of CuckooFilter
 *
 * @author Jakov Novak
 */
template <typename T> class CuckooFilter {
public:
  typedef std::function<T(T)> FingerprintFunction;

	CuckooFilter(HashFunction<T> &hash_function,
               FingerprintFunction &fingerprint_function,
               size_t max_num_kicks = 2)
    : hash_function(hash_function), fingerprint_function(fingerprint_function),
      max_num_kicks(max_num_kicks)
  {
    LOG("Setting max bucket size of {}", hash_function.max_res());
    this->buckets.resize(hash_function.max_res());

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
	void insert(T x, size_t fingerprint = SIZE_MAX) {
    if (fingerprint == SIZE_MAX) {
        fingerprint = this->fingerprint_function(x);
    }
    size_t i1, i2;
    i1 = this->hash_function.hash(x);
    i2 = i1 ^ this->hash_function.hash(fingerprint);

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
	bool lookup(T x, size_t fingerprint = SIZE_MAX) const {
    if (fingerprint == SIZE_MAX) {
        fingerprint = this->fingerprint_function(x);
    }
    size_t i1, i2;
    i1 = this->hash_function.hash(x);
    i2 = i1 ^ this->hash_function.hash(fingerprint);

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
	void del(T x, size_t fingerprint = SIZE_MAX) {
    if (fingerprint == SIZE_MAX) {
        fingerprint = this->fingerprint_function(x);
    }
    size_t i1, i2;
    i1 = this->hash_function.hash(x);
    i2 = i1 ^ this->hash_function.hash(fingerprint);

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
  HashFunction<T> &hash_function;
  FingerprintFunction &fingerprint_function;
  std::vector<std::optional<T>> buckets;
  size_t max_num_kicks;
};

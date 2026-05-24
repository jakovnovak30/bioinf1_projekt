#pragma once

#include "logging.hpp"
#include "HashFunction.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <print>
#include <string>
#include <sys/types.h>
#include <vector>
#include <stdexcept>

/**
 * Implementation of CuckooFilter
 *
 * @author Jakov Novak
 */
template <typename T> class CuckooFilter {
public:
	CuckooFilter(HashFunction<T> &hash_function,
               uint8_t fingerprint_bits = 16,
               uint32_t num_buckets = 32,
               uint8_t bucket_size = 4,
               uint8_t max_num_kicks = 3)
    : m_fingerprint_bits(fingerprint_bits),
      m_num_buckets(num_buckets),
      m_bucket_size(bucket_size),
      m_max_num_kicks(max_num_kicks),
      m_hash_function(hash_function)
  {
    assert(fingerprint_bits < 32);

    LOG("Setting max bucket size of {}", num_buckets);
    m_buckets.resize(num_buckets);
    for (auto it = m_buckets.begin(); it != m_buckets.end();++it)
    {
      *it = Bucket(m_bucket_size);
    }
  }
	virtual ~CuckooFilter() = default;

  /*
   * Function which inserts an item into hash table,
   * using default fingerprint calculations.
   *
   * @see insert
   * @author Jakov Novak
   */
	void insert(T entry) {
    uint32_t fingerprint = get_fingerprint(m_hash_function, entry) & ((1 << m_fingerprint_bits) - 1);
    this->insert(entry, fingerprint);
  }

  /**
   * This function inserts an item into the hash table.
   *
   * @param entry - any hashable type that is to be stored in the filter
   * @param fingerprint - fingerprint of the item
   * @return nothing
   * @throws std::runtime_error - if filter hashtable is full
   *
   * @author Jakov Novak
   */
	void insert(T entry, uint32_t fingerprint) {
    auto [i1, i2] = get_indices(entry, fingerprint);

    /**
     * If we already have the fingerprint,
     * return
     */
    if (m_buckets[i1].contains(fingerprint) ||
        m_buckets[i2].contains(fingerprint))
      return;

    /*
     * If either i1 or i2 is empty, just fill them
     */
    if (!m_buckets[i1].is_full()) {
      m_buckets[i1].insert(fingerprint);
      LOG("Added fingerprint to {}", i1);
      return;
    }
    else if (!m_buckets[i2].is_full()) {
      m_buckets[i2].insert(fingerprint);
      LOG("Added fingerprint to {}", i2);
      return;
    }

    /*
     * Otherwise, we do main loop for cuckoo hashing
     */
    std::vector<SwapOp> log;
    log.reserve(m_max_num_kicks);

    size_t i;
    if (rand() % 2)
      i = i1;
    else
      i = i2;
    for (size_t n=0;n < m_max_num_kicks;n++) {
      // get random entry from bucket[i]
      // & swap fingerprint with it
    size_t index = rand() % m_bucket_size;
    uint32_t evicted = m_buckets[i].m_fingerprints[index];

    log.push_back({i, index, evicted});

    m_buckets[i].m_fingerprints[index] = fingerprint;
    fingerprint = evicted;

    i ^= m_hash_function(m_hash_function.convert_back(fingerprint)) % m_num_buckets;
      // check if bucket[i] has empty entry
      if (!m_buckets[i].is_full()) {
        m_buckets[i].insert(fingerprint);
        LOG("Added fingerprint to {}", i);
        return;
      }
  }

    for (auto it = log.rbegin(); it != log.rend(); ++it) {
        auto &op = *it;
        m_buckets[op.bucket_index].m_fingerprints[op.slot_index] = op.old_value;
    }

    WARN("Hash table is full!");
    throw std::runtime_error("Hash table is full!");
	}

  /*
   * Function to lookup an entry, using default fingerprint.
   *
   * @see lookup
   * @author Jakov Novak
   */
	bool lookup(T entry) const {
    uint32_t fingerprint = get_fingerprint(m_hash_function, entry) & ((1 << m_fingerprint_bits) - 1);
    return lookup(entry, fingerprint);
  }

  /**
   * This function checks if an item is already in the hash table.
   *
   * @param entry - value we want to look up
   * @param fingerprint - fingerprint of the entry
   * @return boolean, true if entry is *possibly* contained in table
   *
   * @author Jakov Novak
   */
	bool lookup(T entry, uint32_t fingerprint) const {
    auto [i1, i2] = get_indices(entry, fingerprint);

    return 
      m_buckets[i1].contains(fingerprint) ||
      m_buckets[i2].contains(fingerprint);
  }

  /*
   * Function to delete an entry, using default fingerprint.
   *
   * @see del
   * @author Jakov Novak
   */
	void del(T entry) {
    uint32_t fingerprint = get_fingerprint(m_hash_function, entry) & ((1 << m_fingerprint_bits) - 1);
    this->del(entry, fingerprint);
  }

  /**
   * Function that deletes an entry from filter
   *
   * @param entry - value that needs to be deleted from filter
   * @param fingerprint - fingerprint of the entry
   * @return nothing
   * @throws std::runtime_error - if entry is not in filter
   *
   * @author Jakov Novak
   */
	void del(T entry, uint32_t fingerprint) {
    auto [i1, i2] = get_indices(entry, fingerprint);

    if (m_buckets[i1].contains(fingerprint))
    {
      m_buckets[i1].remove(fingerprint);
      return;
    }
    else if (m_buckets[i2].contains(fingerprint))
    {
      m_buckets[i2].remove(fingerprint);
      return;
    }

    WARN("Not found in {} nor {}", i1, i2);
    throw std::runtime_error("Entry is not in filter");
  }

  static uint32_t get_fingerprint(const HashFunction<T> &hash_function, const T entry) {
    // use upper 32 bits for fingerprint, lower 32 for hash table
    return hash_function(entry) >> 32;
  }

  void clear() {
    for (auto &bucket : m_buckets) {
      bucket.clear();
    }
  }

  std::string to_string() {
    std::string out = "";

    for(size_t i=0;i < m_buckets.size();++i)
    {
      if (m_buckets[i].is_empty())
        continue;

      out += "Bucket " + std::to_string(i);
      out += m_buckets[i].to_string();
      out += "\n";
    }

    return std::move(out);
  }

private:
  /**
   * Structure used for tracking operations on buckets.
   * Used for rollback on failure for eviction operation.
   * 
   * @author Stjepan Bonić
   */
  struct SwapOp {
      size_t bucket_index;
      size_t slot_index;
      uint32_t old_value;
  };
  /*
   * Structure that models a bucket, which stores
   * multiple fingerprints at a single CF index.
   *
   * @author Jakov Novak
   */
  struct Bucket {
  public:
    std::vector<uint32_t> m_fingerprints;

    Bucket() = default;
    Bucket(uint8_t max_n) : m_max_n(max_n) {}

    /*
     * Try to insert a fingerprint into the bucket.
     *
     * @param f - fingerprint to be inserted
     * @return true if item can be inserted, false otherwise
     * @see insert
     * @autor Jakov Novak
     */
    bool try_insert(uint32_t f) {
      assert (m_fingerprints.size() <= m_max_n);

      if (m_fingerprints.size() == m_max_n)
        return false;

      m_fingerprints.emplace_back(std::move(f));
      return true;
    }

    /*
     * Insert a fingerprint into the bucket.
     *
     * @param f - fingerprint to be inserted
     * @throws std::runtime_error if f can't be inserted
     * @see try_insert
     * @autor Jakov Novak
     */
    void insert(uint32_t f) {
      if (!try_insert(f))
        throw std::runtime_error("bucket could not insert!");
    }

    /*
     * Remove a fingerprint from the bucket.
     *
     * @param f - fingerprint that needs to be removed
     * @throws std::runtime_error if f is not in bucket
     * @author Jakov Novak
     */
    void remove(uint32_t f) {
      auto it = std::find(m_fingerprints.begin(), m_fingerprints.end(), f);

      if (it == m_fingerprints.end())
        throw std::runtime_error("Cannot remove f as it is not in bucket!");

      m_fingerprints.erase(it);
    }

    /*
     * Swap a fingerprint with a random entry.
     *
     * @param index - index of entry to be swapped with random one
     * @paramm f - fingerprint to be swapped
     * @throws assertion error if bucket is empty
     * @return fingerprint which was swapped with f
     * @author Jakov Novak
     */
    uint32_t swap_at(size_t index, uint32_t f)
    {
        uint32_t old = m_fingerprints[index];
        m_fingerprints[index] = f;
        return old;
    }

    /*
     * Check if a given fingerprint is in the bucket.
     *
     * @param f - fingerprint which is checked
     * @return true if f is in bucket, false otherwise
     * @author Jakov Novak
     */
    bool contains(uint32_t f) const {
      auto it = std::find(m_fingerprints.begin(), m_fingerprints.end(), f);
      return it != m_fingerprints.end();
    }

    /*
     * Check if bucket is full.
     *
     * @return true is full, false otherwise
     * @author Jakov Novak
     */
    bool is_full() const {
      return m_fingerprints.size() == m_max_n;
    }

    /*
     * Check is bucket is empty.
     *
     * @return true if empty, false otherwise
     * @author Jakov Novak
     */
    bool is_empty() const {
      return m_fingerprints.empty();
    }

    /*
     * Erase all fingerprints from bucket.
     *
     * @author Jakov Novak
     */
    void clear() {
      m_fingerprints.clear();
    }

    /*
     * Get string representation of bucket.
     * 
     * @return the contents of this bucket as an std::string
     * @author Jakov Novak
     */
    std::string to_string() const {
      std::string out;

      for (auto it = m_fingerprints.begin();
           it != m_fingerprints.end();++it)
      {
        if (it == m_fingerprints.begin())
          out += "{ ";
        else
          out += ", ";

        out += std::to_string(*it);
      }
      out += "}";

      return std::move(out);
    }

  private:
    uint8_t m_max_n;
  };

  virtual std::pair<uint32_t, uint32_t> get_indices(T entry, uint32_t fingerprint) const {
    T f = m_hash_function.convert_back(fingerprint);
    uint32_t i1, i2;
    i1 = (uint32_t) m_hash_function(entry) % m_num_buckets;
    i2 = (uint32_t) (i1 ^ m_hash_function(f)) % m_num_buckets;
    return { i1, i2 };
  }

  uint8_t m_fingerprint_bits;
  uint32_t m_num_buckets;
  uint8_t m_bucket_size;
  uint8_t m_max_num_kicks;
  HashFunction<T> &m_hash_function;
  std::vector<Bucket> m_buckets;
};

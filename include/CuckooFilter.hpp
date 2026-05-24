#pragma once

#include "logging.hpp"
#include "HashFunction.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
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
               uint8_t fingerprint_bits = 32,
               uint32_t num_buckets = 32,
               uint8_t bucket_size = 4,
               uint8_t max_num_kicks = 10)
    : m_fingerprint_bits(fingerprint_bits),
      m_num_buckets(num_buckets),
      m_bucket_size(bucket_size),
      m_max_num_kicks(max_num_kicks),
      m_hash_function(hash_function)
  {
    assert(fingerprint_bits <= 32);

    LOG("Setting max bucket size of {}", num_buckets);
    m_buckets.resize(num_buckets);
    for (auto it = m_buckets.begin(); it != m_buckets.end();++it)
    {
      *it = Bucket(m_bucket_size);
    }
  }
	virtual ~CuckooFilter() = default;

  /*
   * TODO: opis
   *
   * @see insert
   * @author Jakov Novak
   */
	void insert(T entry) {
    uint32_t fingerprint = normalized_fingerprint(entry);

    insert(entry, fingerprint);
}

  /**
   * This function inserts an item into the hash table.
   *
   * @param entry - any hashable type that is to be stored in the filter
   * @return nothing
   * @throws std::runtime_error - if filter hashtable is full
   *
   * @author Jakov Novak
   */
	void insert(T entry, uint32_t fingerprint) {
    auto [i1, i2] = get_indices(fingerprint);

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
    size_t i;
    if (rand() % 2)
      i = i1;
    else
      i = i2;
    for (size_t n=0;n < m_max_num_kicks;n++) {
      // get random entry from bucket[i]
      // & swap fingerprint with it
      fingerprint = m_buckets[i].swap_random(fingerprint);

      // calculate next i
      const uint32_t mask = m_num_buckets - 1;

      uint32_t h1 = mix32(fingerprint);
      uint32_t h2 = mix32(h1);

      uint32_t delta = h2 & mask;

      if (delta == 0)
        delta = 1;  

      i ^= delta;
      // check if bucket[i] has empty entry
      if (!m_buckets[i].is_full()) {
        m_buckets[i].insert(fingerprint);
        LOG("Added fingerprint to {}", i);
        return;
      }
    }

    WARN("Hash table is full!");
    throw std::runtime_error("Hash table is full!");
	}

  /*
   * TODO: opis
   *
   * @see lookup
   * @author Jakov Novak
   */
	bool lookup(T entry, uint32_t fingerprint) const {
    auto [i1, i2] = get_indices(fingerprint);

    return
        m_buckets[i1].contains(fingerprint) ||
        m_buckets[i2].contains(fingerprint);
}

  /**
   * This function checks if an item is already in the hash table.
   *
   * @param entry - value we want to look up
   * @return boolean, true if entry is *possibly* contained in table
   *
   * @author Jakov Novak
   */
	bool lookup(T entry) const {
    return lookup(entry, normalized_fingerprint(entry));
 }

  /*
   * TODO: opis
   *
   * @see del
   * @author Jakov Novak
   */
void del(T entry, uint32_t fingerprint) {
    auto [i1, i2] = get_indices(fingerprint);

    if (m_buckets[i1].contains(fingerprint))
    {
        m_buckets[i1].remove(fingerprint);
        return;
    }

    if (m_buckets[i2].contains(fingerprint))
    {
        m_buckets[i2].remove(fingerprint);
        return;
    }

    throw std::runtime_error("Entry is not in filter");
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
void del(T entry) {
    del(entry, normalized_fingerprint(entry));
 }

static uint32_t get_fingerprint(const HashFunction<T> &hash_function, const T entry) {
    uint64_t h = static_cast<uint64_t>(hash_function(entry));
    uint32_t fp = static_cast<uint32_t>((h >> 32) ^ (h & 0xFFFFFFFF));

    return fp;
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
  /*
   * TODO: opis strukture i metoda
   *
   * @author Jakov Novak
   */
  struct Bucket {
  public:
    Bucket() = default;
    Bucket(uint8_t max_n) : m_max_n(max_n) {}

    /*
     * 
     */
    bool try_insert(uint32_t f) {
      assert (m_fingerprints.size() <= m_max_n);

      if (m_fingerprints.size() == m_max_n)
        return false;

      m_fingerprints.emplace_back(std::move(f));
      return true;
    }

    void insert(uint32_t f) {
      if (!try_insert(f))
        throw std::runtime_error("bucket could not insert!");
    }

    void remove(uint32_t f) {
      auto it = std::find(m_fingerprints.begin(), m_fingerprints.end(), f);

      if (it == m_fingerprints.end())
        throw std::runtime_error("Cannot remove f as it is not in bucket!");

      m_fingerprints.erase(it);
    }

    uint32_t swap_random(uint32_t f) {
      assert(m_fingerprints.size() > 0);
      size_t index = rand() % m_fingerprints.size();

      uint32_t oldv = m_fingerprints[index];
      m_fingerprints[index] = f;
      return oldv;
    }

    bool contains(uint32_t f) const {
      auto it = std::find(m_fingerprints.begin(), m_fingerprints.end(), f);
      return it != m_fingerprints.end();
    }

    bool is_full() const {
      return m_fingerprints.size() == m_max_n;
    }

    bool is_empty() const {
      return m_fingerprints.empty();
    }

    void clear() {
      m_fingerprints.clear();
    }

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
    std::vector<uint32_t> m_fingerprints;
    uint8_t m_max_n;
  };

virtual std::pair<uint32_t, uint32_t>
get_indices(uint32_t fingerprint) const
{
    const uint32_t mask = m_num_buckets - 1;

    uint32_t h1 = mix32(fingerprint);

    uint32_t i1 = h1 & mask;

    uint32_t h2 = mix32(h1);

    uint32_t delta = h2 & mask;

    uint32_t i2 = i1 ^ delta;

    return {i1, i2};
}

uint32_t normalized_fingerprint(T entry) const
{
    uint32_t fp =
        get_fingerprint(m_hash_function, entry);

    uint32_t mask =
        (m_fingerprint_bits == 32)
        ? 0xFFFFFFFFu
        : ((1u << m_fingerprint_bits) - 1);

    fp &= mask;

    if (fp == 0)
        fp = 1;

    return fp;
}

static uint32_t mix32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;

    x ^= x >> 15;
    x *= 0x846ca68b;

    x ^= x >> 16;

    return x;
}

  uint8_t m_fingerprint_bits;
  uint32_t m_num_buckets;
  uint8_t m_bucket_size;
  uint8_t m_max_num_kicks;
  HashFunction<T> &m_hash_function;
  std::vector<Bucket> m_buckets;
};

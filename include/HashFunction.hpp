#pragma once

#include <cstdint>

/**
 * Interface for hash functions used by CuckooFilter
 *
 * @author Jakov Novak
 */
template <typename T>
class HashFunction {
public:
  HashFunction() = default;

  /*
   * hash function implementation
   * @param val - value to be hashed
   * @return hash of val
   * @author Jakov Novak
   */
  virtual uint64_t hash(const T &val) const = 0;

  /*
   * Same as calling *hash*
   *
   * @see hash
   * @author Jakov Novak
   */
  uint64_t operator()(const T &val) const { return this->hash(val); }

  /*
   * Get max size of hashing result
   *
   * @return maximum size of hash
   * @author Jakov Novak
   */
  virtual std::size_t max_res() const = 0;
};

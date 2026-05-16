#pragma once

#include "CuckooFilter.hpp"

template <typename T>
class CuckooFilterLDCF : public CuckooFilter<T> {
public:
  CuckooFilterLDCF(HashFunction<T> &hash_function,
                   uint8_t fingerprint_bits,
                   uint32_t num_buckets = 1024,
                   uint8_t max_num_kicks = 3,
                   uint8_t level = 0, uint8_t level_offset = 0)
    : 
    CuckooFilter<T>(hash_function, fingerprint_bits, num_buckets, max_num_kicks)
  {
    this->m_level = level;
    this->m_loff = level_offset;
  }

  virtual T get_fingerprint(T x) const override {
    uint32_t fingerprint_raw =
      this->hash_function(x) & ((1<<this->fingerprint_bits)-1);

    uint32_t fingerprint_offset = fingerprint_raw & ((1<<m_level)-1);

    // should call can_insert before calling insert
    if (fingerprint_offset != m_loff)
      throw std::runtime_error("Fingerprint offset should be equal to m_loff!");

    return this->hash_function.convert_back(
          (fingerprint_raw-fingerprint_offset) >> m_level
        );
  }

  bool can_insert(T x) {
    try {
      get_fingerprint(x);
    }
    catch (std::runtime_error) {
      return false;
    }

    return true;
  }

private:
  uint8_t m_level;
  uint8_t m_loff;
};

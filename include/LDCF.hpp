#pragma once

#include <cstdint>
#include <memory>
#include <print>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

#include "CuckooFilter.hpp"
#include "HashFunction.hpp"
#include "LDCFLevel.hpp"

/**
 * Interface for Logarithmic Dynamic Cuckoo Filter structure
 *
 * @author Stjepan Bonić
 */
template <typename T>
class LDCF
{
public:
    typedef std::function<uint32_t(const HashFunction<T> &, const T)> FingerprintFunction;
    LDCF(HashFunction<T> &hash_function,
         FingerprintFunction fingerprint_function,
         uint32_t num_buckets = 32,
         uint8_t fingerprint_bits = 16,
         uint8_t bucket_size = 4,
         uint8_t max_num_kicks = 3)
        : m_fingerprint_bits(fingerprint_bits),
          m_num_buckets(num_buckets),
          m_bucket_size(bucket_size),
          m_max_num_kicks(max_num_kicks),
          m_fingerprint_function(fingerprint_function),
          m_hash_function(hash_function)
    {
        appendLevel();
    }
    ~LDCF() = default;

    /**
     * Inserts item in LDCF structure
     *
     * @param item - any hashable type that is to be stored in the structure of filters
     * @returns nothing
     * @author Stjepan Bonić
     */
    bool insert(T item)
    {
        const size_t fp = makeFingerprint(item);

        // Item already in structure, skip inserting
        if (lookup(item))
        {
            return false;
        }

        while (true)
        {
            LDCFLevel<T> &last_level = m_levels.back();
            const size_t level_index = m_levels.size() - 1;

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            try
            {
                last_level.filters[filter_index]->insert(item, stripped_fp);
                return true;
            }
            catch (const std::runtime_error &e)
            {
                appendLevel();
            }
        }

        return false;
    }

    /**
     * Checks if item is inside of the structure
     *
     * @param item - any hashable type
     * @returns true if item is in LDCF structure
     * @author Stjepan Bonić
     */
    bool lookup(T item) const
    {
        const size_t fp = makeFingerprint(item);

        for (size_t level_index = 0; level_index < m_levels.size(); level_index++)
        {
            const LDCFLevel<T> &level = m_levels[level_index];

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            if (level.filters[filter_index]->lookup(item, stripped_fp))
            {
                return true;
            }
        }

        return false;
    }

    /**
     * Removes item from the structure
     *
     * @param item - any hashable type
     * @returns true if item was removed from the structure
     * @author Stjepan Bonić
     */
    bool del(T item)
    {
        const size_t fp = makeFingerprint(item);

        for (size_t level_index = 0; level_index < m_levels.size(); level_index++)
        {
            LDCFLevel<T> &level = m_levels[level_index];

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            try
            {
                level.filters[filter_index]->del(item, stripped_fp);
                return true;
            }
            catch (const std::runtime_error &e)
            {
                return false;
            }
        }

        throw std::runtime_error("could not find entry!");
    }

    /**
     * Returns number of levels in the structure
     *
     * @author Stjepan Bonić
     */
    size_t levelCount() const noexcept
    {
        return m_levels.size();
    }

    /**
     * Clears all items from LDCF structure
     *
     * @returns nothing
     * @author Stjepan Bonić
     */
    void clear()
    {
        for (LDCFLevel<T> &level : m_levels)
        {
            for (auto &filter : level.filters)
            {
                filter->clear();
            }
        }

        m_levels.clear();
        appendLevel();
    }

private:
    /**
     * Adds another level to the LDCF structure
     *
     * @returns nothing
     * @author Stjepan Bonić
     */
    void appendLevel()
    {
        const size_t level_index = m_levels.size();
        const size_t filter_count = static_cast<size_t>(1ULL << level_index);

        LDCFLevel<T> level;
        level.filters.resize(filter_count);

        for (size_t i = 0; i < filter_count; i++)
        {
            size_t fingerprint_bits_new_level = m_fingerprint_bits - level_index;

            if (fingerprint_bits_new_level <= 0)
            {
                throw std::runtime_error(
                    "Fingerprint length too small for requested LDCF level");
            }

            level.filters[i] = std::make_unique<CuckooFilter<T>>(
                m_hash_function,
                fingerprint_bits_new_level,
                m_num_buckets,
                m_bucket_size,
                m_max_num_kicks);
        }

        m_levels.emplace_back(std::move(level));
    }

    /**
     * Derives a fingerprint of an item to be stored in LDCF.
     *
     * @param item - any hashable type
     * @returns fingerprint of an item
     * @author Stjepan Bonić
     */
    size_t makeFingerprint(T item) const noexcept
    {
        size_t fp = m_fingerprint_function(m_hash_function, item);

        const size_t mask = (1ULL << m_fingerprint_bits) - 1ULL;

        fp &= mask;

        if (fp == 0)
        {
            fp = 1;
        }

        return fp;
    }

    /**
     * Strips prefix from a fingerprint based on the level of the structure that the item will be stored.
     *
     * @param fp - fingerprint of an item to be stored
     * @param level - level of the structure where the item will be stored
     * @returns fingerprint of an item without the prefix
     * @author Stjepan Bonić
     */
    size_t stripPrefix(size_t fp, size_t level) const noexcept
    {
        if (level == 0)
        {
            return fp;
        }

        const size_t remaining_bits = m_fingerprint_bits - level;

        const size_t mask = (static_cast<size_t>(1ULL << remaining_bits) - 1ULL);

        return fp & mask;
    }

    /**
     * Returns index of filter where the fingerprint is stored.
     *
     * @param fp - fingerprint
     * @author Stjepan Bonić
     */
    size_t prefixIndex(size_t fp, size_t level) const noexcept
    {
        if (level == 0)
        {
            return 0;
        }

        const size_t shift = m_fingerprint_bits - level;

        return fp >> shift;
    }

private:
    uint8_t m_fingerprint_bits;
    uint32_t m_num_buckets;
    uint8_t m_bucket_size;
    uint8_t m_max_num_kicks;
    std::vector<LDCFLevel<T>> m_levels;
    FingerprintFunction m_fingerprint_function;
    HashFunction<T> &m_hash_function;
};

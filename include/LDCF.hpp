#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

#include "HashFunction.hpp"
#include <LDCFConfig.hpp>

/**
 * Interface for Logarithmic Dynamic Cuckoo Filter structure
 *
 * @author Stjepan Bonić
 */
template <typename T>
class LDCF
{
public:
    typedef std::function<uint32_t(T)> FingerprintFunction;
    LDCF(HashFunction<T> &hash_function,
         FingerprintFunction fingerprint_function,
         const LDCFConfig &config = LDCFConfig())
        : m_config(config),
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
                return false;
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
     * Returns number of buckets per filter
     *
     * @author Stjepan Bonić
     */
    size_t bucketCountPerFilter() const noexcept
    {
        return m_config.initialBuckets;
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
            LDCFConfig cf_config = m_config;

            if (cf_config.fingerprintBits <= level_index)
            {
                throw std::runtime_error(
                    "Fingerprint length too small for requested LDCF level");
            }

            cf_config.fingerprintBits -= level_index;

            level.filters[i] = std::make_unique<CuckooFilter<T>>(
                m_hash_function,
                cf_config.fingerprintBits);
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
        size_t fp = m_fingerprint_function(item);

        const size_t bits = m_config.fingerprintBits;

        const size_t mask = (1ULL << bits) - 1ULL;

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

        const size_t total_bits = m_config.fingerprintBits;
        const size_t remaining_bits = total_bits - level;

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

        const size_t total_bits = m_config.fingerprintBits;
        const size_t shift = total_bits - level;

        return fp >> shift;
    }

private:
    LDCFConfig m_config;
    std::vector<LDCFLevel<T>> m_levels;
    FingerprintFunction m_fingerprint_function;
    HashFunction<T> &m_hash_function;
};

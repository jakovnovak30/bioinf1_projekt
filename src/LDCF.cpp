#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

#include <LDCF.hpp>
#include <LDCFConfig.hpp>
#include <CuckooFilter.hpp>

/**
 * Implementation of LDCF
 *
 * @author Stjepan Bonić
 */
template <typename T>
class LDCF : LDCF<T>
{
public:
    typedef std::function<T(T)> FingerprintFunction;

    LDCF(const Config &cfg = Config())
        : config(cfg),
          fingerprint_function(config.fingerprint_function)
    {
        appendLevel();
    }

    ~LDCF() = default;

    bool insert(T item)
    {
        const size_t fp = makeFingerprint(item);

        while (true)
        {
            LDCFLevel &last_level = levels.back();
            const size_t level_index = levels.size() - 1;

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            if (last_level.filters[filter_index].insert(item, stripped_fp))
            {
                return true;
            }

            appendLevel();
        }

        return false;
    }

    bool lookup(T item) const
    {
        const size_t fp = makeFingerprint(item);

        for (size_t level_index = 0; level_index < levels.size(); level_index++)
        {
            const LDCFLevel &level = levels[level_index];

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            if (level.filters[filter_index].lookup(item, stripped_fp))
            {
                return true;
            }
        }

        return false;
    }

    bool del(T item)
    {
        const size_t fp = makeFingerprint(item);

        for (size_t level_index = 0; level_index < levels.size(); level_index++)
        {
            LDCFLevel &level = levels[level_index];

            const size_t filter_index = prefixIndex(fp, level_index);
            const size_t stripped_fp = stripPrefix(fp, level_index);

            if (level.filters[filter_index].del(item, stripped_fp))
            {
                return true;
            }
        }

        return false;
    }

    size_t levelCount() const noexcept
    {
        return levels.size();
    }

    size_t bucketCountPerFilter() const noexcept
    {
        return config.bucket_count;
    }

    void clear()
    {
        for (LDCFLevel &level : levels)
        {
            for (auto &filter : level.filters)
            {
                filter.clear();
            }
        }

        levels.clear();
        appendLevel();
    }

private:
    void appendLevel()
    {
        const size_t level_index = levels.size();
        const size_t filter_count = static_cast<size_t>(1ULL << level_index);

        LDCFLevel level;
        level.filters.reserve(filter_count);

        for (size_t i = 0; i < filter_count; i++)
        {
            LDCFConfig cf_config = config;

            if (cf_config.fingerprint_bits <= level_index)
            {
                throw std::runtime_error(
                    "Fingerprint length too small for requested LDCF level");
            }

            cf_config.fingerprint_bits -= level_index;

            level.filters.emplace_back(cf_config);
        }

        levels.push_back(std::move(level));
    }

    size_t makeFingerprint(T item) const noexcept
    {
        size_t fp = fingerprint_function(item);

        const size_t bits = config.fingerprint_bits;

        const size_t mask = (1ULL << bits) - 1ULL;

        fp &= mask;

        if (fp == 0)
        {
            fp = 1;
        }

        return fp;
    }

    size_t stripPrefix(size_t fp, size_t level) const noexcept
    {
        if (level == 0)
        {
            return fp;
        }

        const size_t total_bits = config.fingerprint_bits;
        const size_t remaining_bits = total_bits - level;

        const size_t mask = (static_cast<size_t>(1ULL << remaining_bits) - 1ULL);

        return fp & mask;
    }

    size_t prefixIndex(size_t fp, size_t level) const noexcept
    {
        if (level == 0)
        {
            return 0;
        }

        const size_t total_bits = config.fingerprint_bits;
        const size_t shift = total_bits - level;

        return fp >> shift;
    }

private:
    LDCFConfig config;
    std::vector<LDCFLevel> levels;
    FingerprintFunction fingerprint_function;
};
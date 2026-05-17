#pragma once

#include "CuckooFilter.hpp"
#include <cstddef>
#include <vector>

/**
 * Configuration for LDCF structure
 *
 * @author Stjepan Bonić
 */
struct LDCFConfig
{
    size_t initialBuckets = 1024;
    size_t fingerprintBits = 16;
    size_t minFingerprintBits = 4;
};

/**
 * Abstraction of levels for LDCF structure
 *
 * @author Stjepan Bonić
 */
template <typename T>
struct LDCFLevel
{
    std::vector<std::unique_ptr<CuckooFilter<T>>> filters;
};

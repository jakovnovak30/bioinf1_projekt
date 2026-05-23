#pragma once

#include "CuckooFilter.hpp"
#include <cstddef>
#include <vector>

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

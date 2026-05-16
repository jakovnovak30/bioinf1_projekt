#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

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
    typedef std::function<T(T)> FingerprintFunction;

    LDCF(const LDCFConfig &config = Config());

    ~LDCF() = default;

    /**
     * Inserts item in LDCF structure
     *
     * @param item - any hashable type that is to be stored in the structure of filters
     * @returns nothing
     * @author Stjepan Bonić
     */
    bool insert(T item);
    /**
     * Checks if item is inside of the structure
     *
     * @param item - any hashable type
     * @returns true if item is in LDCF structure
     * @author Stjepan Bonić
     */
    bool lookup(T item) const;
    /**
     * Removes item from the structure
     *
     * @param item - any hashable type
     * @returns true if item was removed from the structure
     * @author Stjepan Bonić
     */
    bool del(T item);

    /**
     * Returns number of levels in the structure
     *
     * @author Stjepan Bonić
     */
    size_t levelCount() const noexcept;
    /**
     * Returns number of buckets per filter
     *
     * @author Stjepan Bonić
     */
    size_t bucketCountPerFilter() const noexcept;

    /**
     * Clears all items from LDCF structure
     *
     * @returns nothing
     * @author Stjepan Bonić
     */
    void clear();

private:
    /**
     * Adds another level to the LDCF structure
     *
     * @returns nothing
     * @author Stjepan Bonić
     */
    void appendLevel();

    /**
     * Derives a fingerprint of an item to be stored in LDCF.
     *
     * @param item - any hashable type
     * @returns fingerprint of an item
     * @author Stjepan Bonić
     */
    size_t makeFingerprint(uint64_t hash) const noexcept;
    /**
     * Strips prefix from a fingerprint based on the level of the structure that the item will be stored.
     *
     * @param fp - fingerprint of an item to be stored
     * @param level - level of the structure where the item will be stored
     * @returns fingerprint of an item without the prefix
     * @author Stjepan Bonić
     */
    size_t stripPrefix(size_t fp, size_t level) const noexcept;
    /**
     * Returns index of filter where the fingerprint is stored.
     *
     * @param fp - fingerprint
     * @author Stjepan Bonić
     */
    size_t prefixIndex(size_t fp, size_t level) const noexcept;

    LDCFConfig config;
    std::vector<LDCFLevel> levels;
    FingerprintFunction &fingerprint_function;
};
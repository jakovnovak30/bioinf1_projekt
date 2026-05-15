#include <cstddef>

/**
 * Configuration for LDCF structure
 *
 * @author Stjepan Bonić
 */
struct Config
{
    size_t initialBuckets = 1024;
    size_t fingerprintBits = 16;
    size_t minFingerprintBits = 4;
};
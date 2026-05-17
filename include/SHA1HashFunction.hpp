#include "HashFunction.hpp"
#include <string>

/*
 * HashFunction implementation using OpenSSL's SHA1 implementation.
 *
 * @author Jakov Novak
 */
class SHA1HashFunction : public HashFunction<std::string> {
public:
  SHA1HashFunction() = default;

  /*
   * Hashing method
   *
   * @param str - input string that is to be hashed with SHA1
   * @return SHA1 hash modulo this->m
   * @author Jakov Novak
   */
  virtual uint64_t hash(const std::string &str) const override;

  /*
   * Convert back function. converts integer to string
   *
   * @return returns integer represented as string
   * @author Jakov Novak
   */
  virtual std::string convert_back(uint64_t val) const override;

  /*
   * Maximum result function
   *
   * @return returns this->m
   * @author Jakov Novak
   */
  virtual size_t max_res() const override;
private:
  size_t m;
};

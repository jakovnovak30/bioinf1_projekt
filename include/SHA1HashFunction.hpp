#include "HashFunction.hpp"
#include <string>

/*
 * HashFunction implementation using OpenSSL's SHA1 implementation.
 *
 * @author Jakov Novak
 */
class SHA1HashFunction : public HashFunction<std::string> {
public:
  /*
   * Default constructor of class, accepting size limit argument
   *
   * @param m - maximal size method hash should return
   * @author Jakov Novak
   */
  SHA1HashFunction(size_t m);

  /*
   * Hashing method
   *
   * @param str - input string that is to be hashed with SHA1
   * @return SHA1 hash modulo this->m
   * @author Jakov Novak
   */
  virtual uint64_t hash(const std::string &str) const override;

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

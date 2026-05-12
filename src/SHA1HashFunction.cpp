#include "SHA1HashFunction.hpp"
#include "logging.hpp"

#include <openssl/evp.h>
#include <cassert>

SHA1HashFunction::SHA1HashFunction(size_t m) {
  this->m = m;
  assert(m > 0);
}

uint64_t SHA1HashFunction::hash(const std::string &str) const {
  EVP_MD_CTX *mdctx;  
  const EVP_MD *md;  
  unsigned char md_value[EVP_MAX_MD_SIZE];  
  unsigned int md_len;  

  md = EVP_sha1();  
  mdctx = EVP_MD_CTX_create();  
  EVP_DigestInit_ex(mdctx, md, NULL);  
  EVP_DigestUpdate(mdctx, str.c_str(), str.length());  

  EVP_DigestFinal_ex(mdctx, md_value, &md_len);  
  EVP_MD_CTX_destroy(mdctx);  
#ifdef DEBUG
  std::string digest_str;
  for(size_t i = 0; i < md_len; i++){  
    char curr[3];
    snprintf(curr, 3, "%02x", md_value[i]);  
    curr[2] = '\0';
    digest_str += curr;
  }  
  LOG("Digest is: ", digest_str);
#endif
  EVP_cleanup();  

  return 0;
}

uint64_t SHA1HashFunction::max_res() const {
  return 1;
}


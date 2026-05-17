#include <gtest/gtest.h>
#include <string>

#include "CuckooFilter.hpp"
#include "SHA1HashFunction.hpp"

class CuckooFilterStringTests : public testing::Test {
  protected:
    CuckooFilterStringTests()
    : hf(SHA1HashFunction()),
      cf(hf)
    {}
    
    SHA1HashFunction hf;
    CuckooFilter<std::string> cf;
};

TEST_F(CuckooFilterStringTests, TestInsert) {
  ASSERT_NO_THROW(cf.insert("ATTCA"));
  ASSERT_NO_THROW(cf.insert("AGGCTTCA"));
  ASSERT_NO_THROW(cf.insert("AACTGCA"));
  ASSERT_NO_THROW(cf.insert("AAGGAACTGCAGCTTCAAACTGCAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AATTCAAGGAACTGCAGCTTTTAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AGCAGCTGGTTTAACTGCA"));
}

TEST_F(CuckooFilterStringTests, TestDel) {
  ASSERT_NO_THROW(cf.insert("ATTCA"));
  ASSERT_NO_THROW(cf.insert("AGGCTTCA"));
  ASSERT_NO_THROW(cf.insert("AACTGCA"));
  ASSERT_NO_THROW(cf.insert("AAGGAACTGCAGCTTCAAACTGCAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AATTCAAGGAACTGCAGCTTTTAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AGCAGCTGGTTTAACTGCA"));

  ASSERT_NO_THROW(cf.del("ATTCA"));
  ASSERT_NO_THROW(cf.del("AGGCTTCA"));
  ASSERT_NO_THROW(cf.del("AACTGCA"));
  ASSERT_NO_THROW(cf.del("AAGGAACTGCAGCTTCAAACTGCAACTGCA"));
  ASSERT_NO_THROW(cf.del("AATTCAAGGAACTGCAGCTTTTAACTGCA"));
  ASSERT_NO_THROW(cf.del("AGCAGCTGGTTTAACTGCA"));

  ASSERT_THROW(cf.del("ATTCA"), std::runtime_error);
  ASSERT_THROW(cf.del("AGGTTCA"), std::runtime_error);
  ASSERT_THROW(cf.del("AATTGGTTCA"), std::runtime_error);
}

TEST_F(CuckooFilterStringTests, TestLookup) {
  ASSERT_NO_THROW(cf.insert("ATTCA"));
  ASSERT_NO_THROW(cf.insert("AGGCTTCA"));
  ASSERT_NO_THROW(cf.insert("AACTGCA"));
  ASSERT_NO_THROW(cf.insert("AAGGAACTGCAGCTTCAAACTGCAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AATTCAAGGAACTGCAGCTTTTAACTGCA"));
  ASSERT_NO_THROW(cf.insert("AGCAGCTGGTTTAACTGCA"));

  ASSERT_TRUE(cf.lookup("ATTCA"));
  ASSERT_TRUE(cf.lookup("AGGCTTCA"));
  ASSERT_TRUE(cf.lookup("AACTGCA"));
  ASSERT_TRUE(cf.lookup("AAGGAACTGCAGCTTCAAACTGCAACTGCA"));
  ASSERT_TRUE(cf.lookup("AATTCAAGGAACTGCAGCTTTTAACTGCA"));
  ASSERT_TRUE(cf.lookup("AGCAGCTGGTTTAACTGCA"));

  ASSERT_FALSE(cf.lookup("ATTCA"));
  ASSERT_FALSE(cf.lookup("AGGTTCA"));
  ASSERT_FALSE(cf.lookup("AATTGGTTCA"));
}

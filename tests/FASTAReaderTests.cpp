#include "FASTAReader.hpp"

#include <gtest/gtest.h>

class FASTAReaderTests : public testing::Test {
  protected:
    FASTAReaderTests()
      : reader("datasets/artificial/test_read.fasta")
    {}

    FASTAReader reader;
};

TEST_F(FASTAReaderTests, TestReadAll) {
  std::cout << reader.read_all() << std::endl;
  ASSERT_EQ(reader.read_all(), "ATTCAGCCAGCCCAATTTTACC");
}

TEST_F(FASTAReaderTests, TestRead5) {
  std::string r1 = reader.read_random(5);
  std::string r2 = reader.read_random(5);

  ASSERT_NE(r1, r2);
  ASSERT_EQ(r1.length(), 5);
  ASSERT_EQ(r2.length(), 5);
}

TEST_F(FASTAReaderTests, TestRead10) {
  std::string r1 = reader.read_random(10);
  std::string r2 = reader.read_random(10);

  ASSERT_NE(r1, r2);
  ASSERT_EQ(r1.length(), 10);
  ASSERT_EQ(r2.length(), 10);
}

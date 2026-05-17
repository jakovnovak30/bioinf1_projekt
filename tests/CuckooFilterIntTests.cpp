#include <gtest/gtest.h>
#include <stdexcept>

#include "CuckooFilter.hpp"
#include "HashFunction.hpp"

class CuckooFilterTestInt : public testing::Test {
private:
  class SimpleHashF : public HashFunction<uint32_t> {
  public:
    SimpleHashF() = default;

    virtual uint64_t hash(const uint32_t &val) const override {
      return val % 100;
    }

    virtual uint32_t convert_back(uint64_t val) const override {
      return val;
    }

    virtual size_t max_res() const override {
      return 100;
    }
  };

protected:
  CuckooFilterTestInt() :
    hf(SimpleHashF())
  {
      // fingerprint bits 6
      // buckets: 10
      // bucket size: 2
      // max number of kicks: 1
      cf = new CuckooFilter<uint32_t>(hf, 6, 10, 2, 1);
  }
  ~CuckooFilterTestInt()
  {
    delete cf;
  };

  CuckooFilter<uint32_t> *cf;
  SimpleHashF hf;
};

TEST_F(CuckooFilterTestInt, TestDel) {
  cf->insert(1);
  cf->insert(2);
  cf->insert(3);
  ASSERT_NO_THROW(cf->del(2));
}

TEST_F(CuckooFilterTestInt, TestDelError) {
  ASSERT_THROW(cf->del(1), std::runtime_error);
}

TEST_F(CuckooFilterTestInt, TestLookup) {
  cf->insert(12);
  cf->insert(4);
  cf->insert(8);

  ASSERT_TRUE(cf->lookup(12));
  ASSERT_TRUE(cf->lookup(4));
  ASSERT_TRUE(cf->lookup(8));

  ASSERT_FALSE(cf->lookup(13));
  ASSERT_FALSE(cf->lookup(7));
  ASSERT_FALSE(cf->lookup(1));
}

TEST_F(CuckooFilterTestInt, TestInsertFail) {
  for (size_t i=0;i < 20;++i)
  {
    ASSERT_NO_THROW(cf->insert(i));
  }

  ASSERT_THROW(cf->insert(21), std::runtime_error);
}

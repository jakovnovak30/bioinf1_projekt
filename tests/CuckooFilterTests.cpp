#include <gtest/gtest.h>

#include "CuckooFilter.hpp"
#include "HashFunction.hpp"

class CuckooFilterTestInt : public testing::Test {
private:
  class SimpleHashF : public HashFunction<int> {
  public:
    SimpleHashF() = default;

    virtual size_t hash(const int &val) const override {
      return val % 11;
    }

    virtual size_t max_res() const override {
      return 11;
    }
  };

protected:
  CuckooFilterTestInt() :
    hf(SimpleHashF()),
    ff([](int x) { return x; })
  {
      cf = new CuckooFilter<int>(hf, ff);
  }
  ~CuckooFilterTestInt()
  {
    delete cf;
  };

  CuckooFilter<int> *cf;
  CuckooFilter<int>::FingerprintFunction ff;
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

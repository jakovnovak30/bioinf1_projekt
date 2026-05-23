#include <gtest/gtest.h>

#include "LDCF.hpp"
#include "HashFunction.hpp"

class LDCFTestInt : public testing::Test
{
private:
    class SimpleHashF : public HashFunction<uint32_t>
    {
    public:
        uint64_t hash(const uint32_t &val) const override
        {
            return val % 1024;
        }

        uint32_t convert_back(uint64_t val) const override
        {
            return static_cast<uint32_t>(val);
        }

        size_t max_res() const override
        {
            return 1024;
        }
    };

    class SimpleFingerprintF
    {
    public:
        uint32_t operator()(const HashFunction<uint32_t> &, const uint32_t val) const
        {
            return static_cast<uint32_t>((val * 2654435761) % 4294967291);
        }
    };

protected:
    LDCFTestInt()
        : hf(),
          ff(),
          ldcf(hf, ff)
    {
    }

    SimpleHashF hf;
    SimpleFingerprintF ff;
    LDCF<uint32_t> ldcf;
};

TEST_F(LDCFTestInt, InsertAndLookupBasic)
{
    ldcf.insert(10);
    ldcf.insert(20);
    ldcf.insert(30);

    EXPECT_TRUE(ldcf.lookup(10));
    EXPECT_TRUE(ldcf.lookup(20));
    EXPECT_TRUE(ldcf.lookup(30));

    EXPECT_FALSE(ldcf.lookup(40));
}

TEST_F(LDCFTestInt, DeleteExisting)
{
    ldcf.insert(100);
    ldcf.insert(200);

    EXPECT_TRUE(ldcf.lookup(100));

    EXPECT_TRUE(ldcf.del(100));

    EXPECT_FALSE(ldcf.lookup(100));
    EXPECT_TRUE(ldcf.lookup(200));
}

TEST_F(LDCFTestInt, DeleteNonExisting)
{
    ldcf.insert(1);
    ldcf.insert(2);

    EXPECT_FALSE(ldcf.del(999));
}

TEST_F(LDCFTestInt, ClearResetsStructure)
{
    ldcf.insert(11);
    ldcf.insert(22);
    ldcf.insert(33);

    EXPECT_TRUE(ldcf.lookup(11));

    ldcf.clear();

    EXPECT_FALSE(ldcf.lookup(11));
    EXPECT_FALSE(ldcf.lookup(22));
    EXPECT_FALSE(ldcf.lookup(33));

    EXPECT_EQ(ldcf.levelCount(), 1);
}

TEST_F(LDCFTestInt, LevelCountGrowth)
{
    size_t initial_levels = ldcf.levelCount();

    for (uint32_t i = 0; i < 200; i++)
    {
        ldcf.insert(i);
    }

    EXPECT_GE(ldcf.levelCount(), initial_levels);
}

TEST_F(LDCFTestInt, NoFalseNegatives)
{
    std::vector<uint32_t> values;

    for (uint32_t i = 0; i < 100; i++)
    {
        values.push_back(i);
        ldcf.insert(i);
    }

    for (auto v : values)
    {
        EXPECT_TRUE(ldcf.lookup(v));
    }
}

TEST_F(LDCFTestInt, DuplicateInsertSafety)
{
    ldcf.insert(42);
    ldcf.insert(42);
    ldcf.insert(42);

    EXPECT_TRUE(ldcf.lookup(42));

    EXPECT_TRUE(ldcf.del(42));
    EXPECT_FALSE(ldcf.lookup(42));
}

TEST_F(LDCFTestInt, StressInsertManyElements)
{
    for (uint32_t i = 0; i < 1000; i++)
    {
        EXPECT_NO_THROW(ldcf.insert(i));
    }

    for (uint32_t i = 0; i < 1000; i++)
    {
        EXPECT_TRUE(ldcf.lookup(i));
    }
}

TEST_F(LDCFTestInt, RepeatedClearAndReuse)
{
    for (int round = 0; round < 5; round++)
    {
        for (uint32_t i = 0; i < 50; i++)
        {
            ldcf.insert(i + round * 100);
        }

        for (uint32_t i = 0; i < 50; i++)
        {
            EXPECT_TRUE(ldcf.lookup(i + round * 100));
        }

        ldcf.clear();
    }
}

#include <ArtificalGenomeGenerator.hpp>

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

class ArtificialGenomeGeneratorTests : public testing::Test
{
protected:
    ArtificialGenomeGeneratorTests()
        : generator(1000)
    {
    }

    ArtificialGenomeGenerator generator;
};

TEST_F(ArtificialGenomeGeneratorTests, TestReadAll)
{
    std::string genome = generator.read_all();

    ASSERT_EQ(genome.length(), 1000);

    for (char c : genome)
    {
        ASSERT_TRUE(
            c == 'A' ||
            c == 'C' ||
            c == 'G' ||
            c == 'T');
    }
}

TEST_F(ArtificialGenomeGeneratorTests, TestRead5)
{
    std::string r1 = generator.read_random(5);
    std::string r2 = generator.read_random(5);

    ASSERT_NE(r1, r2);
    ASSERT_EQ(r1.length(), 5);
    ASSERT_EQ(r2.length(), 5);
}

TEST_F(ArtificialGenomeGeneratorTests, TestRead10)
{
    std::string r1 = generator.read_random(10);
    std::string r2 = generator.read_random(10);

    ASSERT_NE(r1, r2);
    ASSERT_EQ(r1.length(), 10);
    ASSERT_EQ(r2.length(), 10);
}

TEST_F(ArtificialGenomeGeneratorTests, TestReadTooLong)
{
    ASSERT_NO_THROW(
        generator.read_random(generator.read_all().length() - 1));

    ASSERT_THROW(
        generator.read_random(generator.read_all().length() + 1),
        std::runtime_error);

    ASSERT_THROW(
        generator.read_random(1000),
        std::runtime_error);
}

TEST_F(ArtificialGenomeGeneratorTests, TestGeneratedAlphabet)
{
    std::string genome = generator.read_all();

    for (char c : genome)
    {
        ASSERT_TRUE(
            c == 'A' ||
            c == 'C' ||
            c == 'G' ||
            c == 'T');
    }
}
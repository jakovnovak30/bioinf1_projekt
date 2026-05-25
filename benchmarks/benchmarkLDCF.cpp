#include <chrono>
#include <cstddef>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <memory>
#include <print>
#include <sys/resource.h>

#include "LDCF.hpp"
#include "CuckooFilter.hpp"
#include "SHA1HashFunction.hpp"

#include "Genome.hpp"
#include "FASTAReader.hpp"
#include "ArtificalGenomeGenerator.hpp"

struct BenchmarkResult
{
    double insertion_ms;
    double lookup_ms;

    double false_positive_rate;
    double false_negative_rate;

    size_t inserted_elements;
    size_t levels;
    size_t memory_kb;
};

size_t get_memory_usage_kb()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

BenchmarkResult insertion_benchmark(
    LDCF<std::string> &ldcf,
    const std::string &genome,
    std::vector<std::string> &inserted,
    std::unordered_set<std::string> &inserted_set,
    size_t k)
{
    BenchmarkResult result{};

    auto start =
        std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i + k <= genome.length(); i++)
    {
        std::string kmer = genome.substr(i, k);

        if (!inserted_set.contains(kmer))
        {
            inserted.push_back(kmer);

            inserted_set.insert(kmer);

            ldcf.insert(kmer);
        }
    }

    auto end =
        std::chrono::high_resolution_clock::now();

    result.insertion_ms =
        std::chrono::duration<double, std::milli>(
            end - start)
            .count();

    result.inserted_elements =
        inserted.size();

    result.levels =
        ldcf.levelCount();

    result.memory_kb =
        get_memory_usage_kb();

    return result;
}

double lookup_benchmark(
    LDCF<std::string> &ldcf,
    const std::vector<std::string> &inserted)
{
    auto start =
        std::chrono::high_resolution_clock::now();

    for (const auto &seq : inserted)
    {
        ldcf.lookup(seq);
    }

    auto end =
        std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(
               end - start)
        .count();
}

double false_positive_benchmark(
    LDCF<std::string> &ldcf,
    Genome &genome,
    size_t tests,
    std::unordered_set<std::string> &inserted_set,
    size_t k)
{
    size_t false_positive = 0;

    size_t valid_tests = 0;

    for (size_t i = 0; i < tests; i++)
    {
        std::string seq =
            genome.read_random(k);

        if (inserted_set.contains(seq))
        {
            continue;
        }

        valid_tests++;

        if (ldcf.lookup(seq))
        {
            false_positive++;
        }
    }

    return static_cast<double>(
               false_positive) /
           valid_tests;
}

double false_negative_benchmark(
    LDCF<std::string> &ldcf,
    const std::vector<std::string> &inserted)
{
    size_t false_negative = 0;

    for (const auto &seq : inserted)
    {
        if (!ldcf.lookup(seq))
        {
            false_negative++;
        }
    }

    return static_cast<double>(
               false_negative) /
           inserted.size();
}

void save_csv(
    const BenchmarkResult &result,
    size_t genome_size,
    size_t k)
{
    std::ofstream file("benchmark.csv", std::ios::app);

    if (file.tellp() == 0)
    {
        file << "genome_size,"
             << "k,"
             << "inserted_elements,"
             << "levels,"
             << "insertion_ms,"
             << "lookup_ms,"
             << "memory_kb,"
             << "false_positive_rate,"
             << "false_negative_rate"
             << "\n";
    }

    file
        << genome_size << ","
        << k << ","
        << result.inserted_elements << ","
        << result.levels << ","
        << result.insertion_ms << ","
        << result.lookup_ms << ","
        << result.memory_kb << ","
        << result.false_positive_rate << ","
        << result.false_negative_rate
        << "\n";
}

BenchmarkResult run_benchmark(
    Genome &insert_genome,
    Genome &query_genome,
    size_t k,
    size_t testlimit)
{
    SHA1HashFunction sha1;

    LDCF<std::string> ldcf(
        sha1,
        CuckooFilter<std::string>::get_fingerprint);

    std::vector<std::string> inserted;

    std::unordered_set<std::string> inserted_set;

    std::string genome =
        insert_genome.read_all();

    BenchmarkResult result =
        insertion_benchmark(
            ldcf,
            genome,
            inserted,
            inserted_set,
            k);

    result.lookup_ms =
        lookup_benchmark(
            ldcf,
            inserted);

    result.false_negative_rate =
        false_negative_benchmark(
            ldcf,
            inserted);

    result.false_positive_rate =
        false_positive_benchmark(
            ldcf,
            query_genome,
            testlimit,
            inserted_set,
            k);

    std::println("\n===== BENCHMARK RESULTS =====");

    std::println(
        "Inserted elements: {}",
        result.inserted_elements);

    std::println(
        "Levels: {}",
        result.levels);

    std::println(
        "Insertion time: {:.3f} ms",
        result.insertion_ms);

    std::println(
        "Lookup time: {:.3f} ms",
        result.lookup_ms);

    std::println(
        "Memory usage: {} KB",
        result.memory_kb);

    std::println(
        "False positive rate: {:.8f}",
        result.false_positive_rate);

    std::println(
        "False negative rate: {:.8f}",
        result.false_negative_rate);

    return result;
}

int main()
{
    constexpr size_t TESTS = 100000;

    size_t Ks[] = {
      5,
      10,
      20,
      50,
      100,
      150,
      200,
      300
    };

    size_t genome_sizes[] =
        {
            500,
            1000,
            5000,
            10000
        };

    for (const size_t genome_size : genome_sizes)
    {
      for (const size_t k : Ks) {
        std::println(
            "\n===== GENOME SIZE: {} =====",
            genome_size);

        std::println(
            "\n===== CURRENT K: {} =====",
            k);

        FASTAReader insert_genome(
            "datasets/ncbi/ecoli.fna");

        ArtificialGenomeGenerator query_genome(
            genome_size);

        BenchmarkResult result =
            run_benchmark(
                insert_genome,
                query_genome,
                k,
                TESTS);

        save_csv(
            result,
            genome_size,
            k);
      }
    }

    return 0;
}

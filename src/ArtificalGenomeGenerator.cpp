#include <ArtificalGenomeGenerator.hpp>

#include <random>
#include <stdexcept>
#include <string>

ArtificialGenomeGenerator::ArtificialGenomeGenerator(const size_t genome_length) : m_rng(std::random_device{}())
{
    static const char bases[] = {'A', 'C', 'G', 'T'};

    std::uniform_int_distribution<int> base_dist(0, 3);

    m_genome.reserve(genome_length);

    for (size_t i = 0; i < genome_length; i++)
    {
        m_genome += bases[base_dist(m_rng)];
    }
}

std::string ArtificialGenomeGenerator::read_all()
{
    return m_genome;
}

std::string ArtificialGenomeGenerator::read_random(const size_t k)
{
    if (m_genome.length() <= k)
    {
        throw std::runtime_error("k is greater than the genome length");
    }

    const size_t max_start = m_genome.length() - k;

    std::uniform_int_distribution<size_t> start_dist(0, max_start);

    const size_t seqstart = start_dist(m_rng);

    return m_genome.substr(seqstart, k);
}
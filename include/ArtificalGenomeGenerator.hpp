#pragma once

#include <random>
#include <string>

/*
 * Class that serves as abstraction for generating artificial DNA genome sequences
 *
 * @author Stjepan Bonić
 */
class ArtificialGenomeGenerator
{
public:
    /*
     * Default constructor which generates an artificial genome of given size
     *
     * @param genome_length - size of genome to generate
     *
     * @author Stjepan Bonić
     */
    ArtificialGenomeGenerator(const size_t genome_length);

    /*
     * Function which returns the entire artificial genome
     *
     * @return artificial genome sequence
     *
     * @author Stjepan Bonić
     */
    std::string read_all();

    /*
     * Read a random part of the artificial genome of length k
     *
     * @param k - size of sequence that needs to be returned
     * @return sequence of size k, starting at a random location
     * @throws std::runtime_error - if k is greater than the genome size
     *
     * @author Stjepan Bonić
     */
    std::string read_random(const size_t k);

private:
    std::string m_genome;
    std::mt19937 m_rng;
};
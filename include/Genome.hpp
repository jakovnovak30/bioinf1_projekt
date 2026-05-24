#pragma once

#include <string>

/*
 * Abstract class which models a Genome
 *
 * @author Jakov Novak
 */
class Genome {
  public:
    Genome() = default;
    virtual ~Genome() = default;

    /*
     * Read the entire genome.
     *
     * @returns entire genome as string
     * @author Jakov Novak
     */
    virtual std::string read_all() = 0;

    /*
     * Read a random subsequence of the genome.
     *
     * @param k - size of subsequence
     * @returns random substring of size k
     * @author Jakov Novak
     */
    virtual std::string read_random(const size_t k) = 0;
};

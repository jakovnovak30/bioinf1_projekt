#include <fstream>
#include <string>

/*
 * Class that serves as abstraction for reading FASTA files
 *
 * @author Jakov Novak
 */
class FASTAReader {
  public:
    /*
     * Default constructor which expectes
     * a relative path to the file to be read
     *
     * @param filename - relative path to file to read
     *
     * @author Jakov Novak
     */
    FASTAReader(const std::string &filename);

    /**
     * Default destructor to cleanup the memory
     *
     * @author Jakov Novak
     */
    ~FASTAReader();

    /*
     * Function which reads an entire FASTA file,
     * *WARNING* this may be slow for large files
     *
     * @return contents of FASTA file, without comments and whitespaces
     *
     * @author Jakov Novak
     */
    std::string read_all();

    /*
     * Read a random part of the genome of length k
     *
     * @param k - size of sequence that needs to be returned
     * @return sequence of size k, starting at a random location
     *
     * @author Jakov Novak
     */
    std::string read_random(const size_t k);

  private:
    std::ifstream m_file;
};

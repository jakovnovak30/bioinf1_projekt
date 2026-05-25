#include "CuckooFilter.hpp"
#include "LDCF.hpp"
#include "SHA1HashFunction.hpp"
#include "FASTAReader.hpp"
#include "ArtificalGenomeGenerator.hpp"

#include <cstdlib>
#include <memory>
#include <print>

// global vars
static struct {
  // benchmark options
  std::string exe_name;
  std::string in1, in2;
  size_t rand_len = 1024*10;
  size_t k = 5;
  bool verbose = false;
  size_t testlimit = 100000;
  std::string output = "ldcf.log";

  // options for LDCF constructor
  uint32_t num_buckets = 32;
  uint8_t fingerprint_bits = 16;
  uint8_t bucket_size = 4;
  uint8_t max_num_kicks = 3;
} state;

void print_help() {
  state.exe_name = state.exe_name.substr(state.exe_name.find_last_of("/")+1);
  std::println("Usage: {} [-h] [-i1] INPUT1 [-i2] INPUT2 [-v] [LDCF_OPTIONS]", state.exe_name);
  std::println();
  std::println("options:");
  std::println("-h, --help\t print this message");
  std::println("-i1, --in1\t");
  std::println("-i2, --in2\t set the first or second input, can be either a filepath or the words \"rand\" or \"random\" for random genome generation");
  std::println("--rand_len\t set the length of the randomly generated genomes, current is: {}", state.rand_len);
  std::println("-k\t\t set the length of the subsequences searched, current is: {}", state.k);
  std::println("-v, --verbose\t set verbose output mode");
  std::println("LDCF_OPTIONS:");
  std::println("--num_buckets \t set the number of buckets in each Cuckoo Filter");
  std::println("--fingerprint_bits \t set the number of fingerprint bits in the root Cuckoo Filter");
  std::println("--bucket_size \t set the bucket size of each Cuckoo Filter");
  std::println("--max_num_kicks \t set the max_num_kicks parameter of the Cuckoo hashing algorithm");
}

char *arg_pop(int *argc, char ***argv) {
  (*argc)--;
  char *curr = **argv;
  (*argv)++;
  return curr;
}

void parse_args(int *argc, char ***argv) {
  if (*argc == 0) return;

  std::string curr_flag = arg_pop(argc, argv);

  if (curr_flag == "-h" || curr_flag == "--help") {
    print_help();
    exit(0);
  }
  else if (curr_flag == "-i1" || curr_flag == "--in1") {
    state.in1 = arg_pop(argc, argv);
  }
  else if (curr_flag == "-i2" || curr_flag == "--in2") {
    state.in2 = arg_pop(argc, argv);
  }
  else if (curr_flag == "--rand_len") {
    state.rand_len = std::atoi(arg_pop(argc, argv));
  }
  else if (curr_flag == "-v" || curr_flag == "--verbose") {
    state.verbose = true;
  }
  else if (curr_flag == "-k") {
    state.k = std::atoi(arg_pop(argc, argv));
  }
  /* Options for LDCF constructor:
   *
   *  num_buckets (default = 32)
   *  fingerprint_bits (default = 16)
   *  bucket_size (default = 4)
   *  max_num_kicks (default = 3)
  */
  else if (curr_flag == "--num_buckets") {
    state.num_buckets = std::atoi(arg_pop(argc, argv));
  }
  else if (curr_flag == "--fingerprint_bits") {
    state.fingerprint_bits = std::atoi(arg_pop(argc, argv));
  }
  else if (curr_flag == "--bucket_size") {
    state.bucket_size = std::atoi(arg_pop(argc, argv));
  }
  else if (curr_flag == "--max_num_kicks") {
    state.max_num_kicks = std::atoi(arg_pop(argc, argv));
  }
  else {
    std::println("Unknown argument: {}", curr_flag);
    print_help();
    exit(1);
  }
}

/**
 * Entry point of program
 * TODO: more tests?
 */

#define INIT_GENOME(num) \
  if (state.in##num == "rand" || state.in##num == "random") { \
    gen##num = std::make_unique<ArtificialGenomeGenerator>(state.rand_len); \
    if (state.verbose) std::println("Loaded genome {} as random generator", num); \
  }\
  else {\
    /* asssume it's a file path if rand is not set */ \
    if (state.verbose) std::println("Loading genome {} from file: {}", num, state.in##num); \
    gen##num = std::make_unique<FASTAReader>(state.in##num);\
  }

int main(int argc, char **argv) {
  state.exe_name = *argv;
  argc--;
  argv++;
  while (argc) {
    parse_args(&argc, &argv);
  }

  std::unique_ptr<Genome> gen1, gen2;
  INIT_GENOME(1);
  INIT_GENOME(2);

  SHA1HashFunction sha1;
  /*
    LDCF(HashFunction<T> &hash_function,
         FingerprintFunction fingerprint_function,
         uint32_t num_buckets = 32,
         uint8_t fingerprint_bits = 16,
         uint8_t bucket_size = 4,
         uint8_t max_num_kicks = 3)
   */
  LDCF<std::string> ldcf(sha1,
      CuckooFilter<std::string>::get_fingerprint,
      state.num_buckets,
      state.fingerprint_bits,
      state.bucket_size,
      state.max_num_kicks);

  std::string gen1_all = gen1->read_all();
  for (size_t i=0;i < gen1_all.length() - state.k;i++) {
    ldcf.insert(gen1_all.substr(i, state.k));
  }

  if (state.verbose) std::println("Beginning to search random gen2 seqs in gen1");

  for (size_t i=0;i < state.testlimit;i++) {
    std::string curr = gen2->read_random(state.k);
    if (ldcf.lookup(curr)) {
      std::println("Found {} in genome 1!", curr);
    }
  }

  return 0;
}

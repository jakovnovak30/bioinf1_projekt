#include "LDCF.hpp"
#include "FASTAReader.hpp"
#include "ArtificalGenomeGenerator.hpp"

#include <cstdlib>
#include <memory>
#include <print>

// global vars
static struct {
 std::string in1, in2;
 size_t rand_len = 1024;
 size_t k = 5;
 bool verbose = false;
} state;

void print_help() {
  std::println("Usage:");
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
  else {
    std::println("Unknown argument: {}", curr_flag);
    print_help();
    exit(1);
  }
}

/**
 * Entry point of program
 * TODO: add cli arguments and proper main.cpp
 * TODO: more doxygen comments
 * TODO: more tests?
 */

#define INIT_GENOME(num) \
  if (state.in##num == "rand" || state.in##num == "random") { \
    gen##num = std::make_unique<ArtificialGenomeGenerator>(state.rand_len); \
  }\
  else {\
    /* asssume it's a file path if rand is not set */ \
    gen##num = std::make_unique<FASTAReader>(state.in##num);\
  }

int main(int argc, char **argv) {
  argc--;
  argv++;
  while (argc) {
    parse_args(&argc, &argv);
  }

  std::unique_ptr<Genome> gen1, gen2;
  INIT_GENOME(1);
  INIT_GENOME(2);

  std::println("random part of gen1, {}", gen1->read_random(state.k));
  std::println("random part of gen2, {}", gen2->read_random(state.k));

  return 0;
}

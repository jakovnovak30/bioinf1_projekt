#include "LDCF.hpp"
#include "FASTAReader.hpp"
#include <cstdlib>
#include <print>

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
  else if (curr_flag == "-s1" || curr_flag == "--seq1") {
    std::string seq1 = arg_pop(argc, argv);
  }
  else if (curr_flag == "-s2" || curr_flag == "--seq2") {
    std::string seq1 = arg_pop(argc, argv);
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
int main(int argc, char **argv) {
  if (argc < 3) {
    std::println("Not enough arguments");
    return 1;
  }

  argc--;
  argv++;
  while (argc) {
    parse_args(&argc, &argv);
  }

  return 0;
}

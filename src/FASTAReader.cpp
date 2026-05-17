#include "FASTAReader.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

FASTAReader::FASTAReader(const std::string &filename) {
  m_genome = "";
  m_file.open(filename);
}

FASTAReader::~FASTAReader() {
  m_file.close();
}

std::string FASTAReader::read_all() {
  if (m_genome.length() > 0) {
    return m_genome;
  }

  // read entire file
  m_file.seekg(0, std::ios::beg);
  std::string line;
  while (std::getline(m_file, line)) {
    if (line[0] == '>')
      continue;
    m_genome += line;
  }

  m_file.clear();
  return m_genome;
}

std::string FASTAReader::read_random(const size_t k) {
  std::string genome = read_all();

  const size_t max_start = genome.length() - k;
  const size_t seqstart = rand() % max_start;

  return genome.substr(seqstart, k);
}

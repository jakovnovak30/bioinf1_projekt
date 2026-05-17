#include "FASTAReader.hpp"

#include <iostream>
#include <string>

FASTAReader::FASTAReader(const std::string &filename) {
  m_file.open(filename);
}

FASTAReader::~FASTAReader() {
  m_file.close();
}

std::string FASTAReader::read_all() {
  std::string out, line;

  // set to start of stream
  m_file.seekg(0, std::ios::beg);

  while (std::getline(m_file, line)) {
    if (line[0] == '>')
      continue;
    out += line;
  }

  m_file.clear();
  return out;
}

std::string FASTAReader::read_random(const size_t k) {
  std::string out, line;

  m_file.seekg(0, std::ios::end);
  auto pos = m_file.tellg();
  std::cout << "size of file: " << pos << std::endl;

  return "TODO";
}

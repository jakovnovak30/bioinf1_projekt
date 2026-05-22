#pragma once

#include <string>

class Genome {
  public:
    Genome() = default;
    virtual ~Genome() = default;

    virtual std::string read_all() = 0;
    virtual std::string read_random(const size_t k) = 0;
};

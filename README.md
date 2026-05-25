# Bioninf1 projekt

## About
This repository was made as part of the class "Bioinformatics 1" at the Faculty of Electronics and Computing at the University of Zagreb.
Course Link: https://www.fer.unizg.hr/predmet/bio1

## Task (3) Logarithmic Dynamic Cuckoo Filter (Zhang et al. 2021) (MDL) 
    - Zhang et al. The Logarithmic Dynamic Cuckoo Filter doi: 10.1109/ICDE51399.2021.00087 
    - Chen et al. 2017. The dynamic cuckoo filter; https://ieeexplore.ieee.org/abstract/document/8117563 
    - Fan et al. 2013. Cuckoo Filter: Better Than Bloom; https://www.cs.cmu.edu/~binfan/papers/login_cuckoofilter.pdf 
    - Fan et al. 2014. Cuckoo Filter: Practically Better Than Bloom; http://www.cs.cmu.edu/%7Ebinfan/papers/conext14_cuckoofilter.pdf 
    - tražiti slučajne podnizove (k-mere uz različite k, npr. k = 10, 20, 50, 100, 200) u E. coli genomu te u umjetno generiranim podatcima 
    - napraviti vlastiti LDCF te usporediti s originalnom implementacijom 

## Usage
There are several targets configured as part of the Makefile. The most important ones are:

    - make run_tests
    - make all
    - make benchmark
    - make docs
    - make cleanall

It is advisable to use the -j flag in order to speed up compilation time, and the optional DEBUG=1 setting for debugging.
The first target compiles and runs unit tests made using GTest, the second one makes the executable (which can be found in the build directory).
The benchmark target compiles and runs a benchmark, which compares random data to the Ecoli genome (https://www.ncbi.nlm.nih.gov/datasets/taxonomy/562/).
Docs target generates documentation from source code comments using Doxygen. Cleanall target removes the generated code and docs from the system.

## Requirements
In order to run the code you need:
- g++
- openssl

For testing, you need (additionally):
- gtest

For generating docs:
- Doxygen

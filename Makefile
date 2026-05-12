DEBUG ?= 1

CXX=g++
CXXFLAGS=-O1 -std=c++26 -Wall -Wextra
TEST_CXXFLAGS=-lgtest
ifeq ($(DEBUG), 1)
	CXXFLAGS:=$(CXXFLAGS) -g -DDEBUG
endif
LD=g++
LDFLAGS=-flto -lcrypto

EXECUTABLE=cuckoo_filter
SRCDIR=src
TESTDIR=tests
IDIR=include
BUILDDIR=build

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(addprefix $(BUILDDIR)/, $(notdir $(SRCS:cpp=o)))
TEST_SRCS=$(wildcard $(TESTDIR)/*.cpp)
TESTS=$(addprefix $(BUILDDIR)/tests/, $(notdir $(TEST_SRCS:cpp=o)))

# colors
RST="\033[0;0m"
GRN="\033[0;32m"

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@echo -e $(GRN)"[linking]" $(RST) $(EXECUTABLE)
	@-$(LD) $(LDFLAGS) $(OBJS) -o $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(IDIR)/*.hpp)
	@mkdir -p build
	@echo -e $(GRN)"[compiling] "$(RST) $<
	@-$(CXX) -I$(IDIR) $(CXXFLAGS) -c -o $@ $<

run_tests: tests
	@$(BUILDDIR)/tests/run_all

tests: $(OBJS) $(TESTS)
	@echo -e $(GRN)"[linking tests] "$(RST) $(notdir $(TESTS))
	@-$(LD) $(TESTS) $(TEST_CXXFLAGS) -o $(BUILDDIR)/tests/run_all

$(BUILDDIR)/tests/%.o: $(TESTDIR)/%.cpp
	@mkdir -p build/tests
	@echo -e $(GRN)"[compiling test] "$(RST) $<
	@-$(CXX) -I$(IDIR) $(TEST_CXXFLAGS) -c -o $@ $<

clean:
	rm -rf build

docs: $(SRCS) $(wildcard $(IDIR)/*.hpp)
	@cd docs && doxygen

cleandocs:
	@rm -rf docs/latex
	@rm -rf docs/html

cleanall:
	make clean
	make cleandocs

.PHONY: cleanall clean cleandocs docs

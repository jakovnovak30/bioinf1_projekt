DEBUG ?= 0

CXX=g++
CXXFLAGS=-O2 -std=c++26 -Wall -Wextra --pedantic
TEST_CXXFLAGS=-g
TEST_LDFLAGS=-lgtest -lgtest_main -pthread -lcrypto
ifeq ($(DEBUG), 1)
	CXXFLAGS:=$(CXXFLAGS) -g -DDEBUG
endif
LD=g++
LDFLAGS=-flto -lcrypto

# detect OS
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S), Linux)
	# NOTE: no windows support

	# macOS config for openssl
	CXXFLAGS += -I/opt/homebrew/include
	LDFLAGS += -L/opt/homebrew/opt/openssl@3/lib -lssl
	# macOS config for google tests
	TEST_CXXFLAGS += -I/opt/homebrew/include
	TEST_LDFLAGS += -L/opt/homebrew/lib
endif

EXECUTABLE=cuckoo_filter
BENCHMARK_EXECUTABLE=ldcf_benchmark
SRCDIR=src
TESTDIR=tests
IDIR=include
BENCHDIR=benchmarks
BUILDDIR=build

SRCS=$(wildcard $(SRCDIR)/*.cpp)
BENCHMARK_SRCS=$(wildcard $(BENCHDIR)/*.cpp)
BENCHMARK_OBJS=$(addprefix $(BUILDDIR)/, $(notdir $(BENCHMARK_SRCS:cpp=o)))
OBJS=$(addprefix $(BUILDDIR)/, $(notdir $(SRCS:cpp=o)))
OBJS_NO_MAIN=$(filter-out \
	$(BUILDDIR)/main.o, \
	$(OBJS))
TEST_SRCS=$(wildcard $(TESTDIR)/*.cpp)
TESTS=$(addprefix $(BUILDDIR)/tests/, $(notdir $(TEST_SRCS:cpp=o)))

# colors
RST="\033[0;0m"
GRN="\033[0;32m"

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@echo -e $(GRN)"[linking]" $(RST) $(EXECUTABLE)
	@-$(LD) $(LDFLAGS) $(OBJS) -o $(BUILDDIR)/$(EXECUTABLE)

$(BENCHMARK_EXECUTABLE): $(OBJS_NO_MAIN) $(BENCHMARK_OBJS)
	@echo -e $(GRN)"[linking benchmark]" $(RST) $(BENCHMARK_EXECUTABLE)
	@-$(LD) $(LDFLAGS) $(OBJS_NO_MAIN) $(BENCHMARK_OBJS) \
		-o $(BUILDDIR)/$(BENCHMARK_EXECUTABLE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(IDIR)/*.hpp)
	@mkdir -p build
	@echo -e $(GRN)"[compiling] "$(RST) $<
	@-$(CXX) -I$(IDIR) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: $(BENCHDIR)/%.cpp $(wildcard $(IDIR)/*.hpp)
	@mkdir -p build
	@echo -e $(GRN)"[compiling benchmark]" $(RST) $<
	@-$(CXX) -I$(IDIR) $(CXXFLAGS) -c -o $@ $<

run_tests: tests
	@$(BUILDDIR)/tests/run_all

tests: $(OBJS_NO_MAIN) $(TESTS)
	@echo -e $(GRN)"[linking tests] "$(RST) $(notdir $(TESTS))
	@-$(LD) $(TESTS) $(OBJS_NO_MAIN) $(TEST_CXXFLAGS) $(TEST_LDFLAGS) -o $(BUILDDIR)/tests/run_all

$(BUILDDIR)/tests/%.o: $(TESTDIR)/%.cpp
	@mkdir -p build/tests
	@echo -e $(GRN)"[compiling test] "$(RST) $<
	@-$(CXX) -I$(IDIR) $(TEST_CXXFLAGS) ${CXXFLAGS} -c -o $@ $<

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

benchmark: $(BENCHMARK_EXECUTABLE)
	@$(BUILDDIR)/$(BENCHMARK_EXECUTABLE)

.PHONY: cleanall clean cleandocs docs

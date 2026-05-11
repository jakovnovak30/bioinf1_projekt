DEBUG ?= 1

CXX=g++
CXXFLAGS=-c -std=c++26
ifeq ($(DEBUG), 1)
	CXXFLAGS:=$(CXXFLAGS) -g -DDEBUG
endif
LD=g++
LDFLAGS=-lto

EXECUTABLE=cuckoo_filter
SRCDIR=src
IDIR=include
BUILDDIR=build

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(addprefix $(BUILDDIR)/, $(notdir $(SRCS:cpp=o)))

# colors
RST="\033[0;0m"
GRN="\033[0;32m"

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@echo "[test] object list:"
	@echo $(OBJS)
	@echo -e $(GRN)"[linking]" $(RST) $(EXECUTABLE)
	@-$(LD) $(OBJS) -o $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p build
	@echo -e $(GRN)"[compiling] "$(RST) $<
	@-$(CXX) -I$(IDIR) $(CXXFLAGS) -o $@ $<

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

.PHONY: cleanall clean cleandocs

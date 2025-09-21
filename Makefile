CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -g -O0

SOURCES=$(shell find src -name "*.cpp")
BUILDDIR=build
TARGET=$(BUILDDIR)/game
PERFT=$(BUILDDIR)/perft

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

perft: $(PERFT)

$(PERFT): test/perft.cpp
	$(CXX) $(CXXFLAGS) test/perft.cpp -o $@

clean:
	rm -rf $(BUILDDIR)/*

.PHONY: all clean

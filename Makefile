CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -g -O0

SOURCES=$(shell find src -name "*.cpp")
BUILDDIR=build
TARGET=$(BUILDDIR)/game

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(BUILDDIR)/*

.PHONY: all clean

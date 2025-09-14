CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -g -O0

SOURCES=$(shell find src -name "*.cpp")
TARGET=build/game

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

clean:
	rm -f $(TARGET) && rm -rf $(TARGET).dSYM

.PHONY: all clean

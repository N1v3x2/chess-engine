CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra
DEBUGFLAGS=-g -O0
OUT=game

all:
	$(CXX) $(CXXFLAGS) -o $(OUT) Game.cpp

debug:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $(OUT) Game.cpp

clean:
	rm -f $(OUT) && rm -r $(OUT).dSYM

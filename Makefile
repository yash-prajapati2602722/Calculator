CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2

.PHONY: all c cpp clean

all: c cpp

c: calculator

calculator: calculator.c
	$(CC) $(CFLAGS) calculator.c -lm -o calculator

cpp: calculator-cpp

calculator-cpp: calculator.cpp
	$(CXX) $(CXXFLAGS) calculator.cpp -o calculator-cpp

clean:
	rm -f calculator calculator-cpp
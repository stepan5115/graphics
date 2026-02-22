CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGETS = zeroLab
SOURCES = zeroLab.cpp

all: $(TARGETS)

zeroLab: zeroLab.cpp
	$(CXX) $(CXXFLAGS) zeroLab.cpp -o zeroLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: zeroLab
	./zeroLab
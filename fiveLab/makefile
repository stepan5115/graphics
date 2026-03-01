CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGL -lGLU -lglut -lSOIL
TARGETS = fiveLab
SOURCES = fiveLab.cpp

all: $(TARGETS)

fiveLab: fiveLab.cpp
	$(CXX) $(CXXFLAGS) fiveLab.cpp -o fiveLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: fiveLab
	./fiveLab


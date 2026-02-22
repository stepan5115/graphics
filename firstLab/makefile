CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGL -lGLU -lglut
TARGETS = firstLab
SOURCES = firstLab.cpp

all: $(TARGETS)

firstLab: firstLab.cpp
	$(CXX) $(CXXFLAGS) firstLab.cpp -o firstLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: firstLab
	./firstLab


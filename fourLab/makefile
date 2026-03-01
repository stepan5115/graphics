CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGL -lGLU -lglut
TARGETS = fourLab
SOURCES = fourLab.cpp

all: $(TARGETS)

fourLab: fourLab.cpp
	$(CXX) $(CXXFLAGS) fourLab.cpp -o fourLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: fourLab
	./fourLab


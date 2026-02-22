CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGL -lGLU -lglut
TARGETS = secondLab
SOURCES = secondLab.cpp

all: $(TARGETS)

secondLab: secondLab.cpp
	$(CXX) $(CXXFLAGS) secondLab.cpp -o secondLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: secondLab
	./secondLab


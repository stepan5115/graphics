CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGL -lGLU -lglut
TARGETS = thirdLab
SOURCES = thirdLab.cpp

all: $(TARGETS)

thirdLab: thirdLab.cpp
	$(CXX) $(CXXFLAGS) thirdLab.cpp -o thirdLab $(LDFLAGS)

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *~

run: thirdLab
	./thirdLab


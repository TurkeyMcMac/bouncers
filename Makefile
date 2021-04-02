exe = bouncers
CPPFLAGS = `sdl2-config --cflags`
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic
LDLIBS = `sdl2-config --libs` -latomic -lpthread

CXX ?= c++
RM ?= rm -f

$(exe): src/*
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ src/*.cpp $(LDLIBS)

.PHONY: fmt
fmt:
	clang-format -i src/*

.PHONY: clean
clean:
	$(RM) $(exe)

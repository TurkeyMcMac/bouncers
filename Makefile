exe = bouncers
optimization = -O3 -flto -march=native
CPPFLAGS = `sdl2-config --cflags`
CXXFLAGS = $(optimization) -std=c++11 -Wall -Wextra -pedantic
LDLIBS = `sdl2-config --libs` -lpthread

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

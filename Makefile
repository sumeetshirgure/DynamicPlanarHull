CXX=g++
CXXFLAGS=-std=c++17 -g

.PHONY: all clean

all: bin/test_convex_hull bin/test_dynamic_hull

bin/test_convex_hull: tests/ConvexHull.cc include/Point.hh include/ConvexHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/ConvexHull.cc

bin/test_dynamic_hull: tests/DynamicHull.cc include/Point.hh include/DynamicHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/DynamicHull.cc

clean :
	rm -f bin/*

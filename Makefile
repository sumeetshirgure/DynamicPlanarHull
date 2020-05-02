CXX=g++
CXXFLAGS=-std=c++11 -g

.PHONY: all clean

all: bin bin/test_convex_hull bin/test_dynamic_hull bin/test_monotone_hull

bin:
	mkdir -p ./bin

bin/test_convex_hull: tests/ConvexHull.cc include/Point.hh include/ConvexHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/ConvexHull.cc

bin/test_monotone_hull: tests/MonotoneHull.cc include/Point.hh include/ConvexHull.hh  include/MonotoneHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/MonotoneHull.cc

bin/test_dynamic_hull: tests/DynamicHull.cc include/Point.hh include/DynamicHull.hh include/Tangent.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/DynamicHull.cc

clean :
	rm -f bin/*

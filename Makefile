CXX=g++
CXXFLAGS=-std=c++17 -g

.PHONY: all clean

all: bin bin/test_convex_hull bin/test_online_hull bin/test_monotone_hull bin/test_perf

bin:
	mkdir -p ./bin

bin/test_perf: tests/perf/AddPoint.cc include/Point.hh include/OnlineHull.hh include/TestGenerator.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/AddPoint.cc

bin/test_convex_hull: tests/val/ConvexHull.cc include/Point.hh include/ConvexHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/ConvexHull.cc

bin/test_monotone_hull: tests/val/MonotoneHull.cc include/Point.hh include/ConvexHull.hh  include/MonotoneHull.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/MonotoneHull.cc

bin/test_online_hull: tests/val/OnlineHull.cc include/Point.hh include/OnlineHull.hh include/Tangent.hh
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/OnlineHull.cc

clean :
	rm -f bin/*

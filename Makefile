CXX=g++
CXXFLAGS=-std=c++17 -g

.PHONY: all clean

all: bin bin/test_convex_hull bin/test_online_hull bin/test_perf

bin:
	mkdir -p ./bin

bin/test_perf:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/AddPoint.cc

bin/test_convex_hull:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/ConvexHull.cc

bin/test_online_hull:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/OnlineHull.cc

clean :
	rm -f bin/*

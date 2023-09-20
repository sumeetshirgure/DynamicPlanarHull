CXX=g++
CXXFLAGS=-std=c++20 -g

.PHONY: all clean

all: DIR bin/test_convex_hull bin/test_online_hull bin/online/test_perf bin/dynamic/test_perf

DIR:
	mkdir -p ./bin
	mkdir -p ./bin/online ./bin/dynamic

bin/online/test_perf:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/online/AddPoint.cc

bin/dynamic/test_perf:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/dynamic/AddPoint.cc

bin/test_convex_hull:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/ConvexHull.cc

bin/test_online_hull:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/OnlineHull.cc

clean :
	rm -rvf bin/*

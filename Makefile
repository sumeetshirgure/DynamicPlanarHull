CXX=g++
CXXFLAGS=-std=c++20 -g

.PHONY: all clean

all: DIR bin/val_convex_hull bin/online/val bin/dynamic/val bin/online/test_perf bin/dynamic/test_perf

DIR:
	mkdir -p ./bin
	mkdir -p ./bin/online ./bin/dynamic

bin/online/test_perf:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/online/AddPoint.cc

bin/dynamic/test_perf:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/dynamic/AddRemovePoint.cc

bin/val_convex_hull:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/ConvexHull.cc

bin/online/val:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/OnlineHull.cc

bin/dynamic/val:
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/DynamicHull.cc

clean :
	rm -rvf bin/*

CXX=g++
CXXFLAGS=-std=c++20 -O3

.PHONY: all clean

all: DIR bin/online/val bin/dynamic/val bin/online/perf bin/dynamic/perf

DIR:
	mkdir -p ./bin
	mkdir -p ./bin/online ./bin/dynamic

bin/online/perf: DIR tests/perf/OnlineHull.cc
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/OnlineHull.cc

bin/online/val: DIR tests/val/OnlineHull.cc
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/OnlineHull.cc

bin/dynamic/perf: DIR tests/perf/DynamicHull.cc
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/perf/DynamicHull.cc

bin/dynamic/val: DIR tests/val/DynamicHull.cc
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ tests/val/DynamicHull.cc

clean :
	rm -rvf bin/*
	rmdir bin


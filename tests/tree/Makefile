
all: hmt it

it: interval_treap.cc
	g++ -g -std=c++20 interval_treap.cc -o it

hmt: hull_merge_test.cc HullTree.hh MergeableLowerHull.hh MergeableUpperHull.hh
	g++ -g -std=c++20 hull_merge_test.cc -o hmt

clean :
	rm -f it hmt

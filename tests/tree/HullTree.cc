#include "HullTree.hh"


#include <iostream>
using namespace std;

#include "Point.hh"

#include "MergeableHull.hh"

int main() {

  using lower_hull_t = MergeableLowerHull<int>;
  lower_hull_t left_hull, right_hull;
  for(int i = 0; i < 5; i++) {
    lower_hull_t::join(left_hull, left_hull, lower_hull_t(
          LineSegment(Point<int>{i-3, (i-3)*(i-3)}, Point<int>{i-2, (i-2)*(i-2)})));
  }
  for(int i = 5; i < 10; i++) {
    lower_hull_t::join(right_hull, right_hull, lower_hull_t(
          LineSegment(Point<int>{i, (i-8)*(i-8) - 9}, Point<int>{i+1, (i-7)*(i-7) - 9})));
  }

  for(auto seg: left_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  for(auto seg: right_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  cout << is_convex(left_hull) << endl;
  cout << is_convex(right_hull) << endl;

  lower_hull_t merged, left_residue, right_residue;

  auto bridge = merge<>(merged, left_hull, right_hull, left_residue, right_residue);

  for(auto it: merged) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: left_residue) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: right_residue) cerr << to_string(it) << " ";cerr << endl;

  cout << is_convex(merged) << endl;
}

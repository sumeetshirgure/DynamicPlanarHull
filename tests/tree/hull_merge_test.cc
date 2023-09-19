#include "HullTree.hh"


#include <iostream>
using namespace std;

#include "Point.hh"

#include "MergeableLowerHull.hh"
#include "MergeableUpperHull.hh"


int test_lower() {
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

  auto bridge = merge_lower_hulls<>(merged, left_hull, right_hull, left_residue, right_residue);

  for(auto it: merged) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: left_residue) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: right_residue) cerr << to_string(it) << " ";cerr << endl;

  cout << is_convex(merged) << endl;

  return 0;
}

int test_upper() {
  using upper_hull_t = MergeableUpperHull<int>;
  upper_hull_t left_hull, right_hull;

  for(int i = 9; i >= 5; i--) {
    upper_hull_t::join(left_hull, left_hull, upper_hull_t(
          LineSegment(
            Point<int>{-i-1, 9-(i-7)*(i-7)},
            Point<int>{-i, 9 -(i-8)*(i-8) }
            )
          )
        );
  }
  for(int i = 4; i >= 0; i--) {
    upper_hull_t::join(right_hull, right_hull, upper_hull_t(
          LineSegment(
            Point<int>{2-i, -(i-2)*(i-2)},
            Point<int>{-i+3, -(i-3)*(i-3)}
            )
          )
        );
  }

  for(auto seg: left_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  for(auto seg: right_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  cout << is_concave(left_hull) << endl;
  cout << is_concave(right_hull) << endl;

  upper_hull_t merged, left_residue, right_residue;


  auto bridge = merge_upper_hulls<>(merged, left_hull, right_hull, left_residue, right_residue);

  for(auto it: merged) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: left_residue) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: right_residue) cerr << to_string(it) << " ";cerr << endl;

  cout << is_concave(merged) << endl;

  return 0;
}


int test_upper_2() {
  using upper_hull_t = MergeableUpperHull<int>;
  upper_hull_t left_hull, right_hull;

  for(int i = 9; i >= 5; i--) {
    upper_hull_t::join(left_hull, left_hull, upper_hull_t(
          LineSegment(
            Point<int>{-i-1, 9-(i-7)*(i-7)},
            Point<int>{-i, 9 -(i-8)*(i-8) }
            )
          )
        );
  }
  for(int i = 4; i >= 0; i--) {
    upper_hull_t::join(right_hull, right_hull, upper_hull_t(
          LineSegment(
            Point<int>{2-i, 23-(i-2)*(i-2)},
            Point<int>{-i+3, 23-(i-3)*(i-3)}
            )
          )
        );
  }

  for(auto seg: left_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  for(auto seg: right_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  cout << is_concave(left_hull) << endl;
  cout << is_concave(right_hull) << endl;

  upper_hull_t merged, left_residue, right_residue;


  auto bridge = merge_upper_hulls<>(merged, left_hull, right_hull, left_residue, right_residue);

  for(auto it: merged) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: left_residue) cerr << to_string(it) << " ";cerr << endl;
  for(auto it: right_residue) cerr << to_string(it) << " ";cerr << endl;

  cout << is_concave(merged) << endl;

  return 0;
}


int main() {

  test_lower();
  test_upper();
  test_upper_2();

  return 0;
}

#include "HullTree.hh"


#include <iostream>
using namespace std;

#include "Point.hh"

template<typename Field>
struct LineSegment {
  Point<Field> u, v;
  LineSegment(Point<Field> const&_u) : u(_u), v(_u) { }
  LineSegment(Point<Field> const&_u, Point<Field> const&_v) : u(_u), v(_v) { }
};

int main() {

  using hull_t = HullTree<LineSegment<int>>;
  hull_t hull;
  for(int i = 0; i < 5; i++) {
    hull_t::join(hull, hull, hull_t(
          LineSegment(Point<int>{i, i*i}, Point<int>{i+1, i*i+2*i+1})));
  }

  for(auto it = hull.begin(); it!=hull.end(); ++it) {
    cerr << "[" << to_string(it->u) << " -- " << to_string(it->v) << "]" << endl;
  }

}

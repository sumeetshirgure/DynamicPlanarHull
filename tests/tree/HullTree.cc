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

template<typename T> std::string to_string(const LineSegment<T>&p)
{ return "[" + to_string(p.u) + " -- " + to_string(p.v) + "]"; };

template<typename Field>
class MergeableLowerHull : public HullTree<LineSegment<Field>> {
  friend LineSegment<Field>
    find_bridge<>(MergeableLowerHull const& left, MergeableLowerHull const& right);

  friend LineSegment<Field> merge<>( MergeableLowerHull &merged,
        MergeableLowerHull left, MergeableLowerHull right,
        MergeableLowerHull &left_residual, MergeableLowerHull &right_residual);

  friend void split<>(LineSegment<Field> bridge, MergeableLowerHull merged,
      MergeableLowerHull &left, MergeableLowerHull &right,
      MergeableLowerHull left_residual, MergeableLowerHull right_residual);
};



template<typename Field>
LineSegment<Field> find_bridge(
    MergeableLowerHull<Field> const& left, MergeableLowerHull<Field> const& right) {
  auto lpt = left.treap, rpt = right.treap;
  auto split_x = right.begin()->u.x;
  LineSegment<Field> left_cur = lpt->element, right_cur = rpt->element;

  // Thank you J.W. Gibbs
  auto ccw = [](Point<Field> const& pivot, Point<Field> const&first, Point<Field> const& second)
  { return ((first - pivot) ^ (second - pivot)) >= 0; };
  auto cw = [](Point<Field> const& pivot, Point<Field> const&first, Point<Field> const& second)
  { return ((first - pivot) ^ (second - pivot)) <= 0; };

  auto lseg = left_cur.u != left_cur.v, rseg = right_cur.u != right_cur.v;
  while( lseg or rseg ) {
    if( not lseg ) { // rseg
      if( ccw(left_cur.v, right_cur.u, right_cur.v) ) {
        rpt = rpt->left;
        if( rpt == nullptr ) right_cur.v = right_cur.u; else right_cur = rpt->element;
      } else {
        rpt = rpt->right;
        if( rpt == nullptr ) right_cur.u = right_cur.v; else right_cur = rpt->element;
      }
    } else if( not rseg ) { // lseg
      if( cw(right_cur.u, left_cur.u, left_cur.v) ) {
        lpt = lpt->left;
        if( lpt == nullptr ) left_cur.v = left_cur.u; else left_cur = lpt->element;
      } else {
        lpt = lpt->right;
        if( lpt == nullptr ) left_cur.u = left_cur.v; else left_cur = lpt->element;
      }
    } else { // lseg and rseg
      if( (right_cur.v-right_cur.u) ^ (left_cur.v - left_cur.u) <= 0 ) {
        lpt = lpt->left;
        if( lpt == nullptr ) left_cur.v = left_cur.u; else left_cur = lpt->element;
        rpt = rpt->right;
        if( rpt == nullptr ) right_cur.u = right_cur.v; else right_cur = rpt->element;
      } else { // check duality on the line x = split_x

      }
    }
    lseg = left_cur.u != left_cur.v, rseg = right_cur.u != right_cur.v;
  }
  return LineSegment(left_cur.u, right_cur.u);
}

template<typename Field>
LineSegment<Field> merge(MergeableLowerHull<Field> &merged,
    MergeableLowerHull<Field> left, MergeableLowerHull<Field> right,
    MergeableLowerHull<Field> &left_residual, MergeableLowerHull<Field> &right_residual) {
  auto segment = find_bridge(left, right);
  auto leftcut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return it->v > segment.u; };
  cut(leftcut, left, left, left_residual);
  auto rightcut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return it->v > segment.v; };
  cut(rightcut, right, right_residual, right);
  join(merged, left, MergeableLowerHull<Field>(segment));
  join(merged, merged, right);
  return segment;
}


template<typename Field>
void split(LineSegment<Field> segment, MergeableLowerHull<Field> merged,
    MergeableLowerHull<Field> &left, MergeableLowerHull<Field> &right,
    MergeableLowerHull<Field> left_residual, MergeableLowerHull<Field> right_residual) {
  auto leftbridgecut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return it->v > segment.u; };
  auto rightbridgecut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return it->v > segment.v; };
  MergeableLowerHull<Field> bridge;
  cut(leftbridgecut, merged, left, right);
  cut(rightbridgecut, right, bridge, right);
  bridge.erase();
  join(left, left, left_residual);
  join(right, right_residual, right);
  return;
}



int main() {

  using lower_hull_t = MergeableLowerHull<int>;
  lower_hull_t left_hull, right_hull;
  for(int i = 0; i < 5; i++) {
    lower_hull_t::join(left_hull, left_hull, lower_hull_t(
          LineSegment(Point<int>{i-3, (i-3)*(i-3)}, Point<int>{i-2, (i-2)*(i-2)})));
  }
  for(int i = 15; i < 20; i++) {
    lower_hull_t::join(right_hull, right_hull, lower_hull_t(
          LineSegment(Point<int>{i-8, (i-8)*(i-8)}, Point<int>{i-7, (i-7)*(i-7)})));
  }

  for(auto seg: left_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  for(auto seg: right_hull) {
    cerr << "[" << to_string(seg.u) << " -- " << to_string(seg.v) << "]" << endl;
  }

  auto bridge = find_bridge<int>(left_hull, right_hull);
  cerr << "[" << to_string(bridge.u) << " -- " << to_string(bridge.v) << "]" << endl;

}

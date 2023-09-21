#pragma once

#include "util/Point.hh"
#include "util/LineSegment.hh"

#include "dynamic/HullTree.hh"

template<typename Field>
class MergeableLowerHull : public HullTree<LineSegment<Field>> {
  friend LineSegment<Field>
    find_lower_bridge<>(MergeableLowerHull const& left, MergeableLowerHull const& right);

  friend LineSegment<Field> merge_lower_hulls<>( MergeableLowerHull &merged,
      MergeableLowerHull left, MergeableLowerHull right,
      MergeableLowerHull &left_residual, MergeableLowerHull &right_residual);

  friend void split_lower_hulls<>(LineSegment<Field> const& bridge, MergeableLowerHull &merged,
      MergeableLowerHull &left, MergeableLowerHull &right,
      MergeableLowerHull left_residual, MergeableLowerHull right_residual);
};



template<typename Field>
LineSegment<Field> find_lower_bridge(
    MergeableLowerHull<Field> const& left, MergeableLowerHull<Field> const& right) {
  auto lpt = left.treap, rpt = right.treap;
  auto split_x = right.begin()->u.x;
  LineSegment<Field> left_cur = lpt->element, right_cur = rpt->element;

  auto cw = [](Point<Field> const& pivot,
      Point<Field> const&first, Point<Field> const& second)
  { return ((first - pivot) * (second - pivot)) <= 0; };

  auto lseg = left_cur.u != left_cur.v, rseg = right_cur.u != right_cur.v;
  while( lseg or rseg ) {
    if( lseg and cw(left_cur.u, left_cur.v, right_cur.u) ) {
      lpt = lpt->left;
      if( lpt == nullptr ) left_cur.v = left_cur.u; else left_cur = lpt->element;
    } else if( rseg and cw(left_cur.v, right_cur.u, right_cur.v) ) {
      rpt = rpt->right;
      if( rpt == nullptr ) right_cur.u = right_cur.v; else right_cur = rpt->element;
    } else if ( not lseg ) { // => ccw0(lv, ru, rv);
      rpt = rpt->left;
      if( rpt == nullptr ) right_cur.v = right_cur.u; else right_cur = rpt->element;
    } else if ( not rseg ) { // => ccw0(lu, lv, ru);
      lpt = lpt->right;
      if( lpt == nullptr ) left_cur.u = left_cur.v; else left_cur = lpt->element;
    } else {
      auto dl  = left_cur.v - left_cur.u, dr = right_cur.u - right_cur.v;
      auto tlx = (split_x - left_cur.u.x), trx = (split_x - right_cur.v.x);
      // if( left_cur.u.y + tlx * dl.y / dl.x <= right_cur.v.y +trx * dr.y / dr.x )
      auto lhs = dr.x * (dl.x * left_cur.u.y + tlx * dl.y),
           rhs = dl.x * (dr.x * right_cur.v.y + trx * dr.y);
      if( dr.x * dl.x <= 0 ) lhs = -lhs, rhs = -rhs;
      if( lhs <= rhs ) {
        lpt = lpt->right;
        if( lpt == nullptr ) left_cur.u = left_cur.v; else left_cur = lpt->element;
      } else {
        rpt = rpt->left;
        if( rpt == nullptr ) right_cur.v = right_cur.u; else right_cur = rpt->element;
      }
    }
    lseg = left_cur.u != left_cur.v, rseg = right_cur.u != right_cur.v;
  }
  return LineSegment(left_cur.u, right_cur.u);
}

template<typename Field>
LineSegment<Field> merge_lower_hulls(MergeableLowerHull<Field> &merged,
    MergeableLowerHull<Field> left, MergeableLowerHull<Field> right,
    MergeableLowerHull<Field> &left_residual, MergeableLowerHull<Field> &right_residual) {
  auto segment = find_lower_bridge(left, right);
  auto leftcut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return segment.u < it->v; };
  MergeableLowerHull<Field>::cut(leftcut, left, left, left_residual);
  auto rightcut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return segment.v < it->v; };
  MergeableLowerHull<Field>::cut(rightcut, right, right_residual, right);
  MergeableLowerHull<Field>::join(merged, left, MergeableLowerHull<Field>(segment));
  MergeableLowerHull<Field>::join(merged, merged, right);
  return segment;
}


template<typename Field>
void split_lower_hulls(LineSegment<Field> const& segment, MergeableLowerHull<Field> &merged,
    MergeableLowerHull<Field> &left, MergeableLowerHull<Field> &right,
    MergeableLowerHull<Field> left_residual, MergeableLowerHull<Field> right_residual) {
  auto leftbridgecut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return segment.u < it->v; };
  auto rightbridgecut = [&segment](MergeableLowerHull<Field>::iterator const& it) -> bool
  { return segment.v < it->v; };
  MergeableLowerHull<Field> bridge;
  MergeableLowerHull<Field>::cut(leftbridgecut, merged, left, right);
  MergeableLowerHull<Field>::cut(rightbridgecut, right, bridge, right);
  MergeableLowerHull<Field>::join(left, left, left_residual);
  MergeableLowerHull<Field>::join(right, right_residual, right);
  return;
}

template<typename Field>
bool is_convex(MergeableLowerHull<Field> const& seq) {
  if( seq.begin() == seq.end() ) return true;
  for(auto it = seq.begin();; ++it) {
    auto jt = it; ++jt;
    if( jt == seq.end() ) break;
    auto u = it->u, v = it->v, w = jt->v;
    if( ((v-u)*(w-u)) < 0 ) return false;
  }
  return true;
};



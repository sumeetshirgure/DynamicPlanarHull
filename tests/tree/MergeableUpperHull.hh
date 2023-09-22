#pragma once

#include "Point.hh"
#include "LineSegment.hh"

template<typename Field>
class MergeableUpperHull : public HullTree<LineSegment<Field>> {
  friend LineSegment<Field>
    find_upper_bridge<>(MergeableUpperHull const& left, MergeableUpperHull const& right);

  friend LineSegment<Field> merge_upper_hulls<>( MergeableUpperHull &merged,
      MergeableUpperHull left, MergeableUpperHull right,
      MergeableUpperHull &left_residual, MergeableUpperHull &right_residual);

  friend void split_upper_hulls<>(LineSegment<Field> bridge, MergeableUpperHull merged,
      MergeableUpperHull &left, MergeableUpperHull &right,
      MergeableUpperHull left_residual, MergeableUpperHull right_residual);
};



template<typename Field>
LineSegment<Field> find_upper_bridge(
    MergeableUpperHull<Field> const& left, MergeableUpperHull<Field> const& right) {
  auto lpt = left.treap, rpt = right.treap;
  auto split_x = right.begin()->u.x;
  LineSegment<Field> left_cur = lpt->element, right_cur = rpt->element;

  auto ccw = [](Point<Field> const& pivot,
      Point<Field> const&first, Point<Field> const& second)
  { return ((first - pivot) * (second - pivot)) >= 0; };

  auto lseg = left_cur.u != left_cur.v, rseg = right_cur.u != right_cur.v;
  while( lseg or rseg ) {
    if( lseg and ccw(left_cur.u, left_cur.v, right_cur.u) ) {
      lpt = lpt->left;
      if( lpt == nullptr ) left_cur.v = left_cur.u; else left_cur = lpt->element;
    } else if( rseg and ccw(left_cur.v, right_cur.u, right_cur.v) ) {
      rpt = rpt->right;
      if( rpt == nullptr ) right_cur.u = right_cur.v; else right_cur = rpt->element;
    } else if ( not lseg ) { // => cw0(lv, ru, rv);
      rpt = rpt->left;
      if( rpt == nullptr ) right_cur.v = right_cur.u; else right_cur = rpt->element;
    } else if ( not rseg ) { // => cw0(lu, lv, ru);
      lpt = lpt->right;
      if( lpt == nullptr ) left_cur.u = left_cur.v; else left_cur = lpt->element;
    } else {
      auto dl  = left_cur.v - left_cur.u, dr = right_cur.u - right_cur.v;
      auto tlx = (split_x - left_cur.u.x), trx = (split_x - right_cur.v.x);
      // if( left_cur.u.y + tlx * dl.y / dl.x <= right_cur.v.y +trx * dr.y / dr.x )
      auto lhs = dr.x * (dl.x * left_cur.u.y + tlx * dl.y),
           rhs = dl.x * (dr.x * right_cur.v.y + trx * dr.y);
      if( dr.x * dl.x <= 0 ) lhs = -lhs, rhs = -rhs;
      if( lhs >= rhs ) {
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
LineSegment<Field> merge_upper_hulls(MergeableUpperHull<Field> &merged,
    MergeableUpperHull<Field> left, MergeableUpperHull<Field> right,
    MergeableUpperHull<Field> &left_residual, MergeableUpperHull<Field> &right_residual) {
  auto segment = find_upper_bridge<>(left, right);
  auto leftcut = [&segment](MergeableUpperHull<Field>::iterator const& it) -> bool
  { return segment.u < it->v; };
  MergeableUpperHull<Field>::cut(leftcut, left, left, left_residual);
  auto rightcut = [&segment](MergeableUpperHull<Field>::iterator const& it) -> bool
  { return segment.v < it->v; };
  MergeableUpperHull<Field>::cut(rightcut, right, right_residual, right);
  MergeableUpperHull<Field>::join(merged, left, MergeableUpperHull<Field>(segment));
  MergeableUpperHull<Field>::join(merged, merged, right);
  return segment;
}


template<typename Field>
void split_upper_hulls(LineSegment<Field> segment, MergeableUpperHull<Field> merged,
    MergeableUpperHull<Field> &left, MergeableUpperHull<Field> &right,
    MergeableUpperHull<Field> left_residual, MergeableUpperHull<Field> right_residual) {
  auto leftbridgecut = [&segment](MergeableUpperHull<Field>::iterator const& it) -> bool
  { return it->v > segment.u; };
  auto rightbridgecut = [&segment](MergeableUpperHull<Field>::iterator const& it) -> bool
  { return it->v > segment.v; };
  MergeableUpperHull<Field> bridge;
  cut(leftbridgecut, merged, left, right);
  cut(rightbridgecut, right, bridge, right);
  bridge.erase();
  join(left, left, left_residual);
  join(right, right_residual, right);
  return;
}

template<typename Field>
bool is_concave(MergeableUpperHull<Field> const& seq) {
  if( seq.begin() == seq.end() ) return true;
  for(auto it = seq.begin();; ++it) {
    auto jt = it; ++jt;
    if( jt == seq.end() ) break;
    auto u = it->u, v = it->v, w = jt->v;
    if( ((v-u)*(w-u)) > 0 ) return false;
  }
  return true;
};



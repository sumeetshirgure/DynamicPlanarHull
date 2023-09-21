#pragma once
#include "Point.hh"

template<typename Field>
struct LineSegment {
  Point<Field> u, v;
  LineSegment() { }
  LineSegment(Point<Field> const&_u) : u(_u), v(_u) { }
  LineSegment(Point<Field> const&_u, Point<Field> const&_v) : u(_u), v(_v) { }
};

template<typename T> std::string to_string(const LineSegment<T>&p)
{ return "[" + to_string(p.u) + " -- " + to_string(p.v) + "]"; };



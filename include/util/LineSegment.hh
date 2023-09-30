#pragma once
#include "Point.hh"

template<typename Field>
struct LineSegment {
  Point<Field> u, v;
  LineSegment() { }
  LineSegment(Point<Field> const&_u) : u(_u), v(_u) { }
  LineSegment(Point<Field> const&_u, Point<Field> const&_v) : u(_u), v(_v) { }
  bool operator==(LineSegment<Field> const&);
  LineSegment operator~();
};

template<typename T> std::string to_string(const LineSegment<T>&p)
{ return "[" + to_string(p.u) + " -- " + to_string(p.v) + "]"; };

template<typename T> LineSegment<T> LineSegment<T>::operator~()
{ return LineSegment<T>(-v, -u); }

template<typename T> bool LineSegment<T>::operator==(LineSegment<T> const&s)
{ return u == s.u and v == s.v; }

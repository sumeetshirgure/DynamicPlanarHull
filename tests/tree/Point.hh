#pragma once

#include <cmath>
#include <string>

template<typename T> struct Point {
  T x, y;
  Point() : x(0), y(0) {}
  Point(const T&_x, const T&_y) : x(_x), y(_y) {}

  inline T norm2() const& { return x*x+y*y; }
  inline auto norm() -> decltype(sqrt(norm2())) {return sqrt(norm2());}
  inline auto arg()  -> decltype(atan2(y, x))   {return atan2(y, x);}
};

/* Vector sums. */
template<typename T> Point<T> operator+(const Point<T>&p, const Point<T>&q)
{ return Point<T>(p.x+q.x, p.y+q.y); }

template<typename T> Point<T> operator-(const Point<T>&p, const Point<T>&q)
{ return Point<T>(p.x-q.x, p.y-q.y); }

/* Scalar products. */
template<typename T, typename F> Point<T> operator*(const Point<T>&p, const F&k)
{ return Point<T>(k*p.x,k*p.y); }
template<typename T, typename F> Point<T> operator*(const F&k, const Point<T>&p)
{ return Point<T>(k*p.x,k*p.y); }
template<typename T, typename F> Point<T> operator/(const Point<T>&p, const F&k)
{ return Point<T>(p.x/k,p.y/k); }

/* Cross product. */
template<typename T> T operator*(const Point<T>&p, const Point<T>&q)
{ return p.x*q.y - q.x*p.y; }

/* Dot product. */
template<typename T> T operator^(const Point<T>&p, const Point<T>&q)
{ return p.x*q.x + p.y*q.y; }

/* Comparators. */
template<typename T> inline bool operator< (const Point<T>&p, const Point<T>&q)
{ return p.x != q.x ? p.x < q.x : p.y < q.y; }

// operator== useful only for integers. use norm(p-q) < eps instead
template<typename T> inline bool operator==(const Point<T>&p, const Point<T>&q)
{ return (p.x == q.x) and (p.y == q.y); }

template<typename T> std::string to_string(const Point<T>&p)
{ return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")"; };

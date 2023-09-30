#pragma once

#include <cassert>
#include <algorithm>
#include <vector>

using std::vector;
using std::pair;

#include <dpch/util/Point.hh>

namespace dpch {

  template<typename Field> std::pair< bool, std::pair< Point<Field>, Point<Field> > > get_tangents(
      Point<Field> const& point, std::vector< Point<Field> > const& convex_polygon) {
    size_t n = convex_polygon.size();
    assert( n >= 3 );

    bool outside = false;
    for(size_t i = 0, j = n - 1; i < n and not outside; j = i++) {
      outside |= ((point - convex_polygon[j]) * (convex_polygon[i] - convex_polygon[j]) >  0);
    }

    std::pair< Point<Field>, Point<Field> > tangents;
    if( not outside ) { return std::make_pair(false , tangents); }

    auto ccw_near = [&point](Point<Field> const&u, Point<Field> const&v) {
      Field cross_product = (v - point) * (u - point);
      return cross_product > 0 or cross_product == 0 and ((v - u) ^ (u - point)) < 0;
    };

    auto ccw_far = [&point](Point<Field> const&u, Point<Field> const&v) {
      Field cross_product = (v - point) * (u - point);
      return cross_product > 0 or cross_product == 0 and ((v - u) ^ (u - point)) > 0;
    };

    tangents.first  = *min_element(convex_polygon.begin(), convex_polygon.end(), ccw_near);

    tangents.second = *max_element(convex_polygon.begin(), convex_polygon.end(), ccw_far);

    if( tangents.second < tangents.first ) std::swap(tangents.first, tangents.second);

    return std::make_pair(true, tangents);
  }

  template<typename Field> std::pair< Point<Field>, Point<Field> > get_extreme_points(
      Point<Field> const& direction, std::vector< Point<Field> > const& convex_polygon) {
    size_t n = convex_polygon.size();
    assert( n >= 3 );

    int found = 1;
    std::pair< Point<Field>, Point<Field> > points;
    points.first = points.second = convex_polygon.front();
    Field max_value = (points.first ^ direction);
    for(size_t j = 0, i = 1; i < n; j = i++) {
      auto dot = (convex_polygon[i] ^ direction);
      if( dot > max_value ) {
        found = 1;
        max_value = dot;
        points.first = points.second = convex_polygon[i];
      } else if( dot == max_value ) {
        found = 2;
        points.second = convex_polygon[i];
      }
    }

    if( points.second < points.first )
      std::swap(points.first, points.second);

    return points;
  }

}; // end namespace dpch

/**
 * This example is illustrative of the API.
 * dpch::DynamicHull and dpch::OnlineHull have similar APIs,
 * except DynamicHull also has a fast real time remove_point implementation.
 */


#include <iostream>
#include <vector>

#include <dpch/util/Point.hh>
#include <dpch/dynamic/DynamicHull.hh>

int main() {

  using Field = int64_t;

  using dpch::DynamicHull;
  using dpch::Point;

  DynamicHull<Field> hull;

  auto points = std::vector<Point<Field>>{
    Point<Field>(0, 0), Point<Field>(3, 0), Point<Field>(0, 4)};
  hull.add_point(points[0]);
  hull.add_point(points[1]);
  hull.add_point(points[2]);

  auto internal_point = Point<Field>(1, 1);
  auto should_be_nothing = hull.get_tangents(internal_point);
  assert(should_be_nothing == std::nullopt);

  auto external_point = Point<Field>(-5, 2);
  auto external_tangents = hull.get_tangents(external_point);
  assert(external_tangents != std::nullopt);
  assert(external_tangents.value().first  == points[0]);
  assert(external_tangents.value().second == points[2]);


  auto direction = Point<Field>(4, 3);
  {
    auto far_points = hull.get_extremal_points(direction);
    assert(far_points.first  == points[2]);
    assert(far_points.second == points[1]);
  }
  {
    auto far_points = hull.get_extremal_points(-direction);
    assert(far_points.first  == points[0]);
    assert(far_points.second == points[0]);
  }

  hull.add_point(Point<Field>(0, -5));

  external_tangents = hull.get_tangents(external_point);
  assert(external_tangents != std::nullopt);
  assert(external_tangents.value().first  == Point<Field>(0, -5));
  assert(external_tangents.value().second == points[2]);

  // hull.remove_point(points[2]);
  // ...and so on...

  std::cout << "All assertions validated" << std::endl;

  return 0;
}

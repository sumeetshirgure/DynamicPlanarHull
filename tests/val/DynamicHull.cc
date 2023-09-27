#include "util/Point.hh"
#include "util/LineSegment.hh"
#include "util/TestGenerator.hh"
#include "util/Tangent.hh"
#include "static/ConvexHull.hh"
#include "dynamic/DynamicHull.hh"

#include <chrono>
#include <cmath>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>


  template<typename T>
void test_extremes(
    Point<T> const& point,
    DynamicHull<T> & dynamic_hull,
    std::vector< Point<T> > const& lower_chain,
    std::vector< Point<T> > const& upper_chain ) {
  if( lower_chain.size() <= 1 ) return;
  if( upper_chain.size() <= 1 ) return;

  std::vector< Point<T> > polygon;
  polygon.insert(polygon.begin(), lower_chain.begin(), lower_chain.end() - 1);
  polygon.insert(polygon.end(), upper_chain.rbegin(), upper_chain.rend() - 1);

  if( polygon.size() < 3 ) return;

  auto [outside, tangents] = get_tangents(point, polygon);
  auto test_inside  = dynamic_hull.point_in_polygon(point);

  auto test_tangents = dynamic_hull.get_tangents(point);

  // std::cerr << polygon.size() << " " << to_string(point) << std::endl;
  // for(auto const&pt: polygon) std::cerr << " " << to_string(pt); std::cerr << std::endl;

  if( outside and test_tangents != std::nullopt ) {
    assert(tangents.first == test_tangents->first);
    assert(tangents.second == test_tangents->second);
  }

  static int const n_points = 1000;
  static double const omega = 2 * acos(-1) / n_points;

  int64_t radius = 1000000;
  for(int i = 0; i < n_points; i++) {
    auto direction = Point< int64_t >(
        radius * cos(i * omega),
        radius * sin(i * omega) );

    auto far_points = get_extreme_points(direction, polygon);
    auto test_far = dynamic_hull.get_extremal_points(direction);
    assert(far_points == test_far);
  }

}

  template<typename T>
void test_val( std::vector< Point<T> > const& points ) {

  static std::default_random_engine random_engine;
  static std::uniform_int_distribution< int64_t >
    random_generator(0, 1000000);

  static auto generate_random_point =
    [] () -> Point< int64_t >  {
      return Point< int64_t >(random_generator(random_engine),
          random_generator(random_engine));
    };

  auto iter = points.begin();
  std::vector< Point<int64_t> > polygon;

  DynamicHull< int64_t > dynamic_hull;

  auto [lower_chain, upper_chain] = convex_hull(polygon, true);

  while( iter != points.end() ) {
    auto const&point = *iter++;

    // auto print_point = [](LineSegment<int64_t> const&seg) { std::cout << "#" << to_string(seg) << "#, "; };
    // dynamic_hull.traverse_upper_hull(print_point); std::cout << "upper done." << std::endl;
    // for(auto pt: upper_chain) std::cout << "@" << to_string(pt) << "@, "; std::cout << "upper done." << std::endl;
    // dynamic_hull.traverse_lower_hull(print_point); std::cout << "lower done." << std::endl;
    // for(auto pt: lower_chain) std::cout << "@" << to_string(pt) << "@, "; std::cout << "lower done." << std::endl;


    polygon.insert(
        std::lower_bound(polygon.begin(), polygon.end(), point), point);
    tie(lower_chain, upper_chain) = convex_hull(polygon, false, false);

    dynamic_hull.add_point(point);

    if( polygon.size() < 3 ) continue;

    test_extremes(generate_random_point(), dynamic_hull, lower_chain, upper_chain);

    std::cout << "(" << std::setw(6) << polygon.size() << "/"
      << std::setw(6) << points.size() << ") ["
      << std::setw(6) << (lower_chain.size() + upper_chain.size() - 2) << " ] "
      << std::setw(6) << dynamic_hull.get_hull_size() << "]\r";

    auto upper_chain_iterator = upper_chain.begin();
    auto check_upper_chain = [&upper_chain_iterator](LineSegment< int64_t > const&seg) { assert(seg.v == *(++upper_chain_iterator)); };
    auto lower_chain_iterator = lower_chain.begin();
    auto check_lower_chain = [&lower_chain_iterator](LineSegment< int64_t > const&seg) { assert(seg.u == *lower_chain_iterator++); };
    dynamic_hull.traverse_lower_hull(check_lower_chain);
    dynamic_hull.traverse_upper_hull(check_upper_chain);

  };

  iter = points.begin();
  while( iter != points.end() ) {

    auto const&point = *iter++;

    polygon.erase(std::find(polygon.begin(), polygon.end(), point));

    tie(lower_chain, upper_chain) = convex_hull(polygon, false, false);
    dynamic_hull.remove_point(point);

    if( polygon.size() < 3 ) continue;

    test_extremes(generate_random_point(), dynamic_hull, lower_chain, upper_chain);

    std::cout << "(" << std::setw(6) << polygon.size() << "/"
      << std::setw(6) << points.size() << ") ["
      << std::setw(6) << (lower_chain.size() + upper_chain.size() - 2) << " ] "
      << std::setw(6) << dynamic_hull.get_hull_size() << "]\r";

    // auto print_point = [](LineSegment<int64_t> const&seg) { std::cout << "#" << to_string(seg) << "#, "; };
    // dynamic_hull.traverse_upper_hull(print_point); std::cout << "upper done." << std::endl;
    // for(auto pt: upper_chain) std::cout << "@" << to_string(pt) << "@, "; std::cout << "upper done." << std::endl;
    // dynamic_hull.traverse_lower_hull(print_point); std::cout << "lower done." << std::endl;
    // for(auto pt: lower_chain) std::cout << "@" << to_string(pt) << "@, "; std::cout << "lower done." << std::endl;

    auto upper_chain_iterator = upper_chain.begin();
    auto check_upper_chain = [&upper_chain_iterator](LineSegment< int64_t > const&seg) { assert(seg.v == *(++upper_chain_iterator)); };
    auto lower_chain_iterator = lower_chain.begin();
    auto check_lower_chain = [&lower_chain_iterator](LineSegment< int64_t > const&seg) { assert(seg.u == *lower_chain_iterator++); };
    dynamic_hull.traverse_lower_hull(check_lower_chain);
    dynamic_hull.traverse_upper_hull(check_upper_chain);
  };

}


int main()
{

  std::vector< size_t > small_sizes = { 5 };
  std::vector< size_t > sizes = {
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
    100, 100, 100, 100, 100,
    500, 500, 500, 500, 500,
    1000, 1000, 1000,
    2000, 2000, 2000,
    2500, 2500, 2500,
  };

  for(size_t n_points: sizes)
  {
    {
      auto random_test = random_int_test<int64_t>(n_points);

      std::cout << "\nrandom test with " << std::setw(6)
        << n_points << " points" << std::endl;
      test_val(random_test);
    }
    {
      std::cout << "\ncircle test with " << std::setw(6)
        << n_points << " points" << std::endl;
      auto random_test = random_circle_int_test<int64_t>(
          n_points, 2 * n_points * (int)(sqrt(n_points)), false);
      test_val(random_test);
    }
    {
      std::cout << "\nspiral test with " << std::setw(6)
        << n_points << " points" << std::endl;
      auto random_test = random_circle_int_test<int64_t>(
          n_points, 2 * n_points * (int)(sqrt(n_points)), true);
      test_val(random_test);
    }
  }
  std::cout << "\nall tests passed" << std::endl;

  return 0;
}

#pragma once

#include "Point.hh"
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

template<typename Integer> std::vector< Point<Integer> > random_int_test(
    size_t n_points, Integer const& range = 1000000) {
  static std::default_random_engine random_engine;
  static std::uniform_int_distribution< Integer >
    random_generator(0, Integer(range));

  static auto generate_random_point = [] () -> Point< Integer >  {
    return Point< Integer >(random_generator(random_engine), random_generator(random_engine));
  };

  std::vector< Point<Integer> > points(n_points);
  std::generate(points.begin(), points.end(), generate_random_point);
  std::sort(points.begin(), points.end());
  points.erase(std::unique(points.begin(), points.end()), points.end());
  auto seed = 42; // std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(points.begin(), points.end(), std::default_random_engine(seed));

  return points;
}

template<typename Integer> std::vector< Point<Integer> >
random_circle_int_test(int32_t n_points, Integer radius, bool spiral = false) {
  double omega = 2 * acos(-1) / n_points;

  std::vector< Point<Integer> > points(n_points);
  for(int i = 0; i < n_points; i++) {
    int radius_i = spiral ? radius / (i+1) : radius;
    points[i] = Point< Integer >(
        radius_i * cos(i * omega),
        radius_i * sin(i * omega)
        );
  }

  std::sort(points.begin(), points.end());
  points.erase(std::unique(points.begin(), points.end()), points.end());
  auto seed = 42; // std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(points.begin(), points.end(), std::default_random_engine(seed));

  return points;
}

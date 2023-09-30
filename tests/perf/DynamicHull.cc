#include <dpch/util/TestGenerator.hh>
#include <dpch/dynamic/DynamicHull.hh>

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <unordered_map>

template<typename Field> void test_perf( std::vector< Point<Field> > points ) {

  auto iter = points.begin();

  DynamicHull< Field > dynamic_hull;

  while( iter != points.end() ) {
    auto const&point = *iter++;
    /* Get hull sizes. */
    auto num_points = dynamic_hull.get_num_points();
    /* Check time. */
    auto tick = std::chrono::high_resolution_clock::now();
    dynamic_hull.add_point(point);
    auto tock = std::chrono::high_resolution_clock::now();
    auto runtime = std::chrono::duration_cast
      <std::chrono::nanoseconds>(tock - tick).count();
    std::cout << num_points << ':' << runtime << '\n';
  };

  std::default_random_engine random_engine;
  std::shuffle(std::begin(points), std::end(points), random_engine);

  iter = points.begin();
  while( iter != points.end() ) {
    auto const&point = *iter++;
    /* Get hull sizes. */
    auto num_points = dynamic_hull.get_num_points();
    /* Check time. */
    auto tick = std::chrono::high_resolution_clock::now();
    dynamic_hull.remove_point(point);
    auto tock = std::chrono::high_resolution_clock::now();
    auto runtime = std::chrono::duration_cast
      <std::chrono::nanoseconds>(tock - tick).count();
    std::cout << num_points << ':' << runtime << '\n';
  };
}

int main(int argc, char* argv[]) {
  int n_points;
  if( argc != 2 ) {
    std::cerr << "No argument provided; going with 10k point additions followed by removal." << std::endl;
    n_points = 10000;
  } else {
    n_points = std::atoi(argv[1]);
  }

  std::vector< Point<int64_t> > points =
    random_circle_int_test<int64_t>(n_points, n_points * (int)(sqrt(n_points)), false);

  test_perf(points);

  return 0;
}

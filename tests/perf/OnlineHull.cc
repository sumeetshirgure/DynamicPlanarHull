#include <dpch/util/Point.hh>
#include <dpch/util/Tangent.hh>
#include <dpch/util/TestGenerator.hh>
#include <dpch/static/ConvexHull.hh>
#include <dpch/online/OnlineHull.hh>


#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <unordered_map>

template<typename Field> void test_perf( std::vector< Point<Field> > const& points ) {
  assert( points.size() > 2 );

  auto iter = points.begin();

  auto const&first_point = *iter++;
  auto const&second_point = *iter++;
  OnlineHull< Field > dynamic_hull(first_point, second_point);

  while( iter != points.end() ) {

    auto const&point = *iter++;

    /* Get hull sizes. */
    auto hull_size = dynamic_hull.get_hull_size();
    /* Check time. */
    auto tick = std::chrono::high_resolution_clock::now();

    dynamic_hull.add_point(point);

    auto tock = std::chrono::high_resolution_clock::now();

    auto runtime = std::chrono::duration_cast
      <std::chrono::nanoseconds>(tock - tick).count();

    std::cout << hull_size << ':' << runtime << '\n';
  };
}

int main(int argc, char* argv[]) {
  int n_points;
  if( argc != 2 ) {
    std::cerr << "No argument provided: going with 10k point additions." << std::endl;
    n_points = 10000;
  } else {
    n_points = std::atoi(argv[1]);
  }

  std::vector< Point<int64_t> > points = random_circle_int_test<int64_t>(n_points, n_points * (int)(sqrt(n_points)), false);

  test_perf(points);

  return 0;
}

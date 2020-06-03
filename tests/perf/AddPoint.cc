#include "Point.hh"
#include "ConvexHull.hh"
#include "DynamicHull.hh"
#include "Tangent.hh"

#include "TestGenerator.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <unordered_map>

template<typename Field>
void test_perf( std::vector< Point<Field> > const& points )
{
	assert( points.size() > 2 );

	auto iter = points.begin();

	auto const&first_point = *iter++;
	auto const&second_point = *iter++;
	DynamicHull< Field > dynamic_hull(first_point, second_point);

	while( iter != points.end() )
	{

		auto const&point = *iter++;

		/* Get hull sizes. */
		auto lower_hull_size = dynamic_hull.get_lower_hull_size();
		auto upper_hull_size = dynamic_hull.get_upper_hull_size();

		/* Check time. */
		auto tick = std::chrono::high_resolution_clock::now();

		dynamic_hull.add_point(point);

		auto tock = std::chrono::high_resolution_clock::now();

		auto runtime = std::chrono::duration_cast
			<std::chrono::nanoseconds>(tock - tick).count();

		std::cout << lower_hull_size + upper_hull_size - 2
			<< ':' << runtime << '\n';
	};
}

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		return -1;
	}

	int n_points = std::atoi(argv[1]);

	std::vector< Point<int64_t> > points =
		random_circle_int_test<int64_t>(
			n_points, 2 * n_points * (int)(sqrt(n_points)), false);

	test_perf(points);

	return 0;
}

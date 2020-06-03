#include "ConvexHull.hh"
#include "MonotoneHull.hh"
#include "TestGenerator.hh"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>

template<typename T>
void test_monotone(std::vector< Point<T> > const& points)
{
	assert( points.size() > 1 );

	std::vector< Point<int64_t> > polygon;
	MonotoneHull<int64_t> monotone_hull;

	for(const auto point: points)
	{
		polygon.emplace_back(point);
		monotone_hull.add_point(point);

		auto [lower_chain, upper_chain] = convex_hull(polygon, true);

		assert( lower_chain == monotone_hull.lower_chain() );
		assert( upper_chain == monotone_hull.upper_chain() );
	}
}

int main()
{

	std::vector< size_t > sizes = {
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
			100, 100, 100, 100, 100,
			500, 500, 500, 500, 500,
			1000, 1000, 1000,
			2000, 2000, 2000 };

	for(size_t n_points: sizes)
	{
		{
			auto random_test = random_int_test<int64_t>(n_points);

			std::cout << "random test with " << std::setw(6)
				<< n_points << " points" << std::endl;
			std::sort(random_test.begin(), random_test.end());
			random_test.erase(
				std::unique(random_test.begin(), random_test.end()),
				random_test.end());
			test_monotone(random_test);
		}
		{
			std::cout << "circle test with " << std::setw(6)
				<< n_points << " points" << std::endl;
			auto random_test = random_circle_int_test<int64_t>(
				n_points, 2 * n_points * (int)(sqrt(n_points)), false);
			std::sort(random_test.begin(), random_test.end());
			random_test.erase(
				std::unique(random_test.begin(), random_test.end()),
				random_test.end());
			test_monotone(random_test);
		}
		{
			std::cout << "spiral test with " << std::setw(6)
				<< n_points << " points" << std::endl;
			auto random_test = random_circle_int_test<int64_t>(
				n_points, 2 * n_points * (int)(sqrt(n_points)), true);
			std::sort(random_test.begin(), random_test.end());
			random_test.erase(
				std::unique(random_test.begin(), random_test.end()),
				random_test.end());
			test_monotone(random_test);
		}
	}
	std::cout << "\nall tests passed" << std::endl;

	return 0;
}
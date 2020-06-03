#include "ConvexHull.hh"
#include "MonotoneHull.hh"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>

void random_test(size_t n_points)
{
	assert( n_points > 1 );

	static std::mt19937_64 random_engine(
		std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution< int64_t > random_generator(0, 1000000);

	static auto generate_random_point =
	[] () -> Point<int64_t>  {
		return Point<int64_t>(random_generator(random_engine),
			random_generator(random_engine));
	};

	std::cout << "random test with " << std::setw(6) << n_points << " points" << std::endl;
	std::vector< Point<int64_t> > points(n_points);
	std::generate(points.begin(), points.end(), generate_random_point);
	std::sort(points.begin(), points.end());

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

	std::vector<int> sizes = { 10, 50, 100, 500, 1000, 2000, 5000 };
	for(int n_points: sizes)
	{
		random_test(n_points);
	}
	std::cout << "tests passed" << std::endl;

	return 0;
}
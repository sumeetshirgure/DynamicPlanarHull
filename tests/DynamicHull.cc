#include "Point.hh"
#include "ConvexHull.hh"
#include "DynamicHull.hh"

#include <chrono>
#include <cmath>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

template<typename T>
void test( std::vector< Point<T> > const& points )
{
	assert( points.size() > 2 );

	auto iter = points.begin();
	std::vector< Point<int64_t> > polygon(2);

	polygon[0] = *iter++;
	polygon[1] = *iter++;
	std::sort(polygon.begin(), polygon.end());

	DynamicHull< int64_t > dynamic_hull(polygon[0], polygon[1]);

	while( iter != points.end() )
	{

		auto const&point = *iter++;

		polygon.insert(
			std::lower_bound(polygon.begin(), polygon.end(), point), point);

		dynamic_hull.add_point(point);

		auto [lower_chain, upper_chain] = convex_hull(polygon, true);

		std::cout << "(" << std::setw(6) << polygon.size() << "/"
			<< std::setw(6) << points.size() << ") ["
			<< std::setw(6) << (lower_chain.size() + upper_chain.size() - 2)
			<< "]\r";

		auto lower_chain_iterator = lower_chain.begin();
		auto upper_chain_iterator = upper_chain.begin();
		auto check_lower_chain =
		[&lower_chain_iterator](Point< int64_t > const&point)
		{
			assert(point == *lower_chain_iterator++);
		};
		auto check_upper_chain =
		[&upper_chain_iterator](Point< int64_t > const&point)
		{ 
			assert(point == *upper_chain_iterator++);
		};

		dynamic_hull.traverse_lower_hull(check_lower_chain);
		assert(lower_chain_iterator == lower_chain.end());
		dynamic_hull.traverse_upper_hull(check_upper_chain);
		assert(upper_chain_iterator == upper_chain.end());

	};

}

void random_test(int n_points)
{

	std::uniform_int_distribution< int64_t > random_generator(0, 1000000);
	std::mt19937_64 random_engine(
		std::chrono::steady_clock::now().time_since_epoch().count());

	auto generate_random_point =
	[&random_generator, &random_engine] () -> Point<int64_t>  {
		return Point<int64_t>(random_generator(random_engine),
			random_generator(random_engine));
	};

	std::cout << "random test with " << std::setw(6) << n_points << " points" << std::endl;
	std::vector< Point<int64_t> > points(n_points);
	std::generate(points.begin(), points.end(), generate_random_point);

	test(points);
}

void circle_test(int n_points)
{
	double omega = 2 * acos(-1) / n_points;

	std::cout << "circle test with " << std::setw(6) << n_points << " points" << std::endl;
	int64_t radius = 1000000;
	std::vector< Point<int64_t> > points(n_points);
	for(int i = 0; i < n_points; i++) {
		points[i] = Point< int64_t >(
			radius * cos(i * omega),
			radius * sin(i * omega)
		);
	}

	std::shuffle(points.begin(), points.end(),
		std::mt19937_64(std::chrono::steady_clock::now().
			time_since_epoch().count()));

	test(points);
}

int main()
{

	std::vector<int> sizes = { 10, 50, 100, 500, 1000, 2000, 5000 };
	for(int n_points: sizes)
	{
		random_test(n_points);
		circle_test(n_points);
	}
	std::cout << "\ntests passed" << std::endl;

	return 0;
}
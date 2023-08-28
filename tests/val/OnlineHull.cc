#include "Point.hh"
#include "ConvexHull.hh"
#include "OnlineHull.hh"
#include "Tangent.hh"
#include "TestGenerator.hh"

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
	OnlineHull<T> & dynamic_hull,
	std::vector< Point<T> > const& lower_chain,
	std::vector< Point<T> > const& upper_chain )
{
	std::vector< Point<T> > polygon;
	polygon.insert(polygon.begin(),
		lower_chain.begin(), lower_chain.end() - 1);
	polygon.insert(polygon.end(),
		upper_chain.rbegin(), upper_chain.rend() - 1);

	if( polygon.size() < 3 )
		return;

	auto [outside, tangents] = get_tangents(point, polygon);
	auto [test_outside, test_tangents] = dynamic_hull.get_tangents(point);

	assert(outside == test_outside);
	if( outside )
	{
		assert(tangents.first == test_tangents.first);
		assert(tangents.second == test_tangents.second);
		// A weaker test would be to check if the three points are collinear.
		// assert((tangents.first - point)*(test_tangents.first - point) == 0);
		// assert((tangents.second - point)*(test_tangents.second - point) == 0);
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
void test_val( std::vector< Point<T> > const& points )
{
	assert( points.size() > 2 );

	auto iter = points.begin();
	std::vector< Point<int64_t> > polygon(2);

	polygon[0] = *iter++;
	polygon[1] = *iter++;
	std::sort(polygon.begin(), polygon.end());

	OnlineHull< int64_t > dynamic_hull(polygon[0], polygon[1]);

	auto [lower_chain, upper_chain] = convex_hull(polygon, true);

	while( iter != points.end() )
	{

		auto const&point = *iter++;

		test_extremes(point, dynamic_hull, lower_chain, upper_chain);

		polygon.insert(
			std::lower_bound(polygon.begin(), polygon.end(), point), point);

		tie(lower_chain, upper_chain) = convex_hull(polygon, true);
		dynamic_hull.add_point(point);

		std::cout << "(" << std::setw(6) << polygon.size() << "/"
			<< std::setw(6) << points.size() << ") ["
			<< std::setw(6) << (lower_chain.size() + upper_chain.size() - 2)
			<< "]\r";

		auto lower_chain_iterator = lower_chain.begin();
		auto upper_chain_iterator = upper_chain.begin();
		auto check_lower_chain =
		[&lower_chain_iterator](Point< int64_t > const&point)
		{ assert(point == *lower_chain_iterator++); };
		auto check_upper_chain =
		[&upper_chain_iterator](Point< int64_t > const&point)
		{ assert(point == *upper_chain_iterator++); };

		dynamic_hull.traverse_lower_hull(check_lower_chain);
		assert(lower_chain_iterator == lower_chain.end());
		dynamic_hull.traverse_upper_hull(check_upper_chain);
		assert(upper_chain_iterator == upper_chain.end());

		assert(dynamic_hull.get_lower_hull_size() == lower_chain.size());
		assert(dynamic_hull.get_upper_hull_size() == upper_chain.size());
	};

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
			test_val(random_test);
		}
		{
			std::cout << "circle test with " << std::setw(6)
				<< n_points << " points" << std::endl;
			auto random_test = random_circle_int_test<int64_t>(
				n_points, 2 * n_points * (int)(sqrt(n_points)), false);
			test_val(random_test);
		}
		{
			std::cout << "spiral test with " << std::setw(6)
				<< n_points << " points" << std::endl;
			auto random_test = random_circle_int_test<int64_t>(
				n_points, 2 * n_points * (int)(sqrt(n_points)), true);
			test_val(random_test);
		}
	}
	std::cout << "\nall tests passed" << std::endl;

	return 0;
}
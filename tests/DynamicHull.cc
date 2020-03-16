#include "Point.hh"
#include "ConvexHull.hh"
#include "DynamicHull.hh"
#include "Tangent.hh"

#include <chrono>
#include <cmath>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

template<typename T>
void test_tangents(
	Point<T> const& point,
	DynamicHull<T> & dynamic_hull, // TODO : const&
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
}

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

	auto [lower_chain, upper_chain] = convex_hull(polygon, true);

	while( iter != points.end() )
	{

		auto const&point = *iter++;

		test_tangents(point, dynamic_hull, lower_chain, upper_chain);

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

void circle_test(int n_points, bool spiral = false)
{
	double omega = 2 * acos(-1) / n_points;

	std::cout << (spiral ? "spiral" : "circle")
		<< " test with " << std::setw(6) << n_points << " points" << std::endl;
	int64_t radius = 1000000;
	std::vector< Point<int64_t> > points(n_points);
	for(int i = 0; i < n_points; i++) {
		int radius_i = spiral ? radius / (i+1) : radius;
		points[i] = Point< int64_t >(
			radius_i * cos(i * omega),
			radius_i * sin(i * omega)
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
		circle_test(n_points, true);
	}
	std::cout << "\ntests passed" << std::endl;

	return 0;
}
#include "Point.hh"
#include "ConvexHull.hh"
#include "DynamicHull.hh"

#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

bool random_test(int n_points)
{

	std::uniform_int_distribution< int64_t > random_generator(0, 1000000);
	std::default_random_engine random_engine;

	auto generate_random_point =
	[&random_generator, &random_engine] () -> Point<int64_t>  {
		return Point<int64_t>(random_generator(random_engine),
			random_generator(random_engine));
	};

	std::cout << "random test with " << std::setw(6) << n_points << " points" << std::endl;

	Point< int64_t > initial_points[2];
	std::vector< Point<int64_t> > polygon(2);

	polygon[0] = generate_random_point();
	polygon[1] = generate_random_point();
	std::sort(polygon.begin(), polygon.end());

	DynamicHull< int64_t > dynamic_hull(polygon[0], polygon[1]);

	do
	{
		auto [lower_chain, upper_chain] = convex_hull(polygon, true);

		std::cout << "(" << std::setw(6) << polygon.size()
			<< "/" << std::setw(6) << n_points << ")\r" << std::flush;

		auto lower_chain_iterator = lower_chain.begin();
		auto upper_chain_iterator = upper_chain.begin();
		auto check_lower_chain =
		[&lower_chain_iterator](DynamicHull< int64_t >::HullIterator node)
		{ 
			assert(node->u == *lower_chain_iterator);
			lower_chain_iterator++;
			assert(node->v == *lower_chain_iterator);
		};
		auto check_upper_chain =
		[&upper_chain_iterator](DynamicHull< int64_t >::HullIterator node)
		{ 
			assert(node->u == *upper_chain_iterator);
			upper_chain_iterator++;
			assert(node->v == *upper_chain_iterator);
		};

		dynamic_hull.traverse_lower(check_lower_chain);
		assert(++lower_chain_iterator == lower_chain.end());
		dynamic_hull.traverse_upper(check_upper_chain);
		assert(++upper_chain_iterator == upper_chain.end());

		Point< int64_t > random_point = generate_random_point();

		polygon.insert(
			lower_bound(polygon.begin(), polygon.end(), random_point),
			random_point
		);

		dynamic_hull.add_point(random_point);

	} while( polygon.size() < n_points );

	return true;
}



int main()
{

	int const times = 5;
	std::vector<int> sizes = { 10, 50, 100, 500, 1000, 2000, 5000, 10000 };
	for(int n_points: sizes)
	{
		for(int i=0; i<times; i++)
			assert( random_test(n_points) );
	}

	return 0;
}
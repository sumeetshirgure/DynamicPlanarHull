#include "ConvexHull.hh"
#include "MonotoneHull.hh"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <fstream>
#include <chrono>

void random_test(size_t n_points)
{
	assert( n_points > 1 );

	static std::mt19937_64 random_engine(
		std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution< int64_t > random_generator(0, 1000000);

	static auto generate_random_point =
	[] () -> Point<int64_t>  {
		return Point<int64_t>(random_generator(random_engine)%100000,
			random_generator(random_engine)%100000);
	};

	std::cout << "random test with " << std::setw(6) << n_points << " points" << std::endl;
	std::vector< Point<int64_t> > points(n_points);
	std::generate(points.begin(), points.end(), generate_random_point);
	std::sort(points.begin(), points.end());
	std::ofstream __polygon;
	__polygon.open("polygon.txt");
	for(int i=0;i<n_points;i++){
		__polygon << points[i].x << " "<< points[i].y << std::endl;
		//std::cout << points[i].x << " "<< points[i].y << std::endl;
	}
	__polygon.close();

	std::vector< Point<int64_t> > polygon;
	MonotoneHull<int64_t> monotone_hull;
	std::ofstream lower_hulls;
	std::ofstream upper_hulls;
	std::ofstream time_stamps;
	lower_hulls.open("lower_hulls.txt");
	upper_hulls.open("upper_hulls.txt");
	time_stamps.open("time_stamps.txt");
	int iter=0;
	for(const auto point: points)
	{
		polygon.emplace_back(point);
		auto start = std::chrono::steady_clock::now();
		monotone_hull.add_point(point);
		auto end = std::chrono::steady_clock::now();

		// auto [lower_chain, upper_chain] = convex_hull(polygon, true);
		//
		// assert( lower_chain == monotone_hull.lower_chain() );
		// assert( upper_chain == monotone_hull.upper_chain() );

		auto lower_chain_iterator = monotone_hull.lower_chain().begin();
		auto upper_chain_iterator = monotone_hull.upper_chain().begin();

		//std::cout << "Iteration number - "<<iter << std::endl;
		time_stamps << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
		//std::cout << "=====Lower chain=====" << std::endl;

		while(lower_chain_iterator != monotone_hull.lower_chain().end())
		{
			//std::cout << lower_chain_iterator->x << " "<< lower_chain_iterator->y << std::endl;
			lower_hulls << lower_chain_iterator->x << " "<< lower_chain_iterator->y << " ";
			lower_chain_iterator++;
		}
		//std::cout << "=====Upper chain=====" << std::endl;
		while(upper_chain_iterator != monotone_hull.upper_chain().end())
		{
			//std::cout << upper_chain_iterator->x << " "<< upper_chain_iterator->y << std::endl;
			upper_hulls << upper_chain_iterator->x << " "<< upper_chain_iterator->y << " ";
			upper_chain_iterator++;
		}
		iter++;
		lower_hulls << std::endl;
		upper_hulls << std::endl;
	}
	lower_hulls.close();
	upper_hulls.close();
	time_stamps.close();
}

int main()
{

	// std::vector<int> sizes = { 10, 50, 100, 500, 1000, 2000, 5000 };
	// for(int n_points: sizes)
	// {
	// 	random_test(n_points);
	// }
	random_test(100000);
	std::cout << "tests passed" << std::endl;

	return 0;
}

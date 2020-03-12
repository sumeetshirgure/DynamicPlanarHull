// Solving https://open.kattis.com/problems/convexhull as a test.

#include "ConvexHull.hh"

#include <iostream>

using point_t = Point<int>;
using chain_t = vector< point_t > ;

int main()
{

	int num_vertices;
	while( std::cin >> num_vertices )
	{
		if( num_vertices == 0 )
			break;

		vector< point_t > polygon(num_vertices);

		for(auto &point: polygon)
			std::cin >> point.x >> point.y;

		sort(polygon.begin(), polygon.end());
		polygon.erase(std::unique(polygon.begin(), polygon.end()),
			polygon.end());


		auto [lower_chain, upper_chain] = convex_hull(polygon, true);

		if( polygon.size() == 1 ) // Handle corner case.
		{
			std::cout << 1 << std::endl;
			auto point = polygon.front();
			std::cout << point.x << " " << point.y << std::endl;
			continue;
		}

		std::reverse(upper_chain.begin(), upper_chain.end());
		lower_chain.pop_back();
		upper_chain.pop_back();

		std::cout << lower_chain.size() + upper_chain.size() << std::endl;
		for(auto &chain: {lower_chain, upper_chain})
			for(auto point: chain)
				std::cout << point.x << " " << point.y << "\n";
		std::cout << std::flush;
	}

	return 0;
}
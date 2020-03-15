#pragma once

#include <cassert>
#include <algorithm>
#include <vector>

using std::vector;
using std::pair;

#include "Point.hh"

template<typename Field>
std::pair< bool, std::pair< Point<Field>, Point<Field> > >
get_tangents(Point<Field> const& point,
	std::vector< Point<Field> > const& convex_polygon)
{
	size_t n = convex_polygon.size();
	assert( n >= 3 );

	bool outside = false;
	for(size_t i = 0, j = n - 1; i < n and not outside; j = i++)
	{
		outside |= ((point - convex_polygon[j]) *
			(convex_polygon[i] - convex_polygon[j]) > 0);
	}

	std::pair< Point<Field>, Point<Field> > tangents;
	if( not outside )
	{
		return std::make_pair(false , tangents);
	}

	auto ccw = [&point](Point<Field> const&u, Point<Field> const&v)
	{
		Field cross_product = (v - point) * (u - point);
		return cross_product > 0 or cross_product == 0 and u < v;
	};

	tangents.first  = *min_element(
		convex_polygon.begin(), convex_polygon.end(), ccw);

	tangents.second = *max_element(
		convex_polygon.begin(), convex_polygon.end(), ccw);

	if( tangents.second < tangents.first )
		std::swap(tangents.first, tangents.second);

	return std::make_pair(true, tangents);
}

#pragma once

#include <cassert>
#include <vector>

#include "Point.hh"

template<typename Field>
class MonotoneHull
{

private :
    std::vector< Point<Field> > upper_hull{}, lower_hull{};

public:

    void add_point(Point<Field> const&);

    std::vector< Point<Field> > const& lower_chain() const {
        return lower_hull;
    }
    std::vector< Point<Field> > const& upper_chain() const {
        return upper_hull;
    }

};

template<typename Field>
void MonotoneHull<Field>::add_point(Point<Field> const& point)
{
    if( lower_hull.empty() )
    {
        lower_hull.emplace_back(point);
        upper_hull.emplace_back(point);
        return;
    }

    assert(lower_hull.back() < point);

    if( lower_hull.size() < 2 )
    {
        lower_hull.emplace_back(point);
        upper_hull.emplace_back(point);
        return;
    }

    while( lower_hull.size() >= 2 )
    {
        auto const& mid  = *(lower_hull.end()-1);
        auto const& left = *(lower_hull.end()-2);
        if( (mid-left) * (point-mid) > 0 )
            break;
        lower_hull.pop_back();
    }
    lower_hull.emplace_back(point);

    while( upper_hull.size() >= 2 )
    {
        auto const& mid  = *(upper_hull.end()-1);
        auto const& left = *(upper_hull.end()-2);
        if( (point-mid) * (mid-left) > 0 )
            break;
        upper_hull.pop_back();
    }
    upper_hull.emplace_back(point);
}
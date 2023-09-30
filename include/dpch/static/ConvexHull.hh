#pragma once

#include <algorithm>
#include <vector>

using std::vector;
using std::pair;

#include <dpch/util/Point.hh>

namespace dpch {
  template<typename T> using Chain = vector< Point<T> >;

  /* Andrew's monotone chain algorithm. Assumes that input points are unique. */
  template<typename T> pair< Chain<T>, Chain<T> > convex_hull
    (vector< Point <T> > polygon, bool sorted = true, bool collinear = false) {

      if( not sorted ) {
        sort(polygon.begin(), polygon.end()); // sort lexicographically 
      }

      if( polygon.size() <= 1 ) {
        return pair<Chain<T>, Chain<T>>{polygon, polygon};
      }

      Chain<T>  lower_chain, upper_chain;

      lower_chain.reserve( polygon.size() / 2 );
      upper_chain.reserve( polygon.size() / 2 );

      for(auto &point: polygon) {
        while( lower_chain.size() >= 2 ) {
          auto first_segment = lower_chain[lower_chain.size()-1] - lower_chain[lower_chain.size()-2];
          auto second_segment = point - lower_chain[lower_chain.size()-1];
          if( (not collinear and first_segment * second_segment <= 0)
              or (collinear and first_segment * second_segment < 0))
            lower_chain.pop_back();
          else break;
        }

        lower_chain.emplace_back(point);

        while( upper_chain.size() >= 2 ) {
          auto first_segment = upper_chain[upper_chain.size()-1] - upper_chain[upper_chain.size()-2];
          auto second_segment = point - upper_chain[upper_chain.size()-1];
          if( (not collinear and first_segment * second_segment >= 0)
              or(collinear and first_segment * second_segment > 0) )
            upper_chain.pop_back();
          else break;
        }

        upper_chain.emplace_back(point);
      }

      return pair<Chain<T>, Chain<T>>{lower_chain, upper_chain};
    }

}; // end namespace dpch

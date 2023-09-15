
#pragma once

#include <cassert>
#include <optional>

template<typename Field>
class DynamicHull {

  private:
    static std::default_random_engine engine;
    static std::uniform_int_distribution< int32_t > rng;

  public :

    using size_t = int32_t;
    using priority_t = int32_t;

    DynamicHull();
    ~DynamicHull();

    bool add_point(Point<Field> const&);
    bool remove_point(Point<Field> const&);

    std::optional< std::pair< std::pair< Point<Field>, Point<Field> > > >
      get_tangents (Point<Field> const&) const;

    std::pair< Point<Field>, Point<Field> >
      get_extremal_points(Point<Field> const&) const;

    template<typename Callback> void traverse_lower_hull(Callback const&) const;
    template<typename Callback> void traverse_upper_hull(Callback const&) const;

    size_t get_lower_hull_size() const;
    size_t get_upper_hull_size() const;
    size_t get_hull_size() const;

    template<typename Callback> void traverse_hull(Callback const&) const;
    template<typename Callback> void traverse_set (Callback const&) const;
};



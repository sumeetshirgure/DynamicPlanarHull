
#include "Point.hh"

template<typename Field>
class OnlineHull
{

private :

public :

	using size_t = int32_t;

	DynamicHull();
	~DynamicHull();

	bool add_point(Point<Field> const&);

	std::pair< bool, std::pair< Point<Field>, Point<Field> > >
    	get_tangents (Point<Field> const&);

	std::pair< Point<Field>, Point<Field> >
	    get_extremal_points(Point<Field> const&);

	template<typename Callback> void traverse_lower_hull(Callback const&);
	template<typename Callback> void traverse_upper_hull(Callback const&);

	size_t get_lower_hull_size() const;
	size_t get_upper_hull_size() const;
	size_t get_hull_size() const;

    size_t get_set_size() const;

    template<typename Callback> void traverse_set (Callback const&);
	template<typename Callback> void traverse_hull(Callback const&);

};
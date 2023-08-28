#pragma once

#include <cassert>
#include <utility>
#include <random>
#include <list>
#include <queue>

#include "Point.hh"

template<typename Field>
class OnlineHull
{

private :
	static std::default_random_engine engine;
	static std::uniform_int_distribution< int32_t > rng;

	struct TreapNode;

	template<typename Predicate>
	static void cut(const Predicate &, Point<Field> &,
		TreapNode *, TreapNode *&, TreapNode *&);

	static void join(TreapNode *&, TreapNode *, TreapNode *);

	std::queue< TreapNode*, std::list<TreapNode*> > dump;
	void erase(TreapNode *&);

	TreapNode *lower_hull = nullptr, *upper_hull = nullptr;
	Point<Field> first, last;

	template<typename Callback>
	void traverse_chain(TreapNode const*, Callback const&);

	bool update_lower_hull
	(Point<Field> const&, Point<Field>&, Point<Field>&, bool);
	bool update_upper_hull
	(Point<Field> const&, Point<Field>&, Point<Field>&, bool);

public :

	using size_t = int32_t;
	using priority_t = int32_t;

	OnlineHull(Point<Field> const&, Point<Field> const&);
	~OnlineHull();

	bool add_point(Point<Field> const&);

	std::pair< bool, std::pair< Point<Field>, Point<Field> > >
	get_tangents (Point<Field> const&);

	std::pair< Point<Field>, Point<Field> >
	get_extremal_points(Point<Field> const&);

	template<typename Callback> void traverse_lower_hull(Callback const&);
	template<typename Callback> void traverse_upper_hull(Callback const&);

	size_t get_lower_hull_size() const;
	size_t get_upper_hull_size() const;

};

template<typename Field>
struct OnlineHull<Field>::TreapNode
{
	OnlineHull<Field>::priority_t priority;
	OnlineHull<Field>::size_t size;

	TreapNode *left, *right;

	Point< Field > u, v;

	TreapNode(Point<Field> const &u, Point<Field> const &v):
		priority(rng(engine)), size(1),
		left(nullptr), right(nullptr),
		u(u), v(v)
	{ }
};

template<typename Field>
std::default_random_engine OnlineHull<Field>::engine;

template<typename Field>
std::uniform_int_distribution< int32_t > OnlineHull<Field>::rng;

template<typename Field>
OnlineHull<Field>::OnlineHull(Point<Field> const&p, Point<Field> const&q)
{
	assert( not (p == q) );

	if( p < q )
		first = p, last = q;
	else
		first = q, last = p;

	lower_hull = new TreapNode(first, last);
	upper_hull = new TreapNode(first, last);
}

template<typename Field>
OnlineHull<Field>::~OnlineHull()
{
	dump.push(lower_hull);
	dump.push(upper_hull);
	while( not dump.empty() ) {
		auto node = dump.front();
		if( node->left != nullptr )
			dump.push(node->left);
		if( node->right != nullptr )
			dump.push(node->right);
		dump.pop();
		delete node;
	}
}

template<typename Field>
void OnlineHull<Field>::erase(TreapNode *&node)
{
	if( node != nullptr )
		dump.push(node);
	node = nullptr;
}

template<typename Field>
template<typename Predicate>
void OnlineHull<Field>::cut(const Predicate &predicate, Point<Field> &split,
	TreapNode *treap_root, TreapNode *&left_root, TreapNode *&right_root)
{
	if( treap_root == nullptr )
	{
		left_root = right_root = nullptr;
		return;
	}
	if( predicate(*treap_root) )
	{
		split = treap_root->u;
		cut(predicate, split,
			treap_root->left, left_root, treap_root->left);
		right_root = treap_root;
	}
	else
	{
		split = treap_root->v;
		cut(predicate, split,
			treap_root->right, treap_root->right, right_root);
		left_root = treap_root;
	}
	treap_root->size = 1
		+ (treap_root->left  == nullptr ? 0 : treap_root->left ->size)
		+ (treap_root->right == nullptr ? 0 : treap_root->right->size);
}

template<typename Field>
void OnlineHull<Field>::join(TreapNode *&root,
	TreapNode *left_root, TreapNode *right_root)
{
	if( left_root == nullptr or right_root == nullptr )
	{
		root = ( left_root == nullptr ? right_root : left_root );
		return;
	}
	if( left_root->priority < right_root->priority )
	{
		join(right_root->left, left_root, right_root->left);
		root = right_root;
	}
	else
	{
		join(left_root->right, left_root->right, right_root);
		root = left_root;
	}
	root->size = 1
		+ (root->left  == nullptr ? 0 : root->left ->size)
		+ (root->right == nullptr ? 0 : root->right->size);
}

template<typename Field>
template<typename Callback>
void OnlineHull<Field>::traverse_lower_hull(Callback const&callback)
{
	traverse_chain(lower_hull, callback);
	callback(last);
}

template<typename Field>
template<typename Callback>
void OnlineHull<Field>::traverse_upper_hull(Callback const&callback)
{
	traverse_chain(upper_hull, callback);
	callback(last);
}

template<typename Field>
typename OnlineHull<Field>::size_t
OnlineHull<Field>::get_lower_hull_size() const
{
	return 1 + lower_hull->size;
}

template<typename Field>
typename OnlineHull<Field>::size_t
OnlineHull<Field>::get_upper_hull_size() const
{
	return 1 + upper_hull->size;
}

template<typename Field>
template<typename Callback>
void OnlineHull<Field>::traverse_chain(TreapNode const*node, Callback const&callback)
{
	if( node == nullptr )
		return;
	traverse_chain(node->left, callback);
	callback(node->u);
	traverse_chain(node->right, callback);
}

template<typename Field>
bool OnlineHull<Field>::add_point(Point<Field> const& point)
{
	Point<Field> left_tangent, right_tangent;
	bool lower_hull_updated =
		update_lower_hull(point, left_tangent, right_tangent, true);
	bool upper_hull_updated =
		update_upper_hull(point, left_tangent, right_tangent, true);

	if( point < first ) first = point;
	if( last < point  ) last  = point;
	return lower_hull_updated || upper_hull_updated;
}

template<typename Field>
std::pair< bool, std::pair< Point<Field>, Point<Field> > >
OnlineHull<Field>::get_tangents(Point<Field> const& point)
{
	bool outside = false;
	std::pair< Point<Field>, Point<Field> > tangents;
	if( point < first or last < point )
	{
		update_lower_hull(point, tangents.first, tangents.second, false);
		update_upper_hull(point, tangents.second, tangents.first, false);
		if( tangents.second < tangents.first )
			std::swap(tangents.first, tangents.second);
		outside = true;
	}
	else
	{
		bool lower_hull_updated = update_lower_hull(point,
			tangents.first, tangents.second, false);
		bool upper_hull_updated = false;
		if( not lower_hull_updated )
			upper_hull_updated = update_upper_hull(point,
				tangents.first, tangents.second, false);
		outside = ( lower_hull_updated or upper_hull_updated );
	}
	return std::make_pair(outside, tangents);
}

template<typename Field>
std::pair< Point<Field>, Point<Field> >
OnlineHull<Field>::get_extremal_points(Point<Field> const&direction)
{

	std::pair< Point<Field>, Point<Field> > points{first, last};

	auto dip = [&direction](TreapNode const&node)
	{ return ( (node.v - node.u) ^ direction ) <= 0; };

	TreapNode const * it = ( direction.y > 0 or
		(direction.y == 0 and direction.x < 0) ? upper_hull : lower_hull );

	while( it != nullptr )
	{
		if( dip(*it) )
		{
			points = std::make_pair(it->u, it->v);
			it = it->left;
		}
		else
		{
			it = it->right;
		}
	}

	if( (points.second ^ direction) < (points.first ^ direction) )
		points.second = points.first;
	if( (points.first ^ direction) < (points.second ^ direction) )
		points.first = points.second;


	return points;
}

template<typename Field>
bool OnlineHull<Field>::update_lower_hull(Point<Field> const& point,
	Point<Field> &left_tangent, Point<Field> &right_tangent, bool update)
{
	auto left_cond = [this, &point](TreapNode const&node) -> bool
	{ return (point - node.v) * (node.v - node.u) >= 0; };

	auto right_cond = [this, &point](TreapNode const&node) -> bool
 	{ return (node.u - point) * (node.v - node.u) > 0; };

	TreapNode *prefix = nullptr, *suffix = nullptr;

	if( last < point )
	{
		cut(left_cond, left_tangent, lower_hull, prefix, suffix);

		if( update )
		{
			erase(suffix);
			join(lower_hull, prefix, new TreapNode(left_tangent, point));
		}
		else
		{
			join(lower_hull, prefix, suffix);
		}
		return true;
	}

	if( point < first )
	{
		cut(right_cond, right_tangent, lower_hull, prefix, suffix);

		if( update )
		{
			erase(prefix);
			join(lower_hull, new TreapNode(point, right_tangent), suffix);
		}
		else
		{
			join(lower_hull, prefix, suffix);
		}
		return true;
	}

	TreapNode *segment = nullptr;
	Point<Field> left_split, right_split;

	cut([this, &point](TreapNode const&node) {return point < node.u;},
		right_split, lower_hull, prefix, suffix);
	cut([this, &point](TreapNode const&node) {return !(node.v < point);},
		left_split, prefix, prefix, segment);

	Field cross_product = ((segment->v-segment->u) * (point-segment->u));
	bool interior_point = (cross_product >= 0);

	if( interior_point )
	{
		join(prefix, prefix, segment);
		join(lower_hull, prefix, suffix);
		return false;
	}

	TreapNode *inside = nullptr, *outside = nullptr;

	if( update )
		erase(segment);

	cut(left_cond, left_split, prefix, outside, inside);
	left_tangent = left_split;

	if( update )
	{
		erase(inside);
		join(prefix, outside, new TreapNode(left_split, point));
	}
	else
	{
		join(prefix, outside, inside);
	}

	cut(right_cond, right_split, suffix, inside, outside);
	right_tangent = right_split;

	if( update )
	{
		erase(inside);
		join(suffix, new TreapNode(point, right_split), outside);
	}
	else
	{
		join(suffix, inside, outside);
	}

	if( update )
	{
		join(lower_hull, prefix, suffix);
	}
	else
	{
		join(prefix, prefix, segment);
		join(lower_hull, prefix, suffix);
	}

	return true;
}

template<typename Field>
bool OnlineHull<Field>::update_upper_hull(Point<Field> const& point,
	Point<Field> &left_tangent, Point<Field> &right_tangent, bool update)
{
	auto left_cond = [this, &point](TreapNode const&node) -> bool
	{ return (node.v - node.u) * (point - node.v) >= 0; };

	auto right_cond = [this, &point](TreapNode const&node) -> bool
	{ return (node.v - node.u) * (node.u - point) > 0; };

	TreapNode *prefix = nullptr, *suffix = nullptr;

	if( last < point )
	{
		cut(left_cond, left_tangent, upper_hull, prefix, suffix);

		if( update )
		{
			erase(suffix);
			join(upper_hull, prefix, new TreapNode(left_tangent, point));
		}
		else
		{
			join(upper_hull, prefix, suffix);
		}
		return true;
	}

	if( point < first )
	{
		cut(right_cond, right_tangent, upper_hull, prefix, suffix);

		if( update )
		{
			erase(prefix);
			join(upper_hull, new TreapNode(point, right_tangent), suffix);
		}
		else
		{
			join(upper_hull, prefix, suffix);
		}
		return true;
	}

	TreapNode *segment = nullptr;
	Point<Field> left_split, right_split;

	cut([this, &point](TreapNode const&node) {return point < node.u;},
		right_split, upper_hull, prefix, suffix);
	cut([this, &point](TreapNode const&node) {return !(node.v < point);},
		left_split, prefix, prefix, segment);

	Field cross_product = ((segment->v-segment->u) * (point-segment->u));
	bool interior_point = (cross_product <= 0);

	if( interior_point )
	{
		join(prefix, prefix, segment);
		join(upper_hull, prefix, suffix);
		return false;
	}

	TreapNode *inside = nullptr, *outside = nullptr;

	if( update )
		erase(segment);

	cut(left_cond, left_split, prefix, outside, inside);
	left_tangent = left_split;

	if( update )
	{
		erase(inside);
		join(prefix, outside, new TreapNode(left_split, point));
	}
	else
	{
		join(prefix, outside, inside);
	}

	cut(right_cond, right_split, suffix, inside, outside);
	right_tangent = right_split;

	if( update )
	{
		erase(inside);
		join(suffix, new TreapNode(point, right_split), outside);
	}
	else
	{
		join(suffix, inside, outside);
	}

	if( update )
	{
		join(upper_hull, prefix, suffix);
	}
	else
	{
		join(prefix, prefix, segment);
		join(upper_hull, prefix, suffix);
	}

	return true;
}

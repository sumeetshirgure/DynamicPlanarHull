#pragma once

#include <random>

#include "Point.hh"

template<typename Field>
class DynamicHull
{

private :
	static std::default_random_engine engine;
	static std::uniform_int_distribution< int32_t > rng;

	struct TreapNode;

	template<typename Predicate>
	void cut(const Predicate &, Point<Field> &,
		TreapNode *, TreapNode *&, TreapNode *&);

	void join(TreapNode *&, TreapNode *, TreapNode *);

	void erase(TreapNode *&);

	TreapNode *lower_hull = nullptr, *upper_hull = nullptr;
	Point<Field> first, last;

	template<typename Callback>
	void traverse(TreapNode const*, Callback const&);

	bool update_lower_hull(Point<Field> const&);
	bool update_upper_hull(Point<Field> const&);

public :

	using HullIterator = TreapNode const*;

	DynamicHull(Point<Field> const&, Point<Field> const&);
	~DynamicHull();

	bool add_point(Point<Field> const&);

	template<typename Callback> void traverse_lower(Callback const&);
	template<typename Callback> void traverse_upper(Callback const&);

};

template<typename Field>
struct DynamicHull<Field>::TreapNode
{
	int32_t priority;

	TreapNode *left, *right;

	Point< Field > u, v;

	TreapNode(Point<Field> const &u, Point<Field> const &v):
		priority(rng(engine)),
		left(nullptr), right(nullptr),
		u(u), v(v)
	{ }
};

template<typename Field>
std::default_random_engine DynamicHull<Field>::engine;

template<typename Field>
std::uniform_int_distribution< int32_t > DynamicHull<Field>::rng;

template<typename Field>
DynamicHull<Field>::DynamicHull(Point<Field> const&p, Point<Field> const&q)
{
	if( p < q )
		first = p, last = q;
	else
		first = q, last = p;

	lower_hull = new TreapNode(first, last);
	upper_hull = new TreapNode(first, last);
}

template<typename Field>
DynamicHull<Field>::~DynamicHull()
{
	erase(lower_hull);
	erase(upper_hull);
}

template<typename Field>
void DynamicHull<Field>::erase(TreapNode *&node)
{
	if( node == nullptr )
		return;
	erase(node->left);
	erase(node->right);
	delete node;
	node = nullptr;
}

template<typename Field>
template<typename Predicate>
void DynamicHull<Field>::cut(const Predicate &predicate, Point<Field> &split,
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
}

template<typename Field>
void DynamicHull<Field>::join(TreapNode *&root,
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
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::traverse_lower(Callback const&callback)
{
	traverse(lower_hull, callback);
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::traverse_upper(Callback const&callback)
{
	traverse(upper_hull, callback);
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::traverse(TreapNode const*node, Callback const&callback)
{
	if( node == nullptr )
		return;
	traverse(node->left, callback);
	callback(node);
	traverse(node->right, callback);
}

template<typename Field>
bool DynamicHull<Field>::add_point(Point<Field> const& point)
{
	bool lower_hull_updated = update_lower_hull(point);
	bool upper_hull_updated = update_upper_hull(point);

	if( point < first ) first = point;
	if( last < point  ) last  = point;
	return lower_hull_updated || upper_hull_updated;
}

template<typename Field>
bool DynamicHull<Field>::update_lower_hull(Point<Field> const& point)
{
	auto left_tangent = [this, &point](TreapNode const&node) -> bool
	{ return (point - node.v) * (node.v - node.u) >= 0; };

	auto right_tangent = [this, &point](TreapNode const&node) -> bool
 	{ return (node.u - point) * (node.v - node.u) > 0; };

	TreapNode *prefix = nullptr, *suffix = nullptr;

	if( last < point )
	{
		cut(left_tangent, last, lower_hull, prefix, suffix);
		erase(suffix);
		join(lower_hull, prefix, new TreapNode(last, point));
		return true;
	}

	if( point < first )
	{
		cut(right_tangent, first, lower_hull, prefix, suffix);
		erase(prefix);
		join(lower_hull, new TreapNode(point, first), suffix);
		return true;
	}

	TreapNode *segment = nullptr;
	Point<Field> left_split_point, right_split_point;
	cut([this, &point](TreapNode const&node )-> bool
		{return point < node.u;},
		right_split_point, lower_hull, prefix, suffix);
	cut([this, &point](TreapNode const&node) -> bool
		{return !(node.v < point);},
		left_split_point, prefix, prefix, segment);

	Field cross_product = ((segment->v-segment->u) * (point-segment->u));
	bool interior_point = (cross_product >= 0);

	if( interior_point ) {
		join(prefix, prefix, segment);
		join(lower_hull, prefix, suffix);
		return false;
	}

	erase(segment);

	TreapNode *inside = nullptr, *outside = nullptr;
	cut(left_tangent, left_split_point, prefix, outside, inside);
	erase(inside);
	join(prefix, outside, new TreapNode(left_split_point, point));

	cut(right_tangent, right_split_point, suffix, inside, outside);
	erase(inside);
	join(suffix, new TreapNode(point, right_split_point), outside);

	join(lower_hull, prefix, suffix);

	return true;
}

template<typename Field>
bool DynamicHull<Field>::update_upper_hull(Point<Field> const& point)
{
	auto left_tangent = [this, &point](TreapNode const&node) -> bool
	{ return (node.v - node.u) * (point - node.v) >= 0; };

	auto right_tangent = [this, &point](TreapNode const&node) -> bool
	{ return (node.v - node.u) * (node.u - point) > 0; };

	TreapNode *prefix = nullptr, *suffix = nullptr;

	if( last < point )
	{
		cut(left_tangent, last, upper_hull, prefix, suffix);
		erase(suffix);
		join(upper_hull, prefix, new TreapNode(last, point));
		return true;
	}

	if( point < first )
	{
		cut(right_tangent, first, upper_hull, prefix, suffix);
		erase(prefix);
		join(upper_hull, new TreapNode(point, first), suffix);
		return true;
	}

	TreapNode *segment = nullptr;
	Point<Field> left_split_point, right_split_point;
	cut([this, &point](TreapNode const&node )-> bool
		{return point < node.u;},
		right_split_point, upper_hull, prefix, suffix);
	cut([this, &point](TreapNode const&node) -> bool
		{return !(node.v < point);},
		left_split_point, prefix, prefix, segment);

	Field cross_product = ((segment->v-segment->u) * (point-segment->u));
	bool interior_point = (cross_product <= 0);

	if( interior_point ) {
		join(prefix, prefix, segment);
		join(upper_hull, prefix, suffix);
		return false;
	}
	erase(segment);

	TreapNode *inside = nullptr, *outside = nullptr;
	cut(left_tangent, left_split_point, prefix, outside, inside);
	erase(inside);
	join(prefix, outside, new TreapNode(left_split_point, point));

	cut(right_tangent, right_split_point, suffix, inside, outside);
	erase(inside);
	join(suffix, new TreapNode(point, right_split_point), outside);

	join(upper_hull, prefix, suffix);

	return true;
}

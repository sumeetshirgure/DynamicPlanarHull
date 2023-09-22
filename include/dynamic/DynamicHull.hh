#pragma once

#include <optional>
#include <vector>
#include <iostream>
#include <cassert>

#include "HullTree.hh"
#include "MergeableLowerHull.hh"
#include "MergeableUpperHull.hh"

template<typename Field>
class DynamicHull {

  public :

    using size_t = int32_t;
    using priority_t = int32_t;

    using lower_hull_t = MergeableLowerHull<Field>;
    using upper_hull_t = MergeableUpperHull<Field>;


    void add_point(Point<Field> const&);
    bool remove_point(Point<Field> const&);

    bool point_in_polygon(Point<Field> const&);

    std::optional< std::pair< Point<Field>, Point<Field> > >
      get_tangents (Point<Field> const&) const;

    std::pair< Point<Field>, Point<Field> >
      get_extremal_points(Point<Field> const&) const;

    template<typename Callback> void traverse_lower_hull(Callback const&) const;
    template<typename Callback> void traverse_upper_hull(Callback const&) const;

    size_t get_lower_hull_size() const;
    size_t get_upper_hull_size() const;
    size_t get_hull_size() const;
    size_t get_num_points() const;

    template<typename Callback> void traverse_hull(Callback const&) const;
    template<typename Callback> void traverse_set (Callback const&) const;

  private:
    static std::default_random_engine engine;
    static std::uniform_int_distribution< int32_t > rng;

    template<typename TotalOrder> class TreapNode {
      public:
        inline virtual lower_hull_t& lower_hull() = 0;
        inline virtual upper_hull_t& upper_hull() = 0;
        inline virtual bool is_leaf() const = 0;
        inline virtual DynamicHull::priority_t priority() const = 0;
        inline virtual TotalOrder lo() const = 0;
        inline virtual TotalOrder hi() const = 0;
    };


    size_t _leaves = 0;
    TreapNode < Point<Field> > * master_root = nullptr;

    template<typename Callback> void __traverse_set (Callback const&, TreapNode<Point<Field>>*) const;

    template<typename TotalOrder> class TreapLeaf : public TreapNode<TotalOrder> {
      TotalOrder point;
      lower_hull_t _lower_hull;
      upper_hull_t _upper_hull;
      public:
      inline bool is_leaf() const { return true; }
      inline DynamicHull::priority_t priority() const { return -1; }
      inline TotalOrder lo() const { return point; }
      inline TotalOrder hi() const { return point; }
      void set_point(TotalOrder const& _point) { point = _point; }

      inline lower_hull_t& lower_hull() { return _lower_hull; }
      inline upper_hull_t& upper_hull() { return _upper_hull; }

      TreapLeaf(const TotalOrder& _point) : point(_point) { 
        _lower_hull = MergeableLowerHull<Field>(LineSegment<Field>{point, point});
        _upper_hull = MergeableUpperHull<Field>(LineSegment<Field>{point, point});
      }
      ~TreapLeaf() {
        _lower_hull.destroy(), _upper_hull.destroy();
      }
    };

    template<typename TotalOrder> class TreapBranch : public TreapNode<TotalOrder> {
      DynamicHull::priority_t _priority = rng(engine);
      TotalOrder _lo, _hi;
      lower_hull_t _lower_hull, lower_left_residue, lower_right_residue;
      upper_hull_t _upper_hull, upper_left_residue, upper_right_residue;
      LineSegment<Field> lower_bridge, upper_bridge;
      public:
      TreapNode<TotalOrder> *left = nullptr, *right = nullptr;
      bool is_leaf() const { return false; }
      inline DynamicHull::priority_t priority() const { return _priority; }
      inline TotalOrder lo() const { return _lo; }
      inline TotalOrder hi() const { return _hi; }
      inline lower_hull_t& lower_hull() { return _lower_hull; }
      inline upper_hull_t& upper_hull() { return _upper_hull; }
      inline void pull() {
        _lo = left->lo(), _hi = right->hi();

        lower_bridge = merge_lower_hulls(lower_hull(),
            left->lower_hull(), right->lower_hull(),
            lower_left_residue, lower_right_residue);

        upper_bridge = merge_upper_hulls(upper_hull(),
            left->upper_hull(), right->upper_hull(),
            upper_left_residue, upper_right_residue);
      }
      void push() {
        split_lower_hulls(lower_bridge, lower_hull(),
            left->lower_hull(), right->lower_hull(),
            lower_left_residue, lower_right_residue);

        split_upper_hulls(upper_bridge, upper_hull(),
            left->upper_hull(), right->upper_hull(),
            upper_left_residue, upper_right_residue);
      }
    };

    template< typename TotalOrder > void join(TreapNode<TotalOrder>* & root,
        TreapNode<TotalOrder> *left, TreapNode<TotalOrder> *right) {

      if( left == nullptr or right == nullptr )
        return void(root = (left == nullptr ? right : left));

      if( left->is_leaf() and right->is_leaf() ) {
        auto* _root = static_cast<TreapBranch<TotalOrder>*>(root);
        _root = new TreapBranch<TotalOrder>();
        _root->left = left, _root->right = right;
        root = _root, _root->pull();
        return;
      }

      if( left->priority() < right->priority() ) {
        auto _right = static_cast<TreapBranch<TotalOrder>*>(right);
        _right->push(); 
        join(_right->left, left, _right->left);
        root = _right;
        if( _right->left->priority() > _right->priority() ) { // fix priority
          auto _left = static_cast<TreapBranch<TotalOrder>*>(_right->left);
          _left->push();
          auto temp = _left->right;
          _left->right = _right, _right->left = temp, root = _left;
          _right->pull(), _left->pull();
        } else {
          _right->pull();
        }
      } else {
        auto _left = static_cast<TreapBranch<TotalOrder>*>(left);
        _left->push(); 
        join(_left->right, _left->right, right);
        root = _left;
        if( _left->right->priority() > _left->priority() ) { // fix priority
          auto _right = static_cast<TreapBranch<TotalOrder>*>(_left->right);
          _right->push();
          auto temp = _right->left;
          _right->left = _left, _left->right = temp, root = _right;
          _left->pull(), _right->pull();
        } else {
          _left->pull();
        }
      }
    }

    template< typename TotalOrder > bool remove(
        TotalOrder const& point, TreapNode<TotalOrder> *&tree, 
        TreapBranch<TotalOrder> *parent = nullptr) {
      if( tree == nullptr ) return false;
      if( tree->is_leaf() ) {
        auto leaf = static_cast<TreapLeaf<TotalOrder>*>(tree);
        if( (leaf->hi() < point) or (point < leaf->lo()) ) return false;
        delete leaf, tree = nullptr;
        _leaves--;
        return true;
      }
      auto _tree = static_cast<TreapBranch<TotalOrder>*>(tree);
      if( (point < _tree->lo()) or (_tree->hi() < point) ) {
        return false;
      }

      auto &left_child = _tree->left, &right_child = _tree->right;
      if( (left_child->hi() < point) and (point < right_child->lo()) ) {
        return false;
      }

      _tree->push();

      bool was_present;
      if( not (left_child->hi() < point) ) {
        was_present = remove(point, left_child, _tree);
      } else {
        was_present = remove(point, right_child, _tree);
      }

      if( left_child == nullptr or right_child == nullptr ) {
        auto child = (left_child == nullptr ? right_child : left_child);
        delete _tree;
        if( parent == nullptr ) {
          tree = child;
        } else {
          if( parent->left == tree ) parent->left = child;
          else parent->right = child;
        }
      } else {
        _tree->pull();
      }

      return was_present;
    }


    template< typename TotalOrder > void cut(
        TotalOrder const& point, TreapNode<TotalOrder> *tree,
        TreapNode<TotalOrder> *&left, TreapNode<TotalOrder> *&right) {
      if( tree == nullptr ) { left = right = nullptr; return; }
      if(tree->is_leaf()) {
        if( tree->lo() < point ) left = tree, right = nullptr;
        else left = nullptr, right = tree;
        return;
      }
      auto _tree = static_cast<TreapBranch<TotalOrder>*>(tree);
      _tree->push();
      auto &left_child = _tree->left, &right_child = _tree->right;

      // _tree->lo(), left_child->hi(), right_child->lo(), _tree->hi();
      if( not (left_child->hi() < point) ) {
        cut(point, left_child, left, left_child);
        right = _tree;
        _tree->pull();
      } else if( right_child->lo() < point ) {
        cut(point, right_child, right_child, right);
        left = _tree;
        _tree->pull();
      } else { // left_child->hi() < point <= right_child->lo()
        left = left_child, right = right_child;
        delete _tree; _tree = nullptr;
      }
    }

    template< typename TotalOrder > void insert(
        TotalOrder const& point, TreapNode<TotalOrder> *&tree) {
      TreapNode<TotalOrder> *left, *right;
      cut(point, tree, left, right);
      TreapLeaf<TotalOrder> *leaf = new TreapLeaf<TotalOrder>(point);
      join(right, leaf, right);
      join(tree, left, right);
    }

};

template<typename Field>
std::default_random_engine DynamicHull<Field>::engine;
template<typename Field>
std::uniform_int_distribution< int32_t > DynamicHull<Field>::rng;

template<typename Field>
void DynamicHull<Field>::add_point(Point<Field> const& point) {
  insert(point, master_root);
  _leaves++;
}

template<typename Field>
bool DynamicHull<Field>::remove_point(Point<Field> const& point) {
  bool was_present = remove(point, master_root);
  if( was_present ) _leaves--;
  return was_present;
}

template<typename Field>
DynamicHull<Field>::size_t DynamicHull<Field>::get_lower_hull_size() const {
  return master_root == nullptr ? 0 : master_root->lower_hull().get_size();
}

template<typename Field>
DynamicHull<Field>::size_t DynamicHull<Field>::get_upper_hull_size() const {
  return master_root == nullptr ? 0 : master_root->upper_hull().get_size();
}

template<typename Field>
DynamicHull<Field>::size_t DynamicHull<Field>::get_hull_size() const {
  return get_lower_hull_size() + get_upper_hull_size();
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::traverse_lower_hull(Callback const& callback) const {
  if( master_root == nullptr ) return;
  for(auto const segment: master_root->lower_hull())
    callback(segment);
}

template<typename Field>
template<typename Callback> 
void DynamicHull<Field>::traverse_upper_hull(Callback const& callback) const {
  if( master_root == nullptr ) return;
  for(auto segment: master_root->upper_hull())
    callback(segment);
}

template<typename Field>
DynamicHull<Field>::size_t DynamicHull<Field>::get_num_points() const {
  return _leaves;
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::__traverse_set(Callback const& callback, TreapNode<Point<Field>> *ptr) const {
  if( ptr->is_leaf() ) {
    callback(ptr->point);
    return;
  }
  auto _ptr = static_cast< TreapBranch<Point<Field>>* >(ptr);
  __traverse_set(callback, _ptr->left);
  __traverse_set(callback, _ptr->right);
}

template<typename Field>
template<typename Callback>
void DynamicHull<Field>::traverse_set(Callback const& callback) const {
}

/******************************************************************************/
/******Point in polygon, tangent and farthes point queries.********************/
/******************************************************************************/

template<typename Field>
bool DynamicHull<Field>::point_in_polygon(Point<Field> const& point) {
  auto const& lower_hull = master_root->lower_hull();
  auto const& upper_hull = master_root->upper_hull();

  auto const lower_segment = lower_hull.binary_search(
      [&](lower_hull_t::iterator const& seg) { return point < seg->v; });
  auto const upper_segment = upper_hull.binary_search(
      [&](upper_hull_t::iterator const& seg) { return point < seg->v; });

  bool lower_enclosed = lower_segment != lower_hull.end() and
    (lower_segment->v - lower_segment->u) * (point - lower_segment->u) > 0;
  bool upper_enclosed = upper_segment != upper_hull.end() and
    (upper_segment->v - upper_segment->u) * (point - upper_segment->u) < 0;

  return lower_enclosed and upper_enclosed;
}

template<typename Field>
std::optional< std::pair< Point<Field>, Point<Field> > >
DynamicHull<Field>::get_tangents (Point<Field> const& point) const {
  if( get_hull_size() <= 2 ) {
    auto ret = *(master_root->lower_hull().begin());
    return {{ret.u, ret.v}};
  }
  auto const& lower_hull = master_root->lower_hull();
  auto const& upper_hull = master_root->upper_hull();

  auto first = lower_hull.begin()->u, last = upper_hull.rbegin()->v;

  if( point < first ) {
    auto lower_segment = lower_hull.binary_search([&](lower_hull_t::iterator const& seg)
        { return (seg->v-seg->u)*(point-seg->u) > 0;});
    auto lower_tangent = lower_segment == lower_hull.end() ? last : lower_segment->u;

    auto upper_segment = upper_hull.binary_search([&](upper_hull_t::iterator const& seg)
        { return (seg->v-seg->u)*(point-seg->u) < 0;});
    auto upper_tangent = upper_segment == upper_hull.end() ? last : upper_segment->u;

    if( upper_tangent < lower_tangent ) std::swap(upper_tangent, lower_tangent);
    return {{lower_tangent, upper_tangent}};
  } else if( last < point ) {
    auto lower_segment = lower_hull.binary_search([&](lower_hull_t::iterator const& seg)
        { return (seg->v-seg->u)*(point-seg->u) <= 0;});
    auto lower_tangent = lower_segment == lower_hull.end() ? last : lower_segment->u;

    auto upper_segment = upper_hull.binary_search([&](upper_hull_t::iterator const& seg)
        { return (seg->v-seg->u)*(point-seg->u) >= 0;});
    auto upper_tangent = upper_segment == upper_hull.end() ? last : upper_segment->u;

    if( upper_tangent < lower_tangent ) std::swap(upper_tangent, lower_tangent);
    return {{lower_tangent, upper_tangent}};
  } else {
    auto const lower_segment = lower_hull.binary_search(
        [&](lower_hull_t::iterator const& seg) { return point < seg->v; });
    auto const upper_segment = upper_hull.binary_search(
        [&](upper_hull_t::iterator const& seg) { return point < seg->v; });

    bool lower_enclosed = lower_segment != lower_hull.end() and
      (lower_segment->v - lower_segment->u) * (point - lower_segment->u) > 0;
    bool upper_enclosed = upper_segment != upper_hull.end() and
      (upper_segment->v - upper_segment->u) * (point - upper_segment->u) < 0;

    if( lower_enclosed and upper_enclosed ) return {};

    if( lower_enclosed ) { // => not upper_enclosed
      auto left_segment = upper_hull.binary_search(
          [&](upper_hull_t::iterator const& seg)
          { return point < seg->v or (seg->v-seg->u)*(point-seg->u) >= 0; });
      auto left_tangent = left_segment == upper_hull.end() ? first : left_segment->u;

      auto right_segment = upper_hull.binary_search(
          [&](upper_hull_t::iterator const& seg)
          { return point < seg->u and (seg->v-seg->u)*(point-seg->u) < 0; });
      auto right_tangent =
        right_segment == upper_hull.end() ? last : right_segment->u;

      if( right_tangent < left_tangent ) std::swap(left_tangent, right_tangent);
      return {{left_tangent, right_tangent}};
    } else { 
      auto left_segment = lower_hull.binary_search(
          [&](lower_hull_t::iterator const& seg)
          { return point < seg->v or (seg->v-seg->u)*(point-seg->u) <= 0; });
      auto left_tangent = left_segment == lower_hull.end() ? first : left_segment->u;

      auto right_segment = lower_hull.binary_search(
          [&](lower_hull_t::iterator const& seg)
          { return point < seg->u and (seg->v-seg->u)*(point-seg->u) > 0; });
      auto right_tangent =
        right_segment == lower_hull.end() ? last : right_segment->u;

      if( right_tangent < left_tangent ) std::swap(left_tangent, right_tangent);
      return {{left_tangent, right_tangent}};
    }
  }

  return {}; // never occurs
};

template<typename Field>
std::pair< Point<Field>, Point<Field> >
DynamicHull<Field>::get_extremal_points (Point<Field> const& direction) const {
  auto const& lower_hull = master_root->lower_hull();
  auto const& upper_hull = master_root->upper_hull();

  auto first = lower_hull.begin()->u, last = upper_hull.rbegin()->v;

  LineSegment<Field> segment{first, last};
  if( direction.y > 0 or (direction.y == 0 and direction.x < 0) ) {
    auto udip = [&direction](upper_hull_t::iterator const& seg)
    { return ((seg->v - seg->u) ^ direction) <= 0; };
    auto seg = upper_hull.binary_search(udip);
    if( seg != upper_hull.end() ) segment = *seg;
  } else {
    auto ldip = [&direction](lower_hull_t::iterator const& seg)
    { return ((seg->v - seg->u) ^ direction) <= 0; };
    auto seg = lower_hull.binary_search(ldip);
    if( seg != lower_hull.end() ) segment = *seg;
  }

  if( (segment.v ^ direction) < (segment.u ^ direction) ) segment.v = segment.u;
  if( (segment.u ^ direction) < (segment.v ^ direction) ) segment.u = segment.v;

  return {segment.u, segment.v};
};



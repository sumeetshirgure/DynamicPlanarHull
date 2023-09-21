
#pragma once

#include <cassert>
#include <optional>
#include <vector>
#include <iostream>

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

    TreapNode < Point<Field> > * master_root = nullptr;

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
        std::cerr << "Created point " << to_string(point) << std::endl;
        _lower_hull = MergeableLowerHull<Field>(LineSegment<Field>{point, point});
        _upper_hull = MergeableUpperHull<Field>(LineSegment<Field>{point, point});
      }
      ~TreapLeaf() {
        std::cerr << "Deleted point " << to_string(point) << std::endl;
        _lower_hull.destroy(), _upper_hull.destroy();
      }
    };

    template<typename TotalOrder> class TreapBranch : public TreapNode<TotalOrder> {
      DynamicHull::priority_t _priority = rng(engine);
      TotalOrder _lo, _hi;
      lower_hull_t _lower_hull, lower_left_residue, lower_right_residue;
      upper_hull_t _upper_hull, upper_left_residue, upper_right_residue;
      LineSegment<Field> lower_bridge, upper_bridge;
      // bool merged = false;
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
        std::cerr << "Pulled branch " << priority() 
          << " " << to_string(_lo) << " " << to_string(_hi) << std::endl;


        std::cerr << "Merging ..." << std::endl;
        for(auto const& seg: left->lower_hull())
          std::cerr << to_string(seg) << " ";
        std::cerr << "... and ..." << std::endl;
        for(auto const& seg: right->lower_hull())
          std::cerr << to_string(seg) << " ";
        std::cerr << "... to get ..." << std::endl;

        lower_bridge = merge_lower_hulls(lower_hull(),
            left->lower_hull(), right->lower_hull(),
            lower_left_residue, lower_right_residue);

        std::cerr << "Bridge :" << to_string(lower_bridge) << std::endl;
        std::cerr << " H "; for(auto const& seg: lower_hull()) std::cerr << to_string(seg) << " ";
        std::cerr << std::endl;
        std::cerr << " llr "; for(auto const& seg: lower_left_residue) std::cerr << to_string(seg) << " ";
        std::cerr << std::endl;
        std::cerr << " lrr "; for(auto const& seg: lower_right_residue) std::cerr << to_string(seg) << " ";
        std::cerr << std::endl;
        std::cerr << " lh "; for(auto const& seg: left->lower_hull()) std::cerr << to_string(seg) << " ";
        std::cerr << std::endl;
        std::cerr << " rh "; for(auto const& seg: right->lower_hull()) std::cerr << to_string(seg) << " ";
        std::cerr << std::endl;
        std::cerr << ".............." << lower_hull().get_size() << std::endl;

        upper_bridge = merge_upper_hulls(upper_hull(),
            left->upper_hull(), right->upper_hull(),
            upper_left_residue, upper_right_residue);
      }
      void push() {
        std::cerr << "Pushed branch " << priority() 
          << " " << to_string(_lo) << " " << to_string(_hi) << std::endl;
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
}

template<typename Field>
bool DynamicHull<Field>::remove_point(Point<Field> const& point) {
  return remove(point, master_root);
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

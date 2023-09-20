
#pragma once

#include <cassert>
#include <optional>

template<typename Field>
class DynamicHull {

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

  private:
    static std::default_random_engine engine;
    static std::uniform_int_distribution< int32_t > rng;

    template<typename TotalOrder> class TreapNode {
      public:
        inline virtual bool is_leaf() const = 0;
        inline virtual DynamicHull::priority_t priority() const = 0;
        inline virtual TotalOrder lo() const = 0;
        inline virtual TotalOrder hi() const = 0;
        // inline virtual ~TreapNode();
    };

    TreapNode < Point<Field> > * master_root = nullptr;

    template<typename TotalOrder> class TreapLeaf : public TreapNode<TotalOrder> {
      TotalOrder point;
      public:
      inline bool is_leaf() const { return true; }
      inline DynamicHull::priority_t priority() const { return -1; }
      inline TotalOrder lo() const { return point; }
      inline TotalOrder hi() const { return point; }
      void set_point(TotalOrder const& _point) { point = _point; }

      TreapLeaf(const TotalOrder& _point) : point(_point) { 
        std::cerr << "Created point " << point << std::endl;
      }
      ~TreapLeaf() {
        std::cerr << "Deleted point " << point << std::endl;
      }
    };

    template<typename TotalOrder> class TreapBranch : public TreapNode<TotalOrder> {
      DynamicHull::priority_t _priority = rng(engine);
      TotalOrder _lo, _hi;
      public:
      TreapNode<TotalOrder> *left = nullptr, *right = nullptr;
      bool is_leaf() const { return false; }
      inline TreapNode<TotalOrder>::priority_t priority() const { return _priority; }
      inline TotalOrder hi() const { return _hi; }
      inline void pull() {
        assert(left != nullptr and right != nullptr);
        _lo = left->lo(), _hi = right->hi();
        std::cerr << "Pulled branch " << priority() << " " << _lo << " " << _hi << std::endl;
      }
      void push() {
        assert(left != nullptr and right != nullptr);
        std::cerr << "Pushed branch " << priority() << " " << _lo << " " << _hi << std::endl;
      }
      inline TotalOrder lo() const { return _lo; }
      TreapBranch() {
        std::cerr << "Created branch " << priority() << std::endl;
      }
      ~TreapBranch() {
        std::cerr << "Deleted branch " << priority() << " " << _lo << " " << _hi << std::endl;
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


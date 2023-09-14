#include <iostream>
#include <random>
#include <cassert>
#include <stack>


template<typename TotalOrder>
class TreapNode {
  public:
  using priority_t = int32_t;

    inline virtual bool is_leaf() const = 0;
    inline virtual priority_t priority() const = 0;
    inline virtual TotalOrder lo() const = 0;
    inline virtual TotalOrder hi() const = 0;
};

template<typename TotalOrder>
class TreapLeaf : public TreapNode<TotalOrder> {
  public:
  TotalOrder point;
  inline bool is_leaf() const { return true; }
  inline TreapNode<TotalOrder>::priority_t priority() const { return -1; }
  inline TotalOrder lo() const { return point; }
  inline TotalOrder hi() const { return point; }
  // TreapLeaf(const TotalOrder& _point) : point(_point) { }
};


template<typename TotalOrder>
class TreapBranch : public TreapNode<TotalOrder> {
  TreapNode<TotalOrder>::priority_t _priority = rand();
  TotalOrder _lo, _hi;
  public:
  TreapNode<TotalOrder> *left = nullptr, *right = nullptr;
  bool is_leaf() const { return false; }
  inline TreapNode<TotalOrder>::priority_t priority() const { return _priority; }
  inline TotalOrder hi() const { return _hi; }
  inline void pull() {
    assert(left != nullptr and right != nullptr);
    _lo = left->lo();
    _hi = right->hi();
  }
  void push() {

  }
  inline TotalOrder lo() const { return _lo; }
};

template< typename TotalOrder >
void join(TreapNode<TotalOrder>* & root,
    TreapNode<TotalOrder> *left, TreapNode<TotalOrder> *right) {
  if( left == nullptr or right == nullptr )
    return void(root = (left == nullptr ? right : left));
  if( left->is_leaf() and right->is_leaf() ) {
    root = new TreapBranch<TotalOrder>();
    auto _root = static_cast<TreapBranch<TotalOrder>*>(root);
    _root->left = left, _root->right = right;
    _root->pull();
    return;
  }

  if( left->priority() < right->priority() ) {
    auto _right = static_cast<TreapBranch<TotalOrder>*>(right);
    _right->push(); 
    join(_right->left, left, _right->left);
    root = _right;
    if( _right->left->priority() > _right->priority() ) { // fix priority
      auto _left = static_cast<TreapBranch<TotalOrder>*>(_right->left);
      // push(_right), push(_left);
      _left->push();
      auto temp = _left->right;
      _left->right = _right;
      _right->left = temp;
      root = _left;
      _left->pull();
    }
    _right->pull();
  } else {
    auto _left = static_cast<TreapBranch<TotalOrder>*>(left);
    _left->push(); 
    join(_left->right, _left->right, right);
    root = _left;
    if( _left->right->priority() > _left->priority() ) { // fix priority
      auto _right = static_cast<TreapBranch<TotalOrder>*>(_left->right);
      _right->push();
      auto temp = _right->left;
      _right->left = _left;
      _left->right = temp;
      root = _right;
      _right->pull();
    }
    _left->pull();
  }
}



template< typename TotalOrder >
bool remove(TotalOrder const& point, TreapNode<TotalOrder> *&tree, 
    TreapBranch<TotalOrder> *parent = nullptr) {

  if( tree->is_leaf() ) {
    auto leaf = static_cast<TreapLeaf<TotalOrder>*>(tree);
    if( (leaf->hi() < point) or (point < leaf->lo()) )
      return false;
    tree = nullptr;
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
    delete tree;
    if( parent == nullptr ) {
      tree = child;
    } else {
      if( parent->left == tree ) parent->left = child;
      else parent->right = child;
      parent->pull();
    }
  } else {
    _tree->pull();
  }

  return was_present;
}


template< typename TotalOrder >
void print_treap(TreapNode<TotalOrder> const* root) {
  if( root == nullptr ) return;
  // std::cerr << ">> " << root->lo() << " " << root->hi() << std::endl;
  if( root->is_leaf() ) {
    std::cout << (static_cast<TreapLeaf<TotalOrder>const*>(root))->point << ' ';
    return;
  }
  auto const* _root = static_cast<TreapBranch<TotalOrder>const*>(root);
  std::cout << "(";
  if(_root->priority() < _root->left->priority()) std::cerr<<"!+";
  if(_root->priority() < _root->right->priority()) std::cerr<<"!-";
  if(_root->lo() != _root->left->lo() or _root->hi() != _root->right->hi()) {
    std::cerr<<"!/";
  }
  print_treap(_root->left);
  print_treap(_root->right);
  std::cout << ")";
}

template< typename TotalOrder >
int get_height(TreapNode<TotalOrder> const* root) {
  if( root->is_leaf() ) return 1;
  auto const* _root = static_cast<TreapBranch<TotalOrder>const*>(root);
  return 1+std::max(get_height(_root->left), get_height(_root->right));
}

int main() {
  using namespace std;

  TreapLeaf<int> leaves[1001];
  for(int i=0; i<=100; i++) {
    leaves[i].point = i;
  }

  TreapNode<int> * root = nullptr;
  vector< TreapNode<int> * > roots(11, nullptr);
  for(int i=1, j = 1; i<=10; i++) {
    for(; j<=i*1; j++) {
      join(roots[i], roots[i], leaves+j);
    }
  }

  for(int i=1; i<=10; i++) {
    join(root, root, roots[i]);
  }
  print_treap(root), cout << endl;
  std::cout << "H" << get_height(root) << std::endl;

  int x; cin >> x;
  cout << ">> " << remove(x, root) << endl;
  print_treap(root), cout << endl;
  std::cout << get_height(root) << std::endl;

  return 0;
}

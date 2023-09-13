#include <iostream>
#include <random>
#include <cassert>


template<typename TotalOrder>
class TreapNode {
  public:
  using priority_t = int32_t;

    virtual bool is_leaf() const = 0;
    inline virtual priority_t priority() const = 0;
};

template<typename TotalOrder>
class TreapLeaf : public TreapNode<TotalOrder> {
  public:
    TotalOrder point;
    bool is_leaf() const { return true; }
    inline TreapNode<TotalOrder>::priority_t priority() const { return -1; }
    // TreapLeaf(const TotalOrder& _point) : point(_point) { }
};


template<typename TotalOrder>
class TreapBranch : public TreapNode<TotalOrder> {
  TreapNode<TotalOrder>::priority_t _priority = rand();
  public:
  TotalOrder lo, hi;
  TreapNode<TotalOrder> *left = nullptr, *right = nullptr;
  bool is_leaf() const { return false; }
  inline TreapNode<TotalOrder>::priority_t priority() const { return _priority; }
  void push();
  void pull() {
    assert( left != nullptr and right != nullptr );
    if( left->is_leaf() ) lo = static_cast<TreapLeaf<TotalOrder>*>(left)->point;
    else lo = static_cast<TreapBranch<TotalOrder>*>(left)->lo;
    if( right->is_leaf() ) hi = static_cast<TreapLeaf<TotalOrder>*>(right)->point;
    else hi = static_cast<TreapBranch<TotalOrder>*>(right)->lo;
  }
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
  if( left->is_leaf() or (not right->is_leaf()) and (left->priority() < right->priority()) ) {
    auto _right = static_cast<TreapBranch<TotalOrder>*>(right);
    join(_right->left, left, _right->left);
    root = _right;
    if( _right->left->priority() > _right->priority() ) { // fix priority
      auto _left = static_cast<TreapBranch<TotalOrder>*>(_right->left);
      // push(_right), push(_left);
      auto temp = _left->right;
      _left->right = _right;
      _right->left = temp;
      root = _left;
      _right->pull(), _left->pull();
    }
  } else {
    auto _left = static_cast<TreapBranch<TotalOrder>*>(left);
    join(_left->right, _left->right, right);
    root = _left;
    if( _left->right->priority() > _left->priority() ) { // fix priority
      auto _right = static_cast<TreapBranch<TotalOrder>*>(_left->right);
      // push(_left), push(_right);
      auto temp = _right->left;
      _right->left = _left;
      _left->right = temp;
      root = _right;
      _left->pull(), _right->pull();
    }
  }
}


template< typename TotalOrder >
void insert(TotalOrder const& point, TreapNode<TotalOrder> *tree) {
  // 
}


template< typename TotalOrder >
void remove(TotalOrder const& point, TreapNode<TotalOrder> *tree) {
  //
}


template< typename TotalOrder >
void print_treap(TreapNode<TotalOrder> const* root) {
  if( root == nullptr ) return;
  if( root->is_leaf() ) {
    std::cout << (static_cast<TreapLeaf<TotalOrder>const*>(root))->point << ' ';
    return;
  }
  auto const* _root = static_cast<TreapBranch<TotalOrder>const*>(root);
  std::cout << "(";
  if(_root->priority() < _root->left->priority()) std::cerr<<"!+";
  if(_root->priority() < _root->right->priority()) std::cerr<<"!-";
  print_treap(_root->left);
  print_treap(_root->right);
  std::cout << ")";
}

template< typename TotalOrder >
int get_height(TreapNode<TotalOrder> const* root) {
  if( root->is_leaf() ) {
    return 1;
  }
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
    for(; j<=i*10; j++) {
      join(roots[i], roots[i], leaves+j);
    }
  }

  for(int i=1; i<=10; i++) {
    join(root, root, roots[i]);
  }
  print_treap(root), cout << endl;
  std::cout << get_height(root) << std::endl;

  return 0;
}

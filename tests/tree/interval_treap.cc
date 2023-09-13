#include <iostream>
#include <random>

template<typename TotalOrder>
class TreapNode {

  public:
    using priority_t = int32_t;

    priority_t priority = rand();

    virtual bool is_leaf() const = 0;
};

template<typename TotalOrder>
class TreapBranch : public TreapNode<TotalOrder> {
  TotalOrder lo, hi;
  public:
  TreapNode<TotalOrder> *left = nullptr, *right = nullptr;
  bool is_leaf() const { return false; }
};

template<typename TotalOrder>
class TreapLeaf : public TreapNode<TotalOrder> {
  public:
    TotalOrder point;
    bool is_leaf() const { return true; }
    // TreapLeaf(const TotalOrder& _point) : point(_point) { }
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
  } else if( left->is_leaf() or (not right->is_leaf()) and (left->priority < right->priority) ) {
    auto _right = static_cast<TreapBranch<TotalOrder>*>(right);
    join(_right->left, left, _right->left);
    root = _right;
    if( _right->left->priority > _right->priority ) { // fix priority
      auto _left = static_cast<TreapBranch<TotalOrder>*>(_right->left);
      // push(_right), push(_left);
      auto temp = _left->right;
      _left->right = _right;
      _right->left = temp;
      root = _left;
      // pull(_right), pull(_left);
    }
  } else {
    // if( right->is_leaf() or (not left->is_leaf()) and (right->priority_t <= left->priority) ) {
    auto _left = static_cast<TreapBranch<TotalOrder>*>(left);
    join(_left->right, _left->right, right);
    root = _left;
    if( _left->right->priority > _left->priority ) { // fix priority
      auto _right = static_cast<TreapBranch<TotalOrder>*>(_left->right);
      // push(_left), push(_right);
      auto temp = _right->left;
      _right->left = _left;
      _left->right = temp;
      root = _right;
      // pull(_left), pull(_right);
    }
  }
}

template< typename TotalOrder >
void cut(TreapNode<TotalOrder> *root, TotalOrder const& threshold,
    TreapNode<TotalOrder> *&left, TreapNode<TotalOrder> *&right) {
  if( root == nullptr ) return void(left = right = nullptr);

  // push(root);
}

template< typename TotalOrder >
void print_treap(TreapNode<TotalOrder> const* root) {
  if( root->is_leaf() ) {
    std::cout << (static_cast<TreapLeaf<TotalOrder>const*>(root))->point << ' ';
    return;
  }
  auto const* _root = static_cast<TreapBranch<TotalOrder>const*>(root);
  std::cout << "(";
  if(!_root->left->is_leaf() and _root->priority < _root->left->priority) std::cerr<<"!+";
  if(!_root->right->is_leaf() and _root->priority < _root->right->priority) std::cerr<<"!-";
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

  TreapLeaf<int> leaves[100001];
  for(int i=0; i<=100000; i++) {
    leaves[i].point = i;
  }

  TreapNode<int> * root = nullptr;
  for(int i=50001; i<=100000; i++) {
    join(root, root, leaves+i);
    // print_treap(root), cout << endl;
    if( i % 100 == 1 )
      std::cout << std::fixed << i << " " << get_height(root) << '\r';
  }
  for(int i=50000; i>=0; i--) {
    join(root, leaves+i, root);
    // print_treap(root), cout << endl;
    if( i % 100 == 0 )
      std::cout << std::fixed << i << " " << get_height(root) << '\r';
  }


  return 0;
}

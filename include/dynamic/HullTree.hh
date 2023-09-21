#pragma once

#include <utility>
#include <cstdint>
#include <random>
#include <vector>
#include <iterator>


template<typename Element>
class HullTree {

  struct TreapNode;

  public:

  using size_t = int32_t;
  using priority_t = int32_t;

  class iterator;
  class reverse_iterator;

  template<typename Predicate>
    static void cut(const Predicate&, HullTree, HullTree&, HullTree&);

  template<typename Predicate> iterator binary_search(Predicate const&);

  static void join(HullTree&, HullTree, HullTree);

  HullTree();
  HullTree(Element const&);

  inline iterator const begin() const;
  inline iterator const end() const;

  inline reverse_iterator const rbegin() const;
  inline reverse_iterator const rend()  const;

  size_t get_size();

  void destroy();

  protected:
  static std::default_random_engine engine;
  static std::uniform_int_distribution< int32_t > rng;

  template<typename Predicate>
    static void __cut(const Predicate &,
        TreapNode *, TreapNode *&, TreapNode *&);

  static void __join(TreapNode *&, TreapNode *, TreapNode *);

  TreapNode * treap = nullptr;

  iterator _begin{nullptr}, _end{nullptr};
  reverse_iterator _rbegin{nullptr}, _rend{nullptr};

  void erase(TreapNode *&);
};

template<typename Element>
class HullTree<Element>::iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Element;
    using pointer    = Element*;
    using reference  = Element&;

    iterator(HullTree<Element>::TreapNode*_ptr) : ptr(_ptr) {}

    reference operator*() const { return ptr->element; }
    pointer operator->() const { return &(ptr->element); }

    iterator& operator++() { ptr = ptr->next; return *this; }
    // iterator  operator++(int) { return ptr->next; }
    iterator& operator--() { ptr = ptr->prev; return *this; }
    // iterator  operator--(int) { return ptr->prev; }

    friend bool operator== (iterator const& a, iterator const& b)
    { return a.ptr == b.ptr; }
    friend bool operator!= (iterator const& a, iterator const& b)
    { return a.ptr != b.ptr; }

  private:

    TreapNode * ptr;
};

template<typename Element>
class HullTree<Element>::reverse_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Element;
    using pointer    = Element*;
    using reference  = Element&;

    reverse_iterator(HullTree<Element>::TreapNode*_ptr) : ptr(_ptr) {}

    reference operator*() const { return ptr->element; }
    pointer operator->() const { return &(ptr->element); }

    reverse_iterator& operator++() { ptr = ptr->prev; return *this; }
    // iterator  operator++(int) { return ptr->next; }
    reverse_iterator& operator--() { ptr = ptr->next; return *this; }
    // iterator  operator--(int) { return ptr->prev; }

    friend bool operator== (reverse_iterator const& a, reverse_iterator const& b)
    { return a.ptr == b.ptr; }
    friend bool operator!= (reverse_iterator const& a, reverse_iterator const& b)
    { return a.ptr != b.ptr; }

  private:

    TreapNode * ptr;
};


template<typename Element>
std::default_random_engine HullTree<Element>::engine;
template<typename Element>
std::uniform_int_distribution< int32_t > HullTree<Element>::rng;



template<typename Element>
inline HullTree<Element>::reverse_iterator const HullTree<Element>::rbegin() const
{ return _rbegin; }
template<typename Element>
HullTree<Element>::reverse_iterator const HullTree<Element>::rend() const
{ return _rend; }

template<typename Element>
inline HullTree<Element>::iterator const HullTree<Element>::begin() const
{ return _begin; }
template<typename Element>
inline HullTree<Element>::iterator const HullTree<Element>::end() const
{ return _end; }


template<typename Element>
struct HullTree<Element>::TreapNode {
  HullTree<Element>::priority_t priority;
  TreapNode *left = nullptr, *right = nullptr,
            *prev = nullptr, *next = nullptr;

  Element element;

  HullTree<Element>::size_t size;

  TreapNode(Element const &_element):
    priority(rng(engine)), element(_element) { }
};

template<typename Element>
HullTree<Element>::HullTree() { }

template<typename Element>
HullTree<Element>::HullTree(Element const& element) { 
  treap = new HullTree<Element>::TreapNode(element); 
  _begin = treap;
  _rbegin = treap;
  treap->size = 1;
}

template<typename Element>
void HullTree<Element>::destroy() { erase(treap); }

template<typename Element>
void HullTree<Element>::erase(TreapNode *& root) {
  if( root == nullptr ) return;
  erase(root->left), erase(root->right);
  delete root, root = nullptr;
}


template<typename Element>
template<typename Predicate>
void HullTree<Element>::__cut(const Predicate &predicate,
    TreapNode *treap_root, TreapNode *&left_root, TreapNode *&right_root) {
  if( treap_root == nullptr )
    return void(left_root = right_root = nullptr);
  if( predicate(iterator(treap_root)) ) {
    __cut(predicate, treap_root->left, left_root, treap_root->left);
    right_root = treap_root;
  } else {
    __cut(predicate, treap_root->right, treap_root->right, right_root);
    left_root = treap_root;
  }
  treap_root->size = 1 + (treap_root->left == nullptr ? 0 : treap_root->left->size) +
    (treap_root->right == nullptr ? 0 : treap_root->right->size);
}

template<typename Element>
template<typename Predicate>
void HullTree<Element>::cut(const Predicate &predicate,
    HullTree from, HullTree &left, HullTree &right) {

  __cut(predicate, from.treap, left.treap, right.treap);

  // update begin, _rbegin, prev, next
  left._begin = right._begin = nullptr;
  left._rbegin = right._rbegin = nullptr;
  auto ll = left.treap, lr = ll, rl = right.treap, rr = rl;
  while( ll != nullptr and ll->left != nullptr )  ll = ll->left;
  while( lr != nullptr and lr->right != nullptr ) lr = lr->right;
  while( rl != nullptr and rl->left != nullptr )  rl = rl->left;
  while( rr != nullptr and rr->right != nullptr ) rr = rr->right;
  if( rl != nullptr ) rl->prev = nullptr;
  if( lr != nullptr ) lr->next = nullptr;
  left._begin = ll, left._rbegin = lr, right._begin = rl, right._rbegin = rr;
}


template<typename Element>
void HullTree<Element>::__join(
    TreapNode *&root, TreapNode *left_root, TreapNode *right_root) {
  if( left_root == nullptr or right_root == nullptr )
    return void(root = ( left_root == nullptr ? right_root : left_root ));
  if( left_root->priority < right_root->priority ) {
    __join(right_root->left, left_root, right_root->left);
    root = right_root;
  } else {
    __join(left_root->right, left_root->right, right_root);
    root = left_root;
  }
  root->size = 1 + (root->left == nullptr ? 0 : root->left->size) +
    (root->right == nullptr ? 0 : root->right->size);
}

template<typename Element>
void HullTree<Element>::join(HullTree &to, HullTree left, HullTree right) {
  auto lt = left.treap, rt = right.treap;
  if( lt != nullptr and rt != nullptr ) {
    while(lt->right != nullptr) lt = lt->right;
    while(rt->left  != nullptr) rt = rt->left;
    lt->next = rt, rt->prev = lt;
  }
  to._begin = left.treap != nullptr ? left._begin : right._begin;
  to._rbegin = right.treap != nullptr ? right._rbegin : left._rbegin;
  __join(to.treap, left.treap, right.treap);
}


template<typename Element>
template<typename Predicate>
HullTree<Element>::iterator HullTree<Element>::binary_search(Predicate const& predicate) {
  HullTree<Element>::TreapNode *ptr = treap, *ret = nullptr;
  while(ptr != nullptr) {
    if( predicate(iterator(ptr)) ) ret = ptr, ptr = ptr->left;
    else ptr = ptr->right;
  }
  return iterator(ret);
}


template<typename Element>
HullTree<Element>::size_t HullTree<Element>::get_size() {
  return (treap == nullptr ? 0 : treap->size);
}

#include "HullTree.hh"


#include <iostream>
using namespace std;

int main() {

  HullTree<int> ht[3];

  int n; cin >> n;
  for(int i=1; i<=n; i++) {
    HullTree<int>::join(ht[0], ht[0], HullTree<int>(i));
  }

  for(auto it: ht[0]) {cerr << it << " ";} cerr << endl;

  int value = 32;
  auto dip = [&value](HullTree<int>::iterator it)
  { return *it > value; };

  auto mid = ht[0].binary_search(dip);
  for(auto it = mid; it != ht[0].end(); ++it) {
    cerr << *it << " ";
  }
  cerr << endl;

}

#include <algorithm>

template<typename TotalOrder>
class BalancedSegmentTree {

public :

    template<typename TotalOrder>
    struct TreeNode;

private :

    TreeNode<TotalOrder> * root;

};

template<typename TotalOrder>
struct BalancedSegmentTree::TreeNode {
    TreeNode * left, * right;
    int32_t height;
    TotalOrder lo, hi;

    BalancedSegmentTree::TreeNode(TotalOrder const& point) :
        left(nullptr), right(nullptr), lo(point), hi(point), height(1) { };

    BalancedSegmentTree::TreeNode(TotalOrder* const& _left, TotalOrder* const& _right):
        left(_left), right(_right) {
        height = 1+std::max(_left->height, _right->height);
        lo = left->lo, hi = right->hi;
    }
};

template<typename TotalOrder>
void BalancedSegmentTree::TreeNode::insert(TotalOrder const& point) {

}


int main() {

    return 0;
}

#include <algorithm>

class WeightBalancedTree {

public :

    struct TreeNode;

private :

    TreeNode * root;

    

};


template<typename T>
struct WeightBalancedTree::TreeNode {
    using WeightBalancedTree::TreeNode;
    using size_t = int32_t;
    TreeNode * left, * right;
    size_t size;

    
};


int main() {

    return 0;
}
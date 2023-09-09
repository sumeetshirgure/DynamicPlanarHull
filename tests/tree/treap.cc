
template<typename TotalOrder>
class OrderedSet
{

private :
	static std::default_random_engine engine;
	static std::uniform_int_distribution< int32_t > rng;

	struct TreapNode;

	template<typename Predicate>
	static void cut(const Predicate &,
		TreapNode *, TreapNode *&, TreapNode *&);

	static void join(TreapNode *&, TreapNode *, TreapNode *);

	TreapNode *root = nullptr;

public :

	using size_t = int32_t;
	using priority_t = int32_t;

};

template<typename TotalOrder>
struct OrderedSet<TotalOrder>::TreapNode
{
	OrderedSet<TotalOrder>::priority_t priority;
	OrderedSet<TotalOrder>::size_t size;

	TreapNode *left, *right;

	Point< Field > lo, hi;

	TreapNode(Point<Field> const &u, Point<Field> const &v):
		priority(rng(engine)), size(1),
		left(nullptr), right(nullptr),
		lo(u), hi(v) { }
};


template<typename TotalOrder>
template<typename Predicate>
void OnlineHull<Field>::cut(const Predicate &predicate, TotalOrder &split,
	TreapNode *treap_root, TreapNode *&left_root, TreapNode *&right_root)
{
	if( treap_root == nullptr )
	{
		left_root = right_root = nullptr;
		return;
	}
	if( predicate(*treap_root) )
	{
		split = treap_root->u;
		cut(predicate, split,
			treap_root->left, left_root, treap_root->left);
		right_root = treap_root;
	}
	else
	{
		split = treap_root->v;
		cut(predicate, split,
			treap_root->right, treap_root->right, right_root);
		left_root = treap_root;
	}
	treap_root->size = 1
		+ (treap_root->left  == nullptr ? 0 : treap_root->left ->size)
		+ (treap_root->right == nullptr ? 0 : treap_root->right->size);
}


template<typename Field>
void OnlineHull<Field>::join(TreapNode *&root,
	TreapNode *left_root, TreapNode *right_root)
{
	if( left_root == nullptr or right_root == nullptr )
	{
		root = ( left_root == nullptr ? right_root : left_root );
		return;
	}
	if( left_root->priority < right_root->priority )
	{
		join(right_root->left, left_root, right_root->left);
		root = right_root;
	}
	else
	{
		join(left_root->right, left_root->right, right_root);
		root = left_root;
	}
	root->size = 1
		+ (root->left  == nullptr ? 0 : root->left ->size)
		+ (root->right == nullptr ? 0 : root->right->size);
}

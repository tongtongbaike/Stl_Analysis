#include<iostream>
using namespace std;
enum COL
{
	RED,
	BLACK
};
template<class K, class V>
struct RBTreeNode
{
	K _key;
	V _val;
	RBTreeNode<K, V>* _left;
	RBTreeNode<K, V>* _right;
	RBTreeNode<K, V>* _parent;
	COL _col;
	RBTreeNode(K& key, V& val)
		:_key(key)
		, _val(val)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
		, _col(RED)
	{}
};
template<class K, class V>
class RBTree{
	typedef RBTreeNode<K, V> Node;
public:
	RBTree()
		:_root(NULL)
	{}
	bool Insert(K& key, V& val)
	{
		if (_root == NULL)
		{
			_root = new Node(key, val);
		}
		else
		{
			Node* parent = NULL;
			Node* cur = _root;
			while (cur)
			{
				if (cur->_key < key)
				{
					parent = cur;
					cur = cur->_right;
				}
				else if (cur->_key>key)
				{
					parent = cur;
					cur = cur->_left;
				}
				else
				{
					return false;
				}
			}
			cur = new Node(key, val);
			if (parent->_key < key)
				parent->_right = cur;
			else
				parent->_left = cur;
			cur->_parent = parent;

			while (cur != _root&&parent->_col == RED)
			{
				Node* grandfather = parent->_parent;
				if (grandfather->_left == parent)
				{
					Node* uncle = grandfather->_right;
					if (uncle&&uncle->_col == RED)//1.
					{
						parent->_col = uncle->_col = BLACK;
						grandfather->_col = RED;


						cur = grandfather;
						parent = cur->_parent;
					}
					else
					{
						if (cur == parent->_right)
						{
							RotateL(parent);
							swap(parent, cur);
						}
						parent->_col = BLACK;
						grandfather->_col = RED;
						RotateR(grandfather);
						break;
					}
				}
				else
				{
					Node* uncle = grandfather->_left;
					if (uncle&&uncle->_col == RED)
					{
						parent->_col = uncle->_col = BLACK;
						grandfather->_col = RED;

						cur = grandfather;
						parent = cur->_parent;
					}
					else
					{
						if (cur == parent->_left)
						{
							RotateR(parent);
							swap(cur, parent);
						}
						parent->_col = BLACK;
						grandfather->_col = RED;
						RotateL(grandfather);
						break;
					}
				}
			}
		}
		_root->_col = BLACK;
		return true;
	}
	Node* Find(const K& key)
	{
		Node* cur = _root;
		while (cur)
		{
			if (cur->_key < key)
				cur = cur->_right;
			else if (cur->_key>key)
				cur = cur->_left;
			else
				return cur;
		}
		return NULL;
	}

	bool isBalance()
	{
		if (_root == NULL)
			return true;
		if (_root->_col == RED)
			return false;
		int k = 0;
		Node* cur = _root;
		while (cur)
		{
			if (cur->_col == BLACK)
				++k;
			cur = cur->_left;
		}
		int count = 0;
		return _IsBalance(_root, k, count);
	}
	void InOrder()
	{
		_InOrder(_root);
	}
protected:
	void _InOrder(Node* root)
	{
		if (root == NULL)
			return;
		_InOrder(root->_left);
		cout << root->_key << " ";
		//      cout << root->_key << "color"<<root->_col << "  ";
		_InOrder(root->_right);
	}
	bool _IsBalance(Node* root, const int k, int count)
	{
		if (root == NULL)
			return true;
		if (root->_col == RED)
		{
			if (root->_parent->_col == RED)
			{
				cout << "颜色不正确" << root->_key << endl;
				return false;
			}
		}
		else
			++count;
		if (root->_left == NULL&&root->_right == NULL)
		{
			if (count == k)
				return true;
			else
			{
				cout << "黑色节点个数不对" << root->_key << endl;
				return false;
			}
		}
		return _IsBalance(root->_left, k, count) && _IsBalance(root->_right, k, count);
	}
	void RotateL(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		parent->_right = subRL;
		if (subRL)
			subRL->_parent = parent;

		Node* ppNode = parent->_parent;
		subR->_left = parent;
		parent->_parent = subR;
		if (ppNode == NULL)
		{
			subR->_parent = NULL;
			_root = subR;
		}
		else
		{
			if (ppNode->_left == parent)
			{
				ppNode->_left = subR;
			}
			else
			{
				ppNode->_right = subR;
			}
			subR->_parent = ppNode;
		}
	}
	void RotateR(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		parent->_left = subLR;
		if (subLR)
			subLR->_parent = parent;

		Node* ppNode = parent->_parent;
		subL->_right = parent;
		parent->_parent = subL;
		if (ppNode == NULL)
		{
			subL->_parent = NULL;
			_root = subL;
		}
		else
		{
			if (ppNode->_left == parent)
			{
				ppNode->_left = subL;
			}
			else
			{
				ppNode->_right = subL;
			}
			subL->_parent = ppNode;
		}
	}
private:
	Node* _root;
};
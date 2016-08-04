#include<iostream>
using namespace std;

template<class Kclass V>
struct AVLTreeNode
{
	AVLTreeNode(K& key, V& val)
	:_key(key)
	, _val(val)
	, _left(NULL)
	, _right(NULL)
	, _parent(NULL)
	, _bf(0)
	{}
	K _key;
	V _val;
	AVLTreeNode<K, V>* _left;
	AVLTreeNode<K, V>* _right;
	AVLTreeNode<K, V>* _parent;
	int _bf;
};
template<class K, class V>
class AVLTree
{
	typedef AVLTreeNode<K, V> Node;
public:
	AVLTree()
		:_root(NULL)
	{}
	bool Insert(K& key, V& val)
	{
		if (_root == NULL)
		{
			_root = new Node(key, val);
			return true;
		}
		else
		{
			Node* cur = _root;
			Node* parent = NULL;
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
					return false;
			}
			cur = new Node(key, val);
			if (parent->_key > key)
			{
				parent->_left = cur;
				cur->_parent = parent;
			}
			else
			{
				parent->_right = cur;
				cur->_parent = parent;
			}


			while (parent != NULL)
			{
				if (cur == parent->_left)
					++parent->_bf;
				else
					--parent->_bf;

				if (parent->_bf == 0)
					break;
				else if (parent->_bf == 1 || parent->_bf == -1)
				{
					cur = parent;
					parent = cur->_parent;
				}
				else
				{
					if (parent->_bf == 2)
					{
						if (1 == cur->_bf)//×óÐýÐý
						{
							RotateL(parent);
						}
						else
						{
							RotateRL(parent);
						}
					}
					else
					{
						if (1 == cur->_bf)//ÓÒ×óÐý
						{
							RotateLR(parent);
						}
						else//ÓÒÐý
						{
							RotateR(parent);
						}
					}
					break;
				}
			}
			return true;
		}
	}
	Node* Find(K& key)
	{
		if (_root == NULL)
			return false;
		else
		{
			Node* cur = _root;
			while (cur)
			{
				if (cur->_key > key)
					cur = cur->_left;
				else if (cur->_key < key)
					cur = cur->_right;
				else
					return cur;
			}
			return NULL;
		}
	}
	void InOrder()
	{
		_InOrder(_root);
	}
	bool IsBalance()
	{
		return _IsBalance(_root);
	}
	int Height()
	{
		return _Height(_root);
	}
protected:
	int _Height(Node* root)
	{
		if (root == NULL)
			return 0;
		int left = _Height(root->_left);
		int right = _Height(root->_right);
		return left > right ? left + 1 : right + 1;
	}
	bool _IsBalance(Node* root)
	{
		if (root == NULL)
			return true;
		int left = _Height(root->_left);
		int right = _Height(root->_right);
		if (left - right != root->_bf)
		{
			cout << "²»Æ½ºâ" << root->_key << endl;
			return false;
		}
		return abs(left - right)<2 && _IsBalance(root->_left) && _IsBalance(root->_right);
	}
	void _InOrder(Node* root)
	{
		if (root == NULL)
			return;
		_InOrder(root->_left);
		cout << root->_key << " ";
		_InOrder(root->_right);
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
			_root = subR;
			subR->_parent = NULL;
		}
		else
		{
			if (ppNode->_left == parent)
				ppNode->_left = subR;
			else
				ppNode->_right = subR;
			subR->_parent = ppNode;
		}
		subR->_bf = parent->_bf = 0;
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
			_root = subL;
			subL->_parent = NULL;
		}
		else
		{
			if (ppNode->_left == parent)
				ppNode->_left = subL;
			else
				ppNode->_right = subL;
			subL->_parent = ppNode;
		}
		subL->_bf = parent->_bf = 0;
	}
	void RotateLR(Node* parent)
	{
		Node* subL = parent->_left;
		Node* subLR = subL->_right;
		int bf = subLR->_bf;
		RotateL(parent->_left);
		RotateR(parent);
		if (bf == 1)
		{
			parent->_bf = -1;
			subL->_bf = 0;
		}
		else if (bf == -1)
		{
			parent->_bf = 0;
			subL->_bf = 1;
		}
		else
			subL->_bf = parent->_bf = 0;
	}
	void RotateRL(Node* parent)
	{
		Node* subR = parent->_right;
		Node* subRL = subR->_left;
		int bf = subRL->_bf;
		RotateR(parent->_right);
		RotateL(parent);
		if (bf == 1)
		{
			parent->_bf = 0;
			subR->_bf = -1;
		}
		else if (bf == -1)
		{
			parent->_bf = 1;
			subR->_bf = 0;
		}
		else
			subR->_bf = parent->_bf = 0;
	}
private:
	Node* _root;
};
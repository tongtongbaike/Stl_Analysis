#pragma once
#include<iostream>
using namespace std;
template<class K, int M>
struct BTreeNode
{
	K  _keys[M];
	BTreeNode<K, M>* _subs[M + 1];
	BTreeNode<K, M>* _parent;
	size_t _size;
	BTreeNode()
		:_parent(NULL)
		, _size(0)
	{
		for (int i = 0; i < M; ++i)
		{
			_keys[i] = K();
			_subs[i] = NULL;
		}
		_subs[M] = NULL;
	}
};
template<class K, class V, int M>
struct BTreeNodeKV
{
	pair<K, V> _kvs[M];
	BTreeNodeKV<K, V, M>* _subs[M + 1];
	BTreeNodeKV<K, V, M>* _parent;
	size_t _size;
};
template<class K, int M>
class BTree
{
	typedef BTreeNode<K, M> Node;
public:
	BTree()
		:_root(NULL)
	{}
	pair<Node*, int> Find(K& key)
	{
		if (_root == NULL)
			return pair<Node*, int>(NULL, -1);
		Node* cur = _root;
		Node* parent = NULL;
		while (cur)
		{
			int size = cur->_size;
			int i;
			for (i = 0; i < size;)
			{
				if (cur->_keys[i] == key)
					return pair<Node*, int>(cur, i);
				else if (cur->_keys[i] < key)
				{
					++i;
				}
				else
					break;
			}
			parent = cur;
			cur = cur->_subs[i];
		}
		return pair<Node*, int>(parent, -1);
	}
	void _Insert(Node* cur, K& key, Node* sub)
	{
		int end = cur->_size - 1;
		while (end >= 0)
		{
			if (cur->_keys[end] > key)
			{
				cur->_keys[end + 1] = cur->_keys[end];
				cur->_subs[end + 2] = cur->_subs[end + 1];
				--end;
			}
			else
				break;
		}
		cur->_keys[end + 1] = key;
		cur->_subs[end + 2] = sub;
		++cur->_size;
		if (sub)
			sub->_parent = cur;
	}
	bool Insert(K& key)
	{
		if (_root == NULL)
		{
			_root = new Node;
			_root->_keys[0] = key;
			_root->_size = 1;
			return true;
		}
		else
		{
			pair<Node*, int> ret = Find(key);
			if (ret.second != -1)
				return false;
			else
			{
				Node* cur = ret.first;
				K newkey = key;
				Node* insert_sub = NULL;
				while (1)
				{
					_Insert(cur, newkey, insert_sub);
					if (cur->_size<M)
						break;
					int div = M / 2;
					Node* sub = new Node;
					int index = 0;
					int i = div + 1;
					while (i < M)
					{
						sub->_keys[index] = cur->_keys[i];
						cur->_keys[i] = K();
						sub->_subs[index] = cur->_subs[i];
						cur->_subs[i] = NULL;
						++index;
						++i;
						++sub->_size;
					}
					sub->_subs[index] = cur->_subs[i];
					cur->_subs[i - 1] = NULL;
					cur->_size -= (index + 1);

					if (cur->_parent == NULL)
					{
						_root = new Node;
						_root->_keys[0] = cur->_keys[div];
						cur->_keys[div] = K();
						_root->_subs[0] = cur;
						cur->_parent = _root;
						_root->_subs[1] = sub;
						sub->_parent = _root;
						_root->_size = 1;
						return true;
					}
					else
					{
						insert_sub = sub;
						newkey = cur->_keys[div];
						cur = cur->_parent;
					}
				}
				return true;
			}
		}
	}
	void InOrder()
	{
		_InOrder(_root);
		cout << endl;
	}
protected:
	void _InOrder(Node* root)
	{
		if (root == NULL)
			return;
		int i = 0;
		for (i = 0; i < root->_size; ++i)
		{
			_InOrder(root->_subs[i]);
			cout << root->_keys[i] << " ";
		}
		_InOrder(root->_subs[i]);
	}
protected:
	Node* _root;
};
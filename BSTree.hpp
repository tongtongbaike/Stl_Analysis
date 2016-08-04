#pragma once
#include <iostream>
using namespace std;


template <class K,class V>
struct BSTreeNode
{
	K _key;
	V _value;

	BSTreeNode<K, V> _left;
	BSTreeNode<K, V> _right;

	BSTreeNode(const K &key,const V &value)
		:_key(key)
		, _value(value)
		, _left(NULL)
		, _right(NULL)
	{
	}
};

template <class K,class V>
class BSTree
{
	typedef BSTreeNode<K,V> Node;
public:
	BSTree();
	~BSTree();
	bool Insert(K &key, V &value);
	Node* Find(K &key);
	bool Remote(K &key);
	void Destroy();

	bool Insert_R(K &key, V &value);
	Node* Find_R(K &key, V &value);
	bool Remote_R(K &key);
protected:
	void _Insert_R(Node*& root, K &key, V &value);
	bool _Remote_R(Node*& root, K &key);
	Node* _Find_R(Node*& root, K &key);
	
	void Destroy(Node* root);
private:
	Node* _root;

};

template <class K, class V>
void BSTree<K, V>::Destroy()
{
	_Destroy(_root);
}

template <class K, class V>
void BSTree<K, V>::Destroy(Node* root)
{
	if (root == NULL)
		return;
	_Destroy(root->_left);
	_Destroy(root->_right);
	
	delete root;
}

template <class K,class V>
BSTree<K, V>::BSTree()
:_root(NULL)
{
}

template <class K, class V>
BSTree<K, V>::~BSTree()
{
	Destroy();
	_root = NULL;
}

template <class K, class V>
bool BSTree<K, V>::Insert(K &key, V &value)
{
	if (_root == NULL)
	{
		_root = new Node(key, value);
		return true;
	}
	Node *parent = NULL;
	Node *cur = _root;
	while (cur)
	{
		if (cur->_key > key)
		{
			parent = cur;
			cur = cur->_left;
		}
		else if (cur->_key < key)
		{
			parent = cur;
			cur = cur->_right;
		}
		else
		{
			cout << "插入节点已经存在" << endl;
			return false;
		}
	}
	cur = new Node(key, value);
	if (parent->_key > key)
		parent->_left = cur;
	if (parent->_key < key)
		parent->_right = cur;
	return true;
}

template <class K, class V>
Node* BSTree<K, V>::Find(K &key)
{
	if (_root == NULL)
	{
		return NULL;
	}
	Node *cur = _root;
	while (cur)
	{
		if (cur->_key > key)
		{
			cur = cur->_left;
		}
		else if (cur->_key < key)
		{
			cur = cur->_right;
		}
		else
		{
			cout << "节点key:"<<cur->_key<<"节点value:"<<cur->_value << endl;
			return cur;
		}
	}
	cout << "节点不存在" << endl;
	return NULL;
}

template <class K, class V>
bool BSTree<K, V>::Remote(K &key)
{
	if (_root == NULL)
	{
		return false;
	}
	Node* del = NULL;
	Node *parent = NULL;
	Node *cur = _root;
	while (cur)
	{
		if (cur->_key > key)
		{
			parent = cur;
			cur = cur->_left;
		}
		else if (cur->_key < key)
		{
			parent = cur;
			cur = cur->_right;
		}
		else
			break;
	}
	if (cur->_left == NULL)
	{
		del = cur;
		if (parent == NULL)
			_root = cur->_right;
		else{
			if (parent->_right == cur)
				parent->_right = cur->_right;
			else
				parent->_left = cur->_right;

		}
	}
	else if (cur->_right == NULL)
	{
		del = cur;
		if (parent == NULL)
			_root = cur->_left;
		else{
			if (parent->_right == cur)
				parent->_right = cur->_left;
			else
				parent->_left = cur->_left;
		}
	}
	else{
		Node* tmp = cur;
		parent = cur;
		cur = cur->_right;

		while (cur->_left)
		{
			parent = cur;
			cur = cur->_left;
		}

		tmp->_key = cur->_key;
		tmp->_value = cur->_value;
		del = cur;
		if (parent->_right == cur)
			parent->_right = cur->_right;
		if (parent->_left == cur)
			parent->right = cur->_right;
	}
	delete del;
	return true;
}



template <class K, class V>
bool BSTree<K, V>::Insert_R(K &key, V &value)
{
	return _Insert_R(_root, key, value);
}

template <class K, class V>
Node* BSTree<K, V>::Find_R(K &key, V &value)
{
	return _Find_R(_root, key);
}

template <class K, class V>
bool BSTree<K, V>::Remote_R(K &key)
{
	return _Remote_R(_root,key);
}

template <class K, class V>
void BSTree<K, V>::_Insert_R(Node*& root, K &key, V &value)
{
	if (root == NULL)
	{
		Node* cur = new Node(key, value);
		root = cur;
		return true;
	}

	if (root->_key > key)
		return _Insert_R(root->_left, key, value);
	else if (root->_key < key)
		return _Insert_R(root->_right, key, value);
	else
		return false;
}


template <class K, class V>
bool BSTree<K, V>::_Remote_R(Node*& root, K &key)
{
	if (root == NULL)
		return false;

	if (root->_key > key)
		return _Remote_R(root->_left, key);
	else if (root->_key < key)
		return _Remote_R(root->_right, key);
	else
	{
		Node *del = root;
		if (root->_left == NULL)
		{
			root = root->_right;
		}
		else if (root->_right == NULL)
		{
			root = root->_left;
		}
		else
		{
			Node*tmp = root->_right;
			Node* parent = root;
			while (tmp->_left)
			{
				parent = tmp;
				tmp = tmp->_left;
			}
			root->_key = tmp->_key;
			root->_value = tmp->_value;

			if (parent->_left == tmp)
				parent->_left = tmp->_right;
			if (patent->_right == tmp)
				parent->_right = tmp->_right;
			//return _Remote_R(root->_right, key);
		}
		delete del;
	}
	return true;
}

template <class K, class V>
void BSTree<K, V>::_Find_R(Node*& root, K &key)
{
	if (NULL == root)
		return NULL;
	if (root->_key > key)
		_Find_(root->_left, key, value);
	else if (root->_key < key)
		_Find_(root->_right, key, value);
	else
		return root;
}

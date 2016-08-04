#pragma once
#include<iostream>
#include<assert.h>
#include "Iterator.hpp"
//List�ڵ�
template <class T>
struct ListNode
{
	__ListNode()
	:_next(NULL)
	, _prev(NULL)
	{
	}

	__ListNode(const T &x)
		:_data(x)
		, _next(NULL)
		, _prev(NULL)
	{
	}

	T _data;
	ListNode<T> *_next;
	ListNode<T> *_prev;
	
};


//ʵ��List�ĵ�����
template<class T>
class __ListIterator
{
public:
	typedef __ListIterator<T, Ref, Ptr> Self;
	typedef BidirectionalIteratorTag IteratorCategory;
	typedef int DifferenceType;
	typedef T ValueType;
	typedef Ref Reference;
	typedef Ptr Pointer;
	
	__ListNode<T>* _node;
	__ListIterator()
	{}
	__ListIterator(__ListNode<T>* node)
		:_node(node)
	{}
	Reference operator*()
	{
		return _node->_data;
	}
	//�������д洢�ṹ��ʱ����Ҫ���ʽṹ���Ա����
	Ptr& operator->()
	{
		//
		//return &(operator*());
		return &(_node->_data);
		//List<AA>::Iterator it=l.Begin();
		//cout<<it->_a<<endl;//�������Ż�������˵��Ҫ����->->
	}
	bool operator==(const Self& s)
	{
		return _node == s._node;
	}
	bool operator!=(const Self& s)
	{
		return _node != s._node;
	}
	Self& operator++()
	{
		_node = _node->_next;
		return *this;
	}
	Self operator++(int)
	{
		Self tmp(*this);
		_node = _noode->_next;
		return tmp;
	}
	Self& operator--()
	{
		_node = _node->_prev;
		return *this;
	}
	Self operator--(int)
	{
		Self tmp(*this);
		_node = _node->_prev;
		return tmp;
	}
	DifferenceType Size()
	{
		return Distance(Begin(), End());
	}
	Reference operator+=(DifferenceType n)
	{
		while (n--)
		{
			++*this;
		}
		return *this;
	}
	Self operator+(DifferenceType n)
	{
		Self tmp(*this);
		tmp += n;
		return tmp;
	}
	Reference operator-=(DifferenceType n)
	{
		while (n--)
		{
			--*this;
		}
		return *this;
	}
	Self operator-(DifferenceType n)
	{
		Self tmp(*this);
		tmp -= n;
		return tmp;
	}
};

//ʵ��һ��˫��ѭ���б������ڱ�λ
template <class T,class Alloc = alloc>
class List
{
//List�еĵ��������ͽڵ㣬dataֵtypedef
public:
	typedef __ListIterator<T, T&, T*> Iterator;
	typedef __ListIterator<T,const T&,const T*> ConstIterator;
	typedef T ValueType;
	typedef __ListNode <T>*LinkType;

	typedef ReverseIterator<ConstIterator> ConstReverseIterator;
	typedef ReverseIterator<Iterator> ReverseIterator;
	typedef T ValueType;
	typedef T* Pointer;
	typedef const T* ConstPointer;
	typedef ValueType& Reference;
	typedef const ValueType& ConstReference;
	typedef int DifferenceType;

	List()
	{
		head._next(NULL);
		head._prev(NULL);
	}
	
	~List()
	{
		Clear();
	}

public:
	//�ڵ�ǰ����һ���ڵ�
	void Insert(Iterator pos, const ValueType&x)
	{
		LinkType tmp = new LinkType(x);
		LinkType prev = pos._node->prev;
		LinkType cur = pos._node;

		tmp->_prev = prev;
		prev->_next = tmp;

		tmp->_next = cur;
		cur->_prev = tmp;
	}
	//ɾ����ǰ�ڵ�
	Iterator Erease(Iterator pos)
	{
		LinkType prev = pos._node->_prev;
		LinkType next = pos._node->_next;
		
		prev->_next = next;
		next->_prev = prev;
		
		delete pos,_node;
		return Iterator(next);
	}

	//PushBack()
	void PushBack(const T& x)
	{
		Insert(End(), x);
	}

	//PushFront()
	void PushFront(const T& x)
	{
		Insert(Begin(), x);
	}
	//PopBack()
	void PopBack()
	{
		Erease(--End(), x);
	}
	//PopFront()
	void PopFront()
	{
		Erease(Begin());
	}

	//�������ڲ�����ʽǿ������ת��������Ҫֱ�ӷ���Iterator,��һ����Ч����
	Iterator Begin()
	{
		return _head._next;
	}

	//���һ�����ݵ���һ���ڵ�;
	Iterator End()
	{
		return _head;
	}
	//�������ڲ�����ʽǿ������ת��������Ҫֱ�ӷ���Iterator,��һ����Ч����
	ConstIterator Begin() const
	{
		return _head._next;
	}

	//���һ�����ݵ���һ���ڵ�;
	ConstIterator End() const
	{
		return _head;
	}

	ReverseIterator RBegin()
	{
		return ReverseIterator(End());
	}
	ReverseIterator REnd()
	{
		return ReverseIterator(Begin());
	}
	ConstReverseIterator RBegin()const
	{
		return ConstReverseIterator(End());//explicit
	}
	ConstReverseIterator REnd()const
	{
		return ConstReverseIterator(Begin());//explicit
		
	}

	void clear()
	{
		Iterator begin= Begin();
		while (Begin != End())
		{
			LinkType del = begin._node;
			++begin;
			delete del;
		}
	}

private:
	LinkType _head;
};
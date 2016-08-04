#pragma once
#include<iostream>
using namespace std;
struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag{};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};


template<class Iterator>
struct IteratorTraits
{
	typedef typename Iterator::IteratorCategory IteratorCategory;
	typedef typename Iterator::ValueType        ValueType;
	typedef typename Iterator::DifferenceType   DifferenceType;
	typedef typename Iterator::Pointer           Pointer;
	typedef typename Iterator::Reference         Reference;

};
template<class T>
struct IteratorTraits<T*>
{
	typedef typename RandomAccessIteratorTag IteratorCategory;
	typedef typename T        ValueType;
	typedef typename int   DifferenceType;
	typedef typename T*           Pointer;
	typedef typename T&         Reference;
};
template<class T>
struct IteratorTraits<const T*>
{
	typedef typename RandomAccessIteratorTag IteratorCategory;
	typedef typename T        ValueType;
	typedef typename int   DifferenceType;
	typedef typename T*           Pointer;
	typedef typename T&         Reference;
};
template <class InputIterator>
int  __Distance(InputIterator first, InputIterator last, InputIteratorTag)
{
	int n = 0;
	while (first != last) {
		++first; ++n;
	}
	return n;
}
template <class RandomAccessIterator>
int __Distance(RandomAccessIterator first, RandomAccessIterator last,
	RandomAccessIteratorTag)
{
	return last - first;
}
template <class InputIterator>
int  Distance(InputIterator first, InputIterator last)
{
	return __Distance(first, last, IteratorTraits <InputIterator>::IteratorCategory());
}

template <class Iterator>
class ReverseIterator
{
protected:
	Iterator _cur;
	typedef ReverseIterator<Iterator> Self;
	typedef typename IteratorTraits<Iterator>::IteratorCategory IteratorCategory;
	typedef typename IteratorTraits<Iterator>::DifferenceType DifferenceType;
	typedef typename IteratorTraits<Iterator>::ValueType  ValueType;
	typedef typename IteratorTraits<Iterator>::Pointer  Pointer;
	typedef typename IteratorTraits<Iterator>::Reference  Reference;


public:
	ReverseIterator()
	{}
	explicit ReverseIterator(Iterator i)
		:_cur(i)
	{}
	Reference operator*()
	{
		Iterator tmp = _cur;
		return *(--tmp);
	}
	//当链表中存储结构体时，需要访问结构体成员变量
	Pointer& operator->()
	{
		//
		//return &(operator*());
		return _cur.operator->();
		//List<AA>::Iterator it=l.Begin();
		//cout<<it->_a<<endl;//编译器优化，按理说需要两个->->
	}
	bool operator==(const Self& s)
	{
		return _cur == s._cur;
	}
	bool operator!=(const Self& s)
	{
		return _cur != s._cur;
	}
	Self& operator++()
	{
		--_cur;
		return *this;
	}
	Self operator++(int)
	{
		Self tmp(*this);
		--_cur;
		return tmp;
	}
	Self& operator--()
	{
		++_cur;
		return *this;
	}
	Self operator--(int)
	{
		Self tmp(*this);
		++_cur;
		return tmp;
	}
	Self& operator+=(int n)
	{
		_cur += n;
		return *this;
	}
	Self& operator-=(int n)
	{
		_cur -= n;
		return *this;
	}
	Self operator+(int n)
	{
		return Self(*this + n);
	}
	Self operator-(int n)
	{
		return Self(*this - n);
	}
	Reference operator[](DifferenceType n) const
	{
		return *(*this + n);
	}
	//Iterator需实现operator+(n);
};


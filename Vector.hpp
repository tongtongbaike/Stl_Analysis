#pragma once
#include<iostream>
#include<vector>
#include<list>
#include<assert.h>
#include"Iterator.hpp"


using namespace std;
template<class T>
class Vector
{
public:
	typedef T* Iterator;
	typedef const T* ConstIterator;
	typedef ReverseIterator<ConstIterator> ConstReverseIterator;
	typedef ReverseIterator<Iterator> ReverseIterator;
	typedef T ValueType;
	typedef T* Pointer;
	typedef T& Reference;
	typedef int DifferenceType;
protected:

	Iterator _start;
	Iterator _finish;
	Iterator _storage;
	void _CheckCapacity()
	{
		if (_finish == _storage)
		{
			int capacity = _storage - _start;
			int newCapacity = capacity * 2 + 3;
			T* tmp = new T[newCapacity];
			for (int i = 0; i < capacity; ++i)
			{
				//operator=
				tmp[i] = _start[i];//不能用memcpy，涉及指针拷贝
			}
			delete[] _start;
			_start = tmp;
			_finish = _start + capacity;
			_storage = _start + newCapacity;
		}
	}
public:
	Vector()
		:_start(NULL)
		, _finish(NULL)
		, _storage(NULL)
	{}
	Vector(size_t size)
		:_start(new T[size])
		, _finish(_start)
		, _storage(_start + size)
	{}
	~Vector()
	{
		if (_start)
			delete[] _start;
	}
	void PushBack(const T& x)
	{
		_CheckCapacity();
		*_finish = x;
		++_finish;
	}
	void PopBack()
	{
		assert(_finish > _start);
		--_finish;
	}
	Iterator Begin()
	{
		return _start;
	}
	Iterator End()
	{
		return _finish;
	}
	ReverseIterator RBegin()
	{
		return ReverseIterator(End());
	}
	ReverseIterator REnd()
	{
		return ReverseIterator(Begin());
	}
	DifferenceType Size()
	{
		return _finish - _start;
	}
	DifferenceType Capacity()
	{
		return _storage - _start;
	}
	Reference operator[](int index)
	{
		assert(index < Size());
		return _start[index];
	}
	ValueType at(int index)
	{
		assert(index < Size());
		return _start[index];
	}
};

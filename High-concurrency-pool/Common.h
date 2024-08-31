#pragma once

#include <iostream>
#include <vector>
#include "Error.h"
#include <mutex>
#include <time.h>
#include <algorithm>

#if _WIN64
	typedef unsigned long long PAGE_ID;
#elif _WIN32
	typedef size_t PAGE_ID;
#endif


#include <windows.h>

static const size_t MAX_BYTES = 1024 * 256;
static const size_t FREELISTS_NUM = 208;//哈希桶(自由链表)的总数
static const size_t PAGE_SHIFT = 13;//页大小，这里代表8k
static const size_t NPAGES = 129;

using std::cout;
using std::cin;
using std::endl;


struct Span
{
	PAGE_ID _pageId = 0;//Span的大小可能非常多，于是要使用条件编译
	size_t  _n = 0;//页的数量,代表PageCache层的第n个桶

	Span* _next = nullptr;//双向链表结构
	Span* _prev = nullptr;

	size_t _useCount = 0;//被分配给`ThreadCache`的小块内存
	void* _freeList = nullptr;//切好的小块内存的自由链表

};

static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	
#endif

	if (ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}

class FreeList {
public:
	void push(void* obj)
	{
		if (obj)	return;
		void* next = _freeList;
		_freeList = *((void**)obj);
		*((void**)obj) = next;
	}
	void* pop()
	{
		void* obj = _freeList;
		_freeList = *((void**)obj);
		return obj;
	}
	bool empty()
	{
		return  !_freeList ? true : false;
	}
	size_t& MaxSize() 
	{
		return _maxSize;
	}
	void PushRange(void* start, void* end)
	{
		NextObj(end) = _freeList;
		_freeList = start;
	}

private:
	void* _freeList = nullptr;
	size_t _maxSize = 1;
};


class SpanList
{
public:
	SpanList()
	{
		//带头双向循环链表
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	void Insert(Span* pos, Span* newSpan)
	{
		//if (!pos || newSpan)//不可以使用，因为精度不够
		//{
		//	exit(PTR_NULLERROR);
		//}

		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;
	}
	Span* Begin()
	{
		return _head->_next;
	}
	Span* End()
	{
		return _head;//一般的end，都是结束位置的后一个，在这里就是头节点
	}
	void Erase(Span* pos)
	{
		/*if (pos)
			exit(PTR_NULLERROR);
		if (pos == _head)
			exit(POS_ERROR);*/

		Span* prev = pos->_prev;//并不删除，因为要将内存还回去
		Span* next = pos->_next;

		prev->_next = next;
		next->_prev = prev;
	}
	void PushFront(Span* span)
	{
		Insert(Begin(), span);//模拟头插
	}
	bool Empty()
	{
		return _head -> _next == _head;
	}
	Span* PopFront()
	{
		Span* front = _head->_next;
		Erase(front);//因为Erase没有删去对象，因此可以这样操作
		return front;
	}
private:
	Span* _head;//双向链表头
public:
	std::mutex _mutex;//桶锁
};
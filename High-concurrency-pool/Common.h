#pragma once

#include <iostream>
#include <vector>
#include "Error.h"
#include <mutex>

#if _WIN64
	typedef unsigned long long PAGE_ID;
#elif _WIN32
	typedef size_t PAGE_ID;
#endif
static const size_t MAX_BYTES = 1024 * 256;
static const size_t FREELISTS_NUM = 208;//哈希桶(自由链表)的总数

using std::cout;
using std::cin;
using std::endl;

struct Span
{
	PAGE_ID _pageId = 0;//Span的大小可能非常多，于是要使用条件编译
	size_t  _n = 0;//页的数量

	Span* _next = nullptr;//双向链表结构
	Span* _prev = nullptr;

	size_t _useCount = 0;//被分配给`ThreadCache`的小块内存
	void* _freeList = nullptr;//切好的小块内存的自由链表
};

static void*& NextObj(void* obj)
{
	return *(void**)obj;
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
private:
	void* _freeList = nullptr;

};


class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	void Insert(Span* pos, Span* newSpan)
	{
		if (!pos || newSpan)
		{
			exit(PTR_NULLERROR);
		}

		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;
	}

	void Erase(Span* pos)
	{
		if (pos)
			exit(PTR_NULLERROR);
		if (pos == _head)
			exit(POS_ERROR);

		Span* prev = pos->_prev;//并不删除，因为要将内存还回去
		Span* next = pos->_next;

		prev->_next = next;
		next->_prev = prev;
	}

private:
	Span* _head;//双向链表头
	std::mutex _mutex;//桶锁
};
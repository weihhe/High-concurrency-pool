#pragma once

#include "Common.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_sInst;
	}
	//获取一个"K页"的span
	Span* NewSpan(size_t k);
	std::mutex _pageMtx;//不适用桶锁，而是一把公共锁
private:
	SpanList _spanLists[NPAGES];
	PageCache()
	{}
	PageCache(const PageCache&) = delete;

	static PageCache _sInst;
};
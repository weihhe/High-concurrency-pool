#pragma once
#include "Common.h"
#include "PageCache.h"
#include "Size.h"


class CentralCache{
public:
	static CentralCache* GetInstance() //全局唯一可以获取资源的函数，单例模式
	{
		return &_sInst;
	}
	//获取一个非空的span
	Span* GetOneSpan(SpanList& list, size_t byte_size);
	//`Thread Cache`向中心缓存获取一定数量的对象
	size_t FetchRangeObj(void*& start, void*& end, size_t batchNum, size_t size);
private:
	SpanList _spanLists[FREELISTS_NUM];
	CentralCache()
	{}
	CentralCache(const CentralCache&) = delete;
	static CentralCache _sInst;
};
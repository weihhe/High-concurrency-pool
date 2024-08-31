#include "CentralCache.h"
#include <assert.h>
#include "Size.h"
#include <thread>

CentralCache CentralCache:: _sInst;

//分配缓存给Thread Cache,由于可能多个线程都向该页索要，因此要加上锁
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t batchNum, size_t size)
{
	size_t index = Size::Index(size); // 计算是哪个桶内的
	_spanLists[index]._mutex.lock();//加锁

	Span* span = CentralCache::GetOneSpan(_spanLists[index], size);
	
	assert(span);//保证页不为空
	assert(span->_freeList);//当前页仍有空间
	start = span->_freeList;
	end = start;//从start开始
	size_t i = 0;
	size_t actualNum = 1;//start算一个
	while (i < batchNum - 1 && NextObj(end) != nullptr)//需要让当前链表可取对象数量大于batchNum个，如果没有。则有多少拿多少
	{
		end = NextObj(end);
		++i;
		++actualNum;
	}
	span->_freeList = NextObj(end);//定位到n个节点之后的新位置
	NextObj(end) = nullptr;//将取出空间的末尾置为空
	span->_useCount += actualNum;//计数器随着使用自增

	_spanLists[index]._mutex.unlock();//解锁

	return actualNum;
}
Span* CentralCache::GetOneSpan(SpanList& list, size_t byte_size)
{
	Span* it = list.Begin();
	while (it != list.End())//遍历CentralCache的自由链表
	{
		if (it->_freeList != nullptr)
		{
			return it;
		}
		else {
			it = it->_next;//当前span为空，移动到下一个span
		}
	}
	//暂时接触桶锁，这样如果其他线程由释放内存对象，不会阻塞
	list._mutex.unlock();

	//至此说明CentralCache已经没有内存了，只能向page Cache索要。
	PageCache::GetInstance()->_pageMtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(Size::NumMovePage(byte_size));
	PageCache::GetInstance()->_pageMtx.unlock();
	//获取内存块的起始结束地址

	//切分不用加锁，因为此时span还没有被挂载
	char* start = (char*)(span->_pageId << PAGE_SHIFT);//起始地址
	size_t bytes = span->_n << PAGE_SHIFT;//页数量乘以页大小
	char* end = start + bytes;
	//对拿过来的span进行切分，并将其进行尾插
	span->_freeList = start;
	//先切一小块
	start += byte_size;
	void* tail = span->_freeList;
	while (start < end)
	{
		NextObj(tail) = start;//start 和 tail遍历大内存，并且切分建立链表
		start += byte_size;
		tail = NextObj(tail);

	}
	//切好之后，再加锁
	list._mutex.lock();
	list.PushFront(span);//将划分好的小对象存储到span中
	return span;
}
//获得一个非空的span


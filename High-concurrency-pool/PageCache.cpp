#include "PageCache.h"
#include <assert.h>


PageCache PageCache::_sInst;

Span* PageCache::NewSpan(size_t k)
{
	assert(k > 0 && k < NPAGES);
	if (!_spanLists[k].Empty())//判断第K个桶是否为空
	{
		return _spanLists->PopFront();
	}

	//检查后面的桶里面有没有span，如果有的话可以对它进行切分
	for (size_t i = k + 1; i < NPAGES; i++) //扫描所有的桶
	{
		if (!_spanLists[i].Empty())//判断第K个桶是否可用
		{
			Span* nSpan = _spanLists[i].PopFront();//弹出第N个不为空的桶
			Span* kSpan = new Span;

			kSpan->_pageId = nSpan->_pageId;//使用弹出来的空间的首id
			kSpan->_n = k;//分配之后页增加k个

			nSpan->_pageId += k; //往后挪k个
			nSpan->_n -= k; //分配之后页减少k个

			//用不完的空间挂载到对应的位置
			_spanLists[nSpan->_n].PushFront(nSpan);
			//返回开辟的空间
			return kSpan;
		}
	}

	//到这里就说明没有所有Span都是空的，此时要去堆申请一个128页的span
	Span* bigSpan = new Span;
	void* ptr = SystemAlloc(NPAGES - 1);
	bigSpan->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT; //分配到的起始地址，除以页大小，就可以得到页编号
	bigSpan->_n = NPAGES - 1;//因为并且这里的bigSpan对应的是最大的桶下标

	_spanLists[bigSpan->_n].PushFront(bigSpan);//为了避免重复代码，在插入空间之后直接复用
	return NewSpan(k);

}
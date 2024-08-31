#include "common.h"
#include "ThreadCache.h"
#include "time.h"
#include "Error.h"
#include "Size.h"
#include "CentralCache.h"
#include "assert.h"
#include <algorithm>

 

void* ThreadCache::FetchFromCentralCache(size_t index,size_t size)
{
	//慢开始调节算法
	void* start = nullptr;//输出型参数
	void* end = nullptr;//输出型参数
	size_t batchNum = min(_freeLists[index].MaxSize(), Size::NumMoveSize(size));//根据当前自由链表节点的大小，来判断分配多少个合适
	size_t actualSize = CentralCache::GetInstance()->FetchRangeObj(start, end, batchNum, size);
	if (actualSize < 0)
	{
		exit(ACTUALSIZE_ERROR);
	}
	if (_freeLists[index].MaxSize() == batchNum) // 满增长
	{
		_freeLists[index].MaxSize() += 1;
	}
	if (actualSize == 1)
	{
		assert(start == end);
		return start;//返回头
	}
	else
	{
		_freeLists[index].PushRange(NextObj(start), end);
		return start;//返回头
	}
}

void* ThreadCache::Allocate(size_t size) //长度
{
	if (size > MAX_BYTES)
	{
		std::cerr << "开辟字节太大" << std::endl;
		exit(BYTES_ERROR);
	}
	size_t alignSize = Size::Alignment(size);
	size_t index = Size::Index(size);

	if (!_freeLists[index].empty())//当前桶下面还有剩余空间
	{
		_freeLists[index].pop();
	}
	else {//没有空间了，向中间层索要空间
		return ThreadCache::FetchFromCentralCache(index,alignSize);
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	assert(ptr);
	assert(size <= MAX_BYTES);

	
	size_t index = Size::Index(size);
	_freeLists[index].push(ptr);

	// ...
}
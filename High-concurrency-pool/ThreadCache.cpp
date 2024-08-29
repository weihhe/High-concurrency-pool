#include "common.h"
#include "ThreadCache.h"
#include "time.h"
#include "Error.h"
#include "Size.h"
 
void* ThreadCache::FetchFromCentralCache(size_t index,size_t size)
{
	return nullptr;
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

}
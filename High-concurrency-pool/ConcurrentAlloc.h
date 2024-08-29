#pragma once
#include "Common.h"
#include "ThreadCache.h"
#include <thread>
#include "Error.h"
#include "Size.h"
 

static void* ConcurrentAlloc(size_t size)//static保证只有在当前文件可见
{
	if (pTLS_ThreadCache == nullptr)
	{
		pTLS_ThreadCache = new ThreadCache;

	}
	cout << std::this_thread::get_id() << ":" << pTLS_ThreadCache << endl;
	return pTLS_ThreadCache->Allocate(size); //线程申请到内存
}
static void ConcurrentFree(void* ptr,size_t size)
{
	if (!ptr || !pTLS_ThreadCache)
	{
		exit(PTR_NULLERROR);
	}
	//将不会的内存，放回到自己的自由链表桶里面
	size_t index = Size::Index(size);
	pTLS_ThreadCache->Deallocate(ptr,index);
	
}
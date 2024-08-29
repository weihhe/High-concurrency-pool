#pragma once
#include "Common.h"



class ThreadCache {
public:
	void* Allocate(size_t size); //长度
	void* FetchFromCentralCache(size_t index, size_t size);
	void Deallocate(void* ptr, size_t size);
private:
	FreeList _freeLists[FREELISTS_NUM];//自由链表集合，指针数组。
};

//TLS 每个线程都有这样的指针，并且这个变量在此线程中是全局，并且对其他线程不可见
static _declspec(thread) ThreadCache* pTLS_ThreadCache = nullptr;
#pragma once
#include <iostream>
const unsigned POOLCAPACITY = 1024*1024;


// 测试申请释放的轮次
const size_t Rounds = 10;
// 测试每轮申请释放多少次
const size_t N = 100000;


struct TreeNode
{
	int _val;
	TreeNode* _left;
	TreeNode* _right;
	TreeNode()
		: _val(0), _left(nullptr), _right(nullptr)
	{
	}
};
//由于使用了模板，就不声明和定义分离了。
template <class T> // 每个对象的大小是固定的
class ObjectPool
{
public:
	T* New()
	{
		T* obj = nullptr;
		if (_freelist != nullptr) // 使用被丢弃的对象内存
		{
			void* next = *((void**)_freelist);
			obj = (T*)_freelist;
			_freelist = next;
		}
		else
		{
			if (_remainByte < sizeof(T)) // 如果剩余空间不够一个对象，那么就再开辟内存
			{
				try
				{
					_memory = (char*)malloc(POOLCAPACITY);
				}
				catch (const std::bad_alloc& e)
				{
					std::cerr << "内存池开辟失败 " << e.what() << std::endl;
				}
				_remainByte = POOLCAPACITY;
			}
			size_t tmp = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*); // 至少给他一个指针的大小,便于回收
			obj = (T*)_memory;
			_memory += tmp; // 使用了空间后，要把_memory指向剩余内存的头部
			_remainByte -= tmp;
		}
		new (obj) T; // 定位new
		return obj;
	}
	void Delete(T* abandonObj)
	{
		if (!abandonObj) return;
		abandonObj->~T(); // 必须显示的调用析构函数
		// if (_freelist == nullptr)
		//{
		//	static_cast<T*>(_freelist) = abandonObj; //链表头
		//	*(static_cast<void**>(abandonObj)) = nullptr;//链表结构
		// }
		// else {
		*((void**)abandonObj) = _freelist; // 头插到自由链表，同样适用于空链表
		_freelist = abandonObj;

		//}
	}

private:
	char* _memory = nullptr; // 便于以字节为单位切割，void *的加减是没有意义的。
	void* _freelist = nullptr;
	size_t _remainByte = 0; // 剩余字节数
	bool _notMalloced = true;
};

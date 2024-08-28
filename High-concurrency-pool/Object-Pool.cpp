#include "object-pool.h"
#include <vector>

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
			_remainByte -= sizeof(tmp);
		}
		new (obj) T; // 定位new
		return obj;
	}
	void Delete(T* abandonObj)
	{
		abandonObj->~T(); // 必须显示的调用析构函数
		// if (_freelist == nullptr)
		//{
		//	static_cast<T*>(_freelist) = abandonObj; //链表头
		//	*(static_cast<void**>(abandonObj)) = nullptr;//链表结构
		// }
		// else {
		*((void**)abandonObj) = _freelist; // 头插，同样适用于空链表
		_freelist = abandonObj;

		//}
	}

private:
	char* _memory = nullptr; // 便于以字节为单位切割，void *的加减是没有意义的。
	void* _freelist = nullptr;
	size_t _remainByte = 0; // 剩余字节数
	bool _notMalloced = true;
};

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
void TestObjectPool()
{
	// 申请释放的轮次
	const size_t Rounds = 10;
	// 每轮申请释放多少次
	const size_t N = 100000;
	size_t begin1 = clock();
	std::vector<TreeNode*> v1;
	v1.reserve(N);
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v1.push_back(new TreeNode);
		}
		for (int i = 0; i < N; ++i)

		{

			delete v1[i];
		}
		v1.clear();
	}
	
	size_t end1 = clock();

	ObjectPool<TreeNode> TNPool;

	size_t begin2 = clock();

	std::vector<TreeNode*> v2;

	v2.reserve(N);

	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v2.push_back(TNPool.New());
		}
		for (int i = 0; i < N; ++i)

		{
			TNPool.Delete(v2[i]);
		}

		v2.clear();
	}

	size_t end2 = clock();

	cout << "常规new,delete耗费时间" << end1 - begin1 << endl;

	cout << "对象池耗费时间:" << end2 - begin2 << endl;
}

int main()
{
	TestObjectPool();
	return 0;
}
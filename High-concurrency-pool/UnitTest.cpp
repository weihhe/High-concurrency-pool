#include "object-pool.h"
#include "ConcurrentAlloc.h"
#include "common.h"

void TestObjectPool()
{
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
void Alloc_1() {
	for (size_t i = 0; i < 5; i++)
	{
		void* ptr = ConcurrentAlloc(6);
	}
}
void Alloc_2()
{
	for (size_t i = 0; i < 5; i++)
	{
		void* ptr = ConcurrentAlloc(7);
	}
}
void TLSTest()
{
	std::thread t1(Alloc_1);
	t1.join();
	std::thread t2(Alloc_2);
	t2.join();
}
void TestConcurrentAlloc1()
{
	void* p1 = ConcurrentAlloc(6);
	void* p2 = ConcurrentAlloc(8);
	void* p3 = ConcurrentAlloc(1);
	void* p4 = ConcurrentAlloc(7);
	void* p5 = ConcurrentAlloc(8);
	//因为这里切分的时候是用一个大块内存切分的，所以p1~-p5可能是连续的
	cout << p1 << endl;
	cout << p2 << endl;
	cout << p3 << endl;
	cout << p4 << endl;
	cout << p5 << endl;
}

void TestConcurrentAlloc2()
{
	for (size_t i = 0; i < 1024; ++i)
	{
		void* p1 = ConcurrentAlloc(6);//8k的页大小，一个对象8字节，刚好1024块
		cout << p1 << endl;
	}

	void* p2 = ConcurrentAlloc(8);//超出了一页的承载量，再次申请页，在本次用例中，应该是span[127]
	cout << p2 << endl;
}

int main()
{
	//TestObjectPool();
	//TLSTest();
	//ConcurrentAlloc(6);
	//TestConcurrentAlloc1();//测试页内划分
	TestConcurrentAlloc2();//测试当一页用完之后，再次申请页
	return 0;
}
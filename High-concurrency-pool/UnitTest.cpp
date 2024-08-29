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
int main()
{
	TestObjectPool();
	TLSTest();
	return 0;
}
#pragma once
//返回对齐后结构的大小
class Size {
public:
	static size_t _Alignment(size_t size, size_t alignment_num)
	{
		size_t aligSize;
		if (size % alignment_num != 0)
		{
			aligSize = (size / alignment_num + 1) * alignment_num;
		}
		else aligSize = size;
		return aligSize;
	}
	//不同大小对应不同的链表结构
	static size_t Alignment(size_t size)
	{
		if (size <= 128)
		{
			_Alignment(size, 8);
		}
		else if (size <= 1024) {
			_Alignment(size, 16);
		}
		else if (size <= 8 * 1024) {
			_Alignment(size, 128);
		}
		else if (size <= 64 * 1024) {
			_Alignment(size, 1024);
		}
		else if (size <= 256 * 1024) {
			_Alignment(size, 8 * 1024);
		}
		else {
			std::cerr << "数据结构过大(大于256*1024)" << std::endl;
			exit(BYTES_ERROR);
		}
	}

	//找到对应桶的位置
	static inline size_t _Index(size_t bytes, size_t alignment_num)
	{
		size_t index_num;
		if (bytes % alignment_num == 0)
			index_num = bytes / alignment_num - 1;
		else
		{
			index_num = bytes / alignment_num;
		}
		return index_num;
	}

	// 计算映射的哪一个自由链表桶
	static size_t Index(size_t bytes)
	{
		static int group_array[4] = { 16, 56, 56, 56 }; //每个自由链表中哈希桶的数量
		if (bytes <= 128) {
			return _Index(bytes, 8);
		}
		else if (bytes <= 1024) {
			return _Index(bytes - 128, 16) + group_array[0];
		}
		else if (bytes <= 8 * 1024) {
			return _Index(bytes - 1024, 128) + group_array[1] + group_array[0];
		}
		else if (bytes <= 64 * 1024) {
			return _Index(bytes - 8 * 1024, 1024) + group_array[2] + group_array[1] + group_array[0];
		}
		else if (bytes <= 256 * 1024) {
			return _Index(bytes - 64 * 1024, 8 * 1024) + group_array[3] + group_array[2] + group_array[1] + group_array[0];
		}
		exit(INDEX_ERROR);
	}
	static size_t NumMoveSize(size_t size)
	{
		if (size < 0)
		{
			exit(SIZE_ERROR);
		}

		int num = MAX_BYTES / size; 
		if (num < 2)//对于大对象来说，至少分配两个
			num = 2;

		if (num > 512)//对于小对象来说，最多一次分配512个
			num = 512;

		return num;
	}
	//计算一次向系统获取几个页
	//单个对象大小 8byte
	//...
	//单个对象 256kb
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;//计算出的字节数

		npage >>= PAGE_SHIFT;//右移13位，除以8k(页大小)
		if (npage == 0)
			npage = 1;//至少需要1个页

		return npage;
	}
};
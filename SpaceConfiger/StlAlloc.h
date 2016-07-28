#include <iostream>
using namespace std;

//异常信息抛出
#   define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
//一级空间配置器
//函数成员全部声明为static保证调用安全性和域名调用

template <int inst>
class __MallocAllocTemplate {

private:
	//内部处理函数，处理无法申请，循环申请，知道申请到为止
	static void *OomMalloc(size_t);
	//同上，只不过进行空间值得初始化
	static void *OomRealloc(void *, size_t);

	//循环的函数指针句柄设置
	static void(*__MallocAllocOomHandler)();


public:
	//Allocate函数，调用malloc进行空间的申请。
	//如果申请失败，则调用oom_malloc(n),进行异常抛出。
	static void * Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result) 
			result = OomMalloc(n);
		return result;
	}
	//释放空间，直接调用free
	static void DeAllocate(void *p, size_t /* n */)
	{
		free(p);
	}

	//直接调用realloc，然后如果失败则调用omm_realloc返回错误信息
	static void * ReAllocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		if (0 == result) 
			result = OomRealloc(p, new_sz);
		return result;
	}

	//句柄处理函数设置，没有特别厉害的情况，就考虑这个情况了。。。
	static void(*SetMallocHandler(void(*f)()))()
	{
		void(*old)() = __MallocAllocOomHandler;
		__MallocAllocOomHandler = f;
		return(old);
	}

};
//全局全局函数指针句柄
template <int inst>
void(*__MallocAllocTemplate<inst>::__MallocAllocOomHandler)() = 0;



template <int inst>
void * __MallocAllocTemplate<inst>::OomMalloc(size_t n)
{
	void(*my_malloc_handler)();
	void *result;

	for (;;) {
		my_malloc_handler = __MallocAllocOomHandler;
		if (0 == my_malloc_handler) 
		{ 
			__THROW_BAD_ALLOC; 
		}
		(*my_malloc_handler)();
		result = malloc(n);
		if (result) 
			return(result);
	}
}

template <int inst>
void * __MallocAllocTemplate<inst>::OomRealloc(void *p, size_t n)
{
	void(*my_malloc_handler)();
	void *result;

	for (;;) {
		my_malloc_handler = __MallocAllocOomHandler;
		if (0 == my_malloc_handler) 
		{ 
			__THROW_BAD_ALLOC; 
		}
		(*my_malloc_handler)();
		result = realloc(p, n);
		if (result) 
			return(result);
	}
}


//提供给simple_alloc,的模板进行类型识别操作。
typedef __MallocAllocTemplate<0> MallocAlloc;
typedef MallocAlloc alloc;








template<class T, class Alloc>
class simple_alloc {

public:
	static T *Allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::Allocate(n * sizeof (T));
	}
	static T *Allocate(void)
	{
		return (T*)Alloc::Allocate(sizeof (T));
	}
	static void DeAllocate(T *p, size_t n)
	{
		if (0 != n) Alloc::DeAllocate(p, n * sizeof (T));
	}
	static void DeAllocate(T *p)
	{
		Alloc::DeAllocate(p, sizeof (T));
	}
};




//默认节点分配器。
//有了一个合理的编译器，这应该是大致一样快
//原来的STL类特定的分配器，但较少的碎片。
// Default_alloc_template参数测试，可能会
//消失于未来。客户应该只使用ALLOC现在。
//
//重要的执行性能：
// 1，如果客户端请求大小> __MAX_BYTES的目的，将所得
//对象将通过malloc直接获得。
// 2。在其他情况下，我们确切地分配大小的物体
// ROUND_UP（requested_size）。因此，客户有足够的大小
//我们可以将它返回到正确自由列表信息
//不会永久失去对象的一部分。
//

//第一个模板参数指定是否多个线程
//可以使用这个分配器。它是安全的，从分配一个对象
//一个default_alloc的一个实例，与另一个释放它
//之一。这有效地传递其至第二个所有权。
//这可能对局部性不良影响。
//第二个参数是未引用并仅提供允许
//创建多个default_alloc实例。
//该容器建立在不同的分配实例都节点
//不同类型的，限制了这种方法的效用。
template <bool threads, int inst>
class __DefaultAllocTemplate {

private:
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
private:
	union Obj {
		union Obj * free_list_link;
		char client_data[1];    /* The client sees this.        */
	};
private:
	static Obj * __VOLATILE free_list[__NFREELISTS];

	static  size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}

	// Returns an Object of size n, and optionally adds to size n free list.
	//返回大小为n的对象，以及可选增加了大小为n空闲列表中。
	static void *refill(size_t n);
	// Allocates a chunk for nObjs of size size.  nObjs may be reduced
	// if it is inconvenient to Allocate the requested number.
	//分配为大小大小nObjs一大块。 nObjs可以减少
	//如果是不方便分配请求的数目。
	static char *ChunkAlloc(size_t size, int &nObjs);

	// Chunk allocation state.
	//块分配状态。
	static char *_start_free;
	static char *_end_free;
	static size_t _heap_size;

public:

	/* n must be > 0      */
	static void * Allocate(size_t n)
	{
		Obj * __VOLATILE * my_free_list;
		Obj * __RESTRICT result;

		if (n > (size_t)__MAX_BYTES) {
			return(malloc_alloc::Allocate(n))my_free_list
		}
		; = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		if (result == 0) {
			void *r = refill(ROUND_UP(n));
			return r;
		}
		*my_free_list = result->free_list_link;
		return (result);
	};

	/* p may not be 0 */
	static void DeAllocate(void *p, size_t n)
	{
		Obj *q = (Obj *)p;
		Obj * __VOLATILE * my_free_list;

		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::DeAllocate(p, n);
			return;
		}
		my_free_list = free_list + FREELIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}

	static void * ReAllocate(void *p, size_t old_sz, size_t new_sz);

};

typedef __DefaultAllocTemplate<false, 0> single_client_alloc;


/*我们在大块为了避免分段分配内存*/
/*malloc的堆太多。 */
/*我们假设尺寸是正确对齐。 */
/*我们认为分配锁。 */
template <bool threads, int inst>
char*
__DefaultAllocTemplate<threads, inst>::ChunkAlloc(size_t size, int& nObjs)
{
	char * result;
	size_t total_bytes = size * nObjs;
	size_t bytes_left = _end_free - _start_free;

	if (bytes_left >= total_bytes) {
		result = _start_free;
		_start_free += total_bytes;
		return(result);
	}
	else if (bytes_left >= size) {
		nObjs = bytes_left / size;
		total_bytes = size * nObjs;
		result = _start_free;
		_start_free += total_bytes;
		return(result);
	}
	else {
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(_heap_size >> 4);

		//尽量利用遗留的一块。
		// Try to make use of the left-over piece.
		if (bytes_left > 0) {
			Obj * __VOLATILE * my_free_list =
				free_list + FREELIST_INDEX(bytes_left);

			((Obj *)_start_free)->free_list_link = *my_free_list;
			*my_free_list = (Obj *)_start_free;
		}
		_start_free = (char *)malloc(bytes_to_get);
		if (0 == _start_free) {
			int i;
			Obj * __VOLATILE * my_free_list, *p;
			// Try to make do with what we have.  That can't
			// hurt.  We do not try smaller requests, since that tends
			// to result in disaster on multi-process machines.
			//尝试凑合用我们所拥有的。这不可能
			//受伤。我们不要试图较小的请求，因为这往往
			//导致在多进程的机器的灾难。
			for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (0 != p) {
					*my_free_list = p->free_list_link;
					_start_free = (char *)p;
					_end_free = _start_free + i;
					return(ChunkAlloc(size, nObjs));
					// Any leftover piece will eventually make it to the
					// right free list.
					//任何剩余片最终会使其对
					//正确的空闲列表中。
				}
			}
			_end_free = 0;	// In case of exception.
			_start_free = (char *)malloc_alloc::Allocate(bytes_to_get);
			// This should either throw an
			// exception or remedy the situation.  Thus we assume it
			// succeeded.
			//这应该不是抛出一个
			//异常或亡羊补牢。因此，我们假设它
			//成功。
		}
		_heap_size += bytes_to_get;
		_end_free = _start_free + bytes_to_get;
		return(ChunkAlloc(size, nObjs));
	}
}


/* Returns an Object of size n, and optionally adds to size n free list.*/
/* We assume that n is properly aligned.                                */
/* We hold the allocation lock.                                         */
/*返回大小为n的一个对象，并任选添加到大小为n自由列表。*/
/*我们假设n被正确对准。 */
/*我们认为分配锁。 */
template <bool threads, int inst>
void* __DefaultAllocTemplate<threads, inst>::refill(size_t n)
{
	int nObjs = 20;
	char * chunk = ChunkAlloc(n, nObjs);
	Obj * __VOLATILE * my_free_list;
	Obj * result;
	Obj * current_Obj, *next_Obj;
	int i;

	if (1 == nObjs) return(chunk);
	my_free_list = free_list + FREELIST_INDEX(n);

	/* Build free list in chunk */
	result = (Obj *)chunk;
	*my_free_list = next_Obj = (Obj *)(chunk + n);
	for (i = 1;; i++) {
		current_Obj = next_Obj;
		next_Obj = (Obj *)((char *)next_Obj + n);
		if (nObjs - 1 == i) {
			current_Obj->free_list_link = 0;
			break;
		}
		else {
			current_Obj->free_list_link = next_Obj;
		}
	}
	return(result);
}

template <bool threads, int inst>
void* __DefaultAllocTemplate<threads, inst>::ReAllocate(void *p,
	size_t old_sz,
	size_t new_sz)
{
	void * result;
	size_t copy_sz;

	if (old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES) {
		return(realloc(p, new_sz));
	}
	if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return(p);
	result = Allocate(new_sz);
	copy_sz = new_sz > old_sz ? old_sz : new_sz;
	memcpy(result, p, copy_sz);
	DeAllocate(p, old_sz);
	return(result);
}


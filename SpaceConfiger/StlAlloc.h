#include <iostream>
using namespace std;

//�쳣��Ϣ�׳�
#   define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
//һ���ռ�������
//������Աȫ������Ϊstatic��֤���ð�ȫ�Ժ���������

template <int inst>
class __MallocAllocTemplate {

private:
	//�ڲ��������������޷����룬ѭ�����룬֪�����뵽Ϊֹ
	static void *OomMalloc(size_t);
	//ͬ�ϣ�ֻ�������пռ�ֵ�ó�ʼ��
	static void *OomRealloc(void *, size_t);

	//ѭ���ĺ���ָ��������
	static void(*__MallocAllocOomHandler)();


public:
	//Allocate����������malloc���пռ�����롣
	//�������ʧ�ܣ������oom_malloc(n),�����쳣�׳���
	static void * Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result) 
			result = OomMalloc(n);
		return result;
	}
	//�ͷſռ䣬ֱ�ӵ���free
	static void DeAllocate(void *p, size_t /* n */)
	{
		free(p);
	}

	//ֱ�ӵ���realloc��Ȼ�����ʧ�������omm_realloc���ش�����Ϣ
	static void * ReAllocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		if (0 == result) 
			result = OomRealloc(p, new_sz);
		return result;
	}

	//������������ã�û���ر�������������Ϳ����������ˡ�����
	static void(*SetMallocHandler(void(*f)()))()
	{
		void(*old)() = __MallocAllocOomHandler;
		__MallocAllocOomHandler = f;
		return(old);
	}

};
//ȫ��ȫ�ֺ���ָ����
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


//�ṩ��simple_alloc,��ģ���������ʶ�������
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




//Ĭ�Ͻڵ��������
//����һ������ı���������Ӧ���Ǵ���һ����
//ԭ����STL���ض��ķ������������ٵ���Ƭ��
// Default_alloc_template�������ԣ����ܻ�
//��ʧ��δ�����ͻ�Ӧ��ֻʹ��ALLOC���ڡ�
//
//��Ҫ��ִ�����ܣ�
// 1������ͻ��������С> __MAX_BYTES��Ŀ�ģ�������
//����ͨ��mallocֱ�ӻ�á�
// 2������������£�����ȷ�еط����С������
// ROUND_UP��requested_size������ˣ��ͻ����㹻�Ĵ�С
//���ǿ��Խ������ص���ȷ�����б���Ϣ
//��������ʧȥ�����һ���֡�
//

//��һ��ģ�����ָ���Ƿ����߳�
//����ʹ����������������ǰ�ȫ�ģ��ӷ���һ������
//һ��default_alloc��һ��ʵ��������һ���ͷ���
//֮һ������Ч�ش��������ڶ�������Ȩ��
//����ܶԾֲ��Բ���Ӱ�졣
//�ڶ���������δ���ò����ṩ����
//�������default_allocʵ����
//�����������ڲ�ͬ�ķ���ʵ�����ڵ�
//��ͬ���͵ģ����������ַ�����Ч�á�
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
	//���ش�СΪn�Ķ����Լ���ѡ�����˴�СΪn�����б��С�
	static void *refill(size_t n);
	// Allocates a chunk for nObjs of size size.  nObjs may be reduced
	// if it is inconvenient to Allocate the requested number.
	//����Ϊ��С��СnObjsһ��顣 nObjs���Լ���
	//����ǲ���������������Ŀ��
	static char *ChunkAlloc(size_t size, int &nObjs);

	// Chunk allocation state.
	//�����״̬��
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


/*�����ڴ��Ϊ�˱���ֶη����ڴ�*/
/*malloc�Ķ�̫�ࡣ */
/*���Ǽ���ߴ�����ȷ���롣 */
/*������Ϊ�������� */
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

		//��������������һ�顣
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
			//���Դպ���������ӵ�еġ��ⲻ����
			//���ˡ����ǲ�Ҫ��ͼ��С��������Ϊ������
			//�����ڶ���̵Ļ��������ѡ�
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
					//�κ�ʣ��Ƭ���ջ�ʹ���
					//��ȷ�Ŀ����б��С�
				}
			}
			_end_free = 0;	// In case of exception.
			_start_free = (char *)malloc_alloc::Allocate(bytes_to_get);
			// This should either throw an
			// exception or remedy the situation.  Thus we assume it
			// succeeded.
			//��Ӧ�ò����׳�һ��
			//�쳣�������Ρ���ˣ����Ǽ�����
			//�ɹ���
		}
		_heap_size += bytes_to_get;
		_end_free = _start_free + bytes_to_get;
		return(ChunkAlloc(size, nObjs));
	}
}


/* Returns an Object of size n, and optionally adds to size n free list.*/
/* We assume that n is properly aligned.                                */
/* We hold the allocation lock.                                         */
/*���ش�СΪn��һ�����󣬲���ѡ��ӵ���СΪn�����б�*/
/*���Ǽ���n����ȷ��׼�� */
/*������Ϊ�������� */
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


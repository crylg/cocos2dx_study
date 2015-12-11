/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	LMemoryManager.h
//	Created: 2004-6-20   1:13
//
**********************************************************************************/
#ifndef __VMEMORY_H__
#define __VMEMORY_H__

#include "VThreading.h"



#if VPLATFORM_WIN32			// TODO ..
#if VDEBUG
#define VMEMORY_DEBUG
#endif 
#endif 


#define VMEM_ALIGNMENT  8



#ifdef __cplusplus 
extern "C" {
#endif //__cplusplus

	// debug version memroy operation
#if defined(VMEMORY_DEBUG)
	enum LMemOP
	{
		VMOP_UNKNOWN = 0,             
		VMOP_NEW,              
		VMOP_NEW_ARR,            
		VMOP_DELETE,  
		VMOP_DELETE_ARR,  
		VMOP_MALLOC,    
		VMOP_REALLOC,
		VMOP_FREE,
		VMOP_ALIGN_MALLOC,     
		VMOP_ALIGN_REALLOC,     
		VMOP_ALIGN_FREE,
	};

	VCoreApi void* _VAllocate(size_t Size, LMemOP Method, BOOL DelSize,const char* File, int Line, const char* Func);
	VCoreApi void  _VDeallocate(void* Ptr, LMemOP Method, size_t Size);
	VCoreApi void* _VMalloc(size_t Size, const char* File, int Line, const char* Func);
	VCoreApi void* _VRealloc(void* Ptr, size_t Size, const char* File, int Line, const char* Func); 
	VCoreApi void  _VFree(void* Ptr);
#define VNew new(__FILE__,__LINE__,__FUNCTION__)
#define VDelete delete
#define VMalloc(size) (_VMalloc(size, __FILE__, __LINE__, __FUNCTION__)) 
#define VRealloc(Ptr, size) (_VRealloc(Ptr, size, __FILE__, __LINE__, __FUNCTION__)) 
#define VFree(p) _VFree(p)
#else
	//VMemApi void*	_VAllocate(size_t Size, size_t Align);
	//VMemApi void	_VDeallocate(void* Ptr, size_t Size);
	VCoreApi void*	_VMalloc(size_t Size);
	VCoreApi void*	_VRealloc(void *Ptr, size_t Size); 
	VCoreApi void	_VFree(void* Ptr);
#define VNew new
#define VDelete delete
#define VMalloc(size) _VMalloc(size) 
#define VRealloc(Ptr, size) _VRealloc(Ptr, size)
#define VFree(p) _VFree(p)
#endif // VMEMORY_DEBUG

#ifdef __cplusplus
}
#endif //__cplusplus

#define VSAFE_FREE(Ptr)  if(Ptr){ VFree(Ptr); (Ptr) = NULL;}
#define VSAFE_DELETE(Ptr) if(Ptr) {VDelete (Ptr); (Ptr) = NULL;}
#define VSAFE_RELEASE(Ptr) if(Ptr) { (Ptr)->Release(); (Ptr) = NULL;}

// memory interface 
struct VAllocator
{
#ifdef VMEMORY_DEBUG
private:
	// The memory debugger uses the file, line, function 
	// routines. Therefore new and new[] should be 
	// unavailable to the outer application.
	static inline void* operator new(size_t Size) { return NULL; }
	static inline void* operator new[](size_t Size){ return NULL; }
public:
	static inline void *operator new(size_t Size, const char *File, int Line, const char* Func)
	{
		return _VAllocate(Size,VMOP_NEW, TRUE, File, Line, Func);
	}
	static void *operator new[](size_t Size, const char *File, int Line, const char* Func)
	{
		return _VAllocate(Size, VMOP_NEW_ARR, FALSE, File, Line, Func);
	}
	// disable warning 4291
	static void operator delete(void*, const char*, int, const char*){}
	static void operator delete[](void *, const char *, int, const char*){}
#else
public:
	static inline void* operator new(size_t Size)
	{
		return _VMalloc(Size);
	}
	static inline void* operator new[](size_t Size)
	{
		return _VMalloc(Size);
	}
#endif
public:
	static inline void operator delete(void *Ptr, size_t Size)
	{
#ifdef VMEMORY_DEBUG
		_VDeallocate(Ptr, VMOP_DELETE, Size);
#else 
		_VFree(Ptr);
#endif 
	}
	static inline void operator delete[](void *Ptr, size_t Size)
	{
#ifdef VMEMORY_DEBUG
		_VDeallocate(Ptr, VMOP_DELETE_ARR, (size_t)-1);
#else 
		_VFree(Ptr);
#endif 
	}
};

#ifdef VMEMORY_DEBUG

#define VIMPLEMENT_ALLOCATOR	\
	private:					\
	static inline void* operator new(size_t Size) { return NULL;} \
	static inline void* operator new[](size_t Size) { return NULL;}\
public:\
	static inline void *operator new(size_t Size, const char *File, int Line, const char* Func)	\
	{ return _VAllocate(Size, VMOP_NEW, TRUE, File, Line, Func); }	\
	static inline void *operator new[](size_t Size, const char *File, int Line, const char* Func)\
	{ return _VAllocate(Size, VMOP_NEW_ARR, FALSE, File, Line, Func);}	\
	static inline void operator delete(void *Ptr, size_t Size)	\
	{ _VDeallocate(Ptr, VMOP_DELETE, Size); }	\
	static inline void operator delete[](void *Ptr, size_t Size)	\
	{ _VDeallocate(Ptr, VMOP_DELETE_ARR, (size_t)-1);}	\
	static inline void operator delete(void*, const char*, int, const char*){}\
	static inline void operator delete[](void *, const char *, int, const char*){}\

#else

#define VIMPLEMENT_ALLOCATOR	\
	public:	\
	static inline void* operator new(size_t Size) { return _VMalloc(Size);}\
	static inline void* operator new[](size_t Size){ return _VMalloc(Size);}\
	static inline void operator delete(void *Ptr, size_t Size){ _VFree(Ptr);}\
	static inline void operator delete[](void *Ptr, size_t Size){ _VFree(Ptr);}\

#endif 


//////////////////////////////////////////////////////////////////////////
//	Memory Pool
//////////////////////////////////////////////////////////////////////////

class VSyncLockDummy
{
public:
	inline void Lock(){}
	inline void Unlock(){}
};

class VSyncLockSpin
{
public:
	inline VSyncLockSpin():mLock(0){}

	inline void Lock()
	{
		VSpinLock(&mLock, 0, 1);
	}
	inline void Unlock()
	{
		VSpinLock(&mLock, 1, 0);
	}
private:
	volatile int mLock;
};

template<typename T>
class VMemPoolElementConstructor
{
public:
	static inline void Construct(T* Ptr)
	{
		::new(Ptr) T;
	}

	static inline void Destruct(T* Ptr)
	{
		Ptr->~T();
	}
};

template<typename T>
class VMemPoolElementConstructor_POD
{
public:
	static inline void Construct(T* Ptr){}
	static inline void Destruct(T* Ptr){}
};



// pool allocator . sizeof(T) >= sizeof(void*)
template<class T, class Locker = VSyncLockDummy, class Constructor = VMemPoolElementConstructor<T> >
class VMemPool
{
public:
	VMemPool():m_FreeListHead(NULL){}
	~VMemPool()
	{
		Clear();
	}

	T* Alloc()
	{
		T* Ret;
		m_Locker.Lock();
		if(m_FreeListHead == NULL)
		{
			Ret = (T*)malloc(sizeof(T));
		}else
		{
			Ret = m_FreeListHead;
			m_FreeListHead = *(T**)m_FreeListHead;
		}
		m_Locker.Unlock();
		Constructor::Construct(Ret);
		return Ret;
	}
	void Free(T* Ptr)
	{
		Constructor::Destruct(Ptr);
		m_Locker.Lock();
		*(T**)Ptr = m_FreeListHead;
		m_FreeListHead = Ptr;
		m_Locker.Unlock();
	}
	void Clear()
	{
		m_Locker.Lock();
#ifdef VMEMORY_DEBUG
		T* Next;
		while(m_FreeListHead)
		{
			// the memory debug will fill the freelist head with some other data.
			Next = *(T**)m_FreeListHead;
			Constructor::Destruct(m_FreeListHead);
			free(m_FreeListHead);
			m_FreeListHead = Next;
		}
#else 
		T* Next;
		while (m_FreeListHead)
		{
			Next = *(T**)m_FreeListHead;
			Constructor::Destruct(m_FreeListHead);
			free(m_FreeListHead);
			m_FreeListHead = Next;
		}

		//while(m_FreeListHead)
		//{
		//	Constructor::Destruct(m_FreeListHead);
		//	VFree(m_FreeListHead);
		//	m_FreeListHead = *(T**)m_FreeListHead;
		//}
#endif 
		m_Locker.Unlock();
	}
private:
	T*				m_FreeListHead;
	Locker			m_Locker;
};



// 较为松散的内存池, 按页进行缓存, 单次分配内存的大小不能大于页面大小.
// 优点: 内存申请速度极快.
// 缺点: 松散, 浪费的内存空间较多. 不能单个销毁, 只能通过FreePool 全部销毁整个缓存.
// Note: 主页页面大小, 较大的页面可以减少生成新页面带来的性能损耗, 但是在松散处可能浪费更多的空间.
class VCoreApi VPageMemoryPool
{
	struct Page
	{
		Page*	Next;
		BYTE*	Buffer;
		UINT CurIndex;
	};
	Page* m_CurPage;
	UINT m_PageSize;
	Page* NewPage(UINT Size);
public:
	VPageMemoryPool(UINT PageSize);
	~VPageMemoryPool();
	void* Alloc(size_t size);
	void FreePool();
};


// 基于call stack的内存池. 非线程安全. 
// 通常也可以理解为基于帧的缓存分配. 在一帧的开始阶段(没有进入堆栈), 内存池所有单元都应该被销毁(IsStackClear)
// 必须由VStackMemMark来管理分配和销毁操作, 以保证适当的顺序(避免分配和销毁操作的缠绕), 操作的必须是位于堆栈顶端的Mark.
// VStackMemMark Mark1(StackPool);
// void* buf1_mark1 = Mark1.Alloc(size);
// void* buf2_mark1 = Mark1.Alloc(size2);
//		{
//			VStackMemMark Mark2;
//			void* buf1_mark2 = Mark2.Alloc(size);	// ok mark2 is on top of stack.
//          do not do any operation on mark1 before mark2 pop from stack!! 
//			Mark2.Pop();
//		}
//  do operation on mark1.
class VStackMemMark;
class VCoreApi VStackMemoryPool
{
	friend class VStackMemMark;
public:
	VStackMemoryPool(UINT PageSize);
	inline ~VStackMemoryPool() { FreePool(); }
	void* Alloc(UINT Size);
	void FreePool();
	inline BOOL IsStackClear() const { return m_CurrPage == NULL && m_TopMark == NULL; }
	UINT NumAllocated() const;
private:
	struct Page
	{
		Page*	Next;		// next page.
		UINT	Size;		// page size
		BYTE	Buffer[1];
	};
	void NewPage(UINT RequestSize);
	void FreePage(Page* P);
	void PopMark(VStackMemMark* Mark);
	UINT	m_PageSize;
	BYTE*	m_BufferBegin;
	BYTE*	m_BufferEnd;
	Page*	m_CurrPage;				// 当前有效的页面.
	Page*	m_FreePages;			// 已经释放的页面.
	VStackMemMark* m_TopMark;		// 处于堆栈顶端的游标
};

class VStackMemMark
{
	friend class VStackMemoryPool;
public:
	inline VStackMemMark(VStackMemoryPool& StackPool):m_StackPool(StackPool),
		m_Marker(StackPool.m_BufferBegin),
		m_InStack(TRUE), 
		m_ActivePages(StackPool.m_CurrPage),
		m_NextMark(StackPool.m_TopMark)
	{
		m_StackPool.m_TopMark = this;		// link with mark list.
	}
	inline ~VStackMemMark()	{	Pop();	}

	inline void* Alloc(UINT Size)
	{
		VASSERT(m_InStack);
		return m_StackPool.Alloc(Size);
	}
	inline void Pop()
	{
		if (m_InStack)
		{
			m_StackPool.PopMark(this);
			m_InStack = FALSE;
		}
	}
private:
	VStackMemoryPool& m_StackPool;
	BYTE* m_Marker;
	BOOL m_InStack;
	VStackMemoryPool::Page* m_ActivePages;
	VStackMemMark* m_NextMark;
};




#endif 
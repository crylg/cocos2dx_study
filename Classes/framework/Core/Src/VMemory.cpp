#include "VStdAfx.h"
#include "VMemory.h"
#include "KMemory.h"
#include "KMemTracker.h"
#if VPLATFORM_WIN32
#include <Psapi.h>
#endif 
#if VUSE_MEMORY


KPoolAllocator* g_PoolAllocator = NULL;

#endif 
BOOL KInitMemory()
{
#if VUSE_MEMORY
	if (!g_PoolAllocator)
	{

#if VPLATFORM_ANDROID
		UINT PageSize = sysconf(_SC_PAGESIZE); 
		//NumPhysPages = sysconf(_SC_PHYS_PAGES);
#elif VPLATFORM_IOS
		vm_size_t PageSize;
		host_page_size(mach_host_self(), &PageSize);
#elif VPLATFORM_WIN32
		/*MEMORYSTATUSEX MemoryStatusEx = { 0 };
		MemoryStatusEx.dwLength = sizeof(MemoryStatusEx);
		::GlobalMemoryStatusEx(&MemoryStatusEx);

		PERFORMANCE_INFORMATION PerformanceInformation = { 0 };
		::GetPerformanceInfo(&PerformanceInformation, sizeof(PerformanceInformation));

		MemoryConstants.TotalPhysical = MemoryStatusEx.ullTotalPhys;
		MemoryConstants.TotalVirtual = MemoryStatusEx.ullTotalVirtual;
		MemoryConstants.PageSize = PerformanceInformation.PageSize;

		MemoryConstants.TotalPhysicalGB = (MemoryConstants.TotalPhysical + 1024 * 1024 * 1024 - 1) / 1024 / 1024 / 1024;*/
		PERFORMANCE_INFORMATION PerformanceInformation = { 0 };
		::GetPerformanceInfo(&PerformanceInformation, sizeof(PerformanceInformation));

		UINT PageSize = PerformanceInformation.PageSize;
#endif 

		g_PoolAllocator = new KPoolAllocator(PageSize);
	}
#ifdef VMEMORY_DEBUG
	g_MemTracker.Initialize();
#endif 
#endif 

	return TRUE;
}

void KShutdownMemory()
{
#if VUSE_MEMORY
	if (g_PoolAllocator)
	{
		delete g_PoolAllocator;
		g_PoolAllocator = NULL;
	}

#ifdef VMEMORY_DEBUG
	g_MemTracker.Shutdown();
#endif 
#endif 
}

#ifdef VMEMORY_DEBUG
void* _VMalloc(size_t Size, const char* File, int Line, const char* Func)
{
	VASSERT(Size);
	return g_MemTracker.Allocate(Size, VMEM_ALIGNMENT, VMOP_MALLOC, FALSE, File, Line, Func);
}
#else
void* _VMalloc(size_t Size)
{
#if VUSE_MEMORY
	VASSERT(Size);
	//return PoolAllocator::allocate(Size);
	return g_PoolAllocator->Malloc(Size, VMEM_ALIGNMENT);
#else 
	return malloc(Size);
#endif 
}
#endif


#ifdef VMEMORY_DEBUG
void* _VRealloc(void *pvMem, size_t stSizeInBytes, const char* pcSourceFile,int iSourceLine, const char* pcFunction)
{
	if (stSizeInBytes == 0 && pvMem != 0)
	{
		VFree(pvMem);
		return NULL;
	}
	else if (pvMem == 0)
	{
		return _VMalloc(stSizeInBytes, pcSourceFile, iSourceLine, pcFunction);
	}

	// Actually reallocate the memory
	return g_MemTracker.Reallocate(pvMem, stSizeInBytes, 
		VMEM_ALIGNMENT, VMOP_REALLOC, FALSE,
		(size_t)-1, pcSourceFile, iSourceLine, pcFunction);
}
#else
void* _VRealloc(void *pvMem, size_t stSizeInBytes)
{

#if VUSE_MEMORY
	if (stSizeInBytes == 0 && pvMem != 0)
	{
		VFree(pvMem);
		return NULL;
	}
	//return PoolAllocator::Realloc(pvMem, stSizeInBytes);
	return g_PoolAllocator->Realloc(pvMem, stSizeInBytes, VMEM_ALIGNMENT);
#else 

	return realloc(pvMem, stSizeInBytes);
#endif
}
#endif

void _VFree(void* pvMem)
{

	if (pvMem == NULL)
		return;
#if VUSE_MEMORY
#ifdef VMEMORY_DEBUG
	g_MemTracker.Deallocate(pvMem, VMOP_FREE, (size_t)-1);
#else 
	g_PoolAllocator->Free(pvMem);
	//PoolAllocator::deallocate(pvMem);
#endif 
#else
	free(pvMem);
#endif 
}

#if !defined(NI_DISABLE_EXCEPTIONS)
#include <exception> // for std::bad_alloc
#include <new>
#endif



#ifdef VMEMORY_DEBUG
void* _VAllocate(size_t Size, LMemOP Method, BOOL DelSize,const char* File, int Line, const char* Func)
{
	VASSERT(Size);
	return g_MemTracker.Allocate(Size, VMEM_ALIGNMENT, Method, DelSize, File, Line, Func);
}
void  _VDeallocate(void* Ptr, LMemOP Method, size_t Size)
{
	VASSERT(Ptr);
	g_MemTracker.Deallocate(Ptr, Method, Size);
}
#else


#endif



//////////////////////////////////////////////////////////////////////////
//	Memory Pool
//////////////////////////////////////////////////////////////////////////

VPageMemoryPool::VPageMemoryPool(UINT PageSize)
{
	m_PageSize          = PageSize;
	m_CurPage = NewPage(m_PageSize);
}

VPageMemoryPool::~VPageMemoryPool()
{
	FreePool();
}

VPageMemoryPool::Page* VPageMemoryPool::NewPage(UINT Size)
{
	// TODO : 类似Stack 那样优化这里, 只分配一次
	Page* pPage = (Page*)VMalloc(sizeof(Page));
	pPage->Next = NULL;
	pPage->CurIndex = 0;
	pPage->Buffer = (BYTE*)VMalloc(Size);
	return pPage;
}

void* VPageMemoryPool::Alloc(size_t size)
{
	VASSERT(size <= m_PageSize);
	if (size > m_PageSize)
	{
		return NULL;
	}
	if(!m_CurPage || ( (size + m_CurPage->CurIndex) > m_PageSize) )
	{
		Page* pNewPage = NewPage(m_PageSize);
		pNewPage->Next = m_CurPage;
		m_CurPage = pNewPage;
	}
	BYTE* ret = m_CurPage->Buffer + m_CurPage->CurIndex;
	m_CurPage->CurIndex += (size + 3) & ~3; // dword align
	return ret;
}

void VPageMemoryPool::FreePool()
{
	Page *temp;
	while(m_CurPage)
	{
		temp = m_CurPage->Next;
		if (m_CurPage->Buffer)
		{
			VFree(m_CurPage->Buffer);
		}
		VFree(m_CurPage);
		m_CurPage = temp;
	}
}


//////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////
VStackMemoryPool::VStackMemoryPool(UINT PageSize):m_PageSize(PageSize),m_BufferBegin(NULL),m_BufferEnd(NULL),m_CurrPage(NULL),m_FreePages(NULL),m_TopMark(NULL)
{
}

void* VStackMemoryPool::Alloc(UINT Size)
{
	BYTE* Ret;
	BYTE* NewEnd = m_BufferBegin + Size;
	if (NewEnd <= m_BufferEnd)
	{
		Ret = m_BufferBegin;
		m_BufferBegin = NewEnd;
	}else
	{
		NewPage(Size);
		Ret = m_BufferBegin;
		m_BufferBegin += Size;
	}
	return Ret;
}

void VStackMemoryPool::NewPage(UINT RequestSize)
{
	Page* ResultPage = NULL;
	
	// 先从释放的页面中寻找符合要求的.
	Page** FreePage = &m_FreePages;
	while(*FreePage)
	{
		if ((*FreePage)->Size >= RequestSize)
		{
			ResultPage = *FreePage;
			*FreePage = (*FreePage)->Next;
			break;
		}
		FreePage = &((*FreePage)->Next);
	}

	// 创建新的页面.
	if(!ResultPage)
	{
		// Create new chunk.
		UINT PageSize = (RequestSize + sizeof(Page));

		// 确保page size 是指定页面大小的倍数
		PageSize = ((PageSize + m_PageSize -1) / m_PageSize) * m_PageSize; 

		// 创建页面和缓存
		ResultPage = (Page*)malloc(PageSize);
		ResultPage->Size = PageSize - sizeof(Page);
	}
	ResultPage->Next = m_CurrPage;
	m_CurrPage   = ResultPage;
	m_BufferBegin   = m_CurrPage->Buffer;
	m_BufferEnd  =  m_BufferBegin + m_CurrPage->Size;
}

void VStackMemoryPool::FreePage(Page* P)
{
	while( m_CurrPage != P )
	{
		Page* RemoveChunk	= m_CurrPage;
		m_CurrPage			= m_CurrPage->Next;
		RemoveChunk->Next   = m_FreePages;
		m_FreePages         = RemoveChunk;
	}
	m_BufferBegin =m_BufferEnd = NULL;
	if( m_CurrPage )
	{
		m_BufferBegin = m_CurrPage->Buffer;
		m_BufferEnd = m_BufferBegin + m_CurrPage->Size;
	}
}

void VStackMemoryPool::PopMark(VStackMemMark* Mark)
{
	// 确保堆栈没有缠绕
	VASSERT(m_TopMark == Mark);

	// 
	if (Mark->m_ActivePages != m_CurrPage)
	{
		// 释放游标标记的活动页面 
		FreePage(Mark->m_ActivePages);
	}

	VASSERT(Mark->m_ActivePages == m_CurrPage);
	m_BufferBegin = Mark->m_Marker;
	m_TopMark = Mark->m_NextMark;
}

void VStackMemoryPool::FreePool()
{
	VASSERT(m_CurrPage == NULL);
	Page* PrevPage;
	while( m_FreePages )
	{
		PrevPage = m_FreePages;
		m_FreePages = m_FreePages->Next;
		free(PrevPage);
	}
}

UINT VStackMemoryPool::NumAllocated() const
{
	UINT Total = 0;
	Page* ActivePage= m_CurrPage;
	while(ActivePage)
	{
		if (ActivePage != m_CurrPage)
		{
			Total += ActivePage->Size;
		}else
		{
			Total += m_BufferBegin - ActivePage->Buffer;
		}
	}
	return Total;
}
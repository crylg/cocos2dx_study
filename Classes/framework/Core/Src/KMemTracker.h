////////////////////////////////////////////////////////////////////////////////
//                      Venus Engine 
//      Copyright(c)2003-2005 PengKai,All Right Reversed.
//	    Created by PengKai .     
//	    KMemTracker.h :        
//	    Revision:	
////////////////////////////////////////////////////////////////////////////////
#ifndef __KMEMTRACKER_H__
#define __KMEMTRACKER_H__
#include "VThreading.h"

#if defined(VMEMORY_DEBUG)
// support LoadLibrary & FreeLibrary.
#define SUPPORT_DYNMAIC_IMPORT

#if defined(SUPPORT_DYNMAIC_IMPORT)
    #define MAX_FLF_LENGTH 256
    #define MAX_FLF_FUNC_LENGTH 256
#endif

// For storing File, Line, and Function of allocation event.   
class KFLF
{
public:
    KFLF();
    KFLF(const char* pcFile, unsigned int uiLine, const char* pcFunc);
    void Set(const char* pcFile, unsigned int uiLine, const char* pcFunc);
    void Reset();
    BOOL IsValid() const;

#if defined(SUPPORT_DYNMAIC_IMPORT)
    char m_pcFile[MAX_FLF_LENGTH];
    char m_pcFunc[MAX_FLF_FUNC_LENGTH];
#else 
    const char* m_pcFile;
    const char* m_pcFunc;
#endif
    unsigned int m_uiLine;
    static const KFLF UNKNOWN;
};

class LAllocUnit
{
public:
    size_t AllocationId;
    void* pMemBlock;
    size_t Alignment;
    size_t RequestSize;
    size_t AllocatedSize;
    unsigned long AllocThreadId;
    LMemOP AllocType;
    KFLF m_kFLF;

	// hash table support
	LAllocUnit* m_pkPrev;
	LAllocUnit* m_pkNext;
    void Reset();
};

class KMemTracker
{
    // Memory tracking interface
public:
    KMemTracker( BOOL bWriteToLog = true,
        unsigned int uiInitialSize = 65536, 
        unsigned int uiGrowBy = 4096, BOOL bAlwaysValidateAll = false,
        BOOL bCheckArrayOverruns = false);
    ~KMemTracker();

    void* Allocate(size_t stSize, size_t stAlignment,
        LMemOP eEventType,BOOL bSizeOnDeallocate,
        const char* pcFile, int iLine, const char* pcFunction);
    void Deallocate(void* pvMemory, 
        LMemOP eEventType, size_t stSizeinBytes);
    void* Reallocate(void* pvMemory, size_t stSize,
        size_t stAlignment, LMemOP eEventType, BOOL bSizeOnDeallocate,
         size_t stSizeCurrent, 
        const char* pcFile, int iLine, const char* pcFunction);



    void Initialize();
    void Shutdown();


    // additional summary statistics
    size_t m_stActiveMemory; // in bytes
    size_t m_stPeakMemory; // in bytes
    size_t m_stAccumulatedMemory; // in bytes
    size_t m_stUnusedButAllocatedMemory; // in bytes

    size_t m_stActiveAllocationCount;
    size_t m_stPeakAllocationCount;
    size_t m_stAccumulatedAllocationCount;

    size_t m_stActiveTrackerOverhead;
    size_t m_stPeakTrackerOverhead;
    size_t m_stAccumulatedTrackerOverhead;
    
    size_t m_stActiveExternalMemory; // in bytes
    size_t m_stPeakExternalMemory; // in bytes
    size_t m_stAccumulatedExternalMemory; // in bytes
    
    size_t m_stActiveExternalAllocationCount;
    size_t m_stPeakExternalAllocationCount;
    size_t m_stAccumulatedExternalAllocationCount;

    float m_fPeakMemoryTime;
    float m_fPeakAllocationCountTime;

    size_t GetCurrentAllocationID() const;

    LAllocUnit* FindAllocUnit(const void* pvMem) const;

   
    void LogMemoryReport() const;
    void LogSummaryStats(FILE* fp) const;
    void ResetSummaryStats();
    void OutputLeakedMemoryToDebugStream(FILE* fp) const;
    void OutputActiveToDebugStream(const char* pcStringId = "", 
        size_t stMinAllocID = 0) const;

	void Report();
    // tracking options
    BOOL GetAlwaysValidateAll() const;
    void SetAlwaysValidateAll(BOOL bOn);

    BOOL GetCheckForArrayOverruns() const;

    BOOL ValidateAllocUnit(const LAllocUnit* pkUnit) const;
    BOOL ValidateAllAllocUnits() const;

protected:
    void GrowReservoir();
    void InsertAllocUnit(LAllocUnit* pkUnit);
    void RemoveAllocUnit(LAllocUnit* pkUnit);
        
    static unsigned int AddressToHashIndex(const void* pvAddress);
  
    void MemoryFillWithPattern(void* pvMemory, size_t stSizeinBytes);
    size_t MemoryBytesWithPattern(void* pvMemory, size_t stSizeinBytes) const;

    // Note that MemoryFillForArrayOverrun and CheckForArrayOverrun
    // modify the address passed in to reflect the sentinel characters
    // before the allocation. PadForArrayOverrun will adjust the size
    // of an allocation to reflect the sentinel characters.
    void MemoryFillForArrayOverrun(void*& pvMemory, size_t stAlignment, 
        size_t stSizeOriginal);
    BOOL CheckForArrayOverrun(void*& pvMemory, size_t stAlignment, 
        size_t stSizeOriginal) const;
    size_t PadForArrayOverrun(size_t stAlignment, 
        size_t stSizeOriginal);

    static const unsigned int ms_uiHashBits = 12;
    static const unsigned int ms_uiHashSize = 1 << ms_uiHashBits;
    
    BOOL m_bAlwaysValidateAll;
    LAllocUnit* m_pkReservoir;
    size_t m_stReservoirGrowBy;
    LAllocUnit** m_ppkReservoirBuffer;
    size_t m_stReservoirBufferSize;
    LAllocUnit* m_pkActiveMem[ms_uiHashSize];
    
    int m_iLoggerLeakChannel;
    int m_iLoggerCompleteChannel;

   // ILAllocator* m_pkActualAllocator;
    size_t m_stCurrentAllocID;

    BOOL m_bCheckArrayOverruns;
    unsigned char m_ucFillChar;
    VSyncCriticalSection m_kCriticalSection;
};
extern KMemTracker g_MemTracker;

#if defined(SUPPORT_DYNMAIC_IMPORT)

inline KFLF::KFLF(const char* pcFile, unsigned int uiLine, 
					const char* pcFunc)
{
	Set(pcFile, uiLine, pcFunc);
}

inline void KFLF::Set(const char* pcFile, unsigned int uiLine, 
					   const char* pcFunc)
{
	if (pcFile != NULL)
	{
		size_t stStrLen = strlen(pcFile);
		if (stStrLen >= MAX_FLF_LENGTH)
		{
			pcFile = pcFile + stStrLen - (MAX_FLF_LENGTH - 1);
		}
#if VPLATFORM_WIN32
		strcpy_s(m_pcFile, MAX_FLF_LENGTH, pcFile);
#else 
		strcpy(m_pcFile, pcFile);
#endif 
	}
	else
	{
		m_pcFile[0] = '\0';
	}

	m_uiLine = uiLine;

	if (pcFunc != NULL)
	{
		size_t stStrLen = strlen(pcFunc);
		if (stStrLen >= MAX_FLF_FUNC_LENGTH)
		{
			pcFunc = pcFunc + stStrLen - (MAX_FLF_FUNC_LENGTH - 1);
		}
#if VPLATFORM_WIN32
		strcpy_s(m_pcFunc, MAX_FLF_FUNC_LENGTH, pcFunc);
#else 
		strcpy(m_pcFunc, pcFunc);
#endif 
	}
	else
	{
		m_pcFunc[0] = '\0';
	}
}

inline void KFLF::Reset()
{
	Set(KFLF::UNKNOWN.m_pcFile, 
		KFLF::UNKNOWN.m_uiLine,
		KFLF::UNKNOWN.m_pcFunc);
}
#else 
inline KFLF::KFLF(const char* pcFile, unsigned int uiLine, 
					const char* pcFunc) :
m_pcFile(pcFile), m_pcFunc(pcFunc), m_uiLine(uiLine) 
{
}
//---------------------------------------------------------------------------
inline void KFLF::Set(const char* pcFile, unsigned int uiLine, 
					   const char* pcFunc)
{
	m_pcFile = pcFile;
	m_uiLine = uiLine;
	m_pcFunc = pcFunc;
}
//---------------------------------------------------------------------------
inline void KFLF::Reset()
{
	m_pcFile = KFLF::UNKNOWN.m_pcFile;
	m_uiLine = KFLF::UNKNOWN.m_uiLine;
	m_pcFunc = KFLF::UNKNOWN.m_pcFunc;
}
#endif
//---------------------------------------------------------------------------
inline KFLF::KFLF()
{
	Reset();
}
//---------------------------------------------------------------------------
inline BOOL KFLF::IsValid() const
{
	return (m_uiLine != 0);
}

//---------------------------------------------------------------------------
inline void LAllocUnit::Reset()
{
	AllocationId = (size_t) -1;
	Alignment = 0;
	AllocThreadId = 0;
	pMemBlock = NULL;
	RequestSize = 0;
	AllocatedSize = 0;
	m_kFLF.Reset();
	AllocType = VMOP_UNKNOWN;
	m_pkPrev = NULL;
	m_pkNext = NULL;
}
//---------------------------------------------------------------------------
inline BOOL KMemTracker::GetAlwaysValidateAll() const
{
	return m_bAlwaysValidateAll;
}
//---------------------------------------------------------------------------
inline void KMemTracker::SetAlwaysValidateAll(BOOL bOn)
{
	m_bAlwaysValidateAll = bOn;
}
//---------------------------------------------------------------------------
inline BOOL KMemTracker::GetCheckForArrayOverruns() const
{
	return m_bCheckArrayOverruns;
}
//---------------------------------------------------------------------------
inline unsigned int KMemTracker::AddressToHashIndex(const void* pvAddress)
{
	// Use the address to locate the hash index. Note that we shift off the 
	// lower four bits. This is because most allocated addresses will be on 
	// four-, eight- or even sixteen-byte boundaries. If we didn't do this, 
	// the hash index would not have very good coverage.

	// The size_t may be cast to an unsigned int as long as ms_uiHashSize
	// is less than the largest unsigned int.

	return (unsigned int)(reinterpret_cast<size_t>(pvAddress) >> 4) & 
		(ms_uiHashSize - 1);
}

//---------------------------------------------------------------------------
inline size_t KMemTracker::GetCurrentAllocationID() const
{
	return m_stCurrentAllocID;
}

#endif 

#endif 

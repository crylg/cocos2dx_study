#include "VStdAfx.h"

#if defined(VMEMORY_DEBUG)
#include "KMemTracker.h"
#include "KMemory.h"

inline const char* MemEventToString(LMemOP eEventType)
{
	switch (eEventType)
	{
	case VMOP_UNKNOWN:
		return "UNKNOWN";
	case VMOP_NEW:
		return "new";
	case VMOP_NEW_ARR:
		return "new[]";
	case VMOP_DELETE:
		return "delete";
	case VMOP_DELETE_ARR:
		return "delete[]";
	case VMOP_MALLOC:
		return "malloc";
	case VMOP_REALLOC:
		return "realloc";
	case VMOP_ALIGN_MALLOC:
		return "align_malloc";
	case VMOP_ALIGN_REALLOC:
		return "align_realloc";
	case VMOP_FREE:
		return "free";
	case VMOP_ALIGN_FREE:
		return "align_free";

	default:
		VASSERT(!"Unknown mem op!");
	};

	return NULL;
}

KMemTracker g_MemTracker;
const KFLF KFLF::UNKNOWN("",0,"");


// Use this setting to determine whether or not to flush the log file
// every time a line is sent or wait until the internal buffer is 
// full to flush the log file.
#define MEM_FLUSH_LOG_ON_WRITE false



void KMemTracker::Initialize()
{
    m_stCurrentAllocID = 0;
    ResetSummaryStats();
   // if (m_pkActualAllocator)
    //    m_pkActualAllocator->Initialize();
}

void KMemTracker::Shutdown()
{

	Report();
   

   // if (m_pkActualAllocator)
    //    m_pkActualAllocator->Shutdown();
}

void KMemTracker::ResetSummaryStats()
{
    m_fPeakMemoryTime = 0.0f;
    m_fPeakAllocationCountTime = 0.0f;
    m_stActiveMemory = 0;
    m_stPeakMemory = 0;
    m_stAccumulatedMemory = 0;
    m_stUnusedButAllocatedMemory = 0;

    m_stActiveAllocationCount = 0;
    m_stPeakAllocationCount = 0;
    m_stAccumulatedAllocationCount = 0;

    m_stActiveExternalMemory = 0; 
    m_stPeakExternalMemory = 0; 
    m_stAccumulatedExternalMemory = 0; 
    
    m_stActiveExternalAllocationCount = 0;
    m_stPeakExternalAllocationCount = 0;
    m_stAccumulatedExternalAllocationCount = 0;
}

KMemTracker::KMemTracker(
    BOOL bWriteToLog, unsigned int uiInitialSize, unsigned int uiGrowBy,
    BOOL bAlwaysValidateAll, BOOL bCheckArrayOverruns) : 
    m_fPeakMemoryTime(0.0f), m_fPeakAllocationCountTime(0.0f), 
    m_bAlwaysValidateAll(bAlwaysValidateAll), 
    m_stReservoirGrowBy(uiGrowBy),
    m_bCheckArrayOverruns(bCheckArrayOverruns), m_ucFillChar(0xBD)
{
    memset(m_pkActiveMem, 0, ms_uiHashSize*sizeof(LAllocUnit*));    
    if (uiInitialSize > 0)
    {
        // create initial set of tracking data structures
        m_stReservoirBufferSize = 1;
        m_pkReservoir = (LAllocUnit*)malloc(uiInitialSize * sizeof(LAllocUnit));

        // If you hit this VASSERT, then the memory manager failed to 
        // allocate internal memory for tracking the allocations
        VASSERT(m_pkReservoir != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        // throw an exception in this situation
        if (m_pkReservoir == NULL) 
            throw "Unable to allocate RAM for internal memory tracking data";
#endif

        // initialize the allocation units
        for (unsigned int i = 0; i < uiInitialSize-1; i++)
        {
            m_pkReservoir[i].m_kFLF = KFLF::UNKNOWN;
            m_pkReservoir[i].m_pkNext = &m_pkReservoir[i+1];
        }
        m_pkReservoir[uiInitialSize-1].m_kFLF = KFLF::UNKNOWN;
        m_pkReservoir[uiInitialSize-1].m_pkNext = NULL;

        m_ppkReservoirBuffer = (LAllocUnit**)malloc(sizeof(LAllocUnit*));

        // If you hit this VASSERT, then the memory manager failed to 
        // allocate internal memory for tracking the allocations
        VASSERT(m_ppkReservoirBuffer != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        // throw an exception in this situation
        if (m_ppkReservoirBuffer == NULL) 
            throw "Unable to allocate RAM for internal memory tracking data";
#endif

        m_ppkReservoirBuffer[0] = m_pkReservoir;

        unsigned int uiInitialBytes = uiInitialSize*sizeof(LAllocUnit) + 
            sizeof(LAllocUnit*);
        m_stActiveTrackerOverhead = uiInitialBytes;
        m_stPeakTrackerOverhead = uiInitialBytes;
        m_stAccumulatedTrackerOverhead = uiInitialBytes;
    }
    else
    {
        m_stReservoirBufferSize = 0;
    }
	Initialize();
}

KMemTracker::~KMemTracker()
{
	Report();
    // Free all memory used by the hash table of alloc units
    if (m_ppkReservoirBuffer)
    {
        for (unsigned int i = 0; i < m_stReservoirBufferSize; i++)
        {
            free(m_ppkReservoirBuffer[i]);
        }
        free(m_ppkReservoirBuffer);
		m_ppkReservoirBuffer = NULL;
    }
	
	// shutdown allocator.
	//Shutdown();
}
//---------------------------------------------------------------------------
void KMemTracker::GrowReservoir()
{
    VASSERT(!m_pkReservoir);

    m_pkReservoir = (LAllocUnit*)malloc(sizeof(LAllocUnit)*m_stReservoirGrowBy);

    // If you hit this VASSERT, then the memory manager failed to allocate 
    // internal memory for tracking the allocations
    VASSERT(m_pkReservoir != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    // throw an exception in this situation
    if (m_pkReservoir == NULL) 
        throw "Unable to allocate RAM for internal memory tracking data";
#endif

    m_stActiveTrackerOverhead += sizeof(LAllocUnit) * 
        m_stReservoirGrowBy + sizeof(LAllocUnit*);

    if (m_stActiveTrackerOverhead > 
        m_stPeakTrackerOverhead)
    {
        m_stPeakTrackerOverhead = 
            m_stActiveTrackerOverhead;
    }
    m_stAccumulatedTrackerOverhead += sizeof(LAllocUnit) * 
        m_stReservoirGrowBy + sizeof(LAllocUnit*);

    // Build a linked-list of the elements in the reservoir
    // Initialize the allocation units
    for (unsigned int i = 0; i < m_stReservoirGrowBy-1; i++)
    {
        m_pkReservoir[i].m_kFLF = KFLF::UNKNOWN;
        m_pkReservoir[i].m_pkNext = &m_pkReservoir[i+1];
    }
    m_pkReservoir[m_stReservoirGrowBy-1].m_kFLF = KFLF::UNKNOWN;
    m_pkReservoir[m_stReservoirGrowBy-1].m_pkNext = NULL;

    // Add this address to the reservoir buffer so it can be freed later
    LAllocUnit **pkTemp = (LAllocUnit**)realloc(
        m_ppkReservoirBuffer, 
        (m_stReservoirBufferSize + 1) * sizeof(LAllocUnit*));

    // If you hit this VASSERT, then the memory manager failed to allocate 
    // internal memory for tracking the allocations
    VASSERT(pkTemp != NULL);
    if (pkTemp)
    {
        m_ppkReservoirBuffer = pkTemp;
        m_ppkReservoirBuffer[m_stReservoirBufferSize] = m_pkReservoir;
        m_stReservoirBufferSize++;
    }
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    else
    {
        // throw an exception in this situation
        throw "Unable to allocate RAM for internal memory tracking data";
    }
#endif
}
//---------------------------------------------------------------------------
void* KMemTracker::Allocate(
    size_t stSizeInBytes,
    size_t stAlignment,
    LMemOP eEventType,
	BOOL bSizeOnDeallocate,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    m_kCriticalSection.Lock();

    size_t stSizeOriginal = stSizeInBytes;

    if (m_bCheckArrayOverruns)
    {
       stSizeInBytes= PadForArrayOverrun(stAlignment, stSizeInBytes);
    }

    // Actually perform the allocation. Note that size and alignment
    // may be adjusted by the call.
   // void* pvMem = m_pkActualAllocator->Allocate(stSizeInBytes, stAlignment,
    //    eEventType, bSizeOnDeallocate, pcFile, iLine, pcFunction); 
	void* pvMem = g_PoolAllocator->Malloc(stSizeInBytes, stAlignment);
	//void* pvMem = PoolAllocator::allocate(stSizeInBytes);
    if (pvMem == NULL)
    {
        // If you hit this, your memory request was not satisfied
        m_kCriticalSection.Unlock();
        return NULL;
    }

    VASSERT(!FindAllocUnit(pvMem));

    // update summary statistics
    m_stActiveAllocationCount++;
    m_stAccumulatedAllocationCount++;
    if (m_stActiveAllocationCount > m_stPeakAllocationCount)
    {
        m_stPeakAllocationCount = m_stActiveAllocationCount;
       // m_fPeakAllocationCountTime = fTime;
    }

    m_stActiveMemory += stSizeInBytes;
    m_stAccumulatedMemory += stSizeInBytes;
    if (m_stActiveMemory > m_stPeakMemory)
    {
        m_stPeakMemory = m_stActiveMemory;
       // m_fPeakMemoryTime = fTime;
    }

    
    // If you hit this VASSERT, your memory request result was smaller 
    // than the input.
    VASSERT(stSizeInBytes >= stSizeOriginal);

    // Pad the start and end of the allocation with the pad character 
    // so that we can check for array under and overruns. Note that the
    // address is shifted to hide the padding before the allocation.
    if (m_bCheckArrayOverruns)
    {
        MemoryFillForArrayOverrun(pvMem, stAlignment, stSizeOriginal);
    }

    // Fill the originally requested memory size with the pad character.
    // This will allow us to see how much of the allocation was 
    // actually used.
    MemoryFillWithPattern(pvMem, stSizeOriginal);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    try
    {
#endif

    // If you hit this VASSERT, the somehow you have allocated a memory
    // unit to an address that already exists. This should never happen
    // and is an indicator that something has gone wrong in the sub-allocator.
    VASSERT(FindAllocUnit(pvMem) == NULL);

    // Grow the tracking unit reservoir if necessary
    if (!m_pkReservoir)
        GrowReservoir();

    // If you hit this VASSERT, the free store for allocation units
    // does not exist. This should only happen if the reservoir 
    // needed to grow and was unable to satisfy the request. In other words,
    // you may be out of memory.
    VASSERT (m_pkReservoir != NULL);

    // Get an allocation unit from the reservoir
    LAllocUnit* pkUnit = m_pkReservoir;
    m_pkReservoir = pkUnit->m_pkNext;

    // fill in the known information
    pkUnit->Reset();
    pkUnit->AllocationId = m_stCurrentAllocID;
    pkUnit->Alignment = stAlignment;

#if VPLATFORM_WIN32
    pkUnit->AllocThreadId = GetCurrentThreadId();
#elif VPLATFORM_ANDROID
	pkUnit->AllocThreadId = gettid();
#endif 
    pkUnit->AllocType = eEventType;
    pkUnit->pMemBlock = pvMem;
    pkUnit->RequestSize = stSizeOriginal;
    pkUnit->AllocatedSize = stSizeInBytes;
    pkUnit->m_kFLF.Set(pcFile, iLine, pcFunction);

    // If you hit this VASSERT, then this allocation was made from a 
    // source that isn't setup to use this memory tracking software, use the 
    // stack frame to locate the source and include KMemManager.h.
    VASSERT(eEventType != VMOP_UNKNOWN);

    // Insert the new allocation into the hash table
    InsertAllocUnit(pkUnit);

    //// Validate every single allocated unit in memory
    //if (m_bAlwaysValidateAll)
    //{
    //    BOOL POSSIBLY_UNUSED bValidateAllAllocUnits = ValidateAllAllocUnits();
    //    VASSERT(bValidateAllAllocUnits);
    //}

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
        // Deal with the errors
        // Deal with the errors
    }
#endif

    ++m_stCurrentAllocID;
    m_kCriticalSection.Unlock();
    
    return pvMem;
}   
//---------------------------------------------------------------------------
void KMemTracker::InsertAllocUnit(LAllocUnit* pkUnit)
{
    VASSERT(pkUnit != NULL && pkUnit->pMemBlock != NULL);
    unsigned int uiHashIndex = AddressToHashIndex(pkUnit->pMemBlock);

    // Remap the new allocation unit to the head of the hash entry
    if (m_pkActiveMem[uiHashIndex])
    {
        VASSERT(m_pkActiveMem[uiHashIndex]->m_pkPrev == NULL);
        m_pkActiveMem[uiHashIndex]->m_pkPrev = pkUnit;
    }

    VASSERT(pkUnit->m_pkNext == NULL);
    pkUnit->m_pkNext = m_pkActiveMem[uiHashIndex];
    pkUnit->m_pkPrev = NULL;
    m_pkActiveMem[uiHashIndex] = pkUnit;
}
//---------------------------------------------------------------------------
void KMemTracker::RemoveAllocUnit(LAllocUnit* pkUnit)
{
    VASSERT(pkUnit != NULL && pkUnit->pMemBlock != NULL);
    unsigned int uiHashIndex = AddressToHashIndex(pkUnit->pMemBlock);

    // If you hit this VASSERT, somehow we have emptied the
    // hash table for this bucket. This should not happen
    // and is indicative of a serious error in the memory
    // tracking infrastructure.
    VASSERT(m_pkActiveMem[uiHashIndex] != NULL);

    if (m_pkActiveMem[uiHashIndex] == pkUnit)
    {
        VASSERT(pkUnit->m_pkPrev == NULL);
        m_pkActiveMem[uiHashIndex] = pkUnit->m_pkNext;
        
        if (m_pkActiveMem[uiHashIndex])
            m_pkActiveMem[uiHashIndex]->m_pkPrev = NULL;
    }
    else
    {      
        if (pkUnit->m_pkPrev)
        {
            VASSERT(pkUnit->m_pkPrev->m_pkNext == pkUnit);
            pkUnit->m_pkPrev->m_pkNext = pkUnit->m_pkNext;
        }
        if (pkUnit->m_pkNext)
        {
            VASSERT(pkUnit->m_pkNext->m_pkPrev == pkUnit);
            pkUnit->m_pkNext->m_pkPrev = pkUnit->m_pkPrev;
        }
    }
}
//---------------------------------------------------------------------------
void* KMemTracker::Reallocate(
    void* pvMemory,
    size_t stSizeInBytes,
    size_t stAlignment,
    LMemOP eEventType,
	BOOL bSizeOnDeallocate,
    size_t stSizeCurrent,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    // Store the original request size for later use.
    size_t stSizeOriginal = stSizeInBytes;

    // If the address is null, then this should function just 
    // like an allocation routine.
    if (pvMemory == NULL)
    {
        return Allocate(stSizeInBytes, stAlignment, eEventType, bSizeOnDeallocate,
            pcFile, iLine, pcFunction);
    }

    // If the requested size is 0, then this should function
    // just like a deallocation routine.
    if (stSizeInBytes== 0)
    {
        Deallocate(pvMemory, eEventType, (size_t)-1);
        return NULL;
    }

    m_kCriticalSection.Lock();
    
  //  float fTime = LGetTimeSec();

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    try
    {
#endif
    // A reallocation is tracked with two allocation units. The first
    // is the old unit. The second unit tracks the new allocation, which 
    // may or may not have the same address as the old allocation. 
    // Its allocation type is set to the event type passed into this function.

    LAllocUnit* pkUnit = FindAllocUnit(pvMemory);

    if (pkUnit == NULL)
    {
        // If you hit this VASSERT, you tried to reallocate RAM that 
        // wasn't allocated by this memory manager.
        VASSERT(pkUnit != NULL);
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        throw "Request to reallocate RAM that was never allocated";
#endif
        m_kCriticalSection.Unlock();
    
        return NULL;
    }


    // If you hit this VASSERT, then the allocation unit that is about 
    // to be deleted requested an initial size that doesn't match
    // what is currently the 'size' argument for deallocation.   
    // This is most commonly caused by the lack of a virtual destructor
    // for a class that is used polymorphically in an array.
    if (stSizeCurrent != (size_t)-1)
	{
        VASSERT(stSizeCurrent == pkUnit->RequestSize);
	}

    if (m_bCheckArrayOverruns)
    {
        // If you hit this VASSERT, you have code that overwrites
        // either before or after the range of an allocation.
        // Check the pkUnit for information about which allocation
        // is being overwritten. 
        VASSERT(!CheckForArrayOverrun(pvMemory,
            pkUnit->Alignment, pkUnit->RequestSize));   
        stSizeInBytes= PadForArrayOverrun(stAlignment, stSizeInBytes);

        if (stSizeCurrent != (size_t)-1)
        {
            stSizeCurrent = PadForArrayOverrun(pkUnit->Alignment, 
                stSizeCurrent);
        }
    }
    
    // If you hit this VASSERT, then the allocation unit that is about to 
    // be reallocated is damaged. 
    VASSERT(ValidateAllocUnit(pkUnit));

    // Alignment should be the same between reallocations
    VASSERT(pkUnit->Alignment == stAlignment);

    // Determine how much memory was actually set
    size_t stSizeUnused = MemoryBytesWithPattern(pkUnit->pMemBlock,
        pkUnit->RequestSize);
    m_stUnusedButAllocatedMemory += stSizeUnused;

    // Save the thread id that freed the memory
    unsigned long ulFreeThreadId = GetCurrentThreadId();

    // Perform the actual memory reallocation
    //void* pvNewMemory = m_pkActualAllocator->Reallocate(pvMemory,
    //    stSizeInBytes, stAlignment, eEventType, bSizeOnDeallocate, stSizeCurrent,
     //   pcFile, iLine, pcFunction);
	void* pvNewMemory = g_PoolAllocator->Realloc(pvMemory, stSizeInBytes, stAlignment);
	//void* pvNewMemory = PoolAllocator::Realloc(pvMemory, stSizeInBytes);
    // If you hit this VASSERT, then the reallocation was unable to
    // be satisfied
    VASSERT(pvNewMemory != NULL);

    // update summary statistics
    size_t stPreReallocSize = pkUnit->RequestSize;
    
    m_stAccumulatedAllocationCount++;
    int iDeltaSize = stSizeInBytes - pkUnit->AllocatedSize;
    m_stActiveMemory += iDeltaSize;

    if (iDeltaSize > 0)
        m_stAccumulatedMemory += iDeltaSize;

    if (m_stActiveMemory > m_stPeakMemory)
    {
        m_stPeakMemory = m_stActiveMemory;
       // m_fPeakMemoryTime = fTime;
    }

    // Pad the start and end of the allocation with the pad character 
    // so that we can check for array under and overruns.
    if (m_bCheckArrayOverruns)
    {
        MemoryFillForArrayOverrun(pvNewMemory, stAlignment, stSizeOriginal);
    }

    // Fill the originally requested memory size with the pad character.
    // This will allow us to see how much of the allocation was 
    // actually used.
    // For reallocation, we only want to fill the unused section
    // that was just allocated.
    if (iDeltaSize > 0)
    {
        MemoryFillWithPattern((char*)pvNewMemory + stPreReallocSize, 
            iDeltaSize);
    }

    // If you hit this VASSERT, then this reallocation was made from a  
    // that isn't setup to use this memory tracking software, use the stack
    // source frame to locate the source and include KMemManager.h.
    VASSERT(eEventType != VMOP_UNKNOWN);

    // If you hit this VASSERT, you were trying to reallocate RAM that was 
    // not allocated in a way that is compatible with realloc. In other 
    // words, you have a allocation/reallocation mismatch.
    VASSERT(pkUnit->AllocType == VMOP_MALLOC ||
            pkUnit->AllocType == VMOP_REALLOC ||
            pkUnit->AllocType == VMOP_ALIGN_MALLOC ||
            pkUnit->AllocType == VMOP_ALIGN_REALLOC);

    // update allocation unit
    LMemOP eDeallocType = eEventType;
   // float fDeallocTime = fTime;
    
    //// Write out the freed memory to the memory log
    //LogAllocUnit(pkUnit, MEM_LOG_COMPLETE, "\t", eDeallocType,
    //    fDeallocTime, ulFreeThreadId, stSizeUnused);

    // Remove this allocation unit from the hash table
    RemoveAllocUnit(pkUnit);   
    --m_stActiveAllocationCount;

    if (m_bAlwaysValidateAll)
        VASSERT(ValidateAllAllocUnits());

    // Recycle the allocation unit
    // Add it to the front of the reservoir 
    pkUnit->m_kFLF = KFLF::UNKNOWN;
    pkUnit->m_pkNext = m_pkReservoir;
    m_pkReservoir = pkUnit;
    pkUnit = NULL;

    // Grow the tracking unit reservoir if necessary
    if (!m_pkReservoir)
        GrowReservoir();

    // If you hit this VASSERT, the free store for allocation units
    // does not exist. This should only happen if the reservoir 
    // needed to grow and was unable to satisfy the request. In other words,
    // you may be out of memory.
    VASSERT (m_pkReservoir != NULL);

    // Get an allocation unit from the reservoir
    LAllocUnit* pkNewUnit = m_pkReservoir;
    m_pkReservoir = pkNewUnit->m_pkNext;

    // fill in the known information
    pkNewUnit->Reset();
    pkNewUnit->AllocationId = m_stCurrentAllocID; 
    pkNewUnit->Alignment = stAlignment;
    pkNewUnit->AllocThreadId = GetCurrentThreadId();
    pkNewUnit->AllocType = eEventType;
   // pkNewUnit->m_fAllocTime = fTime;
    pkNewUnit->pMemBlock = pvNewMemory;
    pkNewUnit->AllocatedSize = stSizeInBytes;
    pkNewUnit->RequestSize = stSizeOriginal;
    pkNewUnit->m_kFLF.Set(pcFile, iLine, pcFunction);

    // If you hit this VASSERT, then this allocation was made from a source
    // that isn't setup to use this memory tracking software, use the 
    // stack frame to locate the source and include KMemManager.h.
    VASSERT(eEventType != VMOP_UNKNOWN);

    // Insert the new allocation into the hash table
    InsertAllocUnit(pkNewUnit);
    ++m_stActiveAllocationCount;
    ++m_stCurrentAllocID;

    if (m_bAlwaysValidateAll)
        VASSERT(ValidateAllAllocUnits());


#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
    }
#endif
    m_kCriticalSection.Unlock();
    
    return pvNewMemory;
}
//---------------------------------------------------------------------------
void KMemTracker::Deallocate(
    void* pvMemory,
    LMemOP eEventType,
    size_t stSizeInBytes)
{
    if (pvMemory)
    {

        m_kCriticalSection.Lock();
    
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        try {
#endif
       // float fTime = LGetTimeSec();

        // Search the tracking unit hash table to find the address
        LAllocUnit* pkUnit = FindAllocUnit(pvMemory);

        if (pkUnit == NULL)
        {
            // If you hit this VASSERT, you tried to deallocate RAM that 
            // wasn't allocated by this memory manager. This may also
            // be indicative of a double deletion. Please check the pkUnit
            // FLF for information about the allocation.
            VASSERT(pkUnit != NULL);
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
            throw "Request to deallocate RAM that was never allocated";
#endif
            m_kCriticalSection.Unlock();
    
            return;
        }

        // If you hit this VASSERT, then the allocation unit that is about 
        // to be deleted requested an initial size that doesn't match
        // what is currently the 'size' argument for deallocation.
        // This is most commonly caused by the lack of a virtual destructor
        // for a class that is used polymorphically in an array.
        if (stSizeInBytes != (size_t)-1)
		{
            VASSERT(stSizeInBytes== pkUnit->RequestSize);
		}

        // If you hit this VASSERT, then the allocation unit that is about 
        // to be deallocated is damaged.
        BOOL bValidateAllocUnit = ValidateAllocUnit(pkUnit);
        VASSERT(bValidateAllocUnit);

        // If you hit this VASSERT, then this deallocation was made from a 
        // source that isn't setup to use this memory tracking software, 
        // use the stack frame to locate the source and include
        // KMemManager.h
        VASSERT(eEventType != VMOP_UNKNOWN);

        // If you hit this VASSERT, you were trying to deallocate RAM that 
        // was not allocated in a way that is compatible with the 
        // deallocation method requested. In other words, you have a 
        // allocation/deallocation mismatch.
        VASSERT((eEventType == VMOP_DELETE && 
            pkUnit->AllocType == VMOP_NEW) ||
            (eEventType == VMOP_DELETE_ARR && 
            pkUnit->AllocType == VMOP_NEW_ARR) ||
            (eEventType == VMOP_FREE && 
            pkUnit->AllocType == VMOP_MALLOC) ||
            (eEventType == VMOP_FREE && 
            pkUnit->AllocType == VMOP_REALLOC) ||
            (eEventType == VMOP_REALLOC && 
            pkUnit->AllocType == VMOP_MALLOC) ||
            (eEventType == VMOP_REALLOC && 
            pkUnit->AllocType == VMOP_REALLOC) ||
            (eEventType == VMOP_ALIGN_FREE && 
            pkUnit->AllocType == VMOP_ALIGN_MALLOC) ||
            (eEventType == VMOP_ALIGN_FREE && 
            pkUnit->AllocType == VMOP_ALIGN_REALLOC) ||
            (eEventType == VMOP_ALIGN_REALLOC && 
            pkUnit->AllocType == VMOP_ALIGN_MALLOC) ||
            (eEventType == VMOP_ALIGN_REALLOC && 
            pkUnit->AllocType == VMOP_ALIGN_REALLOC) ||
            (eEventType == VMOP_UNKNOWN));

        // update allocation unit
        LMemOP eDeallocType = eEventType;
        //float fDeallocTime = fTime;

        // Determine how much memory was actually set
        size_t stSizeUnused = MemoryBytesWithPattern(pvMemory,
            pkUnit->RequestSize);
        m_stUnusedButAllocatedMemory += stSizeUnused;

        // Save the thread id that freed the memory
        unsigned long ulFreeThreadId = GetCurrentThreadId();

        if (m_bCheckArrayOverruns)
        {
            // If you hit this VASSERT, you have code that overwrites
            // either before or after the range of an allocation.
            // Check the pkUnit for information about which allocation
            // is being overwritten. 
            BOOL bCheckForArrayOverrun =
                CheckForArrayOverrun(pvMemory, pkUnit->Alignment,
                    pkUnit->RequestSize);
            VASSERT(!bCheckForArrayOverrun);

            if (stSizeInBytes != (size_t)-1)
            {
                stSizeInBytes= PadForArrayOverrun(pkUnit->Alignment,
                    stSizeInBytes);
            }
        }

        // Perform the actual deallocation
       // m_pkActualAllocator->Deallocate(pvMemory,
       //     eEventType, stSizeInBytes);
		//PoolAllocator::deallocate(pvMemory);
		g_PoolAllocator->Free(pvMemory);
        // Remove this allocation unit from the hash table
        RemoveAllocUnit(pkUnit);

        // update summary statistics
        --m_stActiveAllocationCount;
        m_stActiveMemory -= pkUnit->AllocatedSize;
        
        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            BOOL bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            VASSERT(bValidateAllAllocUnits);
        }

        // Write out the freed memory to the memory log
      /*  LogAllocUnit(pkUnit, MEM_LOG_COMPLETE, "\t", eDeallocType,
            fDeallocTime, ulFreeThreadId, stSizeUnused);
        */
        // Recycle the allocation unit
        // Add it to the front of the reservoir 
        pkUnit->m_kFLF = KFLF::UNKNOWN;
        pkUnit->m_pkNext = m_pkReservoir;
        m_pkReservoir = pkUnit;
   
        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            BOOL bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            VASSERT(bValidateAllAllocUnits);
        }
        m_kCriticalSection.Unlock();
    
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
        // Deal with the errors
    }
#endif
    }
}

LAllocUnit* KMemTracker::FindAllocUnit(const void* pvMem) const
{
    // Just in case...
    VASSERT(pvMem != NULL);

    // Use the address to locate the hash index. Note that we shift off the 
    // lower four bits. This is because most allocated addresses will be on 
    // four-, eight- or even sixteen-byte boundaries. If we didn't do this, 
    // the hash index would not have very good coverage.

    unsigned int uiHashIndex = AddressToHashIndex(pvMem);

    LAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
    while(pkUnit)
    {
        if (pkUnit->pMemBlock == pvMem) 
            return pkUnit;

        pkUnit = pkUnit->m_pkNext;
    }

    return NULL;
}

//---------------------------------------------------------------------------
void KMemTracker::MemoryFillForArrayOverrun(void*& pvMemory, 
    size_t stAlignment, size_t stSizeOriginal)
{   
    char* pcMemArray = (char*) pvMemory;
    pvMemory = pcMemArray + stAlignment;
    MemoryFillWithPattern(pcMemArray, stAlignment);

    pcMemArray = pcMemArray + stAlignment + stSizeOriginal;
    MemoryFillWithPattern(pcMemArray, stAlignment);
}

void KMemTracker::MemoryFillWithPattern(void* pvMemory, 
    size_t stSizeInBytes)
{
   /* unsigned char* pcMemArray = (unsigned char*) pvMemory;
    for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    {
        pcMemArray[ui] = m_ucFillChar;
    }*/
}

size_t KMemTracker::MemoryBytesWithPattern(void* pvMemory, 
    size_t stSizeInBytes) const
{
    //unsigned char* pcMemArray = (unsigned char*) pvMemory;
    //size_t numBytes = 0;
    //for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    //{
    //    if (pcMemArray[ui] == m_ucFillChar)
    //    {
    //        numBytes++;
    //    }
    //}

    //return numBytes;
	return 0;
}

BOOL KMemTracker::CheckForArrayOverrun(void*& pvMemory, 
    size_t stAlignment, size_t stSizeOriginal) const
{
    VASSERT(m_bCheckArrayOverruns);

    char* pcMemArray = (char*) pvMemory;
    pcMemArray -= stAlignment;
    pvMemory = pcMemArray;

    if (stAlignment != MemoryBytesWithPattern(pcMemArray, stAlignment))
        return true;
    
    pcMemArray = pcMemArray + stAlignment + stSizeOriginal;
    if (stAlignment != MemoryBytesWithPattern(pcMemArray, stAlignment))
        return true;
    
    return false;
}

size_t KMemTracker::PadForArrayOverrun(
    size_t stAlignment, 
    size_t stSizeOriginal)
{
    return stSizeOriginal + 2 * stAlignment;
}

BOOL KMemTracker::ValidateAllocUnit(const LAllocUnit* pkUnit) const
{
    if (pkUnit->AllocationId > m_stCurrentAllocID)
        return false;
    if (pkUnit->AllocatedSize < pkUnit->RequestSize)
        return false;
    if (pkUnit->AllocatedSize == 0 ||  pkUnit->RequestSize == 0)
        return false;
    if (pkUnit->pMemBlock == NULL)
        return false;
    if (pkUnit->m_pkNext != NULL && pkUnit->m_pkNext->m_pkPrev != pkUnit)
        return false;
    return true;
}

BOOL KMemTracker::ValidateAllAllocUnits() const
{
    unsigned int uiActiveCount = 0;
    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
        uiHashIndex++)
    {
        LAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
        LAllocUnit* pkPrev = NULL;

        while(pkUnit)
        {
            if (!ValidateAllocUnit(pkUnit))
                return false;
            
            if (pkUnit->m_pkPrev != pkPrev)
                return false;

            if (m_bCheckArrayOverruns)
            {
                void* pvMemStore = pkUnit->pMemBlock;
                if (CheckForArrayOverrun(pvMemStore, pkUnit->Alignment, 
                    pkUnit->RequestSize))
                {
                    return false;
                }
            }

            // continue to the next unit
            pkPrev = pkUnit;
            pkUnit = pkUnit->m_pkNext;
            uiActiveCount++;
        }
    }   

    if (uiActiveCount != this->m_stActiveAllocationCount)
        return false;

    return true;
}


void KMemTracker::LogMemoryReport() const
{
   // LogActiveMemoryReport();
}

void KMemTracker::OutputLeakedMemoryToDebugStream(FILE* fp) const
{
	if( m_stActiveAllocationCount )
	{
		fprintf(fp, "%d Memory leak found:\r\n", m_stActiveAllocationCount);
	}
	else
	{
		fprintf(fp, "No memory leaks found!\r\n");
	}
	fprintf(fp, "\r\n");

	if( m_stActiveAllocationCount )
	{
		fprintf(fp, "Alloc  Thread   Addr       Size                                      \r\n");
		fprintf(fp, " ID     ID     Actual     Actual     Unused    Method   Allocated by \r\n");
		fprintf(fp, "------ ------ ---------- ---------- ---------- -------- ----------------------------------------------------------------------- \r\n");

		for( UINT i = 0; i < ms_uiHashSize; i++ )
		{
			LAllocUnit* pUnit = m_pkActiveMem[i];
			while( pUnit )
			{
				fprintf(fp, "%06d %06d 0x%08X 0x%08X 0x%08X %-8s   %s(%d) %s\r\n",
					pUnit->AllocationId, pUnit->AllocThreadId,
					(size_t) pUnit->pMemBlock, pUnit->AllocatedSize,
					/*gpDbgMemMgr->calcUnused(ptr)*/0,
					MemEventToString(pUnit->AllocType),
					pUnit->m_kFLF.m_pcFile, pUnit->m_kFLF.m_uiLine, pUnit->m_kFLF.m_pcFunc);

					pUnit = pUnit->m_pkNext;
			}
		}
		fprintf(fp, "\r\n");
	}
  
}


void KMemTracker::LogSummaryStats(FILE* fp) const
{

	/*fprintf(fp, "summary stats:\n");
	fprintf(fp, "total active size :\t%d", m_stActiveMemory);
	fprintf(fp, "peak active size  :\t%d", m_stPeakMemory);
	fprintf(fp, "accumulated size  :\t%d", m_stAccumulatedMemory);
	fprintf(fp, "unused size       :\t%d", m_stUnusedButAllocatedMemory);
	fprintf(fp, "peak active size  :\t%d", m_stPeakMemory);
	fprintf(fp, "peak active size  :\t%d", m_stPeakMemory);
	fprintf(fp, "peak active size  :\t%d", m_stPeakMemory);
	fprintf(fp, "peak active size  :\t%d", m_stPeakMemory);*/
 /*   NILOG(MEM_LOG_COMPLETE, "\t<memory_summary timestamp='%f' ",
        LGetTimeSec());
    NILOG(MEM_LOG_COMPLETE, "TotalActiveSize='%d' ", 
        m_stActiveMemory);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveSize='%d' ", 
        m_stPeakMemory);
    NILOG(MEM_LOG_COMPLETE, "AccumulatedSize='%d' ", 
        m_stAccumulatedMemory);
    NILOG(MEM_LOG_COMPLETE, "AllocatedButUnusedSize='%d' ",
        m_stUnusedButAllocatedMemory);
    NILOG(MEM_LOG_COMPLETE, "ActiveAllocCount='%d' ", 
        m_stActiveAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveAllocCount='%d' ", 
        m_stPeakAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "TotalAllocCount='%d' ",
        m_stAccumulatedAllocationCount);    
    
    NILOG(MEM_LOG_COMPLETE, "TotalActiveExternalSize='%d' ", 
        m_stActiveExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveExternalSize='%d' ", 
        m_stPeakExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "AccumulatedExternalSize='%d' ", 
        m_stAccumulatedExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "ActiveExternalAllocCount='%d' ", 
        m_stActiveExternalAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "PeakExternalActiveAllocCount='%d' ", 
        m_stPeakExternalAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "TotalExternalAllocCount='%d' ",
        m_stAccumulatedExternalAllocationCount);    
    
    NILOG(MEM_LOG_COMPLETE, "TotalTrackerOverhead='%d' ", 
        m_stActiveTrackerOverhead);    
    NILOG(MEM_LOG_COMPLETE, "PeakTrackerOverhead='%d' ", 
        m_stPeakTrackerOverhead);    
    NILOG(MEM_LOG_COMPLETE, "AccumulatedTrackerOverhead='%d' ", 
        m_stAccumulatedTrackerOverhead);   

    NILOG(MEM_LOG_COMPLETE, "></memory_summary>\n");*/
}


void KMemTracker::Report()
{
#if VPLATFORM_WIN32
	FILE *fp;
	fopen_s(&fp, "memdbg.log", "wt+");
	VASSERT(fp);
	if( !fp )
		return;

	// Any leaks?

	// Header
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                   Venus Engine Memory Debug Report:  %04d/%02d/%02d %02d:%02d:%02d                                            |\r\n", Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "\r\n");


	
      OutputLeakedMemoryToDebugStream(fp);

    


        //LogMemoryReport();
      //  LogSummaryStats(fp);
        
	fflush(fp);
	fclose(fp);
	fp = NULL;
#endif 
}

#endif 
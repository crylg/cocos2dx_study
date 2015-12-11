#pragma once

#if VPLATFORM_WIN32
#define VUSE_MEMORY 1
#else 
#define VUSE_MEMORY 0
#endif 
#if VUSE_MEMORY

#define FORCEINLINE inline

#define DEFAULT_ALIGNMENT 8

#define MEM_TIME(st)

//#define USE_LOCKFREE_DELETE
#define USE_INTERNAL_LOCKS
#define CACHE_FREED_OS_ALLOCS


#ifdef USE_INTERNAL_LOCKS
//#	define USE_COARSE_GRAIN_LOCKS
#endif

#if defined USE_LOCKFREE_DELETE
#	define USE_INTERNAL_LOCKS
#	define USE_COARSE_GRAIN_LOCKS
#endif

#if defined CACHE_FREED_OS_ALLOCS
#	define MAX_CACHED_OS_FREES (32)
#	define MAX_CACHED_OS_FREES_BYTE_LIMIT (4*1024*1024)
#endif

#if defined USE_INTERNAL_LOCKS && !defined USE_COARSE_GRAIN_LOCKS
#	define USE_FINE_GRAIN_LOCKS
#endif



//
// Large size FixedAllocator.
//
class KPoolAllocator
{
private:

	// Counts.
	enum { POOL_COUNT = 42 };

	/** Maximum allocation for the pooled allocator */
	enum { EXTENED_PAGE_POOL_ALLOCATION_COUNT = 2 };
	enum { MAX_POOLED_ALLOCATION_SIZE   = 32768+1 };
	enum { PAGE_SIZE_LIMIT = 65536 };
	// BINNED_ALLOC_POOL_SIZE can be increased beyond 64k to cause binned malloc to allocate
	// the small size bins in bigger chunks. If OS Allocation is slow, increasing
	// this number *may* help performance but YMMV.
	enum { BINNED_ALLOC_POOL_SIZE = 65536 }; 

	// Forward declares.
	struct FFreeMem;
	struct FPoolTable;
#pragma pack(push)
#pragma pack(1)
	// Memory pool info. 32 bytes.
	struct PoolSlot
	{
		/** Number of allocated elements in this pool, when counts down to zero can free the entire pool. */
		WORD			Taken;		// 2
		/** Index of pool. Index into MemSizeToPoolTable[]. Valid when < MAX_POOLED_ALLOCATION_SIZE, MAX_POOLED_ALLOCATION_SIZE is OsTable.
			When AllocSize is 0, this is the number of pages to step back to find the base address of an allocation. See FindPoolInfoInternal()
		*/
		WORD			TableIndex; // 4		
		/** Number of bytes allocated */
		UINT			AllocSize;	// 8
		/** Pointer to first free memory in this pool or the OS Allocation Size in bytes if this allocation is not binned*/
		FFreeMem*		FirstMem;   // 12/16
		PoolSlot*		Next;		// 16/24
		PoolSlot**		PrevLink;	// 20/32

		void SetAllocationSizes( UINT InBytes, size_t InOsBytes, UINT InTableIndex, UINT SmallAllocLimt )
		{
			TableIndex=InTableIndex;
			AllocSize=InBytes;
			if (TableIndex == SmallAllocLimt)
			{
				FirstMem=(FFreeMem*)InOsBytes;
			}
		}

		inline UINT GetBytes() const
		{
			return AllocSize;
		}

		size_t GetOsBytes( UINT InPageSize, UINT SmallAllocLimt ) const
		{
			if (TableIndex == SmallAllocLimt)
			{
				return (size_t)FirstMem;
			}
			else
			{
				return VAlign(AllocSize, InPageSize);
			}
		}

		void Link( PoolSlot*& Before )
		{
			if( Before )
			{
				Before->PrevLink = &Next;
			}
			Next     = Before;
			PrevLink = &Before;
			Before   = this;
		}

		void Unlink()
		{
			if( Next )
			{
				Next->PrevLink = PrevLink;
			}
			*PrevLink = Next;
		}
	};

	/** Information about a piece of free memory. 8 bytes */
	struct FFreeMem
	{
		/** Next or MemLastPool[], always in order by pool. */
		FFreeMem*	Next;				
		/** Number of consecutive free blocks here, at least 1. */
		UINT		NumFreeBlocks;
	};
#pragma pack(pop)

	/** Default alignment for binned allocator */
	enum { DEFAULT_BINNED_ALLOCATOR_ALIGNMENT = sizeof(FFreeMem) };

	/** Pool table. */
	struct FPoolTable
	{
		PoolSlot*			FirstPool;
		PoolSlot*			ExhaustedPool;
		UINT				BlockSize;
#ifdef USE_FINE_GRAIN_LOCKS
		VSyncCriticalSection CriticalSection;
#endif
#if VPROFILE
		/** Number of currently active pools */
		UINT				NumActivePools;

		/** Largest number of pools simultaneously active */
		UINT				MaxActivePools;

		/** Number of requests currently active */
		UINT				ActiveRequests;

		/** High watermark of requests simultaneously active */
		UINT				MaxActiveRequests;

		/** Minimum request size (in bytes) */
		UINT				MinRequest;

		/** Maximum request size (in bytes) */
		UINT				MaxRequest;

		/** Total number of requests ever */
		UINT64				TotalRequests;

		/** Total waste from all allocs in this table */
		UINT64				TotalWaste;
#endif
		FPoolTable()
			: FirstPool(NULL)
			, ExhaustedPool(NULL)
			, BlockSize(0)
#if VPROFILE
			, NumActivePools(0)
			, MaxActivePools(0)
			, ActiveRequests(0)
			, MaxActiveRequests(0)
			, MinRequest(0)
			, MaxRequest(0)
			, TotalRequests(0)
			, TotalWaste(0)
#endif
		{

		}
	};

	/** Hash table struct for retrieving allocation book keeping information */
	struct PoolHashBucket
	{
		size_t			Key;
		PoolSlot*		FirstPool;
		PoolHashBucket* Prev;
		PoolHashBucket* Next;

		PoolHashBucket()
		{
			Key=0;
			FirstPool=NULL;
			Prev=this;
			Next=this;
		}

		void Link( PoolHashBucket* After )
		{
			Link(After, Prev, this);
		}

		static void Link( PoolHashBucket* Node, PoolHashBucket* Before, PoolHashBucket* After )
		{
			Node->Prev=Before;
			Node->Next=After;
			Before->Next=Node;
			After->Prev=Node;
		}

		void Unlink()
		{
			Next->Prev = Prev;
			Prev->Next = Next;
			Prev=this;
			Next=this;
		}
	};

	UINT64 TableAddressLimit;

	VSyncCriticalSection	AccessGuard;

	// m_PageSize dependent constants
	UINT64 MaxHashBuckets; 
	UINT64 MaxHashBucketBits;

	/** Shift to get the reference from the indirect tables */
	UINT64 PoolBitShift;
	UINT64 IndirectPoolBitShift;
	UINT64 IndirectPoolBlockSize;
	/** Shift required to get required hash table key. */
	UINT64 HashKeyShift;
	/** Used to mask off the bits that have been used to lookup the indirect table */
	UINT64 PoolMask;
	UINT64 BinnedSizeLimit;
	UINT64 BinnedOSTableIndex;

	// Variables.
	FPoolTable  PoolTable[POOL_COUNT];
	FPoolTable	OsTable;
	FPoolTable* MemSizeToPoolTable[MAX_POOLED_ALLOCATION_SIZE];
	PoolHashBucket* m_HashMap;
	PoolHashBucket* HashBucketFreeList;
	UINT		m_PageSize;
#if VPROFILE
	UINT		OsCurrent;
	UINT		OsPeak;
	UINT		WasteCurrent;
	UINT		WastePeak;
	UINT		UsedCurrent;
	UINT		UsedPeak;
	UINT		CurrentAllocs;
	UINT		TotalAllocs;
	double		MemTime;
#endif

#ifdef CACHE_FREED_OS_ALLOCS
	/**  */
	struct FFreePageBlock
	{
		void*				Ptr;
		size_t				ByteSize;

		FFreePageBlock() 
		{
			Ptr = NULL;
			ByteSize = 0;
		}
	};
	FFreePageBlock	FreedPageBlocks[MAX_CACHED_OS_FREES];
	UINT			FreedPageBlocksNum;
	UINT			CachedTotal;
#endif


	// Implementation. 
	void OutOfMemory(UINT64 Size, UINT Alignment=0)
	{
		// this is expected not to return
		//FPlatformMemory::OnOutOfMemory(Size, Alignment);
	}

	FORCEINLINE void TrackStats(FPoolTable* Table, size_t Size)
	{
#if STATS
		// keep track of memory lost to padding
		Table->TotalWaste += Table->BlockSize - Size;
		Table->TotalRequests++;
		Table->ActiveRequests++;
		Table->MaxActiveRequests = FMath::Max(Table->MaxActiveRequests, Table->ActiveRequests);
		Table->MaxRequest = Size > Table->MaxRequest ? Size : Table->MaxRequest;
		Table->MinRequest = Size < Table->MinRequest ? Size : Table->MinRequest;
#endif
	}

	inline void* System_Allocate(size_t Size)
	{
#if VPLATFORM_WIN32
		return VirtualAlloc( NULL, Size, MEM_COMMIT, PAGE_READWRITE);
#elif VPLATFORM_ANDROID || VPLATFORM_IOS

		// NOTE: android sdk 21 已经取消对valloc 的支持. 
		return memalign(sysconf(_SC_PAGESIZE), Size);
		//return valloc(Size);
#else 
		return _aligned_malloc(Size, DEFAULT_BINNED_ALLOCATOR_ALIGNMENT);
#endif 
	}

	inline void System_Free(void* Ptr)
	{
#if VPLATFORM_WIN32
		VirtualFree( Ptr, 0, MEM_RELEASE );
#elif VPLATFORM_ANDROID || VPLATFORM_IOS
		free(Ptr);
#else 
		_aligned_free(Ptr);
#endif 
	}


	inline void CacheFree(void* Ptr, size_t Size)
	{
#ifdef CACHE_FREED_OS_ALLOCS
#ifdef USE_FINE_GRAIN_LOCKS
		VAutoLockerCS MainLock(AccessGuard);
#endif
		if ((CachedTotal + Size > MAX_CACHED_OS_FREES_BYTE_LIMIT) || (Size > BINNED_ALLOC_POOL_SIZE))
		{
			System_Free(Ptr);
			return;
		}
		
		if (FreedPageBlocksNum >= MAX_CACHED_OS_FREES) 
		{
			//Remove the oldest one
			void* FreePtr = FreedPageBlocks[FreedPageBlocksNum-1].Ptr;
			CachedTotal -= FreedPageBlocks[FreedPageBlocksNum-1].ByteSize;
			--FreedPageBlocksNum;
			System_Free(FreePtr);
		}
		FreedPageBlocks[FreedPageBlocksNum].Ptr = Ptr;
		FreedPageBlocks[FreedPageBlocksNum].ByteSize = Size;
		CachedTotal += Size;
		++FreedPageBlocksNum;
#else
		(void)Size;
		System_Free(Ptr);
#endif
	}

	FORCEINLINE void* CacheAlloc(size_t NewSize, size_t& OutActualSize)
	{
#ifdef CACHE_FREED_OS_ALLOCS
		{
#ifdef USE_FINE_GRAIN_LOCKS
			// We want to hold the lock a little as possible so release it
			// before the big call to the OS
			VAutoLockerCS MainLock(AccessGuard);
#endif
			for (UINT i=0; i < FreedPageBlocksNum; ++i)
			{
				// is it possible (and worth i.e. <25% overhead) to use this block
				if (FreedPageBlocks[i].ByteSize >= NewSize && FreedPageBlocks[i].ByteSize * 3 <= NewSize * 4)
				{
					void* Ret=FreedPageBlocks[i].Ptr;
					OutActualSize=FreedPageBlocks[i].ByteSize;
					CachedTotal-=FreedPageBlocks[i].ByteSize;
					FreedPageBlocks[i]=FreedPageBlocks[--FreedPageBlocksNum];
					return Ret;
				}
			};
		}
		OutActualSize=NewSize;
		void* Ptr = System_Allocate(NewSize);
		if (!Ptr)
		{
			//Are we holding on to much mem? Release it all.
			FlushAllocCache();
			Ptr = System_Allocate(NewSize);
		}
		return Ptr;
#else
		(void)OutActualSize;
		return System_Allocate(NewSize);
#endif
	}

#ifdef CACHE_FREED_OS_ALLOCS
	void FlushAllocCache()
	{
#ifdef USE_FINE_GRAIN_LOCKS
		VAutoLockerCS MainLock(AccessGuard);
#endif
		for (int i=0, n=FreedPageBlocksNum; i<n; ++i) 
		{
			//Remove allocs
			System_Free(FreedPageBlocks[i].Ptr);
			FreedPageBlocks[i].Ptr = NULL;
			FreedPageBlocks[i].ByteSize = 0;
		}
		FreedPageBlocksNum = 0;
		CachedTotal = 0;
	}
#endif


	/** 
	 * Create a 64k page of PoolSlot structures for tracking allocations
	 */
	PoolSlot* CreateIndirect()
	{
		VASSERT(IndirectPoolBlockSize * sizeof(PoolSlot) <= m_PageSize);
		PoolSlot* Indirect = (PoolSlot*)System_Allocate(IndirectPoolBlockSize * sizeof(PoolSlot));
		if( !Indirect )
		{
			OutOfMemory(IndirectPoolBlockSize * sizeof(PoolSlot));
		}
        memset(Indirect, 0, IndirectPoolBlockSize*sizeof(PoolSlot));

#if VPROFILE
		WasteCurrent += IndirectPoolBlockSize * sizeof(PoolSlot);
#endif 

		//VPROF(OsPeak = FMath::Max(OsPeak, OsCurrent += Align(IndirectPoolBlockSize * sizeof(PoolSlot), m_PageSize)));
		//VPROF(WastePeak = FMath::Max(WastePeak, WasteCurrent += Align(IndirectPoolBlockSize * sizeof(PoolSlot), m_PageSize)));
		return Indirect;
	}

	/** 
	* Gets the PoolSlot for a memory address. If no valid info exists one is created. 
	* NOTE: This function requires a mutex across threads, but its is the callers responsibility to 
	* acquire the mutex before calling
	*/
	inline PoolSlot* GetPoolInfo(size_t Ptr)
	{
		if (!m_HashMap)
		{
			// Init tables.
			UINT BucketSize = VAlign(MaxHashBuckets*sizeof(PoolHashBucket), m_PageSize);
			m_HashMap = (PoolHashBucket*)System_Allocate(BucketSize);
			for (UINT i=0; i<MaxHashBuckets; ++i) 
			{
				new (m_HashMap+i) PoolHashBucket();
			}
		}

		size_t Key= Ptr >> HashKeyShift;
		size_t Hash=Key&(MaxHashBuckets-1);
		size_t PoolIndex=(Ptr >> PoolBitShift) & PoolMask;
		PoolHashBucket* collision=&m_HashMap[Hash];
		do
		{
			if (collision->Key==Key || !collision->FirstPool)
			{
				if (!collision->FirstPool)
				{
                    collision->Key=Key;
					InitializeHashBucket(collision);
				}
				return &collision->FirstPool[PoolIndex];
			}
			collision=collision->Next;
		} while (collision!=&m_HashMap[Hash]);
		//Create a new hash bucket entry
		PoolHashBucket* NewBucket=CreateHashBucket();
		NewBucket->Key=Key;
		m_HashMap[Hash].Link(NewBucket);
		return &NewBucket->FirstPool[PoolIndex];
	}

	// 根据指针地址查找所属的Pool
	inline PoolSlot* FindPoolInfo(size_t Ptr1, size_t& AllocationBase)
	{
		WORD NextStep = 0;
		
		// 由于我们所有的内存均以PageSize 对齐创建. 这里可以求出 Ptr1 所对应的Pool Header地址
		size_t Ptr=Ptr1&~((size_t)m_PageSize-1);

		for (UINT i=0, n=(BINNED_ALLOC_POOL_SIZE/m_PageSize)+1; i<n; ++i)
		{
			PoolSlot* Pool = FindPoolInfoInternal(Ptr, NextStep);
			if (Pool)
			{
				AllocationBase=Ptr;
				//VASSERT(Ptr1 >= AllocationBase && Ptr1 < AllocationBase+Pool->GetBytes());
				return Pool;
			}
			Ptr = ((Ptr-(m_PageSize*NextStep))-1)&~((size_t)m_PageSize-1);
		}
		AllocationBase=0;
		return NULL;
	}
	//Note Ptr : 不是任意地址, 而是POOL HEADER地址.
	inline PoolSlot* FindPoolInfoInternal(size_t Ptr, WORD& JumpOffset)
	{
		VASSERT(m_HashMap);

		UINT Key=Ptr>>HashKeyShift;
		UINT Hash=Key&(MaxHashBuckets-1);
		UINT PoolIndex=( Ptr >> PoolBitShift) & PoolMask;
		JumpOffset=0;

		PoolHashBucket* collision=&m_HashMap[Hash];
		do
		{
			if (collision->Key==Key)
			{
				if (!collision->FirstPool[PoolIndex].AllocSize)
				{
					JumpOffset = collision->FirstPool[PoolIndex].TableIndex;
					return NULL;
				}
				return &collision->FirstPool[PoolIndex];
			}
			collision=collision->Next;
		} while (collision!=&m_HashMap[Hash]);

		return NULL;
	}

	/**
	 *	Returns a newly created and initialized PoolHashBucket for use.
	 */
	FORCEINLINE PoolHashBucket* CreateHashBucket() 
	{
		PoolHashBucket* bucket=AllocateHashBucket();
		InitializeHashBucket(bucket);
		return bucket;
	}

	/**
	 *	Initializes bucket with valid parameters
	 *	@param bucket pointer to be initialized
	 */
	FORCEINLINE void InitializeHashBucket(PoolHashBucket* bucket)
	{
		if (!bucket->FirstPool)
		{
			bucket->FirstPool=CreateIndirect();
		}
	}

	/**
	 * Allocates a hash bucket from the free list of hash buckets
	*/
	PoolHashBucket* AllocateHashBucket()
	{
		if (!HashBucketFreeList) 
		{
			HashBucketFreeList=(PoolHashBucket*)System_Allocate(m_PageSize); //FPlatformMemory::BinnedAllocFromOS(m_PageSize);
			VPROF(OsPeak = FMath::Max(OsPeak, OsCurrent += m_PageSize));
			VPROF(WastePeak = FMath::Max(WastePeak, WasteCurrent += m_PageSize));
			
			for (size_t i=0, n=(m_PageSize/sizeof(PoolHashBucket)); i<n; ++i)
			{
				HashBucketFreeList->Link(new (HashBucketFreeList+i) PoolHashBucket());
			}
		}
		PoolHashBucket* NextFree=HashBucketFreeList->Next;
		PoolHashBucket* Free=HashBucketFreeList;
		Free->Unlink();
		if (NextFree==Free) 
		{
			NextFree=NULL;
		}
		HashBucketFreeList=NextFree;
		return Free;
	}

	PoolSlot* AllocatePoolMemory(FPoolTable* Table, UINT PoolSize, WORD TableIndex)
	{
		// Must create a new pool.
		UINT Blocks = PoolSize / Table->BlockSize;
		UINT Bytes = Blocks * Table->BlockSize;
		UINT OsBytes = VAlign(Bytes, m_PageSize);		// Note : 以Page 的大小整体分配内存
		VASSERT(Blocks >= 1);
		VASSERT(Blocks * Table->BlockSize <= Bytes && PoolSize >= Bytes);

		// 创建缓存. 我们所有的真实内存均存放在Free所指向的地址空间. 
		FFreeMem* Free = NULL;

		size_t ActualPoolSize; 
		Free = (FFreeMem*)CacheAlloc(OsBytes, ActualPoolSize);

		// 内存必须以PageSize对齐
		VASSERT(VIsAlign((size_t)Free, m_PageSize));

		if( !Free )
		{
			OutOfMemory(OsBytes);
		}

		// 关联
		PoolSlot* Pool;
		{
#ifdef USE_FINE_GRAIN_LOCKS
			VAutoLockerCS PoolInfoLock(AccessGuard);
#endif
			Pool = GetPoolInfo((size_t)Free);

			// 我们是按Page创建的. 有多余的部分是不能映射的!!! 标识清楚!
			for (size_t i=(size_t)m_PageSize, Offset=0; i<OsBytes; i+=m_PageSize, ++Offset)
			{
				PoolSlot* TrailingPool = GetPoolInfo(((size_t)Free)+i);
				VASSERT(TrailingPool);
				//Set trailing pools to point back to first pool
				TrailingPool->SetAllocationSizes(0, 0, Offset, BinnedOSTableIndex);
			}
		}

		// Init pool.
		Pool->Link( Table->FirstPool );
		Pool->SetAllocationSizes(Bytes, OsBytes, TableIndex, BinnedOSTableIndex);
		VPROF(OsPeak = FMath::Max(OsPeak, OsCurrent += OsBytes));
		VPROF(WastePeak = FMath::Max(WastePeak, WasteCurrent += OsBytes - Bytes));
		Pool->Taken		 = 0;
		Pool->FirstMem   = Free;

#if STATS
		Table->NumActivePools++;
		Table->MaxActivePools = FMath::Max(Table->MaxActivePools, Table->NumActivePools);
#endif
		// Create first free item.
		Free->NumFreeBlocks = Blocks;
		Free->Next       = NULL;

		return Pool;
	}

	inline FFreeMem* AllocateBlockFromPool(FPoolTable* Table, PoolSlot* Pool)
	{
		// Pick first available block and unlink it.
		Pool->Taken++;
		VASSERT(Pool->TableIndex < BinnedOSTableIndex); // if this is false, FirstMem is actually a size not a pointer
		VASSERT(Pool->FirstMem);
		VASSERT(Pool->FirstMem->NumFreeBlocks > 0);
		VASSERT(Pool->FirstMem->NumFreeBlocks < PAGE_SIZE_LIMIT);
		FFreeMem* Free = (FFreeMem*)((BYTE*)Pool->FirstMem + --Pool->FirstMem->NumFreeBlocks * Table->BlockSize);
		if( !Pool->FirstMem->NumFreeBlocks )
		{
			Pool->FirstMem = Pool->FirstMem->Next;
			if( !Pool->FirstMem )
			{
				// Move to exhausted list.
				Pool->Unlink();
				Pool->Link( Table->ExhaustedPool );
			}
		}
		VPROF(UsedPeak = FMath::Max(UsedPeak, UsedCurrent += Table->BlockSize));
		return Free;
	}

	/**
	* Releases memory back to the system. This is not protected from multi-threaded access and it's
	* the callers responsibility to Lock AccessGuard before calling this.
	*/
	void FreeInternal( void* Ptr )
	{
		MEM_TIME(MemTime -= FPlatformTime::Seconds());
		VPROF(CurrentAllocs--);

		size_t BasePtr;
		PoolSlot* Pool = FindPoolInfo((size_t)Ptr, BasePtr);
		VASSERT(Pool);
		VASSERT(Pool->GetBytes() != 0);

		if( Pool->TableIndex < BinnedOSTableIndex )
		{
			// Pool 
			FPoolTable* Table=MemSizeToPoolTable[Pool->TableIndex];
#ifdef USE_FINE_GRAIN_LOCKS
			VAutoLockerCS TableLock(Table->CriticalSection);
#endif
#if STATS
			Table->ActiveRequests--;
#endif
			// 如果当前槽位已经占满, 将其移动到可用槽位链表中
			if( !Pool->FirstMem )
			{
				Pool->Unlink();
				Pool->Link( Table->FirstPool );
			}

			// 修改当前BLOCK 标记
			FFreeMem* Free		= (FFreeMem*)Ptr;
			Free->NumFreeBlocks	= 1;
			Free->Next			= Pool->FirstMem;
			Pool->FirstMem		= Free;

			VPROF(UsedCurrent -= Table->BlockSize);

			// Free this pool.
			VASSERT(Pool->Taken >= 1);
			if( --Pool->Taken == 0 )
			{
#if STATS
				Table->NumActivePools--;
#endif
				// Free the OS memory.
				size_t OsBytes = Pool->GetOsBytes(m_PageSize, BinnedOSTableIndex);
				VPROF(OsCurrent -= OsBytes);
				VPROF(WasteCurrent -= OsBytes - Pool->GetBytes());
				Pool->Unlink();
				Pool->SetAllocationSizes(0, 0, 0, BinnedOSTableIndex);
				CacheFree((void*)BasePtr, OsBytes);
			}
		}
		else
		{
			// Free an OS allocation.
			VASSERT(VIsAlign((size_t)Ptr, m_PageSize));
			
			size_t OsBytes = Pool->GetOsBytes(m_PageSize, BinnedOSTableIndex);
			VPROF(UsedCurrent -= Pool->GetBytes());
			VPROF(OsCurrent -= OsBytes);
			VPROF(WasteCurrent -= OsBytes - Pool->GetBytes());
			CacheFree(Ptr, OsBytes);
		}

		MEM_TIME(MemTime += FPlatformTime::Seconds());
	}

public:

	// FMalloc interface.
	// InPageSize - First parameter is page size, all allocs from BinnedAllocFromOS() MUST be aligned to this size
	// AddressLimit - Second parameter is estimate of the range of addresses expected to be returns by BinnedAllocFromOS(). Binned
	// Malloc will adjust it's internal structures to make look ups for memory allocations O(1) for this range. 
	// It's is ok to go outside this range, look ups will just be a little slower
	KPoolAllocator(UINT InPageSize = 0x10000, UINT64 AddressLimit = 0x100000000L)
		:	TableAddressLimit(AddressLimit)
		,	m_HashMap(NULL)
		,	HashBucketFreeList(NULL)
		,	m_PageSize		(InPageSize)
#ifdef CACHE_FREED_OS_ALLOCS
		,	FreedPageBlocksNum(0)
		,	CachedTotal(0)
#endif
#if VPROFILE
		,	OsCurrent		( 0 )
		,	OsPeak			( 0 )
		,	WasteCurrent	( 0 )
		,	WastePeak		( 0 )
		,	UsedCurrent		( 0 )
		,	UsedPeak		( 0 )
		,	CurrentAllocs	( 0 )
		,	TotalAllocs		( 0 )
		,	MemTime			( 0.0 )
#endif
	{
		VASSERT(!(m_PageSize & (m_PageSize - 1)));
		VASSERT(!(AddressLimit & (AddressLimit - 1)));
		VASSERT(m_PageSize <= 65536); // There is internal limit on page size of 64k
		VASSERT(AddressLimit > m_PageSize); // Check to catch 32 bit overflow in AddressLimit

		/** Shift to get the reference from the indirect tables */

		PoolBitShift = (UINT64)VLog2((int)m_PageSize);			// 16
		IndirectPoolBlockSize = m_PageSize/sizeof(PoolSlot);				// 2048
		IndirectPoolBitShift = (UINT64)VLog2((int)IndirectPoolBlockSize);	// 10
		//PoolBitShift = FPlatformMath::CeilLogTwo(m_PageSize);
		//IndirectPoolBitShift = FPlatformMath::CeilLogTwo(m_PageSize/sizeof(PoolSlot));
		

		MaxHashBuckets = AddressLimit >> (IndirectPoolBitShift+PoolBitShift); // 64 ????
		MaxHashBucketBits = VLog2((int)MaxHashBuckets); // 6 //FPlatformMath::CeilLogTwo(MaxHashBuckets);

		/** 
		* Shift required to get required hash table key.
		*/
		HashKeyShift = PoolBitShift+IndirectPoolBitShift;
		/** Used to mask off the bits that have been used to lookup the indirect table */
		PoolMask =  ( ( 1 << ( HashKeyShift - PoolBitShift ) ) - 1 );
		BinnedSizeLimit = PAGE_SIZE_LIMIT/2;
		BinnedOSTableIndex = BinnedSizeLimit+EXTENED_PAGE_POOL_ALLOCATION_COUNT;

		VASSERT((BinnedSizeLimit & (BinnedSizeLimit-1)) == 0);

		// Init tables.
		OsTable.FirstPool = NULL;
		OsTable.ExhaustedPool = NULL;
		OsTable.BlockSize = 0;

		// Block sizes are based around getting the maximum amount of allocations per pool, with as little alignment waste as possible.
		// Block sizes should be close to even divisors of the POOL_SIZE, and well distributed. They must be 16-byte aligned as well.
		static const UINT BlockSizes[POOL_COUNT] =
		{
			8,		16,		32,		48,		64,		80,		96,		112,
			128,	160,	192,	224,	256,	288,	320,	384,
			448,	512,	576,	640,	704,	768,	896,	1024,
			1168,	1360,	1632,	2048,	2336,	2720,	3264,	4096,
			4672,	5456,	6544,	8192,	9360,	10912,	13104,	16384,
			21840,	32768
		};

		for( UINT i = 0; i < POOL_COUNT; i++ )
		{
			PoolTable[i].FirstPool = NULL;
			PoolTable[i].ExhaustedPool = NULL;
			PoolTable[i].BlockSize = BlockSizes[i];
#if STATS
			PoolTable[i].MinRequest = PoolTable[i].BlockSize;
#endif
		}

		for( UINT i=0; i<MAX_POOLED_ALLOCATION_SIZE; i++ )
		{
			UINT Index = 0;
			while( PoolTable[Index].BlockSize < i )
			{
				++Index;
			}
			VASSERT(Index < POOL_COUNT);
			MemSizeToPoolTable[i] = &PoolTable[Index];
		}

		VASSERT(MAX_POOLED_ALLOCATION_SIZE - 1 == PoolTable[POOL_COUNT - 1].BlockSize);
	}
	
	void* Malloc(size_t Size, UINT Alignment )
	{
		// Handle DEFAULT_ALIGNMENT for binned allocator.
		if (Alignment == DEFAULT_ALIGNMENT)
		{
			Alignment = DEFAULT_BINNED_ALLOCATOR_ALIGNMENT;
		}
		VASSERT(Alignment <= m_PageSize);
		Alignment = VMax<UINT>(Alignment, DEFAULT_BINNED_ALLOCATOR_ALIGNMENT);
		Size = VAlign(Size, Alignment);
		Size = VMax<size_t>(Alignment, Size);
		//MEM_TIME(MemTime -= FPlatformTime::Seconds());
		//VPROF(CurrentAllocs++);
		//VPROF(TotalAllocs++);
		FFreeMem* Free;
		if( Size < BinnedSizeLimit )
		{
			// Allocate from pool.

			// 首先找到对应的缓存表.
			FPoolTable* Table = MemSizeToPoolTable[Size];
#ifdef USE_FINE_GRAIN_LOCKS
			VAutoLockerCS TableLock(Table->CriticalSection);
#endif
			VASSERT(Size <= Table->BlockSize);

			// 在表中找到对应的缓存槽
			PoolSlot* Pool = Table->FirstPool;
			if( !Pool )
			{
				// 创建新的缓存槽
				Pool = AllocatePoolMemory(Table, BINNED_ALLOC_POOL_SIZE, Size);
			}

			Free = AllocateBlockFromPool(Table, Pool);
		}
		else
		{
			// Use OS for large allocations.  PageSize Alignment!!!!!
			size_t AlignedSize = VAlign(Size, m_PageSize);
			
			size_t ActualPoolSize; //TODO: use this to reduce waste?
			Free = (FFreeMem*)CacheAlloc(AlignedSize, ActualPoolSize);
			if( !Free )
			{
				OutOfMemory(AlignedSize);
			}
			VASSERT(!((size_t)Free & (m_PageSize-1)));

			// Create indirect.
			PoolSlot* Pool;
			{
#ifdef USE_FINE_GRAIN_LOCKS
				VAutoLockerCS PoolInfoLock(AccessGuard);
#endif
				Pool = GetPoolInfo((size_t)Free);
			}

			Pool->SetAllocationSizes(Size, AlignedSize, BinnedOSTableIndex, BinnedOSTableIndex);
			VPROF(OsPeak = FMath::Max(OsPeak, OsCurrent += AlignedSize));
			VPROF(UsedPeak = FMath::Max(UsedPeak, UsedCurrent += Size));
			VPROF(WastePeak = FMath::Max(WastePeak, WasteCurrent += AlignedSize - Size));
		}

		MEM_TIME(MemTime += FPlatformTime::Seconds());
		return Free;
	}

	/** 
	 * Realloc
	 */
	void* Realloc( void* Ptr, size_t NewSize, UINT Alignment )
	{
		// Handle DEFAULT_ALIGNMENT for binned allocator.
		if (Alignment == DEFAULT_ALIGNMENT)
		{
			Alignment = DEFAULT_BINNED_ALLOCATOR_ALIGNMENT;
		}
		VASSERT(Alignment <= m_PageSize);
		Alignment = VMax<UINT>(Alignment, DEFAULT_BINNED_ALLOCATOR_ALIGNMENT);
		if (NewSize)
		{
			NewSize = VMax<size_t>(Alignment, VAlign(NewSize, Alignment));
		}
		MEM_TIME(MemTime -= FPlatformTime::Seconds());
		size_t BasePtr;
		void* NewPtr = Ptr;
		if( Ptr && NewSize )
		{
			PoolSlot* Pool = FindPoolInfo((size_t)Ptr, BasePtr);

			if( Pool->TableIndex < BinnedOSTableIndex )
			{
				// Allocated from pool, so grow or shrink if necessary.
				VASSERT(Pool->TableIndex > 0); // it isn't possible to allocate a size of 0, Malloc will increase the size to DEFAULT_BINNED_ALLOCATOR_ALIGNMENT
				if( NewSize>MemSizeToPoolTable[Pool->TableIndex]->BlockSize || NewSize<=MemSizeToPoolTable[Pool->TableIndex-1]->BlockSize )
				{
					NewPtr = Malloc( NewSize, Alignment );
					VMemcpy( NewPtr, Ptr, VMin<size_t>( NewSize, MemSizeToPoolTable[Pool->TableIndex]->BlockSize ) );
					Free( Ptr );
				}
			}
			else
			{
				// Allocated from OS.
				VASSERT(!((size_t)Ptr & (m_PageSize-1)));
				if( NewSize > Pool->GetOsBytes(m_PageSize, BinnedOSTableIndex) || NewSize * 3 < Pool->GetOsBytes(m_PageSize, BinnedOSTableIndex) * 2 )
				{
					// Grow or shrink.
					NewPtr = Malloc( NewSize, Alignment );
					VMemcpy( NewPtr, Ptr, VMin<size_t>(NewSize, Pool->GetBytes()) );
					Free( Ptr );
				}
				else
				{
					// Keep as-is, reallocation isn't worth the overhead.
					VPROF(UsedCurrent += NewSize - Pool->GetBytes());
					VPROF(UsedPeak = FMath::Max(UsedPeak, UsedCurrent));
					VPROF(WasteCurrent += Pool->GetBytes() - NewSize);
					Pool->SetAllocationSizes(NewSize, Pool->GetOsBytes(m_PageSize, BinnedOSTableIndex), BinnedOSTableIndex, BinnedOSTableIndex);
				}
			}
		}
		else if( Ptr == NULL )
		{
			NewPtr = Malloc( NewSize, Alignment );
		}
		else
		{
			Free( Ptr );
			NewPtr = NULL;
		}

	//	MEM_TIME(MemTime += FPlatformTime::Seconds());
		return NewPtr;
	}
	
	void Free( void* Ptr )
	{
		if( !Ptr )
		{
			return;
		}
#ifdef USE_COARSE_GRAIN_LOCKS
		VAutoLockerCS ScopedLock(AccessGuard);
#endif
		FreeInternal(Ptr);
	}

	/**
	 * Validates the allocator's heap
	 */
	bool ValidateHeap()
	{
#if 0
#ifdef USE_INTERNAL_LOCKS
		VAutoLockerCS ScopedLock(&AccessGuard);
#endif
		for( int i = 0; i < POOL_COUNT; i++ )
		{
			FPoolTable* Table = &PoolTable[i];
			for( PoolSlot** PoolPtr = &Table->FirstPool; *PoolPtr; PoolPtr = &(*PoolPtr)->Next )
			{
				PoolSlot* Pool = *PoolPtr;
				VASSERT(Pool->PrevLink == PoolPtr);
				VASSERT(Pool->FirstMem /*|| Pool->Taken == (Pool->AllocSize/Pool->TableIndex)*/);
				for( FFreeMem* Free = Pool->FirstMem; Free; Free = Free->Next )
				{
					VASSERT(Free->NumFreeBlocks > 0);
				}
			}
			for( PoolSlot** PoolPtr = &Table->ExhaustedPool; *PoolPtr; PoolPtr = &(*PoolPtr)->Next )
			{
				PoolSlot* Pool = *PoolPtr;
				VASSERT(Pool->PrevLink == PoolPtr);
				VASSERT(!Pool->FirstMem);
			}
		}
#endif 
		return( true );
	}
#if 0
	/**
	 * Dumps details about all allocations to an output device. Subclasses should override to add additional info
	 *
	 * @param Ar	[in] Output device
	 */
	void DumpAllocations( class FOutputDevice& Ar )
	{

#ifdef USE_INTERNAL_LOCKS
		VAutoLockerCS ScopedLock(&AccessGuard);
#endif
		ValidateHeap();

#if STATS
		// This is all of the memory including stuff too big for the pools
		Ar.Logf( TEXT("Tracked Allocation Status") );
		// Waste is the total overhead of the memory system
		Ar.Logf( TEXT("Current Memory %.2f MB used, plus %.2f MB waste"), UsedCurrent / ( 1024.0f * 1024.0f ), ( OsCurrent - UsedCurrent ) / ( 1024.0f * 1024.0f ) );
		Ar.Logf( TEXT("Peak Memory %.2f MB used, plus %.2f MB waste"), UsedPeak / ( 1024.0f * 1024.0f ), ( OsPeak - UsedPeak ) / ( 1024.0f * 1024.0f ) );
		Ar.Logf( TEXT("Allocs      % 6i Current / % 6i Total"), CurrentAllocs, TotalAllocs );
		MEM_TIME(Ar.Logf( TEXT( "Seconds     % 5.3f" ), MemTime ));
		MEM_TIME(Ar.Logf( TEXT( "MSec/Allc   % 5.5f" ), 1000.0 * MemTime / MemAllocs ));

		// This is the memory tracked inside individual allocation pools
		Ar.Logf( TEXT("") );
		Ar.Logf( TEXT("Block Size Num Pools Max Pools Cur Allocs Total Allocs Min Req Max Req Mem Used Mem Slack Mem Waste Efficiency") );
		Ar.Logf( TEXT("---------- --------- --------- ---------- ------------ ------- ------- -------- --------- --------- ----------") );

		UINT TotalMemory = 0;
		UINT TotalWaste = 0;
		UINT TotalActiveRequests = 0;
		UINT TotalTotalRequests = 0;
		UINT TotalPools = 0;
		UINT TotalSlack = 0;

		FPoolTable* Table = NULL;
		for( int i = 0; i < BinnedSizeLimit+EXTENED_PAGE_POOL_ALLOCATION_COUNT; i++ )
		{
			if (Table == MemSizeToPoolTable[i] || MemSizeToPoolTable[i]->BlockSize == 0)
				continue;

			Table = MemSizeToPoolTable[i];
			
			UINT TableAllocSize = (Table->BlockSize > BinnedSizeLimit ? (((3*(i-BinnedSizeLimit))+3)*BINNED_ALLOC_POOL_SIZE) : BINNED_ALLOC_POOL_SIZE);
			// The amount of memory allocated from the OS
			UINT MemAllocated = ( Table->NumActivePools * TableAllocSize) / 1024;
			// Amount of memory actually in use by allocations
			UINT MemUsed = ( Table->BlockSize * Table->ActiveRequests ) / 1024;
			// Wasted memory due to pool size alignment
			UINT PoolMemWaste = Table->NumActivePools * ( TableAllocSize - ( ( TableAllocSize / Table->BlockSize ) * Table->BlockSize ) ) / 1024;
			// Wasted memory due to individual allocation alignment. This is an estimate.
			UINT MemWaste = ( UINT )( ( ( double )Table->TotalWaste / ( double )Table->TotalRequests ) * ( double )Table->ActiveRequests ) / 1024 + PoolMemWaste;
			// Memory that is reserved in active pools and ready for future use
			UINT MemSlack = MemAllocated - MemUsed - PoolMemWaste;

			Ar.Logf( TEXT("% 10i % 9i % 9i % 10i % 12i % 7i % 7i % 7iK % 8iK % 8iK % 9.2f%%"),
				Table->BlockSize,
				Table->NumActivePools,
				Table->MaxActivePools,
				Table->ActiveRequests,
				( UINT )Table->TotalRequests,
				Table->MinRequest,
				Table->MaxRequest,
				MemUsed,
				MemSlack,
				MemWaste,
				MemAllocated ? 100.0f * (MemAllocated - MemWaste) / MemAllocated : 100.0f );

			TotalMemory += MemAllocated;
			TotalWaste += MemWaste;
			TotalSlack += MemSlack;
			TotalActiveRequests += Table->ActiveRequests;
			TotalTotalRequests += Table->TotalRequests;
			TotalPools += Table->NumActivePools;
		}

		Ar.Logf( TEXT( "" ) );
		Ar.Logf( TEXT( "%iK allocated in pools (with %iK slack and %iK waste). Efficiency %.2f%%" ), TotalMemory, TotalSlack, TotalWaste, TotalMemory ? 100.0f * (TotalMemory - TotalWaste) / TotalMemory : 100.0f );
		Ar.Logf( TEXT( "Allocations %i Current / %i Total (in %i pools)"), TotalActiveRequests, TotalTotalRequests, TotalPools );
		Ar.Logf( TEXT("") );
#endif
	}
#endif 

protected:

	///////////////////////////////////
	//// API platforms must implement
	///////////////////////////////////
	/**
	 * @return the page size the OS uses
	 *	NOTE: This MUST be implemented on each platform that uses this!
	 */
	UINT BinnedGetPageSize();
};



extern KPoolAllocator* g_PoolAllocator;
#endif 

BOOL KInitMemory();
void KShutdownMemory();

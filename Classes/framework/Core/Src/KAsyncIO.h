#pragma once
#ifndef __KASYNCIO_H__
#define __KASYNCIO_H__
#include "VThreading.h"

// Òì²½ÎÄ¼þIO

class KFileSystem;
class KAsyncIOThread;

struct KAsyncIOItem
{
	void* FileHandle;

};

class KAsyncIO
{
	struct AsyncIORequest
	{
		/** Index of request.																		*/
		UINT			RequestIndex;
		/** File sort key on media, INDEX_NONE if not supported or unknown.							*/
		int				FileSortKey;

		/** Name of file.																			*/
		VString			FileName;
		/** Offset into file.																		*/
		UINT			Offset;
		/** Size in bytes of data to read.															*/
		UINT			Size;
		/** Uncompressed size in bytes of original data, 0 if data is not compressed on disc		*/
		UINT			UncompressedSize;														
		/** Pointer to memory region used to read data into.										*/
		void*				Dest;
		/** Flags for controlling decompression														*/
	//	ECompressionFlags	CompressionFlags;
		/** Thread safe counter that is decremented once work is done.								*/
	//	FThreadSafeCounter* Counter;
		/** Priority of request.																	*/
	//	EAsyncIOPriority	Priority;
		/** Is this a request to destroy the handle?												*/
	//	BITFIELD			bIsDestroyHandleRequest : 1;
		/** Whether we already requested the handle to be cached.									*/
	//	BITFIELD			bHasAlreadyRequestedHandleToBeCached : 1;
#if 0
		/** Constructor, initializing all member variables. */
		AsyncIORequest()
		:RequestIndex(0)
		,FileSortKey(INDEX_NONE)
		,Offset(INDEX_NONE)
		,Size(INDEX_NONE)
		,UncompressedSize(INDEX_NONE)
		,Dest(NULL)
		,CompressionFlags(COMPRESS_None)
		,Counter(NULL)
		,Priority(AIOP_MIN)
		,bIsDestroyHandleRequest(FALSE)
		{}

		/**
		 * @returns human readable string with information about request
		 */
		FString ToString() const
		{
			return FString::Printf(TEXT("%11.1f, %10d, %10d, %10d, %10d, 0x%p, 0x%08x, 0x%08x, %d, %s"),
				(DOUBLE)RequestIndex, FileSortKey, Offset, Size, UncompressedSize, Dest, (DWORD)CompressionFlags,
				(DWORD)Priority, bIsDestroyHandleRequest ? 1 : 0, *FileName);
		}
#endif 
	};
public:
	KAsyncIO(void);
	~KAsyncIO(void);
	UINT LoadData(const VString& File, UINT Offset, UINT Size, void* Dest);
	void CancelRequest(UINT* RequestIndex, UINT NumRequests);
	void Suspend();
	void Resume();

private:
	KFileSystem* m_FileSystem;
	KAsyncIOThread* m_AsyncIOThread;
	/** Critical section used to syncronize access to outstanding requests map						*/
	VSyncCriticalSection m_CriticalSection;
	/** TMap of file name string hash to file handles												*/
	typedef vtpl::map<VString, KAsyncIOItem> IOItemMap;
	IOItemMap	NameToHandleMap;
#if 0
	/** Array of outstanding requests, processed in FIFO order										*/
	TArray<FAsyncIORequest>			OutstandingRequests;
	/** Event that is signaled if there are outstanding requests									*/
	FEvent*							OutstandingRequestsEvent;
	/** Thread safe counter that is 1 if the thread is currently busy with request, 0 otherwise		*/
	FThreadSafeCounter				BusyWithRequest;
	/** Thread safe counter that is 1 if the thread is available to process requests, 0 otherwise	*/
	FThreadSafeCounter				IsRunning;
	/** Current request index. We don't really worry about wrapping around with a QWORD				*/
	QWORD							RequestIndex;
	/** Counter to indicate that the application requested that IO should be suspended				*/
	FThreadSafeCounter				SuspendCount;
	/** Critical section to sequence IO when needed (in addition to SuspendCount).					*/
	FCriticalSection*				ExclusiveReadCriticalSection;
#endif 
};

#endif 
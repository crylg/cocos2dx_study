#pragma once
#include "VList.h"
#include "VPacketBase.h"

#if !VPLATFORM_WIN32
typedef UINT SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#endif

class VIOBuffer
{
public:
	VIOBuffer(UINT Size)
	{
		Size = VAlign(Size, 4096);
		mBuffer = (BYTE*)VMalloc(Size);
		mBufferEnd = mBuffer + Size;
		mDataEnd = mBuffer;
	}
	~VIOBuffer()
	{
		if (mBuffer)
		{
			VFree(mBuffer);
		}
	}
	inline size_t MemUsage() const { return (mBufferEnd - mBuffer) + sizeof(VIOBuffer); }


	inline BYTE* GetData() { return mBuffer; }

	inline UINT FreeSpace() const
	{
		return mBufferEnd - mDataEnd;
	}

	inline UINT GetSize() const
	{
		return mDataEnd - mBuffer;
	}

	inline void Write(const void* Data, UINT Size)
	{
		VMemcpy(mDataEnd, Data, Size);
		mDataEnd += Size;
		VASSERT(mDataEnd <= mBufferEnd);
	}

	inline BOOL IsEmpty() const
	{
		return mDataEnd == mBuffer;
	}


	inline void Clear()
	{
		mDataEnd = mBuffer;
	}
private:
	BYTE* mBuffer;
	BYTE* mDataEnd;
	BYTE* mBufferEnd;
};

#pragma pack(push)
#pragma pack(1)
struct KPacketBase
{
	UINT			Size;		// Must at offset 0
	BYTE			Data[1];	// if a packet event, 	
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

typedef VListNode<KPacketBase*> KPacketListNode;
typedef VMemPool<KPacketListNode, VSyncLockSpin, VMemPoolElementConstructor_POD<KPacketListNode> > KPacketListNodePool;

/**
* Fast / No memory alloc/free List
*/
class KPacketQueue : private VList < KPacketBase* >
{
public:
	KPacketQueue(KPacketListNodePool& ListNodePool) :mListNodePool(ListNodePool)
	{
	}
	virtual ~KPacketQueue()
	{
	}

	void Clear()
	{
		mMutex.Lock();
		VList<KPacketBase*>::Clear();
		mMutex.Unlock();
	}


	void PushBack(KPacketBase* const& Element)
	{
		mMutex.Lock();
		VList<KPacketBase*>::PushBack(Element);
		mMutex.Unlock();
	}

	KPacketBase* PopFront()
	{
		KPacketBase* Ret = NULL;
		mMutex.Lock();
		if (Size() != 0)
		{
			Ret = VList<KPacketBase*>::PopFront();
		}
		mMutex.Unlock();
		return Ret;
	}

	/**
	* 从队列前面弹出一个大小小于LessThen 的数据包.
	*/
	KPacketBase* TryPopFront(UINT LessThen)
	{
		KPacketBase* Ret = NULL;
		mMutex.Lock();
		if (this->mHead && this->mHead->Element->Size <= LessThen)
		{
			Ret = this->mHead->Element;
			RemoveNode(this->mHead);
		}
		mMutex.Unlock();
		return Ret;
	}

private:
	VSyncLockSpin mMutex;
	KPacketListNodePool& mListNodePool;
	virtual ListNode* AllocNode(KPacketBase *const& Element)
	{
		ListNode* Node = mListNodePool.Alloc();
		VASSERT(Node);
		Node->Element = const_cast<KPacketBase*>(Element);
		Node->NextNode = Node->PrevNode = NULL;
		return Node;
	}
	virtual void FreeNode(ListNode* Node)
	{
		mListNodePool.Free(Node);
	}
};

class VFragmentCompositor;
class ConnectThread;
class SessionThread;
/**
 * 和服务器的会话. 
 */
class VCoreApi VServerSession : public VAllocator
{
	struct Packet
	{
		void* Data;
		int Size;
	};
public:
	VServerSession();
	virtual ~VServerSession();

	static BOOL InitSystem();

	/**
	 * 初始化服务器会话 
	 */
	BOOL Init(const char* sIP, int Port);

	/**
	 * 关闭服务器会话. 将断开连接.  
	 */
	void Shutdown();


	BOOL Send(const void* Data, UINT Size);

	VPacketBase* PeekPacket();

	/**
	 *  开始连接 
	 */
	virtual void BeginConnect();

	BOOL IsValid() const;

	void Disconnect();

	VPacketBase* AllocPacket(UINT Size) const;
	void FreePacket(void* Packet) const;
private:
	void DoConnect();
	void _OnConnectLost();
	virtual void OnConnect(){}
	virtual void OnConnectLost() {}
	void UpdateSession();
	void ProcessReadEvent();
	void ProcessWriteEvent();
	void PrepareSendIOBuffer();
	void OnRead();
	void OnPacket(VPacketBase* Packet);
	std::string mURL;
	char* mServerIP;
	int mServerPort;
	struct sockaddr_in* mServerAddress;
	int mSocket;		// socket 
	int mConnectd;
	volatile int mRun;			// 
	VSyncEvent mConnectEvent;
	SessionThread* mSessionThread;
	KPacketListNodePool mPacketListNodePool;
	KPacketQueue mSendQueue;
	KPacketQueue mRecvQueue;
	VIOBuffer	mSendBuffer;
	VIOBuffer	mRecvBuffer;
	VFragmentCompositor* mFragmentCompositor;
	friend class SessionThread;
};



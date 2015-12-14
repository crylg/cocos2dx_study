#include "VStdAfx.h"
#include "VServerSession.h"
#include "KPacketPool.h"

#if VPLATFORM_WIN32
#include <io.h>
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"Ws2_32.lib")


#else
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define SOCKET_ERROR -1

#endif 



BOOL Nonblock(SOCKET Socket) 
{
	VASSERT(Socket != -1);
#if VPLATFORM_WIN32
	u_long nonblock = 1;
	if (ioctlsocket(Socket, FIONBIO, &nonblock) == -1) return FALSE;
#else
	if (fcntl(Socket, F_SETFL, O_NONBLOCK) == -1) return FALSE;
#endif
	return TRUE;
}

BOOL Block(SOCKET Socket)
{
	VASSERT(Socket != -1);
#if VPLATFORM_WIN32
	u_long nonblock = 0;
	if (ioctlsocket(Socket, FIONBIO, &nonblock) == -1) return FALSE;
#else 
	if (fcntl(Socket, F_SETFL, 0) == -1) return FALSE;
#endif
	return TRUE;
}

void NoSIGPIPE(SOCKET Socket)
{
#if VPLATFORM_IOS
	int set = 1;
	setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
}


inline void CloseSocket(SOCKET Socket)
{
#if VPLATFORM_WIN32
	closesocket(Socket);
#else 
	shutdown(Socket, SHUT_RDWR);
	close(Socket);
#endif 
}



struct VPacketBase;


struct VFragmentCompositorContext
{
	inline VFragmentCompositorContext() :NextPacket(NULL), FragmentSize(0)
	{}
	BYTE* NextPacket;
	UINT FragmentSize;
};

/**
* 重组数据包碎片
*/
class VCoreApi VFragmentCompositor : public VAllocator
{
public:
	VFragmentCompositor(UINT InitBufferSize = 1024)
		:mBuffer(NULL)
		, mCapacity(0)
		, mSize(0)
	{
		if (InitBufferSize)
		{
			Resize(InitBufferSize);
		}
	}

	~VFragmentCompositor()
	{
		if (mBuffer)
		{
			VFree(mBuffer);
		}
	}
	inline const BYTE* GetBuffer() const { return mBuffer; }
	inline BYTE* GetBuffer() { return mBuffer; }
	inline UINT GetSize() const { return mSize; }
	inline UINT GetCapacity() const { return mCapacity;  }

	inline void Clear()
	{
		mSize = 0;
	}
	inline void Commit(UINT Size)
	{
		mSize += Size;
	}


	void Resize(UINT NewSize)
	{
		NewSize = VAlign(NewSize, 1024);
		if (NewSize > mCapacity)
		{
			// create new buffer
			BYTE* NewBuffer = (BYTE*)VMalloc(NewSize);

			//  copy previous data to new buffer.
			if (mSize)
			{
				VMemcpy(NewBuffer, mBuffer, mSize);
			}

			if (mBuffer)
			{
				VFree(mBuffer);
			}
			mBuffer = NewBuffer;
			mCapacity = NewSize;
		}
	}

	BOOL AppendFragment(const void* Data, UINT Size)
	{
		VASSERT(Data && Size);
		UINT NewSize = mSize + Size;

		// 确保有足够的空间容纳数据
		if (NewSize > mCapacity)
		{
			Resize(NewSize);
		}

		BYTE* WritePos = mBuffer + mSize;
		VMemcpy(WritePos, Data, Size);
		mSize = NewSize;

		return TRUE;
	}

	BYTE* DefragmentFirst(VFragmentCompositorContext& Context) const
	{
		VPacketBase* FirstPacket = (VPacketBase*)mBuffer;
		if (mSize >= sizeof(VPacketBase) && mSize >= FirstPacket->Size)
		{
			UINT Size = (UINT)FirstPacket->Size;
			Context.FragmentSize = mSize - Size;
			Context.NextPacket = mBuffer + Size;
			return (BYTE*)FirstPacket;
		}
		Context.FragmentSize = 0;
		Context.NextPacket = NULL;
		return NULL;
	}

	BYTE* DefragmentNext(VFragmentCompositorContext& Context) const
	{
		VASSERT(Context.NextPacket >= mBuffer && Context.NextPacket <= (mBuffer + mSize));
		VPacketBase* FirstPacket = (VPacketBase*)Context.NextPacket;
		if (Context.FragmentSize >= sizeof(VPacketBase) && Context.FragmentSize >= FirstPacket->Size)
		{
			UINT Size = (UINT)FirstPacket->Size;
			Context.FragmentSize -= Size;
			Context.NextPacket += Size;
			return (BYTE*)FirstPacket;
		}
		return NULL;
	}

	void DefragmentEnd(VFragmentCompositorContext& Context)
	{
		if (Context.NextPacket)
		{
			VASSERT(Context.FragmentSize <= mSize);
			VASSERT(Context.NextPacket >= mBuffer && Context.NextPacket <= (mBuffer + mSize - Context.FragmentSize));

			if (Context.FragmentSize)
			{
				VMemmove(mBuffer, Context.NextPacket, Context.FragmentSize);
			}
			mSize = Context.FragmentSize;
		}
	}

	size_t MemUsage() const
	{
		return sizeof(VFragmentCompositor) + (size_t)mCapacity;
	}
private:
	BYTE*	mBuffer;		/// 缓冲
	UINT	mCapacity;		/// 当前缓冲区大小
	UINT	mSize;			/// 已经填充的数据大小	
};



class SessionThread : public VThread, public VAllocator
{
	virtual UINT Process()
	{
#if VPLATFORM_WIN32
		SetName("SessionThread");
#endif 
		mSession->UpdateSession();
		return 0;
	}
	VServerSession* mSession;
public:
	SessionThread(VServerSession* Session) :mSession(Session){}
};


VServerSession::VServerSession()
	:mServerIP(NULL)
	, mServerPort(0)
	, mSocket(-1)
	, mConnectd(0)
	, mRun(1)
	, mSessionThread(NULL)
	, mSendQueue(mPacketListNodePool)
	, mRecvQueue(mPacketListNodePool)
	, mSendBuffer(16 * 1024)
	, mRecvBuffer(64 * 1024)
{
	mServerAddress = (sockaddr_in*)VMalloc(sizeof(sockaddr_in));
	mFragmentCompositor = VNew VFragmentCompositor(16*1024);

}


VServerSession::~VServerSession()
{
	VFree(mServerAddress);
	VDelete mFragmentCompositor;
	if (mServerIP)
	{
		VFree(mServerIP);
	}

}

BOOL VServerSession::InitSystem()
{
#if VPLATFORM_WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return FALSE;
	}
#endif 
	VASSERT(PacketPool == NULL);
	PacketPool = VNew KPacketPool;


	return TRUE;
}

BOOL VServerSession::Init(const char* sIP, int Port)
{
	VASSERT(sIP && sIP[0]);
#if VPLATFORM_WIN32
	/*WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
	return FALSE;
	}*/
#endif 
	mURL = sIP;
	struct addrinfo *result = NULL;
	//struct addrinfo *ptr = NULL;
	//struct addrinfo hints;
	if(getaddrinfo(sIP, NULL, NULL, &result) != 0)
	{
		return FALSE;
	}

	
	memcpy(mServerAddress, result->ai_addr, result->ai_addrlen);
	mServerAddress->sin_family = AF_INET;
	mServerAddress->sin_port = htons(Port);
	freeaddrinfo(result);

	if (mServerIP)
	{
		VFree(mServerIP);
		mServerIP = NULL;
	}
	int Len = strlen(sIP);
	mServerIP = (char*)VMalloc(Len + 1);
	VStrcpy(mServerIP, Len + 1, sIP);
	mServerIP[Len] = 0;
	mServerPort = Port;


	// 创建PacketPool

	//VASSERT(PacketPool == NULL);
	//PacketPool = VNew KPacketPool;

	// 创建连接线程. 
	if (!mConnectEvent.Create())
	{
		return FALSE;
	}
	mSessionThread = VNew SessionThread(this);
	mSessionThread->Run();
	return TRUE;
}

void VServerSession::Shutdown()
{
	Disconnect();


	if (PacketPool)
	{
		VDelete PacketPool;
		PacketPool = NULL;
	}
}


VPacketBase* VServerSession::AllocPacket(UINT Size) const
{
	
	//VLOG("@AllocPacket %d", Size);
	VASSERT(PacketPool);
	UINT* kPacket = (UINT*)PacketPool->AllocPacket(Size);
	//VLOG("@AllocPacket %d At %p", Size, kPacket + 1);
	return (VPacketBase*)(kPacket + 1);
}

void VServerSession::FreePacket(void* Packet) const
{
	VASSERT(PacketPool);
	UINT* iPacket = (UINT*)Packet;
	--iPacket;
	KPacketBase* kPacket = (KPacketBase*)iPacket;
	PacketPool->FreePacket(kPacket);
}

BOOL VServerSession::Send(const void* Data, UINT Size)
{
	VASSERT(PacketPool && Data);
	if (!mConnectd )
	{
		return FALSE;
	}

	KPacketBase* Packet = (KPacketBase*)PacketPool->AllocPacket(Size);
	VASSERT(Packet);
	VMemcpy(Packet->Data, Data, Size);
	mSendQueue.PushBack(Packet);

	return TRUE;
}


VPacketBase* VServerSession::PeekPacket()
{
	VPacketBase* Packet = (VPacketBase*)mRecvQueue.PopFront();
	return Packet;
}

void VServerSession::UpdateSession()
{
	int value = 0;
	struct timeval tval = { 0, 0 };

	fd_set ReadEvent;
	fd_set WriteEvent;
	fd_set ErrorEvent;
	FD_ZERO(&ReadEvent);
	FD_ZERO(&WriteEvent);
	FD_ZERO(&ErrorEvent);

	FD_SET(mSocket, &ReadEvent);
	FD_SET(mSocket, &WriteEvent);
	FD_SET(mSocket, &ErrorEvent);

	while (mRun)
	{
		while (mRun && (!mConnectd || mSocket == -1))
		{
			mConnectEvent.Wait();
			if (mRun)
			{
				DoConnect();
			}
		}

		if (!mConnectd || mSocket == -1)
		{
			VSleep(1);
			continue;
		}

		FD_ZERO(&ReadEvent);
		FD_ZERO(&WriteEvent);
		FD_ZERO(&ErrorEvent);

		FD_SET(mSocket, &ReadEvent);
		FD_SET(mSocket, &WriteEvent);
		FD_SET(mSocket, &ErrorEvent);

		value = select(mSocket + 1, &ReadEvent, &WriteEvent, &ErrorEvent, NULL);
		if (value > 0)
		{
			if (FD_ISSET(mSocket, &ErrorEvent))
			{
				mConnectd = FALSE;
				_OnConnectLost();
				continue;
			}

			if (FD_ISSET(mSocket, &ReadEvent))
			{
				ProcessReadEvent();
			}

			if (FD_ISSET(mSocket, &WriteEvent))
			{
				ProcessWriteEvent();
			}
		}else if (value < 0)
		{

#if VPLATFORM_WIN32
			int Error = WSAGetLastError();
#endif 
			mConnectd = FALSE;
			_OnConnectLost();
		}
		VSleep(1);
	}
}

void VServerSession::ProcessReadEvent()
{
	VASSERT(mSocket != -1 && mFragmentCompositor);

	// 直接将数据读取到重组缓存 
	BYTE* WritePointer = mFragmentCompositor->GetBuffer();
	UINT DataSize = mFragmentCompositor->GetSize();
	UINT Capacity = mFragmentCompositor->GetCapacity();

	// 合适的写入位置
	WritePointer = WritePointer + DataSize;
	UINT ToRead = Capacity - DataSize;
	VASSERT(Capacity >= DataSize);
#if VPLATFORM_ANDROID
	static const int flags = MSG_NOSIGNAL;
#else 
	static const int flags = 0;
#endif 
	int nRecv = recv(mSocket, (char*)WritePointer, (int)ToRead, flags);
	if (nRecv == SOCKET_ERROR)
	{
#if VPLATFORM_WIN32
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			return;
		}
#endif
		_OnConnectLost();
	}
	else if (nRecv > 0)
	{
		mFragmentCompositor->Commit((UINT)nRecv);

		OnRead();
	}
	else
	{
		_OnConnectLost();
	}
}


void VServerSession::OnRead()
{
	// 尝试从碎片组合器中解析出完整数据包.
	VFragmentCompositorContext CompContext;
	VPacketBase* Packet = (VPacketBase*)mFragmentCompositor->DefragmentFirst(CompContext);
	if (Packet)
	{
		// 将数据包解析出来. 
		VPacketBase* QueuePacket;

		do
		{
			// 创建数据包, 并放入到队列中
			QueuePacket = AllocPacket(Packet->Size);
			VMemcpy(QueuePacket, Packet, Packet->Size);
			OnPacket(QueuePacket);

		} while ((Packet = (VPacketBase*)mFragmentCompositor->DefragmentNext(CompContext)) != NULL);

		mFragmentCompositor->DefragmentEnd(CompContext);
	}
}

void VServerSession::OnPacket(VPacketBase* Packet)
{
	mRecvQueue.PushBack((KPacketBase*)Packet);
}

void VServerSession::ProcessWriteEvent()
{
	PrepareSendIOBuffer();

	UINT DataSize = mSendBuffer.GetSize();
	if (DataSize)
	{

#if VPLATFORM_ANDROID
		static const int flags = MSG_NOSIGNAL;
#else 
		static const int flags = 0;
#endif 

		int nSend = send(mSocket, (char*)mSendBuffer.GetData(), DataSize, flags);
		if (nSend == SOCKET_ERROR)
		{
			mConnectd = FALSE;
		}

		mSendBuffer.Clear();
	}
}


void VServerSession::PrepareSendIOBuffer()
{
	if (!mSendBuffer.IsEmpty())
	{
		return;
	}

	KPacketBase* ValidPacket = NULL;
	UINT BufferSpace = mSendBuffer.FreeSpace();
	while (1)
	{
		ValidPacket = mSendQueue.TryPopFront(BufferSpace);
		if (ValidPacket == NULL)
		{
			// 没有数据包, 或者数据包不足以在本次批量发送. 
			break;
		}

		// 将数据包拷贝到批量缓冲中
		mSendBuffer.Write(ValidPacket->Data, ValidPacket->Size);
		PacketPool->FreePacket(ValidPacket);
	}
	
}

void VServerSession::DoConnect()
{
	//VLOG("Server Session DoConnect");
	// 开始连接
	mConnectd = FALSE;

	if (mSocket == -1)
	{
	//	VLOG("Server Session create socket");
		mSocket = socket(AF_INET, SOCK_STREAM, 0);
	}

	// blocking socket.
	Block(mSocket);
	
	//VLOG("Server Session connect to %s:%d", mServerIP, mServerPort);
	int ret = connect(mSocket, (struct sockaddr*)mServerAddress, sizeof(sockaddr_in));
	if (ret != SOCKET_ERROR)
	{
		//VLOG("Server Session DoConnect Success");

		//Nonblock(mSocket);
		NoSIGPIPE(mSocket);
		mConnectd = TRUE;
		OnConnect();
	}
	else
	{
		//VLOG("Server Session DoConnect Failed!");
		_OnConnectLost();
	}
}

void VServerSession::_OnConnectLost()
{
	mConnectd = FALSE;
	if (mSocket != -1)
	{
		CloseSocket(mSocket);
		mSocket = -1;
	}
	OnConnectLost(); 
}

/**
*  开始连接
*/
void VServerSession::BeginConnect()
{
	VASSERT(!mConnectd);
	
	if (mServerIP == nullptr)
	{
		if (!Init(mURL.c_str(), 9800))
		{
			return;
		}
	}
	mRun = 1;

	mConnectEvent.Trigger();
	if (mSessionThread == NULL)
	{
		mSessionThread = VNew SessionThread(this);
		mSessionThread->Run();
	}
}

BOOL VServerSession::IsValid() const
{
	return mConnectd && mSocket != -1;
}

void VServerSession::Disconnect()
{
	mRun = 0;
	if (mSessionThread)
	{
		mConnectEvent.Trigger();
		mSessionThread->WaitForCompletion();
		VDelete mSessionThread;
		mSessionThread = NULL;
	}

	if (mSocket != -1)
	{
		CloseSocket(mSocket);
		mSocket = -1;
	}

	mConnectd = FALSE;

	// clear send queue
	KPacketBase* PendingPacket = NULL;
	while (1)
	{
		PendingPacket = mSendQueue.PopFront();
		if (PendingPacket == NULL)
		{
			break;
		}

		// 将数据包拷贝到批量缓冲中
		PacketPool->FreePacket(PendingPacket);
	}
	// clear recv queue
	while (1)
	{
		PendingPacket = mRecvQueue.PopFront();
		if (PendingPacket == NULL)
		{
			break;
		}
		// 将数据包拷贝到批量缓冲中
		FreePacket(PendingPacket);
	}
	
	mSendBuffer.Clear();
	mRecvBuffer.Clear();

	if (mFragmentCompositor)
	{
		mFragmentCompositor->Clear();
	}

	_OnConnectLost();
}

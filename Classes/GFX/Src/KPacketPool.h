#pragma once
#include "VList.h"


class KPacketPool : public VAllocator
{

	struct PacketBase
	{
		UINT AllocSize;
	};

	struct PacketPoolBase
	{
		virtual void* Alloc() = 0;
		virtual void Free(void*) = 0;
	};

	template<typename T>
	struct PacketPool : public PacketPoolBase
	{
		virtual void* Alloc()
		{
			return MemPool.Alloc();
		}
		virtual void Free(void* ptr)
		{
			MemPool.Free(ptr);
		}
		VMemPool<T, VSyncLockSpin, VMemPoolElementConstructor_POD<T> > MemPool;
	};


#define DECL_PACKET(Size) struct Packet##Size##Bytes : public PacketBase{ \
		char DummyData[Size]; \
			};\
	typedef VMemPool<Packet##Size##Bytes, VSyncLockSpin, VMemPoolElementConstructor_POD<Packet##Size##Bytes> > PacketPool##Size; \
	PacketPool##Size mPacketPool##Size;

	DECL_PACKET(16);
	DECL_PACKET(32);
	DECL_PACKET(64);
	DECL_PACKET(128);
	DECL_PACKET(256);
	DECL_PACKET(512);
	DECL_PACKET(1024);
	DECL_PACKET(2048);
	DECL_PACKET(4096);
	DECL_PACKET(8192);

#undef DECL_PACKET


public:
	KPacketPool();
	~KPacketPool();

	/**
	 * 实际分配的大小为Size + sizeof(UINT) 
	 */
	void* AllocPacket(UINT Size);
	void FreePacket(void* Ptr);
	void Clear();

};

extern KPacketPool* PacketPool;


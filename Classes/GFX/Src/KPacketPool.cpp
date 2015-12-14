#include "VStdAfx.h"
#include "KPacketPool.h"

KPacketPool* PacketPool = nullptr;

KPacketPool::KPacketPool()
{
}


KPacketPool::~KPacketPool()
{
}


void* KPacketPool::AllocPacket(UINT Size)
{
	int psz128 = sizeof(Packet128Bytes);
	//VLOG("128 Byte Size %d", psz128);

	PacketBase* Packet = NULL;
	if (Size <= 16)
	{
		Packet = mPacketPool16.Alloc();
	}
	else if (Size <= 32)
	{
		Packet = mPacketPool32.Alloc();
	}
	else if (Size <= 64)
	{
		Packet = mPacketPool64.Alloc();
	}
	else if (Size <= 128)
	{
		Packet = mPacketPool128.Alloc();
	}
	else if (Size <= 256)
	{
		Packet = mPacketPool256.Alloc();
	}
	else if (Size <= 512)
	{
		Packet = mPacketPool512.Alloc();
	}
	else if (Size <= 1024)
	{
		Packet = mPacketPool1024.Alloc();
	}
	else if (Size <= 2048)
	{
		Packet = mPacketPool2048.Alloc();
	}
	else if (Size <= 4096)
	{
		Packet = mPacketPool4096.Alloc();
	}
	else if (Size <= 8192)
	{
		Packet = mPacketPool8192.Alloc();
	}
	else
	{
		Packet = (PacketBase*)malloc(Size + sizeof(PacketBase));
	}
	VASSERT(Packet);
	Packet->AllocSize = Size;
	return (Packet);
}

void KPacketPool::FreePacket(void* Ptr)
{
	if (Ptr)
	{
		PacketBase* Packet = (PacketBase*)Ptr;
		//Packet = Packet - 1;
		UINT Size = Packet->AllocSize;

		if (Size <= 16)
		{
			mPacketPool16.Free((Packet16Bytes*)Packet);
		}
		else if (Size <= 32)
		{
			mPacketPool32.Free((Packet32Bytes*)Packet);
		}
		else if (Size <= 64)
		{
			mPacketPool64.Free((Packet64Bytes*)Packet);
		}
		else if (Size <= 128)
		{
			mPacketPool128.Free((Packet128Bytes*)Packet);
		}
		else if (Size <= 256)
		{
			mPacketPool256.Free((Packet256Bytes*)Packet);
		}
		else if (Size <= 512)
		{
			mPacketPool512.Free((Packet512Bytes*)Packet);
		}
		else if (Size <= 1024)
		{
			mPacketPool1024.Free((Packet1024Bytes*)Packet);
		}
		else if (Size <= 2048)
		{
			mPacketPool2048.Free((Packet2048Bytes*)Packet);
		}
		else if (Size <= 4096)
		{
			mPacketPool4096.Free((Packet4096Bytes*)Packet);
		}
		else if (Size <= 8192)
		{
			mPacketPool8192.Free((Packet8192Bytes*)Packet);
		}
		else
		{
			free(Packet);
		}

	}
}

void KPacketPool::Clear()
{
	mPacketPool16.Clear();
	mPacketPool32.Clear();
	mPacketPool64.Clear();
	mPacketPool128.Clear();
	mPacketPool256.Clear();
	mPacketPool512.Clear();
	mPacketPool1024.Clear();
	mPacketPool2048.Clear();
	mPacketPool4096.Clear();
	mPacketPool8192.Clear();
}

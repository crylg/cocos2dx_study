#include "VStdAfx.h"
#include "VPacketFactory.h"
#include "KPacketPool.h"

// TODO 评估一下实际占用的内存和性能开销的均衡点!!!

VPacketFactory::VPacketFactory() 
	:mMinOpcode(0), mMaxOpcode(0), mNumOpcode(0), mSharePacketPool(TRUE), mPool(NULL), mPacketInfo(NULL)
{
}


VPacketFactory::~VPacketFactory()
{
	Shutdown();
}

BOOL VPacketFactory::Init(WORD MinOpcode, WORD MaxOpcode, BOOL SharePacketPool)
{
	Shutdown();
	if (MinOpcode >= MaxOpcode)
	{
		return FALSE;
	}

	mMinOpcode = MinOpcode;
	mMaxOpcode = MaxOpcode;
	mNumOpcode = mMaxOpcode - mMinOpcode + 1;

	mSharePacketPool = SharePacketPool;
	if (mSharePacketPool)
	{
		mPool = PacketPool;
	}
	else
	{
		mPool = VNew KPacketPool;
	}

	// 内存不足
	if (mPool == NULL)
	{
		return FALSE;
	}

	mPacketInfo = VNew PacketInfo[mNumOpcode];
	if (mPacketInfo == NULL)
	{
		return FALSE;
	}


	return TRUE;
}

void VPacketFactory::Shutdown()
{
	if (mPacketInfo)
	{
		VDelete[] mPacketInfo;
		mPacketInfo = NULL;
	}

	if (mPool && !mSharePacketPool)
	{
		VDelete mPool;
		mPool = NULL;
	}

	mMinOpcode = mMaxOpcode = mNumOpcode = 0;
}

void VPacketFactory::SetDefaultHandler(VPacketHandlerFunction Handler)
{
	/*for (UINT Index = 0; Index < mNumOpcode; ++Index)
	{
		mPacketInfo[Index].Handler = Handler;
	}*/
}

BOOL VPacketFactory::RegisterPacket(VPacketClass* Packet)
{
	VASSERT(Packet && mPacketInfo);
	UINT Opcode = Packet->GetOpcode();
	VASSERT(Opcode < 0xFFFF);

	// 是否有效的OPCODE?
	if (Opcode  < mMinOpcode || Opcode > mMaxOpcode)
	{
		return FALSE;
	}

	UINT Index = Opcode - mMinOpcode;
	if (mPacketInfo[Index].Class != NULL)
	{
		// 已经注册, 不能重复注册
		return FALSE;
	}

	mPacketInfo[Index].Class = Packet;
//	mPacketInfo[Index].Handler = Handler;
	return TRUE;
}

VPacket* VPacketFactory::AllocPacket(UINT Opcode)
{
	VASSERT(mPool && mPacketInfo);
	VASSERT(Opcode  >= mMinOpcode && Opcode <= mMaxOpcode);
	UINT Index = Opcode - mMinOpcode;
	VPacketClass* PC = mPacketInfo[Index].Class;
	if (PC)
	{
		UINT ObjectSize = PC->GetObjectSize();

		// alloc a memory buffer . 
		void* Buffer = mPool->AllocPacket(ObjectSize);
		if (Buffer)
		{
			// create object from memory buffer.
			VPacket* Packet = PC->CreatePacket(Buffer);
			if (Packet)
			{
				Packet->mFactory = this;
			}
			return Packet;
		}
		
	}
	return NULL;
}

void VPacketFactory::FreePacket(VPacket* Packet)
{
	VASSERT(Packet && mPool);
	Packet->~VPacket();
	mPool->FreePacket(Packet);
}
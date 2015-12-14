#pragma once
#include "VPacketComm.h"

class KPacketPool;

class VCoreApi VPacketFactory : public VAllocator
{
public:
	struct PacketInfo :public VAllocator
	{
		inline PacketInfo() :Class(nullptr) {}
		//inline PacketInfo(VPacketHandlerFunction& Defualt) : Class(nullptr), Handler(Defualt) {}
		VPacketClass* Class;
		//VPacketHandlerFunction Handler;
	};


	/**
	 *   SharePacketPool : 是否使用共享的缓冲, 如果使用共享缓冲, 将会节约内存, 但是会导致性能有所降低. 
	 */
	VPacketFactory();
	~VPacketFactory();
	BOOL Init(WORD MinOpcode, WORD MaxOpcode, BOOL SharePacketPool = TRUE);
	void Shutdown();

	void SetDefaultHandler(VPacketHandlerFunction Handler);

	//inline void SetPacketHandler(UINT Opcode, VPacketHandlerFunction Handler)
	//{
	//	VASSERT(Opcode >= mMinOpcode && Opcode <= mMaxOpcode);
	//	UINT Index = Opcode - mMinOpcode;
	//	mPacketInfo[Index].Handler = Handler;
	//}

	//inline VPacketHandlerFunction& GetPacketHandler(UINT Opcode)
	//{
	//	VASSERT(Opcode >= mMinOpcode && Opcode <= mMaxOpcode);
	//	UINT Index = Opcode - mMinOpcode;
	//	return mPacketInfo[Index].Handler;
	//}


	inline VPacketClass* GetClassAt(UINT Index) const
	{
		VASSERT(Index < mNumOpcode);
		return mPacketInfo[Index].Class;
	}

	inline const PacketInfo& GetClassInfoAt(UINT Index) const
	{
		VASSERT(Index < mNumOpcode);
		return mPacketInfo[Index];
	}


	BOOL RegisterPacket(VPacketClass* Packet);

	template<class PC>
	inline BOOL RegisterPacket()
	{
		return RegisterPacket(PC::GetStaticClass());
	}

	VPacket* AllocPacket(UINT Opcode);
	void FreePacket(VPacket* Packet);


	//inline void HandlePacket(VPacket* Packet, void* userdata)
	//{
	//	VASSERT(Packet && Packet->mFactory == this);
	//	VASSERT(mPacketInfo);
	//	UINT Opcode = Packet->GetOpcode();
	//	VASSERT(Opcode >= mMinOpcode && Opcode <= mMaxOpcode);
	//	UINT Index = Opcode - mMinOpcode;
	//	VPacketHandlerFunction& Handler = mPacketInfo[Index].Handler;
	//	if (Handler)
	//	{
	//		Handler(Packet, userdata);
	//	}
	//}
private:
	UINT mMinOpcode;
	UINT mMaxOpcode;
	UINT mNumOpcode;
	BOOL mSharePacketPool;
	KPacketPool* mPool;
	PacketInfo* mPacketInfo;
};

inline void VPacket::Release()
{
	VASSERT(mFactory);
	mFactory->FreePacket(this);
}
#pragma once
#ifndef __VPACKETCOMM_H__
#define __VPACKETCOMM_H__
#include "VPacketBase.h"
#include "VByteStream.h"


enum EVPacketFlags
{
	VPF_TO_GAME		= 0,
	VPF_TO_ZONE		= 1,

	VPF_RAW			= 2,				// 可以用RAW模式进行转换, 提升性能. 
	VPF_TARGET_MASK = 0x0000000F,

	
};

class VPacket;
class VPacketFactory;

typedef std::function<void(VPacket*, void*)> VPacketHandlerFunction;


class VCoreApi VPacketClass
{
public:
	typedef VPacket* (*CreateObjectFunc)(void* ptr);
	VPacketClass(UINT Opcode, UINT Size, UINT Flags, CreateObjectFunc CreateObjFunc);
	inline UINT GetOpcode() const { return mOpcode; }
	inline UINT GetObjectSize() const { return mObjectSize;  }
	inline UINT GetFlags() const { return mFlags; }
	inline VPacket* CreatePacket(void* ptr)
	{
		return mCreateObjectFunc(ptr);
	}
	inline VPacketClass* GetNextPacketClass() const { return mNextPacketClass; }
	static VPacketClass* GetFirstPacketClass();
private:
	UINT mOpcode;
	UINT mObjectSize;
	UINT mFlags;
	CreateObjectFunc mCreateObjectFunc;
	VPacketClass* mNextPacketClass;
};


template<typename PacketClass>
inline UINT VGetPacketOpcode()
{
	return PacketClass::GetStaticClass()->GetOpcode();
}

class VPacket
{
public:
	inline VPacket() :mFactory(NULL){}
	virtual ~VPacket()
	{

	}
	inline void DoEncode(VByteStream& Stream)
	{
		Encode(Stream);
	}
	virtual BOOL Decode(VByteStream& Stream) = 0;
	virtual void Encode(VByteStream& Stream) = 0;
	virtual const VPacketClass* GetClass() const = 0;
	virtual void Copy(VPacket* Target) const = 0;
	inline void Release();
	virtual UINT GetOpcode() const { return 0; }

	/**
	 * 是否原始数据包(plain old data, fixed size.).
	 * @remarks 如果数据包由POD数据且有固定大小, 那么应该事先原始数据包相关接口, 
	 *	提高数据包发送和接收时的处理效率. 
	 */
	virtual BOOL IsRawPacket() const { return FALSE; }

private:
	static VPacketClass _PacketClass;
	VPacketFactory* mFactory;
	friend class VPacketFactory;
};

struct IRawPacketEncoder
{
	virtual void OnEncode(const VPacketBase* EncodedPacket) = 0;
};

class VPacketRaw : public VPacket
{
	virtual BOOL Decode(VByteStream& Stream) { return FALSE; }
	virtual void Encode(VByteStream& Stream) {}
	virtual BOOL IsRawPacket() const { return TRUE; }
public:
	virtual UINT GetRawPacketSize() const { return 0; }
	virtual void Encode(IRawPacketEncoder* RawPacket) const
	{
	}

	virtual void Decode(const VPacketBase* RawPacket)
	{

	}
};


#define VDECL_PACKET(Class) \
	private:					\
	static VPacketClass _PacketClass;		\
	static VPacket* _CreatePacket(void* ptr);\
	public:				\
	static inline VPacketClass* GetStaticClass() { return &Class::_PacketClass;}\
	virtual const VPacketClass* GetClass() const;\
	virtual UINT GetOpcode() const { return _PacketClass.GetOpcode(); }\
	private:

#define VIMPL_PACKET(Class, Opcode, Flags) \
	VPacket* Class::_CreatePacket(void* ptr)	{  return new(ptr) Class; }	\
	VPacketClass Class::_PacketClass(Opcode, sizeof(Class), Flags, Class::_CreatePacket);\
	const VPacketClass* Class::GetClass() const{ return &Class::_PacketClass;}


#endif 
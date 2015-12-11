#include "VStdAfx.h"
#include "VPacketComm.h"

VPacketClass* smFirstClass = NULL;
VPacketClass::VPacketClass(UINT Opcode, UINT Size, UINT Flags, CreateObjectFunc CreateObjFunc)
	: mOpcode(Opcode)
	, mObjectSize(Size)
	, mFlags(Flags)
	, mCreateObjectFunc(CreateObjFunc)
{
	mNextPacketClass = smFirstClass;
	smFirstClass = this;
}

VPacketClass* VPacketClass::GetFirstPacketClass()
{
	return smFirstClass;
}
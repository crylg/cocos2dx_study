#ifndef __VPACKETBASE_H__
#define __VPACKETBASE_H__

#pragma pack(push)
#pragma pack(1)

struct VPacketBase
{
	unsigned short OpCode;
	unsigned short Size;
};


struct VAGENT_PACKET_HEAD
{
	UINT ClientSessionGuid;
	unsigned short OpCode;
	unsigned short Size;
};

#pragma pack(pop)






#endif 
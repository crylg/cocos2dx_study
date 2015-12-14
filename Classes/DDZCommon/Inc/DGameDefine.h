#pragma once

enum EDConstants
{
	DCONST_NICK_NAME_MAX = 32,		// �ǳ���󳤶�
	ROOM_NAME_MAX = 64,				// ����������󳤶�
};


enum EDGameMode : unsigned char
{
	DGM_UNKNOWN = 0,
	DGM_SINGLE,		//����

	DGM_MP_NORMAL,	//���䶷
	DGM_MP_HAPPY,	//���ֶ�����
	DGM_MP_LAIZI,	//��Ӷ�����
	DGM_MP_TWOPLYER,//����

	DGM_MAX
};


/**
 * �������� 
 */
enum EDRoomType : unsigned char
{
	DRT_NORMAL_FREE = 0,
	DRT_NORMAL_MATCH,
	DRT_NORMAL_PRIVATE,

	DRT_HAPPY_FREE,
	DRT_HAPPY_MATCH,
	DRT_HAPPY_PRIVATE,

	DRT_LAIZI_FREE,
	DRT_LAIZI_MATCH,
	DRT_LAIZI_PRIVATE,

	DRT_2P_FREE,
	DRT_2P_MATCH,
	DRT_2P_PRIVATE,

	DRT_MAX,
};

inline EDGameMode DGetRoomGameMode(EDRoomType Room)
{
	int Mode = Room / 3;
	return (EDGameMode)(Mode + 2);
}

inline BOOL DRoomIsFree(EDRoomType Room)
{
	return (Room % 3) == 0;
}

inline BOOL DRoomIsMatch(EDRoomType Room)
{
	return (Room % 3) == 1;
}

inline BOOL DRoomIsPrivate(EDRoomType Room)
{
	return (Room % 3) == 2;
}

inline EDRoomType DGetFreeRoomType(EDGameMode Mode)
{
	if (Mode == DGM_SINGLE || Mode == DGM_UNKNOWN)
	{
		return DRT_MAX;
	}
	return EDRoomType((Mode - 2) * 3);
}

inline EDRoomType DGetMatchRoomType(EDGameMode Mode)
{
	if (Mode == DGM_SINGLE || Mode == DGM_UNKNOWN)
	{
		return DRT_MAX;
	}
	return EDRoomType((Mode - 2) * 3+1);
}

inline EDRoomType DGetPrivateRoomType(EDGameMode Mode)
{
	if (Mode == DGM_SINGLE || Mode == DGM_UNKNOWN)
	{
		return DRT_MAX;
	}
	return EDRoomType((Mode - 2) * 3+2);
}

// ��Ʒ����
enum EItemType
{
	VITEM_NONE,				// �����ͣ��ǽ������

	VITEM_GOLD =1,			// ���

	VITEM_DIAMOND,			// ��ͷ��
	VITEM_CALLPROP,			// С����
	VITEM_CARDPROP,			// ������

	VITEM_VIP,				// �����
	VITEM_PROP,				// ����
	
	VITEM_CUSTOM =99,		// �Զ�����Ʒ

};

// ��Ʒ����
struct VItemAward
{
	BYTE ItemType;
	UINT ItemNum;

	VItemAward() :ItemType(VITEM_NONE), ItemNum(0) {}
};

#pragma once

enum EDConstants
{
	DCONST_NICK_NAME_MAX = 32,		// 昵称最大长度
	ROOM_NAME_MAX = 64,				// 房间名字最大长度
};


enum EDGameMode : unsigned char
{
	DGM_UNKNOWN = 0,
	DGM_SINGLE,		//单机

	DGM_MP_NORMAL,	//经典斗
	DGM_MP_HAPPY,	//快乐斗地主
	DGM_MP_LAIZI,	//癞子斗地主
	DGM_MP_TWOPLYER,//比赛

	DGM_MAX
};


/**
 * 房间类型 
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

// 物品类型
enum EItemType
{
	VITEM_NONE,				// 无类型，非金币类型

	VITEM_GOLD =1,			// 金币

	VITEM_DIAMOND,			// 街头币
	VITEM_CALLPROP,			// 小喇叭
	VITEM_CARDPROP,			// 记牌器

	VITEM_VIP,				// 贵宾卡
	VITEM_PROP,				// 道具
	
	VITEM_CUSTOM =99,		// 自定义物品

};

// 物品奖励
struct VItemAward
{
	BYTE ItemType;
	UINT ItemNum;

	VItemAward() :ItemType(VITEM_NONE), ItemNum(0) {}
};

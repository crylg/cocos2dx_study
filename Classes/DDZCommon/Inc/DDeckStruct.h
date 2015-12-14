#pragma once
#include "DGameDefine.h"
#include "VByteStream.h"

#pragma pack(push)
#pragma pack(1)
class DRoom : public VAllocator
{
public:
	struct DRoomAwardBase
	{
		UINT		RoundNum;					// 奖励局数
		VItemAward	RoundAward;					// 局数奖励
		VItemAward	WinningStreakAward[9];		// 连胜奖励

		friend inline VByteStream& operator<<(VByteStream& Stream, const DRoomAwardBase& a)
		{
			Stream << a.RoundNum;
			Stream << a.RoundAward.ItemType << a.RoundAward.ItemNum;

			for (int i = 0; i < 9; i++)
			{
				Stream << a.WinningStreakAward[i].ItemType << a.WinningStreakAward[i].ItemNum;
			}
			return Stream;
		}
		friend inline VByteStream& operator>>(VByteStream& Stream, DRoomAwardBase& a)
		{
			Stream >> a.RoundNum;
			Stream >> a.RoundAward.ItemType >> a.RoundAward.ItemNum;

			for (int i = 0; i < 9; i++)
			{
				Stream >> a.WinningStreakAward[i].ItemType >> a.WinningStreakAward[i].ItemNum;
			}
			return Stream;
		}
	};

	WORD TypeGuid;							// 类型GUID
	EDRoomType RoomType;					// 房间类型
	std::string Name;						// 房间名称
	int CoinCondition;						// 入场时需要的金币条件
	int ScoreFactor;						// 底分
	WORD CurrentNumUser;					// 当前用户数量
	DRoomAwardBase NormalAward;				// 普通奖励
	int GoldCost;							// 手续费

	friend inline VByteStream& operator<<(VByteStream& Stream, const DRoom& Room)
	{
		Stream << Room.TypeGuid;
		Stream.Write(&Room.RoomType, sizeof(EDRoomType));
		Stream << Room.Name;
		Stream << Room.CoinCondition;
		Stream << Room.ScoreFactor;
		Stream << Room.CurrentNumUser;
		Stream << Room.NormalAward;
		Stream << Room.GoldCost;

		return Stream;
	}
	friend inline VByteStream& operator>>(VByteStream& Stream, DRoom& Room)
	{
		Stream >> Room.TypeGuid;
		Stream.Read(&Room.RoomType, sizeof(EDRoomType));
		Stream >> Room.Name;
		Stream >> Room.CoinCondition;
		Stream >> Room.ScoreFactor;
		Stream >> Room.CurrentNumUser;
		Stream >> Room.NormalAward;
		Stream >> Room.GoldCost;
		
		return Stream;
	}
};


class DDeckStruct
{
public:
	DDeckStruct();
	~DDeckStruct();
};


struct DDeckPlayerDesc
{
	UINT64 AccountGuid;
	char Name[DCONST_NICK_NAME_MAX];
	UINT64 Gold;
	UINT Diamond;
	WORD Avatar;
	bool Ready;
	char Slot;
	WORD Level;
	BYTE VipLevel;
	UINT WinRound;
	UINT LostRound;
	bool Managed;

	friend inline VByteStream& operator<<(VByteStream& Stream, const DDeckPlayerDesc& dpd)
	{
		Stream << dpd.AccountGuid;
		Stream.WriteCString(dpd.Name);
		Stream << dpd.Gold << dpd.Diamond << dpd.Avatar << dpd.Ready << dpd.Slot;
		Stream << dpd.Level << dpd.VipLevel << dpd.WinRound << dpd.LostRound << dpd.Managed;
		return Stream;
	}
	friend inline VByteStream& operator>>(VByteStream& Stream, DDeckPlayerDesc& dpd)
	{
		Stream >> dpd.AccountGuid;
		Stream.ReadCString(dpd.Name, DCONST_NICK_NAME_MAX);
		Stream >> dpd.Gold >> dpd.Diamond >> dpd.Avatar >> dpd.Ready >> dpd.Slot;
		Stream >> dpd.Level >> dpd.VipLevel >> dpd.WinRound >> dpd.LostRound >> dpd.Managed;
		return Stream;
	}
};


#pragma pack(pop)

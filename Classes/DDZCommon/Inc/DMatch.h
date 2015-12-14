#pragma once
#include "DGameDefine.h"
#include "VByteStream.h"


/**
* 比赛入口的开放类型
*/
enum EMatchEntryOpenType : unsigned char
{
	MEOT_BY_USERNUM = 0,		// 人满即开
	MEOT_TIME_DATE = 1,		// 特定日期开放
	MEOT_TIME_WEEKLY,		// 每周开放
	MEOT_TIME_DAILY,		// 每天开放	
	
};


/**
 * 比赛类型 
 */
//enum EVMatchType : unsigned char
//{
//	VMATCH_TYPE_NORMAL=0,		// 常规比赛.
//	VMATCH_TYPE_SPECIAL,		// 
//};

enum EVMatchStage : unsigned char
{
		VMatch_Idle = 0,
		VMatch_Filter,		// 预赛
		VMatch_Extra,		// 加时赛
		VMatch_Final,		// 决赛
};




#pragma pack(push)
#pragma pack(1)

struct DMatchState
{
	BYTE Status;
	BYTE Subscribed;						// 是否已订阅
	BYTE Enrolled;							//
	BYTE CurrentNumUser;

	friend inline VByteStream& operator<<(VByteStream& Stream, const DMatchState& a)
	{
		Stream << a.Status << a.Subscribed << a.Enrolled << a.CurrentNumUser;
		return Stream;
	}
	friend inline VByteStream& operator>>(VByteStream& Stream, DMatchState& a)
	{
		Stream >> a.Status >> a.Subscribed >> a.Enrolled >> a.CurrentNumUser;
		return Stream;
	}
};

class DMatch : public VAllocator
{
public:
	enum Flags
	{
		Subscription	= 1,		// 可预约
		Continuous		= 2,		// 连续的.		一旦开赛, 将立即开始一个新的赛事
		//Special			= 4,		
	};

	UINT MatchGuid;
	BYTE GameMode;							// 当前赛事使用的游戏模式.
	std::string Name;						// 房间名称
	int CoinCondition;						// 入场时需要的金币条件
	int ScoreFactor;						// 底分
	BYTE IsSpecial;							// 特殊入口比赛
	int Chip;								// 筹码
	int GoldAwardOut;						// 淘汰一个的金币奖励
	WORD CurrentNumUser;					// 当前用户数量
	/**
	 * 比赛场奖励 
	 */
	struct Award
	{
		WORD		RankMin;			// 排名区间
		WORD		RankMax;			
		UINT		Gold;				// 奖励金币数
		UINT		Diamond;			// 奖励街头币数
		UINT		Item;				// 奖励的物品ID.			0: 无效物品, 1: 自定义物品
		UINT		ItemAmount;			// 物品数量
		std::string  CustomItemIcon;	// 自定义物品的图标类型. 		
		friend inline VByteStream& operator<<(VByteStream& Stream, const Award& a)
		{
			Stream << a.RankMin;
			Stream << a.RankMax;
			Stream << a.Gold;
			Stream << a.Diamond;
			Stream << a.Item;
			if (a.Item)
			{
				Stream << a.ItemAmount;
				if (a.Item == VITEM_CUSTOM)
				{
					Stream << a.CustomItemIcon;
				}
			}
			return Stream;
		}
		friend inline VByteStream& operator>>(VByteStream& Stream, Award& a)
		{
			Stream >> a.RankMin;
			Stream >> a.RankMax;
			Stream >> a.Gold;
			Stream >> a.Diamond;
			Stream >> a.Item;
			if (a.Item)
			{
				Stream >> a.ItemAmount;
				if (a.Item == VITEM_CUSTOM)
				{
					Stream >> a.CustomItemIcon;
				}
			}
			return Stream;
		}
	};
	std::vector<Award> RankAwards;	// 排名奖励

	BYTE OpenType;
	UINT OpenTime;
	int OpenInterval;	// 比赛间隔

	WORD MinUser;		// 最小人数要求	
	WORD MaxUser;		// 最大人数要求
	WORD Flags;
	int Icon;			// 图标索引
	BYTE RoundLimit;	// 回合限制

	friend inline VByteStream& operator<<(VByteStream& Stream, const DMatch& Match)
	{
		Stream << Match.MatchGuid;
		Stream << Match.GameMode;
		Stream << Match.Name;
		Stream << Match.CoinCondition;
		Stream << Match.ScoreFactor;
		Stream << Match.IsSpecial;
		Stream << Match.Chip;
		Stream << Match.GoldAwardOut;
		Stream << Match.CurrentNumUser;

		Stream << (BYTE)Match.RankAwards.size();
		for (UINT i = 0; i < Match.RankAwards.size(); i++)
		{
			Stream << Match.RankAwards[i];
		}

		Stream << Match.OpenType;
		Stream << Match.OpenTime;

		Stream << Match.MinUser << Match.MaxUser << Match.Flags << Match.Icon;
		Stream << Match.OpenInterval << Match.RoundLimit;

		return Stream;
	}
	friend inline VByteStream& operator>>(VByteStream& Stream, DMatch& Match)
	{
		Stream >> Match.MatchGuid;
		Stream >> Match.GameMode;
		Stream >> Match.Name;
		Stream >> Match.CoinCondition;
		Stream >> Match.ScoreFactor;
		Stream >> Match.IsSpecial;
		Stream >> Match.Chip;
		Stream >> Match.GoldAwardOut;
		Stream >> Match.CurrentNumUser;

		BYTE RankAwardNum;
		Stream >> RankAwardNum;

		Match.RankAwards.clear();
		Award ra;
		for (UINT i = 0; i < RankAwardNum; i++)
		{
			Stream >> ra;
			Match.RankAwards.push_back(ra);
		}
		Stream >> Match.OpenType;
		Stream >> Match.OpenTime;

		Stream >> Match.MinUser >> Match.MaxUser >> Match.Flags >> Match.Icon;
		Stream >> Match.OpenInterval >> Match.RoundLimit;

		return Stream;
	}

};

#pragma pack(pop)

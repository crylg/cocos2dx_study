#pragma once
#include "DGameDefine.h"
#include "VByteStream.h"


/**
* ������ڵĿ�������
*/
enum EMatchEntryOpenType : unsigned char
{
	MEOT_BY_USERNUM = 0,		// ��������
	MEOT_TIME_DATE = 1,		// �ض����ڿ���
	MEOT_TIME_WEEKLY,		// ÿ�ܿ���
	MEOT_TIME_DAILY,		// ÿ�쿪��	
	
};


/**
 * �������� 
 */
//enum EVMatchType : unsigned char
//{
//	VMATCH_TYPE_NORMAL=0,		// �������.
//	VMATCH_TYPE_SPECIAL,		// 
//};

enum EVMatchStage : unsigned char
{
		VMatch_Idle = 0,
		VMatch_Filter,		// Ԥ��
		VMatch_Extra,		// ��ʱ��
		VMatch_Final,		// ����
};




#pragma pack(push)
#pragma pack(1)

struct DMatchState
{
	BYTE Status;
	BYTE Subscribed;						// �Ƿ��Ѷ���
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
		Subscription	= 1,		// ��ԤԼ
		Continuous		= 2,		// ������.		һ������, ��������ʼһ���µ�����
		//Special			= 4,		
	};

	UINT MatchGuid;
	BYTE GameMode;							// ��ǰ����ʹ�õ���Ϸģʽ.
	std::string Name;						// ��������
	int CoinCondition;						// �볡ʱ��Ҫ�Ľ������
	int ScoreFactor;						// �׷�
	BYTE IsSpecial;							// ������ڱ���
	int Chip;								// ����
	int GoldAwardOut;						// ��̭һ���Ľ�ҽ���
	WORD CurrentNumUser;					// ��ǰ�û�����
	/**
	 * ���������� 
	 */
	struct Award
	{
		WORD		RankMin;			// ��������
		WORD		RankMax;			
		UINT		Gold;				// ���������
		UINT		Diamond;			// ������ͷ����
		UINT		Item;				// ��������ƷID.			0: ��Ч��Ʒ, 1: �Զ�����Ʒ
		UINT		ItemAmount;			// ��Ʒ����
		std::string  CustomItemIcon;	// �Զ�����Ʒ��ͼ������. 		
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
	std::vector<Award> RankAwards;	// ��������

	BYTE OpenType;
	UINT OpenTime;
	int OpenInterval;	// �������

	WORD MinUser;		// ��С����Ҫ��	
	WORD MaxUser;		// �������Ҫ��
	WORD Flags;
	int Icon;			// ͼ������
	BYTE RoundLimit;	// �غ�����

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

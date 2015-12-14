#include "VStdAfx.h"
#include "DAIPostLord.h"


DAIPostLord::DAIPostLord(DDeckCharacter* Owner) :DAIFarmer(Owner)
{
}


DAIPostLord::~DAIPostLord()
{
}

DGameAIHand DAIPostLord::PlayHand() const
{
	DGameAIHand AIHand;

	DDeckCharacter* Lord = GetLord();
	const DDeckAIHint& LordHint = Lord->AI_GetHint();
	 AIHand = Play_Hand_To_Escape(LordHint);
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = Play_Help();
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = Play_Prevent();
	if (AIHand)
	{
		return AIHand;
	}

	DDeckCharacter* Friend = GetFriend();
	int FriendReaminNum = Friend->GetNumCards();
	if (FriendReaminNum == 1)
	{
		AIHand = TryLetEscape1AfterBomb_1(DCN_NULL, Friend);
		/*if (AIHand)
		{
			return AIHand;
		}

		const DCard& Card = Friend->GetCard(0);
		if (mHand.last().Number < Card.Number)
		{
			DHand* Result = VNew DHand(DHT_1, mHand.last().Number);
			Result->AddCard(mHand.last());
			return Result;
		}*/
	}

	if (FriendReaminNum == 2)
	{

	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////


	AIHand = GetHand_Min();
	
	//return GetHand_Min();*/
	return AIHand;
}


DGameAIHand DAIPostLord::Play_Help() const
{
	DDeckCharacter* Friend = GetFriend();
	int FriendReaminNum = Friend->GetNumCards();
	

	if (FriendReaminNum == 1)
	{
		DGameAIHand AIHand = TryLetEscape1AfterBomb_1(DCN_NULL, Friend);
		if (AIHand)
		{
			return AIHand;
		}

		const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
		DCardNumber MinCard = AnyLessThen(FriendRemainCard, DCN_NULL);
		if (MinCard != DCN_NULL)
		{
			// 有小个子过给下家.
			return DGameAIHand(DHT_1, MinCard, 1);
		}
	}else if (FriendReaminNum == 2)
	{

		const DCard& C0 = Friend->GetCard(0);
		const DCard& C1 = Friend->GetCard(1);

		DCardNumber FriendRemainDoubleNumber = DCN_NULL;
		bool FriendIs2K = false;
		if (C0.Number == C1.Number)
		{
			FriendRemainDoubleNumber = C0.Number;
		}
		else if (C0.IsKing() && C1.IsKing())
		{
			FriendIs2K = true;
		}


		DAtom A;
		if (FriendIs2K)
		{
			// Bomb Bomb Bomb!!!
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}

		if (FriendRemainDoubleNumber == DCN_NULL)
		{
			// 友方剩余两个个子. 
			//DCardNumber RemainMinSingle = VMin(C0.Number, C1.Number);
			DCardNumber RemainMaxSingle = VMax(C0.Number, C1.Number);
			if (m1Atoms.size() )
			{
				if (m1Atoms[m1Atoms.size() - 1].Number < RemainMaxSingle)
				{
					return DGameAIHand(DHT_1, m1Atoms[m1Atoms.size() - 1].Number, 1);
				}

			}

			A = Peek1_FromStraightLessThen(RemainMaxSingle);
			if (A)
			{
				return DGameAIHand(DHT_1, A.Number, 1);
			}

			A = Peek1_FromDoubleLessThen(RemainMaxSingle);
			if (A)
			{
				return DGameAIHand(DHT_1, A.Number, 1);
			}

			return DGameAIHand::PASS;
		}
		else
		{
			// 剩余的是对子. 
			A = Peek2_LessThen(FriendRemainDoubleNumber);
			if (A)
			{
				return DGameAIHand(DHT_2, A.Number,2);
			}

			A = Peek2_FromDoubleStaight_LessThen(FriendRemainDoubleNumber);
			if (A)
			{
				return DGameAIHand(DHT_2, A.Number, 2);
			}
		}

	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::Play_Prevent() const
{
	DDeckCharacter* Lord = GetLord();
	int LordRemainNum = Lord->GetNumCards();
	if (LordRemainNum == 1)
	{
	}else if (LordRemainNum == 2)
	{
	}

	return DGameAIHand::PASS;
}

/**
 * 友方剩余1, 尝试炸掉, 扩大战果后让其胜出. 
 */
DGameAIHand DAIPostLord::TryLetEscape1AfterBomb_1(DCardNumber FollowNumber, const DDeckCharacter* Friend) const
{

	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(FriendRemainCard, A.Number) != DCN_NULL)
		{
			// 有小个子过给下家.
			return MakeBomb(A);
		}

		// 没有小个子给下家. 即便是这样, 地主也依然可能有小个子 
		int BombRate = 10;
		if (FriendRemainCard >= MaxCard)
		{
			// 下家余牌很大. 
			BombRate = 90;				// 9成几率炸掉
		}
		if (Chance(BombRate))
		{
			return MakeBomb(A);
		}
	}

	// 当我们无法选出能收回的个子的时候. 依然考虑炸掉的可能性
	// 如果友方剩余的牌比较大的话.其实还是可以炸掉. 
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		int BombRate = 0;
		if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
		{
			// 下家余牌很大. 
			BombRate = 20;				// 9成几率炸掉
		}
		else if (FriendRemainCard >= DCN_Q)
		{
			BombRate = 10;
		}

		if (Chance(BombRate))
		{
			return MakeBomb(A);
		}
	}
	

	return DGameAIHand::PASS;
}

/**
* 跟友方个子, 友方剩余1, 尝试跟个个子, 在友方逃不掉后, 依然可以逃. 
*/
DGameAIHand DAIPostLord::TryLetEscape1AfterPlus_1(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	DAtom A = Select_Ascend(m1Atoms, FollowNumber);
	if (A && A.Number < FriendRemainCard)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}
	return DGameAIHand::PASS;
}

/**
 * 跟地主单张. 友方剩余1, 尝试炸掉, 扩大战果 
 */
DGameAIHand DAIPostLord::TryHelpEscape1AfterBomb_1(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	DAtom A;
	if (FriendRemainCard <= FollowNumber)
	{
		// 下家打不起. 
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	else
	{
		// 下家打得起, 下家此时可以跑. 
		const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			int BombRate = 10;
			if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
			{
				// 下家余牌很大. 
				BombRate = 70;				// 9成几率炸掉
			}
			else if (FriendRemainCard >= DCN_Q)
			{
				BombRate = 50;
			}
			if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
		
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::TryHelpEscape1AfterPlus_1(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	// 没有炸弹. 

	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A;
	if (FriendRemainCard <= FollowNumber)
	{
		// 友方要不起. 
		// 此时如果我有最大的个子, 打最大的个子. 
		A = Select_Descend(m1Atoms, FollowNumber);

		if (A && A.Number >= MaxCard)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}

		return DGameAIHand::PASS;
	}
	else
	{
		// 插一张. 
		A = Select_Descend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	return DGameAIHand::PASS;
}

// 尝试让下家跑...
DGameAIHand DAIPostLord::TryHelpEscape_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// 友方的牌大于2张的时候, 我们不予考虑. 
		return DGameAIHand::PASS;
	}
	DGameAIHand TestHand;
	DAtom A;
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const int BombNum = GetBombNum();
	if (FriendRemainCardNum == 1)
	{
		if (Friend == HandOwner)
		{
			if (BombNum && (TestHand = TryLetEscape1AfterBomb_1(FollowNumber, Friend)))
			{
				return TestHand;
			}
			
			if (TestHand = TryLetEscape1AfterPlus_1(ToFollow, Friend))
			{
				return TestHand;
			}
			
			return DGameAIHand::PASS;
		}
		else
		{
			// 跟地主
			if (BombNum && (TestHand = TryHelpEscape1AfterBomb_1(ToFollow, Friend)))
			{
				return TestHand;
			}

			if (TestHand = TryHelpEscape1AfterPlus_1(ToFollow, Friend))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
	}
	else
	{
		// 友方剩余两张.


	}



	return DGameAIHand::PASS;
}

// 跟地主个子, 避免地主逃跑. 
DGameAIHand DAIPostLord::TryPrevent_1(const DHand* ToFollow, const DDeckCharacter* Lord) const
{
	// 地主未报警, 忽略. 
	const int LordNumRemain = Lord->GetNumCards();
	if (LordNumRemain > 2)
	{
		return DGameAIHand::PASS;
	}

	// 先判断, 下家是否能顶住, 如果顶不住, 我来顶. 
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DDeckCharacter* Friend = GetFriend();
	VASSERT(Friend != Lord);
	const DDeckAIHint& FriendHint = Friend->AI_GetHint();
	if (FriendHint.AI_Determ_Single_Max(FollowNumber, true))
	{
		// 地主上家能打得起, 我不需要强压, 
		return DGameAIHand::PASS;
	}

	// 
	DGameAIHand AIHand;
	DCardNumber OpposeNumber = DCN_NULL;
	if (LordNumRemain == 1)
	{
		// 地主剩余1张. 
		OpposeNumber = Lord->GetCard(0).Number;
		AIHand = TryBreak_1_1(FollowNumber, OpposeNumber);
		if (AIHand)
		{
			return AIHand;
		}
	}
	else if (LordNumRemain == 2)
	{
		const DCard& C0 = Lord->GetCard(0);
		const DCard& C1 = Lord->GetCard(1);
		if (C0.Number == C1.Number)
		{
			OpposeNumber = C0.Number;
			AIHand = TryBreak_1_1(FollowNumber, C0.Number);
		}
		else
		{
			DCardNumber RemainMax = VMax(C0.Number, C1.Number);
			AIHand = TryBreak_1_1(FollowNumber, RemainMax);
		}

		if (AIHand)
		{
			return AIHand;
		}
	}
	
	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	// 如果觉得自己能跑, 自己跑. 
	DGameAIHand AIHand = TryEscape(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber Card = mDeck->AI_QueryCardGreatThen(mCardSlots, FollowNumber);
	// 根本要不起
	if (Card == DCN_NULL && !HasBomberOr2K())
	{
		return DGameAIHand::PASS;
	}


	// 在要得起的情况下, 尝试让友方逃跑. 
	AIHand = TryHelpEscape_1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	// 常规出牌 从个子中选.
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		// 避免地主逃跑. 
		AIHand = TryPrevent_1(ToFollow, HandOwner);


		DAtom A = Select_Ascend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}

		// 如果从个子中打不起. 
		A = Peek1_FromStraight(FollowNumber);
		if (A)
		{
			return	DGameAIHand(DHT_1, A.Number, 1);
		}

		if (mCardSlots[DCN_2] && DCN_2 > FollowNumber)
		{
			return  DGameAIHand(DHT_1, DCN_2, 1);
		}

		
		if (mCardSlots[DCN_A] && DCN_A > FollowNumber)
		{
			return  DGameAIHand(DHT_1, DCN_A, 1);
		}

		int BombRate = 40;
		int LordRemainCards = HandOwner->GetNumCards();
		if (LordRemainCards < 4)
		{
			BombRate = 100;
		}else if (LordRemainCards < 7)
		{
			BombRate = 85;
		}
		else if (LordRemainCards < 12)
		{
			BombRate = 60;
		}
		if (Chance(BombRate))
		{
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}

		return DGameAIHand::PASS;
	}

	// 
	return DGameAIHand::PASS;
}

#pragma region "对子"
DGameAIHand DAIPostLord::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	// 根本要不起
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber Card = Peek2_Force(FollowNumber);
	if (Card == DCN_NULL && !HasBomberOr2K())
	{
		return DGameAIHand::PASS;
	}

	// 尝试自己胜出
	DGameAIHand AIHand = TryEscape_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}


	// AI 打得起对子, 但是不认为自己满足胜出条件. 尝试让下家胜出. 
	bool Continue = true;
	AIHand = TryHelpEscape_2(ToFollow, HandOwner, Continue);
	if (AIHand)
	{
		return AIHand;
	}

	if (!Continue)
	{
		return DGameAIHand::PASS;
	}

	// 避免地主逃跑. 
	AIHand = TryPrevent_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return DGameAIHand::PASS;
	}

	DDeckCharacter* Friend = GetFriend();
	DAtom A;
	// 常规出牌
	if (HandOwner == Friend)
	{
		// 从对子中选择
		if (FollowNumber < DCN_J && (A = Select_Ascend(m2Atoms, FollowNumber)) && A.Number <= (FollowNumber + 4) && A.Number <= DCN_10 )
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		return DGameAIHand::PASS;

	}
	else
	{
		A = Select_Ascend(m2Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		A = Peek2_FromDoubleStaight(FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
		
		int BombRate = 70;
		if (FollowNumber >= DCN_A/* || mDeck->AI_Determ_Double_Greater(mCardSlots, FollowNumber) <= 2*/)
		{
			// 将3个中/4个中的2 拆掉来打????
			DCardNumber C2 = Peek2_Force(FollowNumber);
			if (C2 != DCN_NULL)
			{
				return DGameAIHand(DHT_2, C2, 2);
			}

			// 地主的对子打得很大, 不让其上牌.
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}

	}
	

	return DGameAIHand::PASS;
}

/**
 * 尝试让下家跑. 
 */
DGameAIHand DAIPostLord::TryHelpEscape_2(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// 友方的牌大于2张的时候, 我们不予考虑. 
		return DGameAIHand::PASS;
	}

	DGameAIHand TestHand;
	DAtom A;
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const int BombNum = GetBombNum();
	
	if (FriendRemainCardNum == 1)
	{
		if (Friend == HandOwner)
		{
			// 下家可以跑. 

			// 如果有炸弹, 尝试炸掉后让友方跑.
			if (BombNum && (TestHand = TryLetEscape1AfterBomb_1(FollowNumber, Friend)))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
		else
		{
			// 下家无论如何跑不掉. 
			// 跟地主, 尝试帮组下家胜出
			if (BombNum && (TestHand = TryHelpEscape_Remain1_WithBomb(ToFollow, Friend)))
			{
				return TestHand;
			}

			// 无法确保用炸弹后保证下家胜出. 尝试用最大的对子来过给下家. 
			if (TestHand = TryHelpEscape_Remain1_With2(ToFollow, Friend, Continue))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
	}
	else
	{
		// 友方剩余两张.
		const DCard& C0 = Friend->GetCard(0);
		const DCard& C1 = Friend->GetCard(1);
		
		DCardNumber FriendRemainDoubleNumber = DCN_NULL;
		bool FriendIs2K = false;
		if (C0.Number == C1.Number && C0.Number > FollowNumber)
		{
			FriendRemainDoubleNumber = C0.Number;
		}
		else if (C0.IsKing() && C1.IsKing())
		{
			FriendIs2K = true;
		}

		if (FriendIs2K)
		{
			// Bomb Bomb Bomb!!!
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}

		if (Friend == HandOwner)
		{
			// 跟的是友方. 且友方剩余不是对子. 
			if (FriendRemainDoubleNumber == DCN_NULL)
			{
				// 让其出牌.  并且不要再继续后续行为. 
				Continue = false;
				return DGameAIHand::PASS;
			}
			else
			{
				// 友方是对子. 
					// 尝试炸掉后让其逃跑
					if (TestHand = TryLetEscape2AfterBomb_2(ToFollow, Friend, FriendRemainDoubleNumber))
					{
						return TestHand;
					}

					// 尝试插入一个对子.
					if (TestHand = TryLetEscape2AfterPlus_2(ToFollow, Friend, FriendRemainDoubleNumber))
					{
						return TestHand;
					}
			}
		}
		else
		{
			bool FriendCanEscape = false;
			// 跟地主
			if (FriendRemainDoubleNumber > FollowNumber)
			{
				FriendCanEscape = true;
			}
			

			if (FriendCanEscape)
			{
				// 尝试炸掉后让其逃跑
				if (TestHand = TryLetEscape2AfterBomb_2(ToFollow, Friend, FriendRemainDoubleNumber))
				{
					return TestHand;
				}

				// 尝试插入一个对子.
				if (TestHand = TryLetEscape2AfterPlus_2(ToFollow, Friend, FriendRemainDoubleNumber))
				{
					return TestHand;
				}
			}
			else
			{
				// 下家无法出. 尝试协助其逃跑. 
				if (TestHand = TryHelpEscape_Remain2_2(ToFollow, Friend))
				{
					return TestHand;
				}
			}
		}

	}

	return DGameAIHand::PASS;
}

bool DAIPostLord::AI_Determ_Double_Can_Escape(DCardNumber FriendRemainDoubleNumber, float RateScale) const
{
	// 查询潜在的大于下家对子的牌数量, 小对子越多, 逃跑几率越高
	int NumLesser = mDeck->AI_Determ_Double_Lesser(mCardSlots, FriendRemainDoubleNumber);
	int BombRate = 0;
	if (NumLesser >= 3)
	{
		BombRate = 60;
	}
	else if (NumLesser == 2)
	{
		BombRate = 30;
	}
	else
	{
		BombRate = 10;
	}
	// 查询玩家手中的对子是否足够大. 如果足够大的话, 也可以拆开来打. 大于对子的牌越少, 逃跑几率越高.
	int CardMaxRate = 0;
	int NumGreater = mDeck->AI_Determ_Double_Greater(mCardSlots, FriendRemainDoubleNumber);
	if (NumGreater == 0)
	{
		CardMaxRate = 100;
	}
	else if (NumGreater == 1)
	{
		CardMaxRate = 30;
	}
	else if (NumGreater == 2)
	{
		CardMaxRate = 10;
	}

	BombRate = (int)(BombRate + CardMaxRate) >> 1;
	BombRate = (int)(BombRate * RateScale);

	if (Chance(BombRate))
	{
		return true;
	}
	return false;
}

/**
 * 友方剩余对子, 本来可以直接跑. 
 */
DGameAIHand DAIPostLord::TryLetEscape2AfterBomb_2(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber FriendRemainDoubleNumber) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(FriendRemainCard, A.Number, 2) != DCN_NULL)
		{
			// 有小对子给下家. 
			return MakeBomb(A);
		}

		// 没有小对子给下家. 即便是这样, 地主也依然可能有小对子, 或者对子足够大, 可以拆开打 
		// 查询潜在的大于下家对子的牌数量
		if (AI_Determ_Double_Can_Escape(FriendRemainDoubleNumber, 1.0f))
		{
			return MakeBomb(A);
		}
	}

	// 当我们无法选出能收回的炸弹的时候. 如果认为友方的对子有逃跑几率的话依然考虑炸掉的可能性

	A = SelectBomb(DCN_NULL);
	if (A && AI_Determ_Double_Can_Escape(FriendRemainDoubleNumber, 0.6f))
	{
		return MakeBomb(A);
	}

	return DGameAIHand::PASS;
}

/**
 * 友方剩余对子, 本来可以直接跑, 尝试插入一个对子 
 */
DGameAIHand DAIPostLord::TryLetEscape2AfterPlus_2(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber FriendRemainDoubleNumber) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A = Select_Ascend(m2Atoms, FollowNumber);
	if (A && A.Number < FriendRemainDoubleNumber)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}
	return DGameAIHand::PASS;
}

/**
 * 下家剩余1个单张, 且无法跟当前牌的情况下, 用炸弹协助其胜出 
 */
DGameAIHand DAIPostLord::TryHelpEscape_Remain1_WithBomb(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DDeckCharacter* Lord = GetLord();
	const int LordRemainNum = Lord->GetNumCards();
	float BombLordFactor = ((float)LordRemainNum / 6.0f);
	BombLordFactor *= BombLordFactor;
	BombLordFactor = VClamp(BombLordFactor, 0.1f, 1.6f);
	DAtom A;
	
	// 选择炸弹. 
	A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(FriendRemainCard, A.Number) != DCN_NULL)
		{
			// 有小个子过给下家.
			return MakeBomb(A);
		}

		// 没有小个子给下家. 即便是这样, 地主也依然可能有小个子 
		int BombRate = 10;
		if (FriendRemainCard >= MaxCard)
		{
			// 下家余牌很大. 
			BombRate = 60;				// 9成几率炸掉
		}else if (FriendRemainCard >= (MaxCard - 1))
		{
			BombRate = 30;
		}
		BombRate = (int)(BombRate * BombLordFactor);

		if (Chance(BombRate))
		{
			return MakeBomb(A);
		}
	}

	// 到达这里的时候, 下家是要不起当前牌的, 如果下家的个子较大, 我们依然会炸掉, 无论是否能收回炸弹

	A = SelectBomb(DCN_NULL);
	if (A)
	{

		int BombRate = 10;
		if (AnyLessThen(FriendRemainCard, A.Number) != DCN_NULL)
		{
			BombRate = 70;
		}
		else
		{
			// 我不能过牌给下家, 但是只要下家的牌够大, 我们假定地主会过牌给下家. 
			const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
			if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
			{
				// 下家余牌很大. 
				BombRate = 50;				// 9成几率炸掉
			}
			else if (FriendRemainCard >= DCN_Q)
			{
				BombRate = 30;
			}
		}

		BombRate = (int)(BombRate * BombLordFactor);

		if (Chance(BombRate))
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

// 下家剩余对子, 视图炸掉后协助其逃跑
DGameAIHand DAIPostLord::TryHelpEscape_Remain2_WithBomb(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber RemainDoubleNumber) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	//DDeckCharacter* Lord = GetLord();
	//const int LordRemainNum = Lord->GetNumCards();
	//float BombLordFactor = ((float)LordRemainNum / 5.0f);
	//BombLordFactor *= BombLordFactor;
	//BombLordFactor = VClamp(BombLordFactor, 0.2f, 1.8f);

	
	DAtom A;

	// 选择炸弹. 
	A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(RemainDoubleNumber, A.Number, 2) != DCN_NULL)
		{
			// 有小对子过给下家.
			return MakeBomb(A);
		}

		// 没有小对子给下家. 即便是这样, 地主也依然可能有小对子
		if (AI_Determ_Double_Can_Escape(RemainDoubleNumber, 2.0f))
		{
			return MakeBomb(A);
		}
	}

	// 到达这里的时候, 我们的炸弹打出去, 可能收不回来. 但是, 依然进行协助. 

	A = SelectBomb(DCN_NULL);
	if (A)
	{
		float EscapeFactor = 0.4f;
		if (AnyLessThen(RemainDoubleNumber, A.Number, 2) != DCN_NULL)
		{
			EscapeFactor = 1.0f;
		}

		if (AI_Determ_Double_Can_Escape(RemainDoubleNumber, EscapeFactor))
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::TryHelpEscape_Remain1_With2(const DHand* ToFollow, const DDeckCharacter* Friend, bool& Continue) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A;

	DCardNumber Number = Peek2_Force(FollowNumber, false);
	if (Number == DCN_NULL )
	{
		// 完全无法用对子打.
		return DGameAIHand::PASS;
	}
	if (mDeck->AI_Determ_Double_Is_Max(mCardSlots, Number))
	{
		int Rate = 0;
		if (AnyLessThen(FriendRemainCard, Number) || ((Number < FriendRemainCard) && mCardSlots[Number] >= 3))
		{
			Rate = 100;
		}
		else
		{
			// 我们不能确保传给子给下家. 但是地主依然可以传. 
			if (FriendRemainCard >= MaxCard)
			{
				// 下家余牌很大. 
				Rate = 90;				// 9成几率炸掉
			}
			else if (FriendRemainCard >= (MaxCard - 1))
			{
				Rate = 60;
			}
		}

		if (Rate && Chance(Rate))
		{
			return DGameAIHand(DHT_2, Number, 2);
		}
	}
	
	return DGameAIHand::PASS;
}

// 协助下家逃跑. 
DGameAIHand DAIPostLord::TryHelpEscape_Remain2_2(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 选择一个较大的对子.
	DCardNumber DoubleNum = FollowNumber;
	DAtom A;
	while (A = Select_Ascend(m2Atoms, DoubleNum))
	{
		if (mDeck->AI_Determ_Double_Is_Max(mCardSlots, A.Number))
		{
			break;
		}
		DoubleNum = A.Number;
	}

	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}

	A = SelectBomb(DCN_NULL);

	if (A)
	{
		return MakeBomb(A);
	}

	return DGameAIHand::PASS;
}

DGameAIHand  DAIPostLord::TryPrevent_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Lord = GetLord();
	if (HandOwner != Lord)
	{
		return DGameAIHand::PASS;
	}
	
	const int LordNumRemain = Lord->GetNumCards();


	if (LordNumRemain == 1)
	{
		// 地主剩余1张. 
		if (HandOwner == Lord)
		{
			// 默认情况下, 我们丢给下家去做防地主的事情. 但是, 有可能下家无法防住地主.  

		}

		return DGameAIHand::PASS;
	}
	return DGameAIHand::PASS;
}

#pragma endregion "对子"

// 尝试协助下家逃跑, 所跟牌为非单,非对的. 
DGameAIHand DAIPostLord::TryHelpEscapeCommon(const DHand* ToFollow, const DDeckCharacter* HandOwner, int& NextAction) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// 友方的牌大于2张的时候, 我们不予考虑. 
		return DGameAIHand::PASS;
	}

	DGameAIHand TestHand;
	DAtom A;
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const int BombNum = GetBombNum();

	if (FriendRemainCardNum == 1)
	{
		if (Friend == HandOwner)
		{
			// 下家可以跑. 

			// 如果有炸弹, 尝试炸掉后让友方跑.
			if (BombNum && (TestHand = TryLetEscape1AfterBomb_1(FollowNumber, Friend)))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
		else
		{
			// 下家无论如何跑不掉. 
			// 跟地主, 尝试帮组下家胜出
			if (BombNum && (TestHand = TryHelpEscape_Remain1_WithBomb(ToFollow, Friend)))
			{
				return TestHand;
			}

			// 无法确保用炸弹后保证下家胜出. 需要帮助
			NextAction = CallHelp;
			return DGameAIHand::PASS;
		}
	}
	else
	{
		// 友方剩余两张.
		const DCard& C0 = Friend->GetCard(0);
		const DCard& C1 = Friend->GetCard(1);

		DCardNumber FriendRemainDoubleNumber = DCN_NULL;
		bool FriendIs2K = false;
		if (C0.Number == C1.Number && C0.Number > FollowNumber)
		{
			FriendRemainDoubleNumber = C0.Number;
		}
		else if (C0.IsKing() && C1.IsKing())
		{
			FriendIs2K = true;
		}

		if (FriendIs2K)
		{
			// Bomb Bomb Bomb!!!
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}

		if (Friend == HandOwner)
		{
			// 跟的是友方. 且友方剩余不是对子. 
			if (FriendRemainDoubleNumber == DCN_NULL)
			{
				// 让其出牌.  并且不要再继续后续行为. 
				NextAction = NeedPassOneHand;
				return DGameAIHand::PASS;
			}
			else
			{
				// 友方是对子. 
				// 尝试炸掉后让其逃跑
				if (TestHand = TryLetEscape2AfterBomb_2(ToFollow, Friend, FriendRemainDoubleNumber))
				{
					return TestHand;
				}

			}
			NextAction = NeedPassOneHand;
			return DGameAIHand::PASS;
		}
		else
		{
			// 下家跑不掉. 

			// 下家无法出. 尝试协助其逃跑. 
#pragma message ("TODO ")
			//if (TestHand = TryHelpEscape_Remain2_2(ToFollow, Friend))
			//{
			//	return TestHand;
			//}

			NextAction = CallHelp;
			return DGameAIHand::PASS;
		}

	}

	return DGameAIHand::PASS;
}

#pragma region "3个"
DGameAIHand DAIPostLord::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}
	int NextAction = DontCare;
	AIHand = TryHelpEscapeCommon(ToFollow, HandOwner, NextAction);

	bool Force = false;

	if (AIHand)
	{
		return AIHand;
	}
	if (NextAction == NeedPassOneHand)
	{
		return DGameAIHand::PASS;
	}else if (NextAction == CallHelp)
	{
		Force = true;
	}

	


	//////////////////////////////////////////////////////////////////////////
	// 常规出牌
	//////////////////////////////////////////////////////////////////////////
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner == Friend)
	{
		return DGameAIHand::PASS;
	}else
	{
		DDeckCharacter* Lord = GetLord();
		int LordRemain = Lord->GetNumCards();
		if (LordRemain <= 3)
		{
			Force = true;
		}

		const DCardNumber FollowNumber = ToFollow->GetHandNumber();
		DAtom A;
		if (Force)
		{
			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}

			A = Select_Descend(m3Atoms, FollowNumber);
			if (A)
			{
				return DGameAIHand(DHT_3, A.Number, 3);
			}

			A = Peek3_From3S(FollowNumber, false);
			if (A)
			{
				return DGameAIHand(DHT_3, A.Number, 3);
			}
		}
		else
		{
			A = Select_Ascend(m3Atoms, FollowNumber);
			if (A)
			{
				return DGameAIHand(DHT_3, A.Number, 3);
			}

			A = Peek3_From3S(FollowNumber, true);
			if (A)
			{
				return DGameAIHand(DHT_3, A.Number, 3);
			}

			A = SelectBomb(DCN_NULL);
			if (A)
			{
				return MakeBomb(A);
			}
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}
	int NextAction = DontCare;
	AIHand = TryHelpEscapeCommon(ToFollow, HandOwner, NextAction);

	bool Force = false;

	if (AIHand)
	{
		return AIHand;
	}
	if (NextAction == NeedPassOneHand)
	{
		return DGameAIHand::PASS;
	}
	else if (NextAction == CallHelp)
	{
		Force = true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 常规出牌
	//////////////////////////////////////////////////////////////////////////
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner == Friend)
	{
		// 对于友方的牌, 不跟
		return DGameAIHand::PASS;
	}

	// 跟地主
	DDeckCharacter* Lord = GetLord();
	int LordRemain = Lord->GetNumCards();
	if (LordRemain <= 3)
	{
		Force = true;
	}
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	if (Force)
	{
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}


		AIHand = Peek_3_1_Descend(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		A = Peek3_From3S(FollowNumber, false);
		if (A)
		{
			DCardNumber X1 = FH_Peek_X1_For_3_1(true);
			if (X1 != DCN_NULL)
			{
				return DGameAIHand(DHT_3_1, A.Number, 4, DAIXInfo(X1, 1));
			}
		}
	}
	else
	{
		AIHand = Peek_3_1(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}
	int NextAction = DontCare;
	AIHand = TryHelpEscapeCommon(ToFollow, HandOwner, NextAction);

	bool Force = false;

	if (AIHand)
	{
		return AIHand;
	}
	if (NextAction == NeedPassOneHand)
	{
		return DGameAIHand::PASS;
	}
	else if (NextAction == CallHelp)
	{
		Force = true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 常规出牌
	//////////////////////////////////////////////////////////////////////////
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner == Friend)
	{
		// 对于友方的牌, 不跟
		return DGameAIHand::PASS;
	}

	// 跟地主
	DDeckCharacter* Lord = GetLord();
	int LordRemain = Lord->GetNumCards();
	if (LordRemain <= 3)
	{
		Force = true;
	}
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	if (Force)
	{
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}


		AIHand = Peek_3_2_Descend(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		A = Peek3_From3S(FollowNumber, false);
		if (A)
		{
			DCardNumber X2 = FH_Peek_X2_For_3_2();
			if (X2 != DCN_NULL)
			{
				return DGameAIHand(DHT_3_2, A.Number, 5, DAIXInfo(X2, 2));
			}
		}
	}
	else
	{
		AIHand = Peek_3_2(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}

	return DGameAIHand::PASS;
}

#pragma endregion "3个"

DGameAIHand DAIPostLord::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size();
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		DGameAIHand AIHand = Peek_Straight(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			int BombForHelp = 10, BombForPreventLord = 10;

			int FriendRemain = Friend->GetNumCards();
			if (FriendRemain <= 2)
			{
				BombForHelp = 100;
			}
			else if (FriendRemain < 5)
			{
				BombForHelp = 40;
			}


			int LordRemain = GetLord()->GetNumCards();
			if (LordRemain <= 3)
			{
				BombForPreventLord = 100;
			}
			else if (LordRemain <= 6)
			{
				BombForPreventLord = 30;
			}

			// 炸弹的大小也可以作为一个权重进行考虑

			int BombRate = (BombForPreventLord + BombForHelp) / 2;
			BombRate = VClamp(BombRate, 0, 100);

			if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
	}

	return DGameAIHand::PASS;
}
DGameAIHand DAIPostLord::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size() / 2;
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		DGameAIHand AIHand = Peek_Straight2(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			int BombForHelp = 10, BombForPreventLord = 10;

			int FriendRemain = Friend->GetNumCards();
			if (FriendRemain <= 2)
			{
				BombForHelp = 100;
			}
			else if (FriendRemain < 5)
			{
				BombForHelp = 40;
			}


			int LordRemain = GetLord()->GetNumCards();
			if (LordRemain <= 3)
			{
				BombForPreventLord = 100;
			}
			else if (LordRemain <= 6)
			{
				BombForPreventLord = 30;
			}

			// 炸弹的大小也可以作为一个权重进行考虑

			int BombRate = (BombForPreventLord + BombForHelp) / 2;
			BombRate = VClamp(BombRate, 0, 100);

			if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const int NumX = ToFollow->GetStraightLen();
	VASSERT(NumX >= 2);

	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		DCardNumber FollowNumber = ToFollow->GetHandNumber();
		const int XPartSize = (ToFollow->Size() - NumX * 3) / NumX;
		

		DGameAIHand AIHand = Peek_Straight3(FollowNumber, NumX);
		if (AIHand)
		{
			// 找到主体. 
			if (XPartSize == 2)
			{
				if (Peek_3_straight_2(AIHand.X, NumX))
				{
					return AIHand;
				}
				if (Peek_3_straight_2_s(AIHand.X, NumX))
				{
					return AIHand;
				}
			}
			else if (XPartSize)
			{
				if (Peek_3_straight_1(AIHand.X, NumX))
				{
					return AIHand;
				}
				if (Peek_3_straight_1_s(AIHand.X, NumX))
				{
					return AIHand;
				}
			}
			else
			{
				return AIHand;
			}
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			int BombForHelp = 10, BombForPreventLord = 10;

			int FriendRemain = Friend->GetNumCards();
			if (FriendRemain <= 2)
			{
				BombForHelp = 100;
			}
			else if (FriendRemain < 5)
			{
				BombForHelp = 40;
			}


			int LordRemain = GetLord()->GetNumCards();
			if (LordRemain <= 3)
			{
				BombForPreventLord = 100;
			}
			else if (LordRemain <= 6)
			{
				BombForPreventLord = 30;
			}

			// 炸弹的大小也可以作为一个权重进行考虑

			int BombRate = (BombForPreventLord + BombForHelp) / 2;
			BombRate = VClamp(BombRate, 0, 100);

			BombRate *= 2;

			if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		DAtom A = SelectBomb(FollowNumber);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DAIPostLord::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		DAtom A = SelectBomb(FollowNumber);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

#if 0 
DHand* DAIPostLord::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Friend = GetFriend();
	BOOL HandOwnerIsFriend = IsFriend(HandOwner);
	if (HandOwnerIsFriend)
	{
		return GetHand_ToFollow_1_Friend(ToFollow, HandOwner);
	}
	else
	{
		return GetHand_ToFollow_1_Lord(ToFollow, HandOwner);
	}
#if 0
	DCardNumber Number = ToFollow->GetHandNumber();

	if (Friend->GetNumCards() == 1)
	{
		if (HandOwnerIsFriend)
		{
			// 地主要不起
			// 不出. TODO: 这里还可以考虑下, BOMB BOMB 然后出个单个. 
			return NULL;
		}
		else
		{
			// 被地主打了. 
			// 尝试过牌给下家. 
			
			// CHEAT ..... 
			const DCard& FriendRemain = Friend->GetCard(0);

			
		}
	}

	// 
	if (HandOwnerIsFriend)
	{
		// 如果是友方打出的大于K的牌, 不接. 
		

		if (Number >= DCN_A)
		{
			return NULL;
		}
	}

	return DAIFarmer::GetHand_ToFollow_1(ToFollow, HandOwner);
#endif 
}

#endif 
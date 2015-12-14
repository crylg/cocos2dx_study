#include "VStdAfx.h"
#include "DAIPreLord.h"
//解决应对单张牌的时候, 地主报价留A时候, 地主上家居然有2不出. 

DAIPreLord::DAIPreLord(DDeckCharacter* Owner) :DAIFarmer(Owner)
{
}


DAIPreLord::~DAIPreLord()
{
}

bool DAIPreLord::NeedBombLord(DDeckCharacter* Lord, float BombRateFactor) const
{
	int RemainCardNums = Lord->GetNumCards();
	int BombRate = 0;
	if (RemainCardNums <= 6)
	{
		BombRate = (6 - RemainCardNums) * 20 + 20;
	}
	else
	{
		BombRate = (20 - RemainCardNums) * 3;
	}

	//const DDeckAIHint& LordAIHint = Lord->AI_GetHint();

	if (Chance(BombRate))
	{
		return true;
	}
	return false;
}

bool DAIPreLord::CanWin() const
{
	// 判断自己是否能赢??? 
	int NumHands = GetHandNum();
	int Num2 = GetNum2();
	int NumKing = GetNumKing();
	int NumBombers = m4Atoms.size();

	int MyValue = Num2 * 3 + NumKing * 5 + NumBombers * 1;
	
	return false;
}

DGameAIHand DAIPreLord::PlayHand() const
{
	// 计算我的牌面价值, 如果可以胜利的话, 就打小牌, 否则打大牌, 避免地主胜利. 
	DDeckCharacter* Lord = mDeck->GetPlayer(DDCT_LORD);
	const DDeckAIHint& LordHint = Lord->AI_GetHint();
	DGameAIHand AIHand = Play_Hand_To_Escape(LordHint);
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = Play_GuardLord();
	if (AIHand)
	{
		return AIHand;
	}

	// 支援友方
	if (AIHand = Play_AssistFriend())
	{
		return AIHand;
	}


	int NumHands = GetHandNum();
	if (NumHands <= 2)
	{
		
	}
	

	return Play_Normal();
}

DGameAIHand DAIPreLord::Play_GuardLord() const
{
	DDeckCharacter* Lord = mDeck->GetPlayer(DDCT_LORD);
	int LordRemainCards = Lord->GetNumCards();
	if (LordRemainCards == 1)
	{
		const DCardNumber RemainCardNumber = Lord->GetCard(0).Number;
		return PlayHand_Min_NoSingle(RemainCardNumber);
	}
	else if (LordRemainCards == 2)
	{
		//const DDeckAIHint& AIHint = Lord->AI_GetHint();
		const DCard& C0 = Lord->GetCard(0);
		const DCard& C1 = Lord->GetCard(1);

		if (C0.Number == C1.Number)
		{
			return PlayHand_GuardDouble(C0.Number);
		}
		else
		{
			DCardNumber CardNumber = VMin(C0.Number, C1.Number);
			return PlayHand_Min_GuardSingle(CardNumber);
		}
	}
	// TODO  避免3个, 顺子.

	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::Play_AssistFriend() const
{
	//DDeckCharacter* Friend = GetFriend();
	//int FriendRemainNumber = Friend->GetNumCards();
	//DDeckCharacter* Lord = mDeck->GetPlayer(DDCT_LORD);
	//int LordRemainCards = Lord->GetNumCards();

	//if (FriendRemainNumber == 1)
	//{
	//	// 


	//}else if (FriendRemainNumber == 2)
	//{
	//}



	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::Play_Normal() const
{
	DGameAIHand AIHand;
	DGameAIHand BestHand;
	int BestNumber = DCN_RED_JOKER + 1;
	AIHand = Peek_3_x();
	if (AIHand)
	{
		// 3x1 的评分: 
		// 如果3X1 数量较多. 加分. ??? 

		int HandNumber = AI_Calc_3XValue(AIHand);
		
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}

	AIHand = Peek_3_straight();
	if (AIHand)
	{
		int HandNumber = AIHand.Number - 2;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}
	

	// 单个
	int Num3 = GetNum3();
	int TailIndex = Num3;
	DAtom A;
	if (m1Atoms.size() > TailIndex)
	{
		A = m1Atoms[m1Atoms.size() - TailIndex - 1];
	}
	else if (!m1Atoms.empty())
	{
		A = m1Atoms[0];
	}
	if (A)
	{
		int HandNumber = A.Number;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	// 对子
	A.Number = DCN_NULL;
	A.Len = 0;
	if (m2Atoms.size() > TailIndex)
	{
		A = m2Atoms[m2Atoms.size() - TailIndex - 1];
	}
	else if (!m2Atoms.empty())
	{
		A = m2Atoms[0];
	}
	if (A)
	{
		int HandNumber = A.Number;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = DGameAIHand(DHT_2, A.Number, 2);
		}
	}

	// 顺子
	AIHand = GetStraight();
	if (AIHand)
	{
		int HandNumber = AIHand.Number;
		HandNumber -= 2;

		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}
	// 双顺
	AIHand = GetStraight2();
	if (AIHand)
	{
		int HandNumber = AIHand.Number;
		HandNumber -= 2;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}

	if (BestHand)
	{
		return BestHand;
	}


	// 没有找到期望的牌型. 
	A = SelectBomb(DCN_NULL);
	VASSERT(A);
	return MakeBomb(A);
}

//DHand* DAIPreLord::PlayHand()
//{
//	// 不要让地主逃跑.
//	DDeckCharacter* Lord = mDeck->GetPlayer(DDCT_LORD);
//	if (Lord->GetNumCards() == 1)
//	{
//		const DCardNumber RemainCardNumber = Lord->GetCard(0).Number;
//
//		return PlayHand_Min_NoSingle(RemainCardNumber);
//	}
//
//	// 能否让友方逃跑? 
//
//
//	// 正常出牌.
//	//return GetHand_Min();
//	return nullptr;
//}

DGameAIHand DAIPreLord::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	// 避免地主逃跑
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	bool Continue = true;
	AIHand = GuardLord_1(ToFollow, HandOwner, Continue);
	if (AIHand)
	{
		return AIHand;
	}

	if (!Continue)
	{
		return DGameAIHand::PASS;
	}

	// 正常跟牌. 尝试将牌抬高 . 
	AIHand = TryPrevent1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	DDeckCharacter* Lord = GetLord();
	if (HandOwner == Lord)
	{
		// 抬牌阶段如果打不起. 那么这里只有炸弹了. 

		DAtom A = Select_Ascend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}


		A = SelectBomb(DCN_NULL);
		if (A)
		{
			
			if (NeedBombLord(Lord))
			{
				return MakeBomb(A);
			}
		}
	}
	else
	{
		if (FollowNumber >= DCN_A)
		{
			return DGameAIHand::PASS;
		}

		

		DAtom A = Select_Ascend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1,  A.Number, 1);
		}
	}


	return DGameAIHand::PASS;
}

/**
 * 跟个子的时候, 避免地主逃跑.  
 */
DGameAIHand DAIPreLord::GuardLord_1(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const
{
	Continue = true;
	DDeckCharacter* Lord = GetLord();
	int NumRemainCards = Lord->GetNumCards();
	if (NumRemainCards > 2)
	{
		return DGameAIHand::PASS;
	}
	DGameAIHand AIHand;
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber OpposeNumber = DCN_NULL;
	if (HandOwner != Lord)
	{
		// 跟的是友方的牌. 
		if (NumRemainCards == 1)
		{
			// 地主剩余单张
			const DCardNumber RemainNumber = Lord->GetCard(0).Number;
			if (RemainNumber <= FollowNumber)
			{
				// 地主根本打不起. 
				Continue = false;
				return DGameAIHand::PASS;
			}
			else
			{
				// 地主打得起. 
				AIHand = TryBreak_1_1(FollowNumber, RemainNumber);
				if (AIHand)
				{
					return AIHand;
				}
			}
		}
		else if (NumRemainCards == 2)
		{
			// 地主剩余2张.
			const DCard& C0 = Lord->GetCard(0);
			const DCard& C1 = Lord->GetCard(1);
			if (C0.Number == C1.Number && C0.Number > FollowNumber)
			{
				// 地主剩余的是对子. 
				if (C0.Number <= FollowNumber)
				{
					// 地主剩余对子打不起. 
					Continue = false;
					return DGameAIHand::PASS;
				}
				else
				{
					AIHand = TryBreak_1_1(FollowNumber, C0.Number);
					if (AIHand)
					{
						return AIHand;
					}
				}
			}
			else
			{
				// 剩余2个个子的时候, 不能让地主的大牌跑掉. 
				DCardNumber RemainMax = VMax(C0.Number, C1.Number);
				if (RemainMax > FollowNumber)
				{
					AIHand = TryBreak_1_1(FollowNumber, RemainMax);
					if (AIHand)
					{
						return AIHand;
					}
				}
			}
		}
	}
	else
	{
		// 跟的是地主

		if (NumRemainCards == 1)
		{
			const DCardNumber RemainNumber = Lord->GetCard(0).Number;
			AIHand = TryBreak_1_1(FollowNumber, RemainNumber);
			if (AIHand)
			{
				return AIHand;
			}
		}
		else if (NumRemainCards == 2)
		{
			const DCard& C0 = Lord->GetCard(0);
			const DCard& C1 = Lord->GetCard(1);
			if (C0.Number == C1.Number && C0.Number > FollowNumber)
			{
				AIHand = TryBreak_1_1(FollowNumber, C0.Number);
				if (AIHand)
				{
					return AIHand;
				}
			}
			else
			{
				// 地主剩余2个个子. 
				DCardNumber RemainMax = VMax(C0.Number, C1.Number);
				if (RemainMax > FollowNumber)
				{
					AIHand = TryBreak_1_1(FollowNumber, RemainMax);
					if (AIHand)
					{
						return AIHand;
					}
				}
			}
		}
	}


	return DGameAIHand::PASS;

}


DGameAIHand DAIPreLord::TryPrevent1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Lord = GetLord();
	const DDeckAIHint& LordHint = Lord->AI_GetHint();
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int RemainNumber = Lord->GetNumCards();
	if (RemainNumber == 1)
	{
		const DCardNumber Number = Lord->GetCard(0).Number;

	}else if (RemainNumber == 2)
	{
	}


	if (HandOwner != Lord)
	{
		// 跟的是友方的牌
		if (FollowNumber >= DCN_A)
		{
			return DGameAIHand::PASS;
		}

		int NumGreater = LordHint.AI_Determ_Single_Max(FollowNumber, false);
		if (NumGreater == 0)
		{
			return DGameAIHand::PASS;
		}
	}
	
	//bool CanFollowByLord = mDeck->AI_Determ_Single_CanBeFollowByLord(Lord->AI_GetHint().Cards, FollowNumber);
	//if (!CanFollowByLord)
	//{
	//}

	//int NumGreater = mDeck->AI_Determ_Single_Max()

	//if (HandOwner != Lord && )
	{
	}

	
	// 先尝试从个子中选择一个牌来打. 
	// 
	DCardNumber From1 = FollowNumber;
	DAtom A;
	while (1)
	{
		A = Select_Ascend(m1Atoms, From1);
		if (!A)
		{
			break;
		}
		if (A.Number > DCN_2)
		{
			break;
		}

		From1 = A.Number;
		if (From1 >= DCN_K)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	From1 = FollowNumber;
	while (1)
	{
		A = Peek1_FromStraight(From1);
		if (!A)
		{
			break;
		}

		From1 = A.Number;
		if (From1 >= DCN_K)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}
	From1 = FollowNumber;
	while (1)
	{
		A = Peek1_FromDouble(From1);
		if (!A)
		{
			break;
		}

		From1 = A.Number;
		if (From1 >= DCN_K)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}
	
	// 可能有3个2 组成一块了. 
	if (mCardSlots[DCN_2] && DCN_2 > FollowNumber)
	{
		return DGameAIHand(DHT_1, DCN_2, 1);
	}

	if (mCardSlots[DCN_A] && DCN_A > FollowNumber)
	{
		return DGameAIHand(DHT_1, DCN_A, 1);
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	// 避免地主逃跑
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	bool Continue = true;
	AIHand = GuardLord_2(ToFollow, HandOwner, Continue);
	if (AIHand)
	{
		return AIHand;
	}

	if (!Continue)
	{
		return DGameAIHand::PASS;
	}

	// 常规出牌
	bool AllowBomb = false;
	DDeckCharacter* Lord = GetLord();
	if (HandOwner == Lord)
	{
		
		AIHand = Get2_FroLord(FollowNumber, Lord->AI_GetHint());

		if (AIHand)
		{
			return AIHand;
		}

		AllowBomb = true;
	}
	else
	{
		// 跟友方牌, 如果觉得地主可能跟得起牌的话. 
		const DDeckAIHint& LordAIHint = Lord->AI_GetHint();
		if (LordAIHint.CanFollow2(FollowNumber))
		{
			AIHand = Get2_FroLord(FollowNumber, Lord->AI_GetHint());
			if (AIHand)
			{
				return AIHand;
			}
		}
		else
		{
			return DGameAIHand::PASS;
		}

		AllowBomb = false;


	}


	// fall back 能出就出. 
	DAtom A = Select_Ascend(m2Atoms, FollowNumber);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}

	A = Peek2_FromDoubleStaight(FollowNumber);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}
	if (AllowBomb)
	{
		DAtom A = SelectBomb(DCN_NULL);
		if (A && NeedBombLord(Lord))
		{
			return MakeBomb(A);
		}
	}
	


	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GuardLord_2(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const
{
	Continue = true;
	DDeckCharacter* Lord = GetLord();
	int NumRemainCards = Lord->GetNumCards();
	if (NumRemainCards > 2)
	{
		return DGameAIHand::PASS;
	}
	DGameAIHand AIHand;
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber OpposeNumber = DCN_NULL;
	if (HandOwner != Lord)
	{
		// 跟的是友方的牌. 
		if (NumRemainCards == 1)
		{
			// 地主剩余单张
			Continue = false;
			return DGameAIHand::PASS;
		}
		else if (NumRemainCards == 2)
		{
			const DCard& C0 = Lord->GetCard(0);
			const DCard& C1 = Lord->GetCard(1);
			if (C0.Number == C1.Number && C0.Number > FollowNumber)
			{
				// 地主能够打得起对子. 
				AIHand = TryBreak_2(FollowNumber, C0.Number);
				if (AIHand)
				{
					return AIHand;
				}
			}
			else
			{
				// 没办法
				Continue = false;
				return DGameAIHand::PASS;
			}
		}
	}
	else
	{
		if (NumRemainCards == 1)
		{
			AIHand = TryBreak_2(FollowNumber, DCN_NULL);
			if (AIHand)
			{
				return AIHand;
			}
		}
		else if (NumRemainCards == 2)
		{
			const DCard& C0 = Lord->GetCard(0);
			const DCard& C1 = Lord->GetCard(1);
			if (C0.Number == C1.Number && C0.Number > FollowNumber)
			{
				AIHand = TryBreak_2(FollowNumber, C0.Number);
				if (AIHand)
				{
					return AIHand;
				}

				// 没办法
				Continue = false;
				return DGameAIHand::PASS;
			}
		}
	}

	
	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::Get2_FroLord(DCardNumber FollowNumber, const DDeckAIHint& LordHint) const
{
	DCardNumber TestNumber = FollowNumber;
	DAtom A;
	// 首先从对子中选择一个最好能让地主打不起的牌. 
	while (1)
	{
		A = Select_Ascend(m2Atoms, TestNumber);
		if (A)
		{
			if (!LordHint.CanFollow2(A.Number))
			{
				return DGameAIHand(DHT_2, A.Number, 2);
			}
			TestNumber = A.Number;
		}
		else
		{
			break;
		}
	}
	// 选择双顺中的牌. 
	TestNumber = FollowNumber;
	while (1)
	{
		A = Peek2_FromDoubleStaight(TestNumber);
		if (A)
		{
			if (!LordHint.CanFollow2(A.Number))
			{
				return DGameAIHand(DHT_2, A.Number, 2);
			}
			TestNumber = A.Number;
		}
		else
		{
			break;
		}
	}




	return DGameAIHand::PASS;
}



DGameAIHand DAIPreLord::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	// 避免地主逃跑
	DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 常规出牌
	bool AllowBomb = false;
	DDeckCharacter* Lord = GetLord();
	if (HandOwner == Lord)
	{

		DAtom A = Select_Ascend(m3Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_3, A.Number, 3);
		}

		

		A = SelectBomb(DCN_NULL);
		if (A && A.Number < DCN_2)
		{
			return MakeBomb(A);
		}

		A = Peek3_From3S(FollowNumber, true);
		if (A)
		{
			return DGameAIHand(DHT_3, A.Number, 3);
		}
		A = SelectBomb(DCN_NULL);
		if (A )
		{
			return MakeBomb(A);
		}

	}
	else
	{
		

	}


	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GuardLord_3(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const
{
	//Continue = true;
	//DDeckCharacter* Lord = GetLord();
	//int NumRemainCards = Lord->GetNumCards();
	//if (NumRemainCards > 2)
	//{
	//	return DGameAIHand::PASS;
	//}
	//DGameAIHand AIHand;
	//DCardNumber FollowNumber = ToFollow->GetHandNumber();
	//DCardNumber OpposeNumber = DCN_NULL;
	//if (HandOwner != Lord)
	//{
	//	// 跟的是友方的牌. 
	//	if (NumRemainCards == 1)
	//	{
	//		// 地主剩余单张
	//		Continue = false;
	//		return DGameAIHand::PASS;
	//	}
	//	else if (NumRemainCards == 2)
	//	{
	//		const DCard& C0 = Lord->GetCard(0);
	//		const DCard& C1 = Lord->GetCard(1);
	//		if (C0.Number == C1.Number && C0.Number > FollowNumber)
	//		{
	//			// 地主能够打得起对子. 
	//			AIHand = TryBreak_2(FollowNumber, C0.Number);
	//			if (AIHand)
	//			{
	//				return AIHand;
	//			}
	//		}
	//		else
	//		{
	//			// 没办法
	//			Continue = false;
	//			return DGameAIHand::PASS;
	//		}
	//	}
	//}
	//else
	//{
	//	if (NumRemainCards == 1)
	//	{
	//		AIHand = TryBreak_2(FollowNumber, DCN_NULL);
	//		if (AIHand)
	//		{
	//			return AIHand;
	//		}
	//	}
	//	else if (NumRemainCards == 2)
	//	{
	//		const DCard& C0 = Lord->GetCard(0);
	//		const DCard& C1 = Lord->GetCard(1);
	//		if (C0.Number == C1.Number && C0.Number > FollowNumber)
	//		{
	//			AIHand = TryBreak_2(FollowNumber, C0.Number);
	//			if (AIHand)
	//			{
	//				return AIHand;
	//			}

	//			// 没办法
	//			Continue = false;
	//			return DGameAIHand::PASS;
	//		}
	//	}
	//}


	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 常规出牌
	DDeckCharacter* Lord = GetLord();
	if (HandOwner == Lord)
	{
		AIHand = Peek_3_1(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A && A.Number < DCN_2)
		{
			return MakeBomb(A);
		}

		A = Peek3_From3S(FollowNumber, true);
		if (A)
		{
			DCardNumber X = FH_Peek_X1_For_3_1(false);
			if (X != DCN_NULL)
			{
				return DGameAIHand(DHT_3_1, A.Number, 4, DAIXInfo(X, 1));
			}
		}
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	
	return DGameAIHand::PASS;
}
DGameAIHand DAIPreLord::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 常规出牌
	DDeckCharacter* Lord = GetLord();
	if (HandOwner == Lord)
	{
		AIHand = Peek_3_2(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A && A.Number < DCN_2)
		{
			return MakeBomb(A);
		}

		A = Peek3_From3S(FollowNumber, true);
		if (A)
		{
			DCardNumber X = FH_Peek_X2_For_3_2();
			if (X != DCN_NULL)
			{
				return DGameAIHand(DHT_3_2, A.Number, 5, DAIXInfo(X, 2));
			}
		}
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Lord = GetLord();
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size();
	if (HandOwner == Lord)
	{
		DGameAIHand AIHand = Peek_Straight(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Lord = GetLord();
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size() / 2;
	if (HandOwner == Lord)
	{
		DGameAIHand AIHand = Peek_Straight2(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Lord = GetLord();
	
	int Len = ToFollow->Size();
	if (HandOwner != Lord)
	{
		return DGameAIHand::PASS;
	}
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const int NumX = ToFollow->GetStraightLen();
	VASSERT(NumX >= 2);
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
		DDeckCharacter* Friend = GetFriend();

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



	return DGameAIHand::PASS;
}

DGameAIHand DAIPreLord::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
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

DGameAIHand DAIPreLord::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
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


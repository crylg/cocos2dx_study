#include "VStdAfx.h"
#include "DAILord.h"


DAILord::DAILord(DDeckCharacter* Owner) :DNormalAI(Owner)
{
}


DAILord::~DAILord()
{
}

DGameAIHand DAILord::PlayHand() const
{
	VASSERT(IsLord());
	DGameAIHand AIHand = Play_Hand_To_Escape();
	if (AIHand)
	{
		return AIHand;
	}

	DCardNumber GuardSingle = GetGuardSingle();
	if (GuardSingle != DCN_NULL)
	{
		return PlayHand_Min_NoSingle(GuardSingle);
	}
	
	return Play_Normal();
}

DGameAIHand DAILord::Play_Hand_To_Escape() const
{
	int NumHands = AI_QueryNumHands();
	if (m2K)
	{
		NumHands--;
	}

	if (NumHands > 2)
	{
		// 剩余手数不在考虑范围之内. 
		return DGameAIHand::PASS;
	}
	if (NumHands == 1)
	{
		return Play_Normal();
	}


	int NumRemainBombs = mDeck->AI_QueryRemainBombers(mCardSlots, DCN_NULL);

	//if (NumRemainBombs < 2)
	
		// 视为外面没有炸弹. 
		// 优先出3带.
		if (m3Atoms.size())
		{
			const DAtom& A = m3Atoms[0];
			if (mDeck->AI_Determ_3_Is_Max(mCardSlots, A.Number))
			{
				DGameAIHand AIHand = Peek_3_x();
				if (AIHand)
				{
					return AIHand;
				}
			}
		}

		if (m1SAtoms.size())
		{
			const DAtom& A = m1SAtoms[0];
			int Greater = mDeck->AI_Determ_S_Greater(mCardSlots, A.Number, A.Len);
			if (Greater < 2)
			{
				return DGameAIHand(DHT_straight, A.Number, A.Len);
			}
		}

		if (m2SAtoms.size())
		{
			const DAtom& A = m2SAtoms[0];
			int Greater = mDeck->AI_Determ_2S_Greater(mCardSlots, A.Number, A.Len);
			if (Greater <= 2)
			{
				return DGameAIHand(DHT_straight_2, A.Number, A.Len);
			}
		}

		if (m3SAtoms.size())
		{
			const DAtom& A = m3SAtoms[0];
			int Greater = mDeck->AI_Determ_3S_Greater(mCardSlots,A.Number, A.Len);
			if (Greater <= 2)
			{
				return Peek_3_straight();
			}
		}


		if (m4Atoms.size())
		{
			const DAtom& A = m4Atoms[0];
			int Greater = mDeck->AI_QueryRemainBombers(mCardSlots, A.Number);
			if (Greater <= 1)
			{
				return DGameAIHand(DHT_4, A.Number, 4);
			}
		}

		if (m2Atoms.size())
		{
			const DAtom& A = m2Atoms[0];
			int Greater = mDeck->AI_Determ_Double_Greater(mCardSlots, A.Number);
			if (Greater <= 2)
			{
				return DGameAIHand(DHT_2, A.Number, 2);
			}
		}
	

	// 尝试看看是否有一手是最大的, 如果是, 打出最大手. 
	for (int i = 0; i < m1Atoms.size(); ++i)
	{
		const DAtom& A = m1Atoms[i];
		int Greater = mDeck->AI_Determ_Single_Max(mCardSlots, A.Number, false);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	return DGameAIHand::PASS;
}


DGameAIHand DAILord::Play_Normal() const
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

BOOL DAILord::IsAnyOpponentHasOnlyOne() const
{
	DDeckCharacter* PreLord = mDeck->GetPlayer(DDCT_PRE_LORD);
	DDeckCharacter* PostLord = mDeck->GetPlayer(DDCT_POST_LORD);
	if (PreLord->GetNumCards() == 1 || PostLord->GetNumCards() == 1)
	{
		return TRUE;
	}
	return FALSE;
}

DCardNumber DAILord::GetGuardSingle() const
{
	DDeckCharacter* PreLord = mDeck->GetPlayer(DDCT_PRE_LORD);
	DDeckCharacter* PostLord = mDeck->GetPlayer(DDCT_POST_LORD);
	DCardNumber GuardSingle = DCN_NULL;
	if (PreLord == nullptr || PostLord == nullptr)
	{
		return GuardSingle;
	}
	if (PreLord->GetNumCards() == 1)
	{
		GuardSingle = PreLord->GetCard(0).Number;
	}

	if (PostLord->GetNumCards() == 1)
	{
		GuardSingle = VMax(PostLord->GetCard(0).Number, GuardSingle);
	}

	return GuardSingle;
}

BOOL DAILord::IsAnyOpponentWarning() const
{
	DDeckCharacter* PreLord = mDeck->GetPlayer(DDCT_PRE_LORD);
	DDeckCharacter* PostLord = mDeck->GetPlayer(DDCT_POST_LORD);
	if (PreLord->GetNumCards() <= 2 || PostLord->GetNumCards() <= 2)
	{
		return TRUE;
	}

	return FALSE;
}


DDeckAIHint* DAILord::GetGuardOpponent(const DDeckAIHint& PrevLord, const DDeckAIHint& PostLord) const
{
	int RemainHands = PostLord.AI_QueryNumHands();
	if (RemainHands <= 2)
	{
	}

	return nullptr;
}


DGameAIHand DAILord::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	// 尝试自己逃跑. 
	DGameAIHand AIHand = TryEscape(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	
	//
	AIHand = GuardOpponent_1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A = Select_Ascend(m1Atoms, FollowNumber);
	if (A)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}

	// 从个子中选不出来.
	// 拆,A,2.
	if (mCardSlots[DCN_2] && DCN_2 > FollowNumber)
	{
		return  DGameAIHand(DHT_1, DCN_2, 1);
	}

	if (mCardSlots[DCN_A] && DCN_A > FollowNumber)
	{
		return  DGameAIHand(DHT_1, DCN_A, 1);
	}

	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}


	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GuardOpponent_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	if (!IsAnyOpponentWarning())
	{
		return DGameAIHand::PASS;
	}
	// 有一家只剩余1张牌, 需要压死. 
	DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 如果有最大的个子,用最大的个子压死. 
	DCardNumber OpponentMaxSingle = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A;
	DCardNumber TestNumber = FollowNumber;
	while (1)
	{
		A = Select_Ascend(m1Atoms, TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= OpponentMaxSingle)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
		TestNumber = A.Number;
	}

	// 未能找到一个合适的个子. 
	// 如果对手剩余的最大牌是2. 而且我们可以用2干掉??
	if (OpponentMaxSingle <= DCN_2)
	{
		if (mCardSlots[DCN_2] && DCN_2 > FollowNumber)
		{
			return DGameAIHand(DHT_1, DCN_2, 1);
		}
	}

	// 需要拆牌的应对策略. 
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GuardOpponent_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	if (!IsAnyOpponentWarning())
	{
		return DGameAIHand::PASS;
	}

	// 有一家只剩余1张牌, 需要压死. 
	DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// 如果有最大的个子,用最大的个子压死. 
	DAtom A;
	DCardNumber TestNumber = FollowNumber;
	while (1)
	{
		A = Select_Ascend(m2Atoms, TestNumber);
		if (!A)
		{
			break;
		}

		if (mDeck->AI_Determ_Double_Is_Max(mCardSlots, A.Number))
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
		TestNumber = A.Number;
	}

	// 未能找到一个合适的个子. 
	// 如果对手剩余的最大牌是2. 而且我们可以用2干掉??

	if (mCardSlots[DCN_2] >= 2 && DCN_2 > FollowNumber)
	{
		return DGameAIHand(DHT_2, DCN_2, 2);
	}

	if (mCardSlots[DCN_A] >= 2 && DCN_A > FollowNumber)
	{
		return DGameAIHand(DHT_2, DCN_A, 2);
	}

	// 需要拆牌的应对策略. 
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A = Select_Ascend(m3Atoms, FollowNumber);
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
	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// 常规出牌
	//////////////////////////////////////////////////////////////////////////
	
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();

	
		AIHand = Peek_3_1(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	

	return DGameAIHand::PASS;

}

DGameAIHand DAILord::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DGameAIHand AIHand = TryEscape_3X2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}


	//////////////////////////////////////////////////////////////////////////
	// 常规出牌
	//////////////////////////////////////////////////////////////////////////

	const DCardNumber FollowNumber = ToFollow->GetHandNumber();


	AIHand = Peek_3_2(FollowNumber);
	if (AIHand)
	{
		return AIHand;
	}

	DAtom A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}


	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size();
	
		DGameAIHand AIHand = Peek_Straight(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			

			//if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
	

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	int Len = ToFollow->Size() / 2;

		DGameAIHand AIHand = Peek_Straight2(FollowNumber, Len);
		if (AIHand)
		{
			return AIHand;
		}

		DAtom A = SelectBomb(DCN_NULL);
		if (A)
		{
			
			//if (Chance(BombRate))
			{
				return MakeBomb(A);
			}
		}
	

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
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
		
			{
				return MakeBomb(A);
			}
		}
	

	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();
	
		DAtom A = SelectBomb(FollowNumber);
		if (A)
		{
			return MakeBomb(A);
		}
	
	return DGameAIHand::PASS;
}

DGameAIHand DAILord::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber FollowNumber = ToFollow->GetHandNumber();

		DAtom A = SelectBomb(FollowNumber);
		if (A)
		{
			return MakeBomb(A);
		}
	
	return DGameAIHand::PASS;
}

#if 0
DHand* DAILord::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber Number = ToFollow->GetHandNumber();
	DDeckCharacter* PostLord = mDeck->GetPlayer(DDCT_POST_LORD);
	if (PostLord->GetNumCards() == 1)
	{
		DHand* Result = FollowHand_1_Bomb(Number);
		if (Result)
		{
			return Result;
		}
	}

	return DNormalAI::FollowHand_1_Normal(Number, FALSE);
}


DHand* DAILord::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_2(ToFollow, HandOwner);
}

DHand* DAILord::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3_1(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3_2(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight_2(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight_3(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_4(ToFollow, HandOwner);
}
DHand* DAILord::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_4_2(ToFollow, HandOwner);
}
#endif 

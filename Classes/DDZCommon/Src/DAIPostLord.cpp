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
			// ��С���ӹ����¼�.
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
			// �ѷ�ʣ����������. 
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
			// ʣ����Ƕ���. 
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
 * �ѷ�ʣ��1, ����ը��, ����ս��������ʤ��. 
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
			// ��С���ӹ����¼�.
			return MakeBomb(A);
		}

		// û��С���Ӹ��¼�. ����������, ����Ҳ��Ȼ������С���� 
		int BombRate = 10;
		if (FriendRemainCard >= MaxCard)
		{
			// �¼����ƺܴ�. 
			BombRate = 90;				// 9�ɼ���ը��
		}
		if (Chance(BombRate))
		{
			return MakeBomb(A);
		}
	}

	// �������޷�ѡ�����ջصĸ��ӵ�ʱ��. ��Ȼ����ը���Ŀ�����
	// ����ѷ�ʣ����ƱȽϴ�Ļ�.��ʵ���ǿ���ը��. 
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		int BombRate = 0;
		if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
		{
			// �¼����ƺܴ�. 
			BombRate = 20;				// 9�ɼ���ը��
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
* ���ѷ�����, �ѷ�ʣ��1, ���Ը�������, ���ѷ��Ӳ�����, ��Ȼ������. 
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
 * ����������. �ѷ�ʣ��1, ����ը��, ����ս�� 
 */
DGameAIHand DAIPostLord::TryHelpEscape1AfterBomb_1(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	DAtom A;
	if (FriendRemainCard <= FollowNumber)
	{
		// �¼Ҵ���. 
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}
	}
	else
	{
		// �¼Ҵ����, �¼Ҵ�ʱ������. 
		const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			int BombRate = 10;
			if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
			{
				// �¼����ƺܴ�. 
				BombRate = 70;				// 9�ɼ���ը��
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
	// û��ը��. 

	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	const DCardNumber FriendRemainCard = Friend->GetCard(0).Number;
	const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
	DAtom A;
	if (FriendRemainCard <= FollowNumber)
	{
		// �ѷ�Ҫ����. 
		// ��ʱ����������ĸ���, �����ĸ���. 
		A = Select_Descend(m1Atoms, FollowNumber);

		if (A && A.Number >= MaxCard)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}

		return DGameAIHand::PASS;
	}
	else
	{
		// ��һ��. 
		A = Select_Descend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	return DGameAIHand::PASS;
}

// �������¼���...
DGameAIHand DAIPostLord::TryHelpEscape_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// �ѷ����ƴ���2�ŵ�ʱ��, ���ǲ��迼��. 
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
			// ������
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
		// �ѷ�ʣ������.


	}



	return DGameAIHand::PASS;
}

// ����������, �����������. 
DGameAIHand DAIPostLord::TryPrevent_1(const DHand* ToFollow, const DDeckCharacter* Lord) const
{
	// ����δ����, ����. 
	const int LordNumRemain = Lord->GetNumCards();
	if (LordNumRemain > 2)
	{
		return DGameAIHand::PASS;
	}

	// ���ж�, �¼��Ƿ��ܶ�ס, �������ס, ������. 
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DDeckCharacter* Friend = GetFriend();
	VASSERT(Friend != Lord);
	const DDeckAIHint& FriendHint = Friend->AI_GetHint();
	if (FriendHint.AI_Determ_Single_Max(FollowNumber, true))
	{
		// �����ϼ��ܴ����, �Ҳ���Ҫǿѹ, 
		return DGameAIHand::PASS;
	}

	// 
	DGameAIHand AIHand;
	DCardNumber OpposeNumber = DCN_NULL;
	if (LordNumRemain == 1)
	{
		// ����ʣ��1��. 
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
	// ��������Լ�����, �Լ���. 
	DGameAIHand AIHand = TryEscape(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber Card = mDeck->AI_QueryCardGreatThen(mCardSlots, FollowNumber);
	// ����Ҫ����
	if (Card == DCN_NULL && !HasBomberOr2K())
	{
		return DGameAIHand::PASS;
	}


	// ��Ҫ����������, �������ѷ�����. 
	AIHand = TryHelpEscape_1(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}

	// ������� �Ӹ�����ѡ.
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner != Friend)
	{
		// �����������. 
		AIHand = TryPrevent_1(ToFollow, HandOwner);


		DAtom A = Select_Ascend(m1Atoms, FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}

		// ����Ӹ����д���. 
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

#pragma region "����"
DGameAIHand DAIPostLord::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	// ����Ҫ����
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DCardNumber Card = Peek2_Force(FollowNumber);
	if (Card == DCN_NULL && !HasBomberOr2K())
	{
		return DGameAIHand::PASS;
	}

	// �����Լ�ʤ��
	DGameAIHand AIHand = TryEscape_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return AIHand;
	}


	// AI ��������, ���ǲ���Ϊ�Լ�����ʤ������. �������¼�ʤ��. 
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

	// �����������. 
	AIHand = TryPrevent_2(ToFollow, HandOwner);
	if (AIHand)
	{
		return DGameAIHand::PASS;
	}

	DDeckCharacter* Friend = GetFriend();
	DAtom A;
	// �������
	if (HandOwner == Friend)
	{
		// �Ӷ�����ѡ��
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
			// ��3����/4���е�2 �������????
			DCardNumber C2 = Peek2_Force(FollowNumber);
			if (C2 != DCN_NULL)
			{
				return DGameAIHand(DHT_2, C2, 2);
			}

			// �����Ķ��Ӵ�úܴ�, ����������.
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
 * �������¼���. 
 */
DGameAIHand DAIPostLord::TryHelpEscape_2(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// �ѷ����ƴ���2�ŵ�ʱ��, ���ǲ��迼��. 
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
			// �¼ҿ�����. 

			// �����ը��, ����ը�������ѷ���.
			if (BombNum && (TestHand = TryLetEscape1AfterBomb_1(FollowNumber, Friend)))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
		else
		{
			// �¼���������ܲ���. 
			// ������, ���԰����¼�ʤ��
			if (BombNum && (TestHand = TryHelpEscape_Remain1_WithBomb(ToFollow, Friend)))
			{
				return TestHand;
			}

			// �޷�ȷ����ը����֤�¼�ʤ��. ���������Ķ����������¼�. 
			if (TestHand = TryHelpEscape_Remain1_With2(ToFollow, Friend, Continue))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
	}
	else
	{
		// �ѷ�ʣ������.
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
			// �������ѷ�. ���ѷ�ʣ�಻�Ƕ���. 
			if (FriendRemainDoubleNumber == DCN_NULL)
			{
				// �������.  ���Ҳ�Ҫ�ټ���������Ϊ. 
				Continue = false;
				return DGameAIHand::PASS;
			}
			else
			{
				// �ѷ��Ƕ���. 
					// ����ը������������
					if (TestHand = TryLetEscape2AfterBomb_2(ToFollow, Friend, FriendRemainDoubleNumber))
					{
						return TestHand;
					}

					// ���Բ���һ������.
					if (TestHand = TryLetEscape2AfterPlus_2(ToFollow, Friend, FriendRemainDoubleNumber))
					{
						return TestHand;
					}
			}
		}
		else
		{
			bool FriendCanEscape = false;
			// ������
			if (FriendRemainDoubleNumber > FollowNumber)
			{
				FriendCanEscape = true;
			}
			

			if (FriendCanEscape)
			{
				// ����ը������������
				if (TestHand = TryLetEscape2AfterBomb_2(ToFollow, Friend, FriendRemainDoubleNumber))
				{
					return TestHand;
				}

				// ���Բ���һ������.
				if (TestHand = TryLetEscape2AfterPlus_2(ToFollow, Friend, FriendRemainDoubleNumber))
				{
					return TestHand;
				}
			}
			else
			{
				// �¼��޷���. ����Э��������. 
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
	// ��ѯǱ�ڵĴ����¼Ҷ��ӵ�������, С����Խ��, ���ܼ���Խ��
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
	// ��ѯ������еĶ����Ƿ��㹻��. ����㹻��Ļ�, Ҳ���Բ�����. ���ڶ��ӵ���Խ��, ���ܼ���Խ��.
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
 * �ѷ�ʣ�����, ��������ֱ����. 
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
			// ��С���Ӹ��¼�. 
			return MakeBomb(A);
		}

		// û��С���Ӹ��¼�. ����������, ����Ҳ��Ȼ������С����, ���߶����㹻��, ���Բ𿪴� 
		// ��ѯǱ�ڵĴ����¼Ҷ��ӵ�������
		if (AI_Determ_Double_Can_Escape(FriendRemainDoubleNumber, 1.0f))
		{
			return MakeBomb(A);
		}
	}

	// �������޷�ѡ�����ջص�ը����ʱ��. �����Ϊ�ѷ��Ķ��������ܼ��ʵĻ���Ȼ����ը���Ŀ�����

	A = SelectBomb(DCN_NULL);
	if (A && AI_Determ_Double_Can_Escape(FriendRemainDoubleNumber, 0.6f))
	{
		return MakeBomb(A);
	}

	return DGameAIHand::PASS;
}

/**
 * �ѷ�ʣ�����, ��������ֱ����, ���Բ���һ������ 
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
 * �¼�ʣ��1������, ���޷�����ǰ�Ƶ������, ��ը��Э����ʤ�� 
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
	
	// ѡ��ը��. 
	A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(FriendRemainCard, A.Number) != DCN_NULL)
		{
			// ��С���ӹ����¼�.
			return MakeBomb(A);
		}

		// û��С���Ӹ��¼�. ����������, ����Ҳ��Ȼ������С���� 
		int BombRate = 10;
		if (FriendRemainCard >= MaxCard)
		{
			// �¼����ƺܴ�. 
			BombRate = 60;				// 9�ɼ���ը��
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

	// ���������ʱ��, �¼���Ҫ����ǰ�Ƶ�, ����¼ҵĸ��ӽϴ�, ������Ȼ��ը��, �����Ƿ����ջ�ը��

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
			// �Ҳ��ܹ��Ƹ��¼�, ����ֻҪ�¼ҵ��ƹ���, ���Ǽٶ���������Ƹ��¼�. 
			const DCardNumber MaxCard = mDeck->AI_QueryMaxCard(mCardSlots);
			if (FriendRemainCard >= MaxCard || FriendRemainCard >= DCN_2)
			{
				// �¼����ƺܴ�. 
				BombRate = 50;				// 9�ɼ���ը��
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

// �¼�ʣ�����, ��ͼը����Э��������
DGameAIHand DAIPostLord::TryHelpEscape_Remain2_WithBomb(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber RemainDoubleNumber) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	//DDeckCharacter* Lord = GetLord();
	//const int LordRemainNum = Lord->GetNumCards();
	//float BombLordFactor = ((float)LordRemainNum / 5.0f);
	//BombLordFactor *= BombLordFactor;
	//BombLordFactor = VClamp(BombLordFactor, 0.2f, 1.8f);

	
	DAtom A;

	// ѡ��ը��. 
	A = Peek_Bomb_CanRec(FollowNumber);
	if (A)
	{
		if (AnyLessThen(RemainDoubleNumber, A.Number, 2) != DCN_NULL)
		{
			// ��С���ӹ����¼�.
			return MakeBomb(A);
		}

		// û��С���Ӹ��¼�. ����������, ����Ҳ��Ȼ������С����
		if (AI_Determ_Double_Can_Escape(RemainDoubleNumber, 2.0f))
		{
			return MakeBomb(A);
		}
	}

	// ���������ʱ��, ���ǵ�ը�����ȥ, �����ղ�����. ����, ��Ȼ����Э��. 

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
		// ��ȫ�޷��ö��Ӵ�.
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
			// ���ǲ���ȷ�������Ӹ��¼�. ���ǵ�����Ȼ���Դ�. 
			if (FriendRemainCard >= MaxCard)
			{
				// �¼����ƺܴ�. 
				Rate = 90;				// 9�ɼ���ը��
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

// Э���¼�����. 
DGameAIHand DAIPostLord::TryHelpEscape_Remain2_2(const DHand* ToFollow, const DDeckCharacter* Friend) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();

	// ѡ��һ���ϴ�Ķ���.
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
		// ����ʣ��1��. 
		if (HandOwner == Lord)
		{
			// Ĭ�������, ���Ƕ����¼�ȥ��������������. ����, �п����¼��޷���ס����.  

		}

		return DGameAIHand::PASS;
	}
	return DGameAIHand::PASS;
}

#pragma endregion "����"

// ����Э���¼�����, ������Ϊ�ǵ�,�ǶԵ�. 
DGameAIHand DAIPostLord::TryHelpEscapeCommon(const DHand* ToFollow, const DDeckCharacter* HandOwner, int& NextAction) const
{
	DDeckCharacter* Friend = GetFriend();
	const int FriendRemainCardNum = Friend->GetNumCards();
	if (FriendRemainCardNum > 2)
	{
		// �ѷ����ƴ���2�ŵ�ʱ��, ���ǲ��迼��. 
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
			// �¼ҿ�����. 

			// �����ը��, ����ը�������ѷ���.
			if (BombNum && (TestHand = TryLetEscape1AfterBomb_1(FollowNumber, Friend)))
			{
				return TestHand;
			}

			return DGameAIHand::PASS;
		}
		else
		{
			// �¼���������ܲ���. 
			// ������, ���԰����¼�ʤ��
			if (BombNum && (TestHand = TryHelpEscape_Remain1_WithBomb(ToFollow, Friend)))
			{
				return TestHand;
			}

			// �޷�ȷ����ը����֤�¼�ʤ��. ��Ҫ����
			NextAction = CallHelp;
			return DGameAIHand::PASS;
		}
	}
	else
	{
		// �ѷ�ʣ������.
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
			// �������ѷ�. ���ѷ�ʣ�಻�Ƕ���. 
			if (FriendRemainDoubleNumber == DCN_NULL)
			{
				// �������.  ���Ҳ�Ҫ�ټ���������Ϊ. 
				NextAction = NeedPassOneHand;
				return DGameAIHand::PASS;
			}
			else
			{
				// �ѷ��Ƕ���. 
				// ����ը������������
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
			// �¼��ܲ���. 

			// �¼��޷���. ����Э��������. 
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

#pragma region "3��"
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
	// �������
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
	// �������
	//////////////////////////////////////////////////////////////////////////
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner == Friend)
	{
		// �����ѷ�����, ����
		return DGameAIHand::PASS;
	}

	// ������
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
	// �������
	//////////////////////////////////////////////////////////////////////////
	DDeckCharacter* Friend = GetFriend();
	if (HandOwner == Friend)
	{
		// �����ѷ�����, ����
		return DGameAIHand::PASS;
	}

	// ������
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

#pragma endregion "3��"

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

			// ը���Ĵ�СҲ������Ϊһ��Ȩ�ؽ��п���

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

			// ը���Ĵ�СҲ������Ϊһ��Ȩ�ؽ��п���

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
			// �ҵ�����. 
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

			// ը���Ĵ�СҲ������Ϊһ��Ȩ�ؽ��п���

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
			// ����Ҫ����
			// ����. TODO: ���ﻹ���Կ�����, BOMB BOMB Ȼ���������. 
			return NULL;
		}
		else
		{
			// ����������. 
			// ���Թ��Ƹ��¼�. 
			
			// CHEAT ..... 
			const DCard& FriendRemain = Friend->GetCard(0);

			
		}
	}

	// 
	if (HandOwnerIsFriend)
	{
		// ������ѷ�����Ĵ���K����, ����. 
		

		if (Number >= DCN_A)
		{
			return NULL;
		}
	}

	return DAIFarmer::GetHand_ToFollow_1(ToFollow, HandOwner);
#endif 
}

#endif 
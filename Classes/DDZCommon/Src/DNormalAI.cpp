#include "VStdAfx.h"
#include "DNormalAI.h"
void PHSort(DAIPotentialHandList& Hands)
{
	if (Hands.IsEmpty())
	{
		return;
	}
	DAIPotentialHand* Hand = Hands.GetData();
	std::sort(Hand, Hand + Hands.Size(), [](const DAIPotentialHand& a0, const DAIPotentialHand& a1)->bool
	{
		return a0.Weight > a1.Weight;
	}
	);
}

DNormalAI::DNormalAI(DDeckCharacter* Owner) :DGameAI(Owner)
{
}


DNormalAI::~DNormalAI()
{
}

void DNormalAI::UpdateCards(const DCard* Cards, int NumCards)
{
	if (NumCards == 0)
	{
		return;
	}
	Init(Cards, NumCards);
	SloverTypes();

	for (int i = 0; i <= DCN_RED_JOKER; ++i)
	{
		mAICardSlot[i].NumCards = 0;
	}

	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& c = Cards[i];
		mAICardSlot[c.Number].AddCard(c);
	}


}

int DNormalAI::CalcCallLordScore() const
{
	const int RocketWeight = 8;
	const int BombWeight = 6;
	const int BigKingWeight = 4;
	const int SmallKingWeight = 3;
	const int Weight_2 = 2;

	int Weight = 0;
	if (m2K)
	{
		Weight += RocketWeight;
	}
	else
	{
		if (mCardSlots[DCN_BLACK_JOKER])
		{
			Weight += SmallKingWeight;
		}

		if (mCardSlots[DCN_RED_JOKER])
		{
			Weight += BigKingWeight;
		}
	}

	int NumBombs = m4Atoms.size();
	Weight += NumBombs * BombWeight;
	
	if (mCardSlots[DCN_2] == 4)
	{
		Weight -= BombWeight;
	}

	Weight += (mCardSlots[DCN_2] * Weight_2);

	if (Weight >= 7)
	{
		return 3;
	}

	if (Weight >= 5)
	{
		return 2;
	}

	if (Weight >= 3)
	{
		return 1;
	}

	return 0;
}


void DNormalAI::TranslateHandFromGameAIHand(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (!AIHand)
	{
		Hand.SetHandType(DHT_UNKNOWN);
		Hand.SetHandNumber(DCN_NULL);
		Hand.Clear();
		return;
	}

	switch (AIHand.Type)
	{
	case DHT_1:	TranslateHand_1(AIHand, Hand); return;
	case DHT_2: TranslateHand_2(AIHand, Hand); return;
	case DHT_3: TranslateHand_3(AIHand, Hand); return;
	case DHT_3_1: TranslateHand_3x1(AIHand, Hand); return;
	case DHT_3_2: TranslateHand_3x2(AIHand, Hand); return;
	case DHT_4_2: TranslateHand_4x2(AIHand, Hand); return;
	case DHT_straight: TranslateHand_1s(AIHand, Hand); return;
	case DHT_straight_2: TranslateHand_2s(AIHand, Hand); return;
	case DHT_straight_3: TranslateHand_3s(AIHand, Hand); return;
	case DHT_4:	TranslateHand_4(AIHand, Hand); return;
	case DHT_2K: TranslateHand_2k(AIHand, Hand); return;
	default:
		break;
	}
}


void DNormalAI::TranslateHand_1(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		_GetCard(Hand, AIHand.Number, 1);
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_2(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		_GetCard(Hand, AIHand.Number, 2);
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_3(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		_GetCard(Hand, AIHand.Number, 3);
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}
void DNormalAI::TranslateHand_3x1(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		VASSERT(AIHand.X[0].X != DCN_NULL && AIHand.X[0].XLen == 1);
		_GetCard(Hand, AIHand.Number, 3);
		_GetCard(Hand, AIHand.X[0].X, 1);

		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}
void DNormalAI::TranslateHand_3x2(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		VASSERT(AIHand.X[0].X != DCN_NULL && AIHand.X[0].XLen == 2);
		_GetCard(Hand, AIHand.Number, 3);
		_GetCard(Hand, AIHand.X[0].X, 2);

		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_4(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		_GetCard(Hand, AIHand.Number, 4);
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_4x2(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		VASSERT(!"UNCKECK!");
		VASSERT(AIHand.X[0].X != DCN_NULL && AIHand.X[0].XLen == 2);
		_GetCard(Hand, AIHand.Number, 3);
		_GetCard(Hand, AIHand.X[0].X, 2);

		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_1s(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		VASSERT(AIHand.Len >= 5);
		DCardNumber Start = AIHand.Number;
		DCardNumber Tail = (DCardNumber)((int)Start + AIHand.Len);
		for (int i = Start; i < Tail; ++i)
		{
			_GetCard(Hand, (DCardNumber)i, 1);
		}
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_2s(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		VASSERT(AIHand.Len >= 3);
		DCardNumber Start = AIHand.Number;
		DCardNumber Tail = (DCardNumber)((int)Start + AIHand.Len);
		for (int i = Start; i < Tail; ++i)
		{
			_GetCard(Hand, (DCardNumber)i, 2);
		}
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::TranslateHand_3s(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		int Len = AIHand.Len;
		VASSERT(Len >= 2 && Len <= 4);
		for (int i = 0; i < Len; ++i)
		{
			_GetCard(Hand, (DCardNumber)(AIHand.Number + i), 3);
		}

		for (int i = 0; i < Len; ++i)
		{
			if (AIHand.X[i].XLen)
			{
				_GetCard(Hand, AIHand.X[i].X, AIHand.X[i].XLen);
			}
		}
		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
		Hand.SetStraightLen(Len);
	}
}

void DNormalAI::TranslateHand_2k(const DGameAIHand& AIHand, DHand& Hand) const
{
	if (AIHand)
	{
		_GetCard(Hand, DCN_BLACK_JOKER, 1);
		_GetCard(Hand, DCN_RED_JOKER, 1);

		Hand.SetHandType(AIHand.Type);
		Hand.SetHandNumber(AIHand.Number);
	}
}

void DNormalAI::_GetCard(DHand& Hand, const DCardNumber Number, int Num) const
{
	VASSERT(Num > 0 && Num <= 4);
	VASSERT(mAICardSlot[Number].NumCards >= Num);
	

	for (int i = 0; i < Num; i++)
	{
		DCard Card = mAICardSlot[Number].PopBack();
		Hand.AddCard(Card);
	}
}

/**
 * ����, �ұ����¼�ʣ��ĸ����ӳ�.  
 */
DGameAIHand DNormalAI::PlayHand_Min_NoSingle(DCardNumber Remain1Number) const
{
	// ���ƽ׶�, �����������ȳ��ܴ��Ƴ�ȥ������, �����ǵ�С���Ӹ������. 
	//DAIPotentialHand AIHand;
	//VPODArray<DAIPotentialHand> Hands;
	float Score = 100.0f;
	DGameAIHand AIHand;

	int MinCardNumber = DCN_RED_JOKER;

	// ���Ƚ��ɻ����ȥ. 
	AIHand = Peek_3_straight();
	if (AIHand)
	{
		return AIHand;
	}

	// ˳��
	AIHand = GetStraight();
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = GetStraight2();
	if (AIHand)
	{
		return AIHand;
	}


	// 3x1
	DAtom A;
	if (!m3Atoms.empty())
	{
		DAtom A3 = Select_Ascend(m3Atoms, DCN_NULL);

		// ���ܶ��ĸ��Ӷ�����ȥ
		A = Select_Ascend(m1Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(A.Number, 1));
		}

		// û�и��ӵ������. ������. 
		A = Select_Ascend(m2Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(A.Number, 2));
		}		

		AIHand = DGameAIHand(DHT_3, A3.Number, 3);
		return AIHand;
	}

	A = Select_Ascend(m2Atoms, DCN_NULL);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}

	DCardNumber Number = DCN_NULL;
	while (1)
	{
		A = Select_Ascend(m1Atoms, Number);
		if (!A)
		{
			break;
		}
		Number = A.Number;
		if (A.Number >= Remain1Number)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}


	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	A = Select_Descend(m1Atoms, DCN_NULL);
	VASSERT(A.Number != DCN_NULL);
	return DGameAIHand(DHT_1, A.Number, 1);
}
/**
 * ��һ����, ����ʣ��ĵ����ӳ�ȥ.  
 */
DGameAIHand DNormalAI::PlayHand_Min_GuardSingle(DCardNumber Remain1Number) const
{
	DGameAIHand AIHand;
	// ���Ƚ��ɻ����ȥ. 
	AIHand = Peek_3_straight();
	if (AIHand)
	{
		return AIHand;
	}

	// ˳��
	AIHand = GetStraight();
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = GetStraight2();
	if (AIHand)
	{
		return AIHand;
	}


	// 3x1
	DAtom A;
	if (!m3Atoms.empty())
	{
		DAtom A3 = Select_Ascend(m3Atoms, DCN_NULL);

		// ���ܶ��ĸ��Ӷ�����ȥ
		A = Select_Ascend(m1Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(A.Number, 1));
		}

		// û�и��ӵ������. ������. 
		A = Select_Ascend(m2Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(A.Number, 2));
		}

		AIHand = DGameAIHand(DHT_3, A3.Number, 3);
		return AIHand;
	}


	DCardNumber Number = DCN_NULL;
	while (1)
	{
		A = Select_Ascend(m1Atoms, Number);
		if (!A)
		{
			break;
		}
		Number = A.Number;
		if (A.Number >= Remain1Number)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	A = Select_Ascend(m2Atoms, DCN_NULL);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}


	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	A = Select_Descend(m1Atoms, DCN_NULL);
	VASSERT(A.Number != DCN_NULL);
	return DGameAIHand(DHT_1, A.Number, 1);
}

DGameAIHand DNormalAI::PlayHand_GuardDouble(DCardNumber RemainDoubleNumber) const
{
	DGameAIHand AIHand;
	int MinCardNumber = DCN_RED_JOKER;
	// ���Ƚ��ɻ����ȥ. 
	AIHand = Peek_3_straight();
	if (AIHand)
	{
		return AIHand;
	}

	// ˳��
	AIHand = GetStraight();
	if (AIHand)
	{
		return AIHand;
	}

	AIHand = GetStraight2();
	if (AIHand)
	{
		return AIHand;
	}


	// 3x1
	DAtom A;
	if (!m3Atoms.empty())
	{
		DAtom A3 = Select_Ascend(m3Atoms, DCN_NULL);

		// ���ܶ��ĸ��Ӷ�����ȥ
		A = Select_Ascend(m1Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(A.Number, 1));
		}

		// û�и��ӵ������. ������. 
		A = Select_Ascend(m2Atoms, DCN_NULL);
		if (A)
		{
			return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(A.Number, 2));
		}

		AIHand = DGameAIHand(DHT_3, A3.Number, 3);
		return AIHand;
	}

	A = Select_Ascend(m1Atoms, DCN_NULL);
	if (A)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}

	// ѡ����, ���Ǳ���Է��ܵ�
	DCardNumber TestNumber = DCN_NULL;
	while (1)
	{
		A = Select_Ascend(m2Atoms, TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= RemainDoubleNumber)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
		TestNumber = A.Number;
	}

	// �����Ӳ�ɵ����� 
	A = Peek1_FromDouble(DCN_NULL);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 1);
	}

	A = SelectBomb(DCN_NULL);
	VASSERT(A.Number != DCN_NULL);
	return MakeBomb(A);
}

/**
 * ѡ�����пɳ����Ƶ����. 
 */
void DNormalAI::GetAllPlayableHands(DAIPotentialHandList& Hands) const
{
	
}


bool DNormalAI::GetHand_ToFollow(const DHand* ToFollow, const DDeckCharacter* HandOwner, DHand& Result)
{
	VASSERT(ToFollow);
	DHandType Type = ToFollow->GetHandType();
	VASSERT(Type != DHT_UNKNOWN);
	DGameAIHand AIHand;

	Result.SetHandType(DHT_UNKNOWN);
	Result.SetHandNumber(DCN_NULL);
	Result.Clear();
	switch (Type)
	{
	case DHT_1:	AIHand = GetHand_ToFollow_1(ToFollow, HandOwner); break;
	case DHT_2:AIHand = GetHand_ToFollow_2(ToFollow, HandOwner);break;
	case DHT_3: AIHand = GetHand_ToFollow_3(ToFollow, HandOwner); break;
	case DHT_3_1: AIHand = GetHand_ToFollow_3_1(ToFollow, HandOwner); break;
	case DHT_3_2: AIHand = GetHand_ToFollow_3_2(ToFollow, HandOwner); break;
	case DHT_4_2: AIHand = GetHand_ToFollow_4_2(ToFollow, HandOwner); break;
	case DHT_straight: AIHand = GetHand_ToFollow_straight(ToFollow, HandOwner); break;
	case DHT_straight_2: AIHand = GetHand_ToFollow_straight_2(ToFollow, HandOwner); break;
	case DHT_straight_3: AIHand = GetHand_ToFollow_straight_3(ToFollow, HandOwner); break;
	case DHT_4:		AIHand = GetHand_ToFollow_4(ToFollow, HandOwner); break;
	case DHT_2K: return false;
	default:
		break;
	}
	TranslateHandFromGameAIHand(AIHand, Result);
	return AIHand;
}


bool DNormalAI::FollowHand(const DHand* Hand, DDeckCharacter* HandOwner, DHand& Result)
{
	if (HandOwner == mOwner || HandOwner == nullptr)
	{
		// ����
		DGameAIHand AIHand = PlayHand();
		VASSERT(AIHand.IsValid());
		TranslateHandFromGameAIHand(AIHand, Result);
		return true;
	}
	else
	{
		return GetHand_ToFollow(Hand, HandOwner, Result);
	}
	return false;
}

DGameAIHand DNormalAI::PlayHand() const
{
	return DGameAIHand::PASS;
}


DGameAIHand DNormalAI::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}
DGameAIHand DNormalAI::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DGameAIHand::PASS;
}


void DNormalAI::PH_Bomb(const DCardNumber Number, DAIPotentialHandList PotentialHands) const
{
	for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
	{
		if (mCardSlots[i] == 4)
		{
			PotentialHands.PushBack(DAIPotentialHand(DHT_4, (DCardNumber)i, 4));
		}
	}

	if (Has2King())
	{
		PotentialHands.PushBack(DAIPotentialHand(DHT_2K, (DCardNumber)DCN_RED_JOKER, 2));
	}
}



#if 0


DHand* DNormalAI::GetHand_3_straight() const
{
	
}

DHand* DNormalAI::GetHand_3_straight_1(const DHand* H3S, int x) const
{
	// Note: ���Ƕ�AI����û�г��ķɻ�. 
	int NumK2 = 0;
	int NumSingleNK2 = GetSingleNum_Not_KingOr2();
	
	if (NumSingleNK2 >= x)
	{
		goto found;
	}

	// ������2�ĵ����޷���������. 
	NumK2 = mSingleHand.Size() - NumSingleNK2;
	if (!NumK2)
	{
		return NULL;
	}
	
	// ������2. 
	if ((mSingleHand.Size() ==x) &&				// �����պ���ȫ������. 
		(mHand.Size() == H3S->Size() + x)		// ��һ���Գ���.  ???
		)
	{
		goto found;
	}

	// TODO �������һ���Գ�������, ����һ������, ģ��ģ���߼�, ��AI�л�����Ȼ����2����. 


	return NULL;
	
found:
	DHand* Result = VNew DHand(DHT_straight_3, H3S);
	for (int Index = (int)mSingleHand.Size() - x; Index < mSingleHand.Size(); ++Index)
	{
		Result->AddCard(mSingleHand[Index]);
	}
	return Result;
}


DHand* DNormalAI::GetHand_1() const
{
	// Note: ������Ҫ���Ƶ�3,3S ���ƿɴ�. 
	if (mSingleHand.IsEmpty())
	{
		return NULL;
	}

	int NumSingles = mSingleHand.Size();
	int NumDoubles = mDoubleHands.Size();
	int NumXNeed = NumXNeedFor_3x_And_Plane();
	if ((NumSingles + NumDoubles) >(NumXNeed))
	{
		const DCard& Raw = mSingleHand.last();
		DHand* Result = VNew DHand(DHT_1);
		Result->AddCard(Raw);
		Result->SetHandNumber(Raw.Number);

		return Result;
	}

	return NULL;
}

DHand* DNormalAI::GetHand_2() const
{
	// Note: ������Ҫ���Ƶ�3,3S ���ƿɴ�. 
	if (mDoubleHands.IsEmpty())
	{
		return NULL;
	}

	int NumSingles = mSingleHand.Size();
	int NumDoubles = mDoubleHands.Size();
	int Num3 = m3Hands.Size();
	int Num3s = m3StraightHands.Size();

	int Num3sx = 0;
	for (int Index = 0; Index < Num3s; ++Index)
	{
		DHand* H3S = m3StraightHands[Index];
		Num3sx += H3S->Size();
	}
	Num3sx = Num3sx / 3;
	if ((NumSingles + NumDoubles) > (Num3sx + Num3))
	{
		DHand* Raw = mDoubleHands.GetLast();
		DHand* Result = VNew DHand(DHT_2, Raw);
		return Result;
	}
	
	return NULL;
}

DHand* DNormalAI::GetHand_straight() const
{
	if (mStraightHands.IsEmpty())
	{
		return NULL;
	}

	DHand* Raw = mStraightHands.GetLast();
	DHand* Result = VNew DHand(DHT_straight, Raw);
	return Result;
}


DHand* DNormalAI::GetHand_Bomb() const
{
	if (m4Hands.IsEmpty())
	{
		return NULL;
	}

	DHand* Raw = m4Hands.GetLast();
	DHand* Result = VNew DHand(DHT_4, Raw);
	return Result;
}


DHand* DNormalAI::GetHand_2K() const
{
	if (m2KHand)
	{
		DHand* Result = VNew DHand(DHT_2K, m2KHand);
		return Result;
	}
	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow(const DHand* ToFollow, const DDeckCharacter* HandOwner)
{
	VASSERT(ToFollow);
	DHandType Type = ToFollow->GetHandType();
	VASSERT(Type != DHT_UNKNOWN);

	switch (Type)
	{
	case DHT_1:	return GetHand_ToFollow_1(ToFollow, HandOwner);
	case DHT_2: return GetHand_ToFollow_2(ToFollow, HandOwner);
	case DHT_3: return GetHand_ToFollow_3(ToFollow, HandOwner);
	case DHT_3_1: return GetHand_ToFollow_3_1(ToFollow, HandOwner);
	case DHT_3_2: return GetHand_ToFollow_3_2(ToFollow, HandOwner);
	case DHT_4_2: return GetHand_ToFollow_4_2(ToFollow, HandOwner);
	case DHT_straight: return GetHand_ToFollow_straight(ToFollow, HandOwner);
	case DHT_straight_2: return GetHand_ToFollow_straight_2(ToFollow, HandOwner);
	case DHT_straight_3: return GetHand_ToFollow_straight_3(ToFollow, HandOwner);
	case DHT_4:		return GetHand_ToFollow_4(ToFollow, HandOwner);
	case DHT_2K: return NULL;
	default:
		break;
	}
	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);
	return FollowHand_1_Normal(HandValue, FollowFriend);
}

DHand* DNormalAI::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);
	if (FollowFriend)
	{
		// ������ѷ�������, >= A ����, ����Ҫ. 
		if (HandValue >= DCN_A)
		{
			return NULL;
		}
	}

	// ����ѡ��һ������
	DHand* Result = FH_Peek_2(HandValue);
	if (Result)
	{
		if (FollowFriend && Result->GetHandNumber() >= DCN_A)
		{
			// ���ѡ���Ķ����� AA , 22 , ��û��Ҫѹ. 
			VDelete Result;
			return NULL;
		}
		return Result;
	}

	// û��ԭʼ�Ķ���
	if (!FollowFriend)
	{
		// ����ǶԵз�. 

		// ��˫˳
		Result = FH_Peek_2_Split2S(HandValue);
		if (Result)
		{
			return Result;
		}

		// ը��
		int bomb_rate = GetBomb_2_Rate(HandValue);
		if (bomb_rate && Chance(bomb_rate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}
	}
	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);

	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_3(HandValue);
		if (Result)
		{
			return Result;
		}

		int bomb_rate = GetBomb_3_Rate(HandValue);
		if (bomb_rate && Chance(bomb_rate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}
	}

	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);

	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_3_1(HandValue);
		if (Result)
		{
			return Result;
		}

		int bomb_rate = GetBomb_3_Rate(HandValue);
		if (bomb_rate && Chance(bomb_rate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}
	}
	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);

	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_3_2(HandValue);
		if (Result)
		{
			return Result;
		}

		int bomb_rate = GetBomb_3_Rate(HandValue);
		if (bomb_rate && Chance(bomb_rate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}
	}

	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);
	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_Straight(HandValue, ToFollow->Size());
		if (Result)
		{
			return Result;
		}

		int BombRate = 50;

		if (ToFollow->Size() >= 8 )
		{
			BombRate = 100;
		}

		if (HandOwner->GetNumCards() <= 2)
		{
			BombRate = 100;
		}

		if (BombRate == 100 || Chance(BombRate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}	
	}

	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);

	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_Straight2(HandValue, ToFollow->Size());
		if (Result)
		{
			return Result;
		}

		int BombRate = 60;

		if (ToFollow->Size() >= 10)
		{
			BombRate = 100;
		}

		if (HandOwner->GetNumCards() <= 2)
		{
			BombRate = 100;
		}

		if (BombRate == 100 || Chance(BombRate))
		{
			if ((Result = GetHand_Bomb()) != NULL)
			{
				return Result;
			}

			if ((Result = GetHand_2K()) != NULL)
			{
				return Result;
			}
		}
	}

	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);

	if (!FollowFriend)
	{
		const int NumCards = ToFollow->Size();
		const DCard* Cards = ToFollow->GetCards();
		const DCardNumber HandValue = ToFollow->GetHandNumber();
		DHand Hand3;
		int Last = NumCards - 2;
		int Index = 0;
		while (Index < Last)
		{
			if (Cards[Index].Number <= DCN_A && Cards[Index].Number == Cards[Index + 1].Number && Cards[Index].Number == Cards[Index + 2].Number)
			{
				// �ҵ�һ������. 
				Hand3.AddCards(&Cards[Index], 3);
				Index += 3;
			}
			else
			{
				Index++;
			}
		}
		DHand* Result = NULL;
		int NumX = NumCards - Hand3.Size();
		if (NumX == 0)
		{
			// û�д�����.
			Result = FH_Peek_Straight3(HandValue, NumCards);
			if (Result)
			{
				return Result;
			}
		}


		if ((Result = GetHand_Bomb()) != NULL)
		{
			return Result;
		}

		if ((Result = GetHand_2K()) != NULL)
		{
			return Result;
		}
	}

	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);
	if (!FollowFriend)
	{
		DHand* Result = FH_Peek_Bomb(HandValue);
		if (Result)
		{
			return Result;
		}
	}
	return NULL;
}

DHand* DNormalAI::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	DCardNumber HandValue = ToFollow->GetHandNumber();
	BOOL FollowFriend = IsFollowFriend(HandOwner);
	if (!FollowFriend)
	{
		DHand* Result;
		if ((Result = GetHand_Bomb()) != NULL)
		{
			return Result;
		}

		if ((Result = GetHand_2K()) != NULL)
		{
			return Result;
		}
	}
	return NULL;
}





DHand* DNormalAI::GetHand_1_Max(DCardNumber Number) const
{
	// û�и���. 
	if (mSingleHand.IsEmpty())
	{
		return NULL;
	}

	//���ĸ���Ҳ����
	if (mSingleHand[0].Number <= Number)
	{
		return NULL;
	}

	
	DHand* Result = VNew DHand(DHT_1, mSingleHand[0].Number);
	Result->SetCards(&mSingleHand[0], 1);
	return Result;
	
}

DHand* DNormalAI::FollowHand_1_Normal(DCardNumber Number, BOOL FollowFriend) const
{
	// TODO : Optimization . 
	DHand* Result = GetHand_1_ToFollow_CareX(Number);
	if (Result)
	{
		return Result;
	}

	// �Ƿ����һ������>?>>>??? 
	if ((Result = GetHand_1_ToFollow_DontCareX(Number)) != NULL)
	{
		return Result;
	}

	if (FollowFriend)
	{

	}
	else
	{

		// ��˳��. 
		if ((Result = GetHand_1_Splits(Number)) != NULL)
		{
			return Result;
		}


		// �����
		// �������Խ��, ��ļ��ʾ�Խ��. 
		int SplitChance = rand() % Number;
		if (GetHandNum() == 1 ||  SplitChance > 5)
		{
			if ((Result = GetHand_1_Split2(Number)) != NULL)
			{
				return Result;
			}
		}

		// ��. 
		if (m2KHand)
		{
			// ����ҵ�С���������Ƚ϶�, ���������. 
			// TODO : �����������, ���������ϵĿ���, ���ж�С����. ����. 4A �Ѿ�����, ��ô K ���Ǵ��. 
			int NumSmallSingles = 0;
			for (int Index = 0; Index < mSingleHand.Size(); ++Index)
			{
				if (mSingleHand[Index].Number < DCN_A)
				{
					NumSmallSingles++;
				}
			}

			// С������������2
			if (NumSmallSingles > 2)
			{
				// ���С���ʹ���������ν. 
				static int _joker_rand = 0;
				_joker_rand++;
				if (_joker_rand & 1)
				{
					Result = VNew DHand(DHT_1, DCN_BLACK_JOKER);
					Result->SetCards(&DCard::SMALL_KING, 1);
				}
				else
				{
					Result = VNew DHand(DHT_1, DCN_RED_JOKER);
					Result->SetCards(&DCard::BIG_KING, 1);
					
				}
				return Result;
			}
		}

		// Bomb
		if ((Result = GetHand_Bomb()) != NULL)
		{
			return Result;
		}
		
		// ����? 
		if (m2KHand)
		{
			Result = VNew DHand(*m2KHand);
			return Result;
		}
	}

	return NULL;
}

DHand* DNormalAI::FollowHand_1_Bomb(DCardNumber Number) const
{

	// TODO
	//int NumX = NumXNeedFor_3x_And_Plane();
	//NumX -= (int)mDoubleHands.Size();

	DHand* Result;
	if ((Result = GetHand_Bomb()) != nullptr)
	{
		return Result;
	}

	if (m2KHand)
	{
		Result = VNew DHand(*m2KHand);
		return Result;
	}

	if ((Result = GetHand_1_Max(Number)) != nullptr)
	{
		return Result;
	}

	return NULL;
}


DHand* DNormalAI::FH_Peek_2(DCardNumber Number) const
{
	if (mDoubleHands.IsEmpty())
	{
		return NULL;
	}

	for (int Index = mDoubleHands.Size() - 1; Index >= 0; --Index)
	{
		DHand* H2 = mDoubleHands[Index];
		if (H2->GetHandNumber() > Number)
		{
			DHand* Result = VNew DHand(*H2);
			return Result;
		}
	}
	return NULL;
}

DHand* DNormalAI::FH_Peek_2_Split2S(DCardNumber Number) const
{
	if (m2StraightHands.IsEmpty())
	{
		return NULL;
	}

	//ֻ��˳�ӴӲ����С��.
	for (int Index = (int)m2StraightHands.Size() - 1; Index >= 0; --Index)
	{
		DHand* HS2 = m2StraightHands[Index];
		if (HS2->Size() > 6 && HS2->last().Number > Number)
		{
			DHand* Result = VNew DHand(DHT_2, HS2->last().Number);
			Result->SetCards(&HS2->At(HS2->Size()- 2), 2);
			return Result;
		}
	}

	return NULL;
}

int DNormalAI::GetBomb_2_Rate(DCardNumber Number) const
{
	if (Number< DCN_J)
	{
		return 0;
	}
	if (Number > DCN_2)
	{
		return 0;
	}

									// J    Q    K  A   2 
	static const int bomb_rate[5] = {30, 35, 50, 80, 100 };
	return bomb_rate[Number - DCN_J];
}

DHand* DNormalAI::FH_Peek_3(DCardNumber Number) const
{
	if (m3Hands.IsEmpty())
	{
		return NULL;
	}

	for (int Index = m3Hands.Size() - 1; Index >= 0; --Index)
	{
		DHand* H3 = m3Hands[Index];
		if (H3->GetHandNumber() > Number)
		{
			DHand* Result = VNew DHand(*H3);
			return Result;
		}
	}
	return NULL;
}



DHand* DNormalAI::FH_Peek_3_2(DCardNumber Number) const
{
	if (m3Hands.IsEmpty())
	{
		return NULL;
	}
	DHand* H3 = NULL;
	for (int Index = m3Hands.Size() - 1; Index >= 0; --Index)
	{
		if (m3Hands[Index]->GetHandNumber() > Number)
		{
			H3 = m3Hands[Index];
			break;
		}
	}
	if (H3 == nullptr)
	{
		return NULL;
	}


	int NumHands = GetHandNum();

	DHand* H2 = FH_Peek_2(Number);
	if (H2)
	{
		if (H2->GetHandNumber() >= DCN_A && NumHands > 2)
		{
			// ���ѡ���Ķ����� AA , 22 , ��û��Ҫѹ. 
			VDelete H2;
			H2 = NULL;
		}
	}

	// û��ԭʼ�Ķ���
	if (!H2)
	{
		H2 = FH_Peek_2_Split2S(Number);
	}
	if (H2)
	{
		DHand* Result = VNew DHand(*H3);
		Result->AddCards(H2->GetCards(), H2->Size());
		Result->SetHandNumber(H3->GetHandNumber());
		Result->SetHandType(DHT_3_2);

		VDelete H2;
		return Result;
	}
	return NULL;
}

int DNormalAI::GetBomb_3_Rate(DCardNumber Number) const
{
	if (Number < DCN_J)
	{
		return 20;
	}
	if (Number > DCN_2)
	{
		return 0;
	}

									// J    Q    K  A   2 
	static const int bomb_rate[5] = { 40, 70, 80, 90, 100 };
	return bomb_rate[Number - DCN_J];
}

DHand* DNormalAI::FH_Peek_Bomb(DCardNumber Number) const
{
	if (!m4Hands.IsEmpty())
	{
		for (int Index = m4Hands.Size() - 1; Index >= 0; --Index)
		{
			DHand* h = m4Hands[Index];
			if (h->GetHandNumber() > Number)
			{
				return VNew DHand(*h);
			}
		}
	}

	return GetHand_2K();
}


BOOL DNormalAI::CanEscapeAfterBomb(DCardNumber RemainCardNumber) const
{
	int NumBombers = GetBombNum();
	if (NumBombers)
	{
		// �����ը��....
		int NumX = NumXNeedFor_3x_And_Plane();
		if (!m2KHand)
		{
			NumX += (NumBombers - 1) * 2;			// 4-2
		}
		else
		{
			NumX += (NumBombers)* 2;	// 4-2
		}

		int RemainSingles = mSingleHand.Size() - NumX - 1;

		for (int i = 0; i < RemainSingles; ++i)
		{
			if (mSingleHand[i].Number < RemainCardNumber)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
#endif 

DAtom DNormalAI::Peek_MaxBomb(DCardNumber Number) const
{

	// 
	DAtom Bomb = SelectBomb(Number);
	while (Bomb)
	{
		int NumGreater = mDeck->AI_QueryRemainBombers(mCardSlots, Bomb.Number);
		if (NumGreater == 0)
		{
			break;
		}
		Bomb = SelectBomb(Bomb.Number);
	}
	return Bomb;
}

bool DNormalAI::IsBombCanRec(DCardNumber Bomber) const
{
	DCardNumber MaxBomb = mDeck->AI_QueryMaxBomber(mCardSlots, Bomber);
	if (!MaxBomb)
	{
		return true;
	}

	DAtom Rec = SelectBomb(MaxBomb);
	if (Rec)
	{
		// �ܱ��ջ�
		return true;
	}
	return false;
}

DAtom DNormalAI::Peek_Bomb_CanRec(DCardNumber Number) const
{
	
	DAtom Bomb = SelectBomb(Number);
	while (Bomb)
	{
		if (IsBombCanRec(Bomb.Number))
		{
			return Bomb;
		}

		// ѡ������ը��
		Bomb = SelectBomb(Bomb.Number);
	}
	return Bomb;
}

/**
* �ж��߼�:
����������. 
   ��ȥ3���ĸ��ӻ����. 
   Ȼ��, ���ֻ��2��, ��ô��Ȼһ����������. �����, ���Ǵ������. 
*/
DGameAIHand DNormalAI::Play_Hand_To_Escape(const BYTE* CardSlots) const
{
	int NumHands = GetRawHandNum();
	int NumX = NumXNeedFor_3x_And_Plane();

	if (m2K)
	{
		NumHands--;
	}

	if (NumX > 1)
	{
		//Ŀǰ ��������ֻ��һ��3�������.
		return DGameAIHand::PASS;
	}else if (NumX == 1)
	{
		NumHands--;
	}

	if (NumHands > 2)
	{
		// ʣ���������ڿ��Ƿ�Χ֮��. 
		return DGameAIHand::PASS;
	}

	// ���Կ����Ƿ���һ��������, �����, ��������. 
	for (int i = 0; i < m1Atoms.size(); ++i)
	{
		const DAtom& A = m1Atoms[i];
		int Greater = mDeck->AI_Determ_Single_Max(mCardSlots, A.Number, true);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	for (int i = 0; i < m2Atoms.size(); ++i)
	{
		const DAtom& A = m2Atoms[i];
		int Greater = mDeck->AI_Determ_Double_Greater(mCardSlots, A.Number);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
	}

	for (int i = 0; i < m3Atoms.size(); ++i)
	{
		const DAtom& A = m3Atoms[i];
		int Greater = mDeck->AI_Determ_3_Greater(mCardSlots, A.Number);
		if (Greater == 0)
		{
			DGameAIHand AIHand = Peek_3_x();
			if (AIHand)
			{
				return AIHand;
			}
		}
	}

	for (int i = 0; i < m4Atoms.size(); ++i)
	{
		const DAtom& A = m4Atoms[i];
		int Greater = mDeck->AI_QueryRemainBombers(mCardSlots, A.Number);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_4, A.Number, 4);
		}
	}

	for (int i = 0; i < m1SAtoms.size(); ++i)
	{
		const DAtom& A = m1SAtoms[i];
		int Greater = mDeck->AI_Determ_S_Greater(mCardSlots, A.Number, A.Len);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_straight, A.Number, A.Len);
		}
	}

	for (int i = 0; i < m2SAtoms.size(); ++i)
	{
		const DAtom& A = m2SAtoms[i];
		int Greater = mDeck->AI_Determ_2S_Greater(mCardSlots, A.Number, A.Len);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_straight_2, A.Number, A.Len);
		}
	}



	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Play_Hand_Normal() const
{
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::TryEscape(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	// �����Լ���. 
	const int MyHandNum = GetHandNum();
	if (MyHandNum == 1)
	{
		A = Select_Ascend(m1Atoms, FollowNumber);
		if (A.IsValid())
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}

		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return DGameAIHand(A.Len == 4 ? DHT_4 : DHT_2K, A.Number, A.Len);
		}

		// �Ӷ�����ѡ. 
		A = Select_Ascend(m2Atoms, FollowNumber);
		if (A && mDeck->AI_QueryMaxCard() == A.Number)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}
	else if (MyHandNum == 2)
	{
		if (m1Atoms.size() == 2)
		{
			// ʣ����������.
			const DAtom& AMax = m1Atoms[0];
			const DAtom& AMin = m1Atoms[1];
			DCardNumber MaxCardNumber = mDeck->AI_QueryMaxCard(mCardSlots);

			if (AMin.Number > FollowNumber)
			{
				if (AMin.Number >= MaxCardNumber)
				{
					// С�����ܳ�, ���޷��������ջ�. 
					return DGameAIHand(DHT_1, AMin.Number, 1);
				}
				else if (AMax.Number > FollowNumber)
				{
					return DGameAIHand(DHT_1, AMax.Number, 1);
				}
				else
				{
					return DGameAIHand(DHT_1, AMin.Number, 1);
				}
			}
			else if (AMax.Number > FollowNumber)
			{
				// TODO : ����
				return DGameAIHand(DHT_1, AMax.Number, 1);
			}

			return DGameAIHand::PASS;
		}

		// ʣ����Ʋ�����������. 
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			// ����һ������ը��
			if (A.Len == 2)
			{
				// 2K.
				return DGameAIHand(DHT_2K, DCN_RED_JOKER, 2);
			}else if (mDeck->AI_Bomb(mCardSlots, A.Number))
			{
				// ��ը��
				return DGameAIHand(DHT_4, A.Number, 4);
			}
		}

		DAIPotentialHandList HandList;

		A = Peek1_FromStraight(FollowNumber);		// ��˳����ѡ. 
		if (A)
		{
			HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_1S));
		}
		A = Peek1_FromDouble(FollowNumber);	// �Ӷ�����ѡ. 
		if (A)
		{
			HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_2));
		}
		A = Select_Ascend(m1Atoms, FollowNumber);
		if (A)
		{
			HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_1));
		}

		if (!HandList.IsEmpty())
		{
			DCardNumber MaxCardNumber = mDeck->AI_QueryMaxCard(mCardSlots);

			for (int index = 0; index < (int)HandList.Size(); ++index)
			{
				DAIPotentialHand& ph = HandList[index];
				if (ph.Hand.Number >= MaxCardNumber)
				{
					ph.Weight += 100000;
				}

				if (ph.Source == HS_1)
				{
					ph.Weight += 1000;
				}

				if (ph.Source == HS_1S)
				{
					ph.Weight += 100;
				}
			}

			PHSort(HandList);

			if (HandList[0].Weight >= 100000)
			{
				return HandList[0].Hand;
			}
			
		}

	
	}

	return DGameAIHand::PASS;
}

/**
 * �Ӷ���, �����Լ����� 
 */
DGameAIHand DNormalAI::TryEscape_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	// �����Լ���. 
	const int MyHandNum = GetHandNum();
	if (MyHandNum == 1)
	{
		// �����ҵ�һ�����Ӵ��
		A = Select_Ascend(m2Atoms, FollowNumber);
		if (A.IsValid())
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		// ʣ���һ����ը��
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}

		// �Ƿ�ʣ���Ϊ3��? 
		A = Select_Ascend(m3Atoms, FollowNumber);
		if (A)
		{
			// ʣ����3����ʱ��, �ز�. (��Ϊһ����˵, �Է������ƻ���3X1,3X2,)
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		A = Peek2_FromDoubleStaight(FollowNumber);
		if (A)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		DCardNumber Number = Peek2_Force(FollowNumber);
		if (Number != DCN_NULL)
		{
			return DGameAIHand(DHT_2, Number, 2);
		}
		return DGameAIHand::PASS;
	}
	else if (MyHandNum == 2)
	{
		if (m2Atoms.size() == 2)
		{
			// ʣ�����2������. 
			const DAtom& AMax = m2Atoms[0];
			const DAtom& AMin = m2Atoms[1];

			bool MinIsMax = mDeck->AI_Determ_Double_Is_Max(mCardSlots, AMin.Number);
			bool MaxIsMax = mDeck->AI_Determ_Double_Is_Max(mCardSlots, AMax.Number);

			if (MinIsMax && AMin.Number > FollowNumber)
			{
				return DGameAIHand(DHT_2, AMin.Number, 2);
			}
			else if (MaxIsMax && AMax.Number > FollowNumber)
			{
				return DGameAIHand(DHT_2, AMax.Number, 2);
			}
			
			return DGameAIHand::PASS;
		}


		// ѡ�����е�һ������(����һ��)
		A = Select_Descend(m2Atoms, FollowNumber);
		if (A && mDeck->AI_Determ_Double_Is_Max(mCardSlots, A.Number))
		{
			// ���AI��Ϊ����Ǿ��Ե�, ���.
			return DGameAIHand(DHT_2, A.Number, 2);
		}

		// ��3������ѡ.
		A = Select_Descend(m3Atoms, FollowNumber);
		if (A && mDeck->AI_Determ_Double_Is_Max(mCardSlots, A.Number))
		{
			// ���AI��Ϊ����Ǿ��Ե�, ���.
			return DGameAIHand(DHT_2, A.Number, 2);
		}

	
		// ʣ����Ʋ�����������, �����Ƿ���ը��
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			// ����һ������ը��
			if (A.Len == 2)
			{
				// 2K.
				return DGameAIHand(DHT_2K, DCN_RED_JOKER, 2);
			}
			else if (mDeck->AI_Bomb(mCardSlots, A.Number))
			{
				// ��ը��
				return DGameAIHand(DHT_4, A.Number, 4);
			}
		}

		// ��˫˳��ѡ
		A = Peek2_FromDoubleStaight(FollowNumber);
		if (A && mDeck->AI_Determ_Double_Is_Max(mCardSlots, A.Number))
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}

	}

	return DGameAIHand::PASS;
}

/**
 * ���Ը�һ���ƺ�, ʣ�������ֱ��ʤ�� 
 */
DGameAIHand DNormalAI::TryEscape_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	// �����Լ���. 
	int MyHandNum = GetHandNum();
	MyHandNum += NumXNeedFor_3x_And_Plane();	// ��Ҫ��ԭ.

	if (MyHandNum == 1)
	{
		// �����ҵ�һ��3�����
		A = Select_Ascend(m3Atoms, FollowNumber);
		if (A.IsValid())
		{
			return DGameAIHand(DHT_3, A.Number, 3);
		}
		// ʣ���һ����ը��
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}

		return DGameAIHand::PASS;
	}
	else if (MyHandNum == 2)
	{
		// ʣ��2��3��?????
		if (m3Atoms.size() == 2)
		{
			// ���Դ�����Դ��. 
			const DAtom& AMax = m3Atoms[0];
			const DAtom& AMin = m3Atoms[1];
			bool MinIsMax = mDeck->AI_Determ_3_Is_Max(mCardSlots, AMin.Number);
			bool MaxIsMax = mDeck->AI_Determ_3_Is_Max(mCardSlots, AMax.Number);
			if (MinIsMax && AMin.Number > FollowNumber)
			{
				return DGameAIHand(DHT_3, AMin.Number, 3);
			}
			else if (MaxIsMax && AMax.Number > FollowNumber)
			{
				return DGameAIHand(DHT_3, AMax.Number, 3);
			}

			// û�о��Ե���, �Ȳ�Ҫ��. 
			return DGameAIHand::PASS;
		}

		// ѡ�����е�һ��3��
		A = Select_Descend(m3Atoms, FollowNumber);
		if (A && mDeck->AI_Determ_3_Is_Max(mCardSlots, A.Number))
		{
			// ���AI��Ϊ����Ǿ��Ե�, ���.
			return DGameAIHand(DHT_3, A.Number, 3);
		}

		// ����ը��
		A = SelectBomb(DCN_NULL);
		if (A && mDeck->AI_Bomb(mCardSlots, A.Number))
		{
			// ����һ������ը��
			return MakeBomb(A);
		}

		// ��3˳��ѡ
		A = Peek3_From3S(FollowNumber, false);
		if (A && mDeck->AI_Determ_3_Is_Max(mCardSlots, A.Number))
		{
			return DGameAIHand(DHT_3, A.Number, 3);
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::TryEscape_3X1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	DGameAIHand AIHand;
	int MyHandNum = GetHandNum();
	//MyHandNum += NumXNeedFor_3x_And_Plane();	// ��Ҫ��ԭ.
	MyHandNum += 1;
	if (MyHandNum <= 1)
	{
		AIHand = Peek_3_1(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		// ʣ���һ����ը��
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}

		return DGameAIHand::PASS;
	}
	else if(MyHandNum == 2) 
	{
		// ����ը��
		A = SelectBomb(DCN_NULL);
		if (A && mDeck->AI_Bomb(mCardSlots, A.Number))
		{
			// ����һ������ը��
			return MakeBomb(A);
		}

		AIHand = Peek_3_1(FollowNumber);
		if (AIHand && mDeck->AI_Determ_3_Is_Max(mCardSlots, AIHand.Number))
		{
			return AIHand;
		}
	}
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::TryEscape_3X2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	const DCardNumber FollowNumber = ToFollow->GetHandNumber();
	DAtom A;
	DGameAIHand AIHand;
	int MyHandNum = GetHandNum();
	//MyHandNum += NumXNeedFor_3x_And_Plane();	// ��Ҫ��ԭ.
	MyHandNum += 1;
	if (MyHandNum <= 1)
	{
		AIHand = Peek_3_2(FollowNumber);
		if (AIHand)
		{
			return AIHand;
		}

		// ʣ���һ����ը��
		A = SelectBomb(DCN_NULL);
		if (A)
		{
			return MakeBomb(A);
		}

		return DGameAIHand::PASS;
	}
	else if (MyHandNum == 2)
	{
		// ����ը��
		A = SelectBomb(DCN_NULL);
		if (A && mDeck->AI_Bomb(mCardSlots, A.Number))
		{
			// ����һ������ը��
			return MakeBomb(A);
		}

		AIHand = Peek_3_2(FollowNumber);
		if (AIHand && mDeck->AI_Determ_3_Is_Max(mCardSlots, AIHand.Number))
		{
			return AIHand;
		}
	}
	return DGameAIHand::PASS;
}

DAtom DNormalAI::Peek1_FromStraight(DCardNumber Number) const
{
	if (m1SAtoms.empty())
	{
		return DAtom();
	}

	//ֻ��˳�ӴӲ����С��.
	for (int Index = m1SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& a = m1SAtoms[Index];
		if (a.Len > 5)
		{
			DCardNumber LastNumber = a.GetStraightLast();
			if (LastNumber > Number)
			{
				// �ɲ�. 
				return DAtom(LastNumber, 1);
			}
			
		}
	}

	return DAtom();
}

DAtom DNormalAI::Peek1_FromStraightLessThen(DCardNumber Number) const
{
	if (m1SAtoms.empty())
	{
		return DAtom();
	}

	//ֻ��˳�ӴӲ����С��.
	for (int Index = 0; Index < m1SAtoms.size(); ++Index)
	{
		const DAtom& a = m1SAtoms[Index];
		if (a.Len > 5 && a.Number < Number)
		{
			return DAtom(a.Number, 1);
		}
	}

	return DAtom();
}

DAtom DNormalAI::Peek1_FromDouble(DCardNumber Number) const
{
	if (m2Atoms.empty())
	{
		return DAtom();
	}
	for (int Index = (int)m2Atoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& a = m2Atoms[Index];
		if (a.Number > Number)
		{
			return DAtom(a.Number, 1);
		}
	}
	
	return DAtom();
}

DAtom DNormalAI::Peek1_FromDoubleLessThen(DCardNumber Number) const
{
	if (m2Atoms.empty())
	{
		return DAtom();
	}
	for (int Index = 0; Index < m2Atoms.size(); ++Index)
	{
		const DAtom& a = m2Atoms[Index];
		if (a.Number < Number)
		{
			return DAtom(a.Number, 1);
		}
	}
	return DAtom();
}

DAtom DNormalAI::Peek1_From3(DCardNumber Number) const
{
	if (m3Atoms.empty())
	{
		return DAtom();
	}

	for (int Index = m3SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& A = m3Atoms[Index];
		if (A.Number > Number)
		{
			return A;
		}
	}
	return DAtom();
}



DGameAIHand DNormalAI::GetHand_1_ToFollow_CareX(DCardNumber Number) const
{
	// û�и���. 
	if (!m1Atoms.size())
	{
		return DGameAIHand::PASS;
	}

	//���ĸ���Ҳ����
	if (m1Atoms[0].Number <= Number)
	{
		return DGameAIHand::PASS;
	}

	// 
	int NumSinglesNeedForX = NumXNeedFor_3x_And_Plane();
	NumSinglesNeedForX -= (int)m2Atoms.size();

	if (NumSinglesNeedForX > m1Atoms.size())
	{
		DAtom A = Select_Ascend(m1Atoms, Number);
		if (A)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::GetHand_1_ToFollow_DontCareX(DCardNumber Number) const
{
	// û�и���. 
	if (!m1Atoms.size())
	{
		return DGameAIHand::PASS;
	}

	//���ĸ���Ҳ����
	if (m1Atoms[0].Number <= Number)
	{
		return DGameAIHand::PASS;
	}

	DAtom A = Select_Ascend(m1Atoms, Number);
	if (A)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}
	return DGameAIHand::PASS;
}

int DNormalAI::GetBombRateByHand() const
{
	int NumHands = GetHandNum();
	if (NumHands <= 1)
	{
		return 100;
	}else if (NumHands <= 2)
	{
		return 90;
	}else if (NumHands <= 3 )
	{
		return 40;
	}

	return 30;
}
DGameAIHand DNormalAI::FHand1_Normal(DCardNumber Number, bool AllowBomb) const
{
	DGameAIHand AIHand;

	if (AIHand = GetHand_1_ToFollow_CareX(Number))
	{
		return AIHand;
	}

	DAtom A = Select_Ascend(m1Atoms, Number);
	if (A)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}
	
	// �Ƚϴ�˳����ѡ��Ľ��
	A = Peek1_FromStraight(Number);
	if (A)
	{
		return DGameAIHand(DHT_1, A.Number, 1);
	}


	//DAtom A2 = Peek1_FromDouble(Number);

	//DCardNumber BestNumber = A ? A.Number : DCN_NULL;
	//if (AS && AS.Number < BestNumber)
	//{
	//	BestNumber = AS.Number;
	//}
	//if (A2 && A2.Number < (BestNumber - 1))
	//{
	//	// HACK @@@@@ 

	//	BestNumber = A2.Number;
	//}

	//if (BestNumber != DCN_NULL)
	//{
	//	return DGameAIHand(DHT_1, BestNumber, 1);
	//}

	//if (AllowBomb)
	//{
	//	A = SelectBomb(DCN_NULL);
	//	if (A)
	//	{
	//		int BombRate = 0;
	//		if (IsBombCanRec(A.Number))
	//		{
	//			BombRate = 100;
	//		}
	//		else
	//		{
	//			BombRate = GetBombRateByHand();
	//			int NumBombers = mDeck->AI_QueryRemainBombers(mCardSlots, A.Number);
	//			if (NumBombers == 0)
	//			{
	//			}else if (NumBombers == 1)
	//			{
	//				BombRate = (int)(BombRate * 0.9);
	//			}else if (NumBombers == 2)
	//			{
	//				BombRate = (int)(BombRate * 0.8);
	//			}else if (NumBombers == 3)
	//			{
	//				BombRate = (int)(BombRate * 0.6);
	//			}
	//			else
	//			{
	//				BombRate = (int)(BombRate * 0.2);
	//			}
	//		}

	//		if (Chance(BombRate))
	//		{
	//			return MakeBomb(A);
	//		}
	//		
	//	}
	//}

	return DGameAIHand::PASS;
}






void DNormalAI::Peek_1(DAIPotentialHandList& HandList, DCardNumber Number, UINT Flags) const
{
	UINT PeekSource = Flags & PEEK_SOURCE_MASK;
	DAtom A;
	if (PeekSource & HS_1)
	{
		for (int Index = m1Atoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& A = m1Atoms[Index];
			if (A.Number > Number)
			{
				HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_1));
			}
		}
	}

	if (PeekSource & HS_2)
	{
		for (int Index = (int)m2Atoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& A = m2Atoms[Index];
			if (A.Number > Number)
			{
				HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_2));
			}
		}
	}

	if (PeekSource & HS_3)
	{
		for (int Index = m3SAtoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& A = m3Atoms[Index];
			if (A.Number > Number)
			{
				HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_3));
			}
		}
	}

	if (PeekSource & HS_1S)
	{
		for (int Index = m1SAtoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& A = m1SAtoms[Index];
			if (A.Len > 5)
			{
				DCardNumber LastNumber = A.GetStraightLast();
				if (LastNumber > Number)
				{
					// �ɲ�. 
					HandList.PushBack(DAIPotentialHand(DHT_1, A.Number, 1, HS_1S));
				}

			}
		}
	}




	if (Flags & PEEK_FLAG_BOMB)
	{
		for (int Index = m4Atoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& A = m4Atoms[Index];
			HandList.PushBack(DAIPotentialHand(DHT_4, A.Number, 4, HS_4));
		}

		if (m2K)
		{
			HandList.PushBack(DAIPotentialHand(DHT_2K, DCN_RED_JOKER, 2, HS_4));
		}
	}

}

DAtom DNormalAI::Peek2_LessThen(DCardNumber Number) const
{
	if (m2Atoms.empty())
	{
		return DAtom();
	}

	for (int Index = (int)m2Atoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& a = m2Atoms[Index];
		if (a.Number < Number)
		{
			return a;
		}
	}
	return DAtom();
}


DAtom DNormalAI::Peek2_FromDoubleStaight(DCardNumber Number) const
{
	if (m2SAtoms.empty())
	{
		return DAtom();
	}
	for (int Index = (int)m2SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& S2 = m2SAtoms[Index];
		DCardNumber LastNumber = S2.GetStraightLast();
		if (S2.Len > 3 && LastNumber > Number)
		{
			return DAtom(LastNumber, 2);
		}
	}
	
	return DAtom();
}

DAtom DNormalAI::Peek2_FromDoubleStaight_LessThen(DCardNumber Number) const
{
	if (m2SAtoms.empty())
	{
		return DAtom();
	}
	for (int Index = 0; Index < m2SAtoms.size(); ++Index)
	{
		const DAtom& S2 = m2SAtoms[Index];
		
		if (S2.Len > 3 && S2.Number < Number)
		{
			return DAtom(S2.Number, 2);
		}
	}

	return DAtom();
}

DCardNumber DNormalAI::Peek2_Force(DCardNumber Number, bool Ascend) const
{
	if (Ascend)
	{
		for (int i = Number + 1; i <= DCN_2; ++i)
		{
			if (mCardSlots[i] >= 2)
			{
				return (DCardNumber)i;
			}
		}
	}
	else
	{
		for (int i = DCN_2; i > Number; --i)
		{
			if (mCardSlots[i] >= 2)
			{
				return (DCardNumber)i;
			}
		}
	}
	

	return DCN_NULL;
}


DGameAIHand DNormalAI::FHand2_Normal(DCardNumber FollowNumber, bool AllowBomb) const
{
	DGameAIHand AIHand;

	// �Ӷ�����ѡ��
	DAtom A = Select_Ascend(m2Atoms, FollowNumber);
	if (A.IsValid())
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}

	// ��������ѡ��
	A = Peek2_FromDoubleStaight(FollowNumber);
	if (A)
	{
		return DGameAIHand(DHT_2, A.Number, 2);
	}

	// ��3����ѡ???
	// 

	A = SelectBomb(DCN_NULL);


	return DGameAIHand::PASS;
}



DCardNumber DNormalAI::FH_Peek_X1_For_3_1(bool SpliteDouble) const
{
	int NumHands = GetHandNum();
	const DCardNumber Number = mDeck->AI_QueryMaxCard(mCardSlots);
	VPODArray<DAtom> Atoms;

	// �ȴӸ����в���. 
	DAtom A = Select_Ascend(m1Atoms, DCN_NULL);
	if (A)
	{
		if (A.Number < DCN_K)
		{
			return A.Number;
		}
		/*int NumGreater = mDeck->AI_Determ_Single_Max(mCardSlots, A.Number);
		if (NumGreater >= 6 && A.Number < DCN_A)
		{
			return A.Number;
		}*/
	}


	DAtom A2 = Peek1_FromStraight(DCN_NULL);
	if (A2)
	{
		if ( (A &&  A2.Number < (A.Number - 2)) || !A)
		{
			A = A2;
		}
		
	}

	if (SpliteDouble)
	{
		A2 = Peek1_FromDouble(DCN_NULL);
		if (A2)
		{
			if ((A &&  A2.Number < (A.Number - 2)) || !A)
			{
				A = A2;
			}
		}
	}
	

	if (A)
	{
		return A.Number;
	}

	return DCN_NULL;
}

DCardNumber DNormalAI::FH_Peek_X2_For_3_2() const
{
	DAtom A = Select_Ascend(m2Atoms, DCN_NULL);
	if (A)
	{
		if (A.Number < DCN_J)
		{ 
			return A.Number;
		}
		
	}
	DAtom A2 = Peek2_FromDoubleStaight(DCN_NULL);
	if (A2)
	{
		if ((A &&  A2.Number <= (A.Number - 1)) || !A)
		{
			A = A2;
		}
	}
	return A.Number;
}

// ѡ��һ��3��1 ����3��2 ����. 
DGameAIHand DNormalAI::Peek_3_x() const
{
	if (m3Atoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom A3 = Select_Ascend(m3Atoms, DCN_NULL);		// 3��2 , 3��A ?????? 

	DCardNumber X1 = FH_Peek_X1_For_3_1(false);
	DCardNumber X2 = FH_Peek_X2_For_3_2();

	if (X1 == DCN_NULL && X2 == DCN_NULL)
	{
		return DGameAIHand(DHT_3, A3.Number, 3);
	}

	if (X1 == DCN_NULL || X2 == DCN_NULL)
	{
		if (X1 != DCN_NULL)
		{
			return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(X1, 1));
		}else if (X2 != DCN_NULL)
		{
			return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(X2, 2));
		}
	}

	// x1 ,x2 ����Ч
	if (X1 >= X2)
	{
		return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(X2, 2));
	}else if (X1 < X2)
	{
		return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(X1, 1));
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Peek_3_1(DCardNumber FollowNumber) const
{
	if (m3Atoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom A3 = Select_Ascend(m3Atoms, FollowNumber);
	if (!A3)
	{
		return DGameAIHand::PASS;
	}

	
	// �ҵ�һ�����ʵĸ���. 
	DCardNumber X1 = FH_Peek_X1_For_3_1(true);
	if (X1 != DCN_NULL)
	{
		return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(X1,1));
	}
	
	return DGameAIHand::PASS;
}


DGameAIHand DNormalAI::Peek_3_2(DCardNumber FollowNumber) const
{
	if (m3Atoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom A3 = Select_Ascend(m3Atoms, FollowNumber);
	if (!A3)
	{
		return DGameAIHand::PASS;
	}


	// �ҵ�һ�����ʵĶ��� 
	DCardNumber X2= FH_Peek_X2_For_3_2();
	if (X2 != DCN_NULL)
	{
		return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(X2, 2));
	}

	return DGameAIHand::PASS;
}

// �ɴ�����ѡ��һ��������3_1
DGameAIHand DNormalAI::Peek_3_1_Descend(DCardNumber FollowNumber) const
{
	if (m3Atoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom A3 = Select_Descend(m3Atoms, FollowNumber);
	if (!A3)
	{
		return DGameAIHand::PASS;
	}


	// �ҵ�һ�����ʵĸ���. 
	DCardNumber X1 = FH_Peek_X1_For_3_1(true);
	if (X1 != DCN_NULL)
	{
		return DGameAIHand(DHT_3_1, A3.Number, 4, DAIXInfo(X1, 1));
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Peek_3_2_Descend(DCardNumber FollowNumber) const
{
	if (m3Atoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom A3 = Select_Descend(m3Atoms, FollowNumber);
	if (!A3)
	{
		return DGameAIHand::PASS;
	}


	// �ҵ�һ�����ʵĶ��� 
	DCardNumber X2 = FH_Peek_X2_For_3_2();
	if (X2 != DCN_NULL)
	{
		return DGameAIHand(DHT_3_2, A3.Number, 5, DAIXInfo(X2, 2));
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Peek_3_straight() const
{
	if (m3SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}
	
	DAtom A3S = Select_Ascend(m3SAtoms, DCN_NULL);
	if (!A3S)
	{
		return DGameAIHand::PASS;
	}

	int x = A3S.Len;
	if (x > 4)
	{
		x = 4; // �������֧��4�����ķɻ�.... ���� DGameAIHand ����Ϣ��������. 
	}
	
	// 
	DGameAIHand AIHand;
	AIHand.Type = DHT_straight_3;
	AIHand.Number = A3S.Number;
	AIHand.Len = (int)x;
	VASSERT(AIHand.Len >= 2 && AIHand.Len <= 4);

	// ���ȳ��Դ�����
	if (Peek_3_straight_1(AIHand.X, x))
	{
		//AIHand.Len = (int)(A3S.Len * 3) + x;
		return AIHand;
	}
	// ���Դ�����
	if (Peek_3_straight_2(AIHand.X, x))
	{
		//AIHand.Len = (int)(A3S.Len * 3) + x*2;
		return AIHand;
	}
	// ���Դ�˳�����ó�������. 
	if (Peek_3_straight_1_s(AIHand.X, x))
	{
		//AIHand.Len = (int)(A3S.Len * 3) + x * 2;
		return AIHand;
	}

	// ���Բ�˫˳
	if (Peek_3_straight_2_s(AIHand.X, x))
	{
		//AIHand.Len = (int)(A3S.Len * 3) + x * 2;
		return AIHand;
	}


	if (Peek_3_straight_1_2(AIHand.X, x))
	{
		//AIHand.Len = (int)(A3S.Len * 3) + x ;
		return AIHand;
	}


	// û�г��ķɻ�
	AIHand.X[0].X = DCN_NULL;
	AIHand.X[0].XLen = 0;
	//AIHand.Len = (int)(A3S.Len * 3);
	return AIHand;
}

bool DNormalAI::Peek_3_straight_1(DAIXInfo* XInfos, int x) const
{
	// Note: ���Ƕ�AI����û�г��ķɻ�. 
	int NumK2 = 0;
	int NumSingleNK2 = GetSingleNum_Not_KingOr2();

	if (NumSingleNK2 >= x)
	{

		for (int Index = 0; Index < x; ++Index)
		{
			int XIndex = m1Atoms.size() - Index - 1;
			const DAtom& X1 = m1Atoms[XIndex];
			XInfos[Index].X = X1.Number;
			XInfos[Index].XLen = 1;
		}

		return true;
	}

	// ������2�ĵ����޷���������. 
	NumK2 = m1Atoms.size() - NumSingleNK2;
	if (!NumK2)
	{
		return false;
	}

#if 0
	// ������2. 
	if ((m1Atoms.size() == x) &&				// �����պ���ȫ������. 
		(mHand.Size() == H3S->Size() + x)		// ��һ���Գ���.  ???
		)
	{
		goto found;
	}
#endif 
	// TODO �������һ���Գ�������, ����һ������, ģ��ģ���߼�, ��AI�л�����Ȼ����2����. 


	return false;
}

bool DNormalAI::Peek_3_straight_1_2(DAIXInfo* XInfos, int x) const
{
#if 0
	// ���ȴ�˳���в��ҿ��ܰ������������. 
	if (mDoubleHands.IsEmpty())
	{
		return NULL;
	}



	int NumValidSingle = mDoubleHands.Size() * 2;	// �ɰ����˳�ӵ�������
	int NumFromSingle = mSingleHand.Size();
	if ((NumValidSingle + NumFromSingle) < x)
	{
		return NULL;
	}

	int NumValidSingleN2 = NumValidSingle;
	if (mDoubleHands[0]->GetHandNumber() == DCN_2)
	{
		// ��������д���2. 
		NumValidSingleN2 -= 2;
	}



	const int NumSingleNK2 = GetSingleNum_Not_KingOr2();
	const int NumK2 = mSingleHand.Size() - NumSingleNK2;

	VASSERT(NumSingleNK2 < x);		// �����ٵ����������������������. 

	int NumHands = 0;

	if ((NumSingleNK2 + NumValidSingleN2) >= x)
	{
		// �����ͷ�2�ĸ�������ɷɻ����
		NumFromSingle = NumSingleNK2;
		goto found;
	}

	NumHands = GetHandNum();

	//����, ������Ҫ�Ѷ��ӻ��߸����е�2����KING ������ȥ.
	if (!NumK2)
	{
		// ������û��king,2, 
		if (NumValidSingleN2 != NumValidSingle)
		{
			// �����д���2. ��ô������Ҫ�����е�2���ֲ�. 

			// �������ը��, ���2. 
			if (!m4Hands.IsEmpty() || m2KHand)
			{
				NumFromSingle = mSingleHand.Size();
				goto found;
			}

			int RemainHands = NumHands - mSingleHand.Size();
			RemainHands -= (int)mDoubleHands.Size();
			RemainHands -= 1;		// ONE FOR 3 STRAIGHT.
			if (RemainHands > 2)
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		NumFromSingle = NumSingleNK2;
		goto found;
	}

	return NULL;

found:
	DHand* Result = VNew DHand(DHT_straight_3, H3S);
	DHand Single;

	for (int Index = (int)mSingleHand.Size() - NumFromSingle; Index < mSingleHand.Size(); ++Index)
	{
		Single.AddCard(mSingleHand[Index]);
	}

	int NumFromSplice = x - NumFromSingle;
	VASSERT(NumFromSplice > 0);
	for (int Index = (mDoubleHands.Size() - 1); Index >= 0; --Index)
	{
		DHand* h2 = mDoubleHands[Index];
		int taken = 2;
		if (taken > NumFromSplice)
		{
			taken = NumFromSplice;
		}
		VASSERT(taken > 0);
		Single.AddCards(h2->GetCards(), taken);
		NumFromSplice -= taken;
		if (NumFromSplice == 0)
		{
			break;
		}
	}

	VASSERT(Single.Size() == x);
	Single.Sort();
	Result->AddCards(Single.GetCards(), Single.Size());
	return Result;
#endif 
	return false;
}
bool DNormalAI::Peek_3_straight_1_s(DAIXInfo* XInfos, int x) const	// �ɻ�. �ӵ�����˳�������
{
	// ���ȴ�˳���в��ҿ��ܰ������������. 
	if (m1SAtoms.empty())
	{
		return false;
	}
	
	int NumValidSingle = 0;	// �ɰ����˳�ӵ�������
	for (int Index = (m1SAtoms.size() - 1); Index >= 0; --Index)
	{
		const DAtom& s1 = m1SAtoms[Index];
		if (s1.Len > 5)
		{
			NumValidSingle += (s1.Len - 5);
		}
	}
	if (NumValidSingle == 0)
	{
		return false;
	}

	const int NumSingleNK2 = GetSingleNum_Not_KingOr2();
	const int NumK2 = m1Atoms.size() - NumSingleNK2;
	int NumFromSingle = m1Atoms.size();
	VASSERT(NumSingleNK2 < x);		// �����ٵ�������������������˳��. 
	if ((NumSingleNK2 + NumValidSingle) >= x)
	{
		NumFromSingle = NumSingleNK2;
		goto found;
	}

	// û��k,2, �ҵ����Ϳɰ���Ķ�������ɷɻ�.
	if (!NumK2)
	{
		return false;
	}

	// ��k,2. 
	if ((m1Atoms.size() + NumValidSingle) >= x)
	{
		// ��k,2.����k,2����ɷɻ� ���Ǻ���Ȼ, ���Ǵ�˳���в����������, ��˿϶�������һ���Գ��굱ǰ����.
		int crazy = AIRand(10);
		if (crazy > 6)
		{
			// ����,2 ����ȥ. 
			NumFromSingle = m1Atoms.size();
			goto found;
		}
	}
	return false;

found:

	int xIndex = 0;

	for (int Index = (int)m1Atoms.size() - NumFromSingle; Index < m1Atoms.size(); ++Index)
	{
		XInfos[xIndex].X = m1Atoms[Index].Number;
		XInfos[xIndex].XLen = 1;
		xIndex++;
	}

	int NumFromSplice = x - NumFromSingle;
	if (NumFromSplice)
	{
		VASSERT(NumFromSplice >= 0);
		for (int Index = (m1SAtoms.size() - 1); Index >= 0; --Index)
		{
			const DAtom& A = m1SAtoms[Index];
			if (A.Len > 5)
			{
				int taken = A.Len - 5;
				if (taken > NumFromSplice)
				{
					taken = NumFromSplice;
				}
				VASSERT(taken > 0);

				DCardNumber taken_number;
				for (int ti = 0; ti < taken; ++ti)
				{
					taken_number = (DCardNumber)(A.Number + A.Len - 1 - ti);
					XInfos[xIndex].X = taken_number;
					XInfos[xIndex].XLen = 1;
					xIndex++;
					VASSERT(xIndex <= x);
				}

				NumFromSplice -= taken;
				if (NumFromSplice == 0)
				{
					break;
				}
			}
		}
	}
	
	VASSERT(xIndex == x);
	return true;

	
}
bool DNormalAI::Peek_3_straight_2(DAIXInfo* XInfos, int x) const		// �Ӷ��������
{
	int Num2 = 0;
	int NumN2 = GetDoubleNum_Not_2();
	int HandNum = GetHandNum();
	if (NumN2 >= x   || (m2Atoms.size() == x && HandNum < 3) )
	{
		for (int Index = 0; Index < x; ++Index)
		{
			int XIndex = m2Atoms.size() - Index - 1;
			const DAtom& X2 = m2Atoms[XIndex];
			XInfos[Index].X = X2.Number;
			XInfos[Index].XLen = 2;
		}
		return true;
	}
	return false;
}
bool DNormalAI::Peek_3_straight_2_s(DAIXInfo* XInfos, int x) const	// ��˫˳�Ͷ��������
{
	// ���ȴ�˳���в��ҿ��ܰ������������. 
	if (m2SAtoms.empty())
	{
		return false;
	}

	int NumValid = 0;	// �ɰ����˳�ӵ�������
	for (int Index = (m2SAtoms.size() - 1); Index >= 0; --Index)
	{
		const DAtom& A = m2SAtoms[Index];
		if (A.Len > 3)
		{
			NumValid += (A.Len - 3);
		}
	}
	if (NumValid == 0)
	{
		return false;
	}

	const int NumN2 = GetDoubleNum_Not_2();
	const int Num2 = m2Atoms.size() - NumN2;
	int NumFromRaw = m2Atoms.size();
	VASSERT(NumN2 < x);		// 
	if ((NumN2 + NumValid) >= x)
	{
		NumFromRaw = NumN2;
		goto found;
	}

	// û��k,2, �ҵ����Ϳɰ���Ķ�������ɷɻ�.
	if (!Num2)
	{
		return false;
	}

	//// ��k,2. 
	//if (((int)mDoubleHands.Size() + NumValid) >= x)
	//{
	//	// ��k,2.����k,2����ɷɻ� ���Ǻ���Ȼ, ���Ǵ�˳���в����������, ��˿϶�������һ���Գ��굱ǰ����.
	//	int crazy = rand() % 10;
	//	if (crazy > 7)
	//	{
	//		// ����,2 ����ȥ. 
	//		NumFromRaw = mDoubleHands.Size();
	//		goto found;
	//	}
	//}
	//return NULL;

found:

	int xIndex = 0;
	for (int Index = (int)m2Atoms.size() - NumFromRaw; Index >= 0; --Index)
	{
		const DAtom& A2 = m2Atoms[Index];
		XInfos[xIndex].X = A2.Number;
		XInfos[xIndex].XLen = 2;
		xIndex++;
	}

	int NumFromSplice = x - NumFromRaw;
	VASSERT(NumFromSplice > 0);
	if (NumFromSplice)
	{
		for (int Index = (m2SAtoms.size() - 1); Index >= 0; --Index)
		{
			const DAtom& A2 = m2SAtoms[Index];
			if (A2.Len > 3)
			{
				int taken = (A2.Len - 3);
				if (taken > NumFromSplice)
				{
					taken = NumFromSplice;
				}
				VASSERT(taken > 0);

				DCardNumber taken_number;
				for (int ti = 0; ti < taken; ++ti)
				{
					taken_number = (DCardNumber)(A2.Number + A2.Len - 1 - ti);
					XInfos[xIndex].X = taken_number;
					XInfos[xIndex].XLen = 2;
					xIndex++;
					VASSERT(xIndex <= x);
				}

				NumFromSplice -= taken;
				if (NumFromSplice == 0)
				{
					break;
				}
			}
		}
	}
	

	return xIndex == x;
}

DAtom DNormalAI::Peek3_From3S(DCardNumber FollowNumber, bool Ascender) const
{
	if (m3Atoms.empty())
	{
		return DAtom();
	}
	if (!Ascender)
	{
		// �ɴ�С
		DCardNumber LastValid = DCN_NULL;
		for (int Index = 0; Index < m3SAtoms.size(); ++Index)
		{
			const DAtom& S3 = m3SAtoms[Index];
			for (int j = (S3.Len - 1); j >= 0; --j)
			{
				DCardNumber Number = (DCardNumber)(S3.Number + j);
				if (Number > FollowNumber)
				{
					if (Number < LastValid)
					{
						LastValid = Number;
					}
				}
				else
				{
					return DAtom();
				}
			}
		}
	}
	else
	{
		// ��С����
		for (int Index = (int)m3SAtoms.size() - 1; Index >= 0; --Index)
		{
			const DAtom& S3 = m3SAtoms[Index];
			for (int j = 0; j < S3.Len; ++j)
			{
				DCardNumber Number = (DCardNumber)(S3.Number + j);
				if (Number > FollowNumber)
				{
					return DAtom(Number, 3);
				}
			}
		}
	}
	

	return DAtom();
}


DGameAIHand DNormalAI::Peek_Straight(DCardNumber Number, int Len) const
{
	if (m1SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}

	for (int Index = m1SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& A = m1SAtoms[Index];
		if (A.Number > Number && A.Len >= Len )
		{
			int Remain = A.Len - Len;
			if (Remain >= 5 || Remain <= 2)
			{
				return DGameAIHand(DHT_straight, A.Number, Len);
			}
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Peek_Straight2(DCardNumber Number, int Len) const
{
	if (m2SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}

	for (int Index = m2SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& A = m2SAtoms[Index];
		if (A.Number > Number && A.Len == Len)
		{
			return DGameAIHand(DHT_straight_2, A.Number, Len);
		}
	}

	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::Peek_Straight3(DCardNumber Number, int Len) const
{
	if (m3SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}

	DAtom SResult;
	for (int Index = m3SAtoms.size() - 1; Index >= 0; --Index)
	{
		const DAtom& A = m3SAtoms[Index];
		SResult = StraightFollow(A, Number, Len);
		if (SResult)
		{
			return DGameAIHand(DHT_straight_3, SResult.Number, SResult.Len);
		}
	}

	return DGameAIHand::PASS;
}



DGameAIHand DNormalAI::GetHand_Min() const
{
	DGameAIHand AIHand;
	DGameAIHand BestHand;
	int BestNumber = DCN_RED_JOKER + 1;


	AIHand = Peek_3_x();
	if (AIHand)
	{
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
		int HandNumber = AIHand.Number - 4;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}
	
	// ����
	DAtom A = Select_Ascend(m1Atoms, DCN_NULL);
	if (A)
	{
		int HandNumber = A.Number;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	// ����
	A = Select_Ascend(m2Atoms, DCN_NULL);
	if (A)
	{
		int HandNumber = A.Number;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = DGameAIHand(DHT_2, A.Number, 2);
		}
	}
	
	// ˳��
	AIHand = GetStraight();
	if (AIHand)
	{
		int HandNumber = AIHand.Number;
		HandNumber--;
		if (HandNumber < BestNumber)
		{
			BestNumber = HandNumber;
			BestHand = AIHand;
		}
	}
	// ˫˳
	AIHand = GetStraight2();
	if (AIHand)
	{
		int HandNumber = AIHand.Number;
		HandNumber--;
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
	

	// û���ҵ�����������. 
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	// ����С��һ���� 
	VASSERT(!"sth goes wrong.");
	
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::GetStraight() const
{
	if (m1SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}
	const DAtom& A = m1SAtoms[m1SAtoms.size() - 1];
	return DGameAIHand(DHT_straight, A.Number, A.Len);

}


DGameAIHand DNormalAI::GetStraight2() const
{
	if (m2SAtoms.empty())
	{
		return DGameAIHand::PASS;
	}
	const DAtom& A = m2SAtoms[m2SAtoms.size() - 1];
	return DGameAIHand(DHT_straight_2, A.Number, A.Len);

}

bool DNormalAI::AI_Determ_CanEscape(const DGameAIHand& AfterHand) const
{
	return false;
}

DGameAIHand DNormalAI::TryBreak_1_1(DCardNumber FollowNumber, DCardNumber RemainNumber) const
{
	DCardNumber Select = DCN_NULL;
	DCardNumber TestNumber = FollowNumber;
	DAtom A;
	while (1)
	{
		A = Select_Ascend(m1Atoms, TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= RemainNumber)
		{
			Select = A.Number;
			break;
		}

		TestNumber = A.Number;
	}

	if (Select != DCN_NULL)
	{
		return DGameAIHand(DHT_1, Select, 1);
	}

	// ���Դ�˳���в�

	TestNumber = FollowNumber;
	while (1)
	{
		A = Peek1_FromStraight(TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= RemainNumber)
		{
			Select = A.Number;
			break;
		}

		TestNumber = A.Number;
	}

	if (Select != DCN_NULL)
	{
		return DGameAIHand(DHT_1, Select, 1);
	}

	// ��2��. 
	TestNumber = FollowNumber;
	while (1)
	{
		A = Peek1_FromDouble(TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= RemainNumber)
		{
			Select = A.Number;
			break;
		}

		TestNumber = A.Number;
	}
	if (Select != DCN_NULL)
	{
		return DGameAIHand(DHT_1, Select, 1);
	}


	//////////////////////////////////////////////////////////////////////////
	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	TestNumber = FollowNumber;
	while (1)
	{
		A = Peek1_From3(TestNumber);
		if (!A)
		{
			break;
		}

		if (A.Number >= RemainNumber)
		{
			Select = A.Number;
			break;
		}
		TestNumber = A.Number;
	}
	if (Select != DCN_NULL)
	{
		return DGameAIHand(DHT_1, Select, 1);
	}
	return DGameAIHand::PASS;
}

DGameAIHand DNormalAI::TryBreak_2(DCardNumber FollowNumber, DCardNumber RemainNumber) const
{
	DCardNumber Select = DCN_NULL;
	DCardNumber TestNumber = FollowNumber;
	DAtom A;
	while (1)
	{
		A = Select_Ascend(m2Atoms, TestNumber);
		if (!A)
		{
			break;
		}
		if ((A.Number >= RemainNumber && RemainNumber != DCN_NULL) || RemainNumber == DCN_NULL)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
		TestNumber = A.Number;
	}



	// ����ѡ��һ��ը��.  
	A = SelectBomb(DCN_NULL);
	if (A && A.Number < DCN_A)
	{
		return MakeBomb(A);
	}

	if (mCardSlots[DCN_2] >= 2 && DCN_2 > FollowNumber)
	{
		return DGameAIHand(DHT_2, DCN_2, 2);
	}

	if (DCN_A > FollowNumber && mCardSlots[DCN_A] >= 2 && (DCN_A >= RemainNumber || RemainNumber == DCN_NULL))
	{
		return DGameAIHand(DHT_2, DCN_A, 2);
	}

	A = SelectBomb(DCN_NULL);
	if (A)
	{
		return MakeBomb(A);
	}

	// ��˫˳. 
	/*
		TestNumber = FollowNumber;
		while (1)
		{
		A = Peek2_From3(TestNumber);
		if (!A)
		{
		break;
		}

		if (A.Number >= RemainNumber)
		{
		Select = A.Number;
		break;
		}
		TestNumber = A.Number;
		}
		if (Select)
		{
		return DGameAIHand(DHT_1, Select, 1);
		}*/
	return DGameAIHand::PASS;
}
#include "VStdAfx.h"
#include "DDeck.h"
#include "DAISlover.h"
#include "DCard.h"
#include "VTemplate.h"
#include "DHand.h"
using namespace vtpl;



bool IsValidPlane(const BYTE* CardSlots, int XSize);
int AIRand(int Max)
{
	static const float RMAX = 1.0f/(RAND_MAX + 1.0f);
	float r = rand() * RMAX;	// convert to 0 - 1.0 to prevent low-order bits. we should use high-order bits for rand .
	return (int)(r * Max);
}

void  AISRand(size_t Seed)
{
	srand(Seed);
}

inline bool Chance(int rate)
{
	int r = AIRand(100);
	return rate > r;
}



static inline DHand& Increment(VArray<DHand>& Hands)
{
	int PrevIndex = (int)Hands.Size();
	Hands.Increment(1);
	DHand& Ret = Hands[PrevIndex];
	Ret.Clear();
	Ret.SetHandNumber(DCN_NULL);
	Ret.SetStraightLen(0);
	Ret.SetHandType(DHT_UNKNOWN);
	return Ret;
}
static inline DHand& Increment(VArray<DHand>& Hands, const DHand& Copy)
{
	int PrevIndex = (int)Hands.Size();
	Hands.Increment(1, Copy);
	DHand& Ret = Hands[PrevIndex];
	return Ret;
}



int DDeckAIHint::AI_QueryNumHands() const
{
	const DAtomList& Atoms_1 = Get1Atoms();
	const DAtomList& Atoms_2 = Get2Atoms();

	int NumHands = Atoms_1.size();
	NumHands += Atoms_2.size();
	NumHands += Get3Atoms().size();
	NumHands += Get4Atoms().size();
	NumHands += Get1SAtoms().size();
	NumHands += Get2SAtoms().size();
	NumHands += Get3SAtoms().size();


	int NumX = AI_QueryNumX();
	if (NumX)
	{
		int ValidX = Atoms_1.size() + Atoms_2.size();
		NumX = VMin(NumX, ValidX);

		NumHands -= NumX;
	}
	return NumHands;
}

int DDeckAIHint::AI_QueryNumX() const
{
	int NumX = Get3Atoms().size();
	const DAtomList& S3Atoms = Get3SAtoms();
	for (int i = 0; i < S3Atoms.size(); ++i)
	{
		NumX += S3Atoms[i].Len;
	}
	return NumX;
}


void DDeckAIHint::UpdateCards(const DCard* HandCards, int NumCards)
{
	memset(mCardSlots, 0, DCN_RED_JOKER + 1);

	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& Card = HandCards[i];
		mCardSlots[Card.Number]++;
	}

}

void DDeckAIHint::OnCardPlayed(const DCard* PlayedCards, int NumCards)
{
	for (int Index = 0; Index < NumCards; ++Index)
	{
		const DCard& Card = PlayedCards[Index];
		mCardSlots[Card.Number]--;
		VASSERT(mCardSlots[Card.Number] >= 0);
	}
}

int DDeckAIHint::NumBombersGreaterThen(DCardNumber Bomber) const
{
	return 0;
}

bool DDeckAIHint::CanFollow2(DCardNumber Number) const
{
	float NumGreater = 0;
	for (int Index = DCN_2; Index > Number; --Index)
	{
		if (mCardSlots[Index] == 2)
		{
			NumGreater += 1.0f;
		}
		else if (mCardSlots[Index] == 3)
		{
			NumGreater += 0.6f;
		}
		else if (mCardSlots[Index] == 4)
		{
			NumGreater += 0.3f;
		}
	}

	// round
	int iNumGreater = (int)(NumGreater + 0.5f);
	static const int Rate[] = { 0, 40, 80, 100 };
	if (iNumGreater >= 3)
	{
		return true;
	}else if (iNumGreater == 0)
	{
		return false;
	}
	return Chance(Rate[iNumGreater]);
}

bool DDeckAIHint::IsRemainDouble() const
{
	return false;
}


int DDeckAIHint::AI_QueryRemainBombers(DCardNumber GreatThen) const
{
	int GreatBomber = 0;
	for (int i = DCN_2; i > GreatThen; --i)
	{
		if (mCardSlots[i] == 4)
		{
			GreatBomber++;
		}
	}

	if (mCardSlots[DCN_BLACK_JOKER] && mCardSlots[DCN_RED_JOKER])
	{
		GreatBomber++;
	}
	return GreatBomber;
}

int DDeckAIHint::AI_Determ_Single_Max(DCardNumber SingleNumber, bool IncludeBomber) const
{
	// 外面的炸弹数量
	int NumBombers = IncludeBomber ? AI_QueryRemainBombers(DCN_NULL) : 0;
	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > SingleNumber; --i)
	{
		int Remain = mCardSlots[i];
		if (Remain && Remain != 4)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	int RemainKS = mCardSlots[DCN_BLACK_JOKER];
	int RemainKB = mCardSlots[DCN_RED_JOKER];
	if (RemainKS != RemainKB)
	{
		NumGreater++;
	}

	NumGreater += NumBombers;
	return NumGreater;
}


int DDeckAIHint::AI_Determ_Double_Greater(DCardNumber DoubleNumber) const
{
	// 外面的炸弹数量
	int NumBombers = AI_QueryRemainBombers(DCN_NULL);

	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > DoubleNumber; --i)
	{
		int Remain = mCardSlots[i];
		if (Remain >= 2 && Remain != 4)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	NumGreater += NumBombers;
	return NumGreater;
}


int DDeckAIHint::AI_Determ_3_Greater(DCardNumber ThreeNumber) const
{
	// 外面的炸弹数量
	int NumBombers = AI_QueryRemainBombers(DCN_NULL);

	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > ThreeNumber; --i)
	{
		int Remain = mCardSlots[i];
		if (Remain == 3)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	NumGreater += NumBombers;
	return NumGreater;
}


int DDeckAIHint::AI_Determ_S_Greater(DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}

	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight(mCardSlots, i, Len))
		{
			NumValidStraights++;
		}
	}

	return NumValidStraights;

}

int DDeckAIHint::AI_Determ_2S_Greater(DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}
	
	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight2(mCardSlots, i, Len))
		{
			NumValidStraights++;
		}
	}

	return NumValidStraights;
}

int DDeckAIHint::AI_Determ_3S_Greater(DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}

	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight3(mCardSlots, i, Len))
		{
			NumValidStraights++;
		}
	}

	return NumValidStraights;
}

bool DDeck::AI_CreateAIHint()
{
	VASSERT(mDeckAIHint == nullptr);
	mDeckAIHint = new DAISlover;
	return true;
}

void DDeck::AI_DestoryAIHint()
{
	if (mDeckAIHint)
	{
		mDeckAIHint->DeleteThis();
		mDeckAIHint = nullptr;
	}
}

DDeck::DDeck() :mNumPlayedCards(0), mDeckAIHint(nullptr), mLaiZi(DCN_NULL)
{

}


DDeck::~DDeck()
{
	if (mDeckAIHint)
	{
		mDeckAIHint->DeleteThis();
	}
}


void DDeck::Reset()
{


	mNumPlayedCards = 0;
	

	for (int i = 0; i < DDECK_NUM_POCKETS; ++i)
	{
		mPockets[i].Clear();
	}
}

void DDeck::Shuffle()
{
	const int NumCards = DDECK_NUM_CARDS;

	//mCards[DDECK_NUM_CARDS - 1] = DCard::SMALL_KING;
	//mCards[DDECK_NUM_CARDS - 2] = DCard::BIG_KING;
	//mCards[DDECK_NUM_CARDS - 3] = DCard(DCC_DIAMONDS,DCN_K);
	//return;

	// 首先, 重置牌堆, 加入牌. 
	int CardIndex = 0;
	for (int c = DCC_DIAMONDS; c <= DCC_SPADES; c++)
	{
		for (int n = DCN_3; n <= DCN_2; n++)
		{
			DCard& Card = mCards[CardIndex++];
			Card.Color = (DCardColor)c;
			Card.Number = (DCardNumber)n;
		}
	}
	mCards[CardIndex++] = DCard::SMALL_KING;
	mCards[CardIndex++] = DCard::BIG_KING;
	VASSERT(CardIndex == NumCards);

	// 开始洗牌
	DCard tempCard;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < NumCards; i++)
		{
			CardIndex = AIRand(NumCards);

			// swap cards;
			tempCard = mCards[i];
			mCards[i] = mCards[CardIndex];
			mCards[CardIndex] = tempCard;
		}
	}



}

namespace shuffle_ex
{
	struct Context
	{
		DHand Hands[4];	// 0 , 1, 2 为3个玩家的牌, 4 为底牌. 
		
	};


	int Deal(Context& C, const DCard& Card, const int* Rate)
	{
		//static const int Rate[4] = { 40, 40, 15, 5 };
		
		int HandIndex[4] = { -1, -1, -1, -1 };
		for (int i = 0; i < 3; ++i)
		{
			if (C.Hands[i].Size() < 17)
			{
				HandIndex[i] = i;
			}
		}
		if (C.Hands[3].Size() < 3)
		{
			HandIndex[3] = 3;
		}

		int TotalRates = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (HandIndex[i] != -1)
			{
				TotalRates += Rate[i];
			}
		}

		int Roll = AIRand(TotalRates);

		int Index;
		for (int i = 0; i < 4; ++i)
		{
			Index = HandIndex[i];
			if (Index != -1 )
			{
				if (Roll < Rate[Index])
				{
					C.Hands[Index].AddCard(Card);
					return Index;
				}
				Roll -= Rate[Index];
			}
		}
		return -1;
	}

	static void Shuffle(DCard* Cards)
	{
		Context ShuffleContext;

		static const int Rate_King[4] =		{ 5,	50,		40,		5 };	// 王落入3家的几率
		static const int Rate_2[4] =		{10,	50,		35,		5 };	// 2落入3家的几率
		static const int Rate_A[4] =		{20,	30,		40,		10 };	// A落入3家的几率
		static const int Rate_JQK[4] =		{20,	30,		40,		10 };	
		static const int Rate_7[4] =		{5,		40,		35,		10 };		// 断掉顺子
		static const int Rate_Small[4] =	{ 20,	20,		20,		20 };
		Deal(ShuffleContext, DCard::BIG_KING, Rate_King);
		Deal(ShuffleContext, DCard::SMALL_KING, Rate_King);

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_2), Rate_2);
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_A), Rate_A);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_K), Rate_JQK);
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_Q), Rate_JQK);
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_J), Rate_JQK);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_7), Rate_7);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_10), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_9), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_8), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_6), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_5), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_4), Rate_Small);
		}

		for (int i = DCC_DIAMONDS; i <= DCC_SPADES; i++)
		{
			Deal(ShuffleContext, DCard((DCardColor)i, DCN_3), Rate_Small);
		}


		DCard* pCards = Cards;
		for (int i = 0; i < 3; i++)
		{
			VASSERT(ShuffleContext.Hands[i].Size() == 17);
			memcpy(pCards, ShuffleContext.Hands[i].GetCards(), 17 * sizeof(DCard));
			pCards += 17;
		}
		VASSERT(ShuffleContext.Hands[3].Size() == 3);
		memcpy(pCards, ShuffleContext.Hands[3].GetCards(), 3 * sizeof(DCard));
	}
};


void DDeck::ShuffleEx()
{
	// 先
	shuffle_ex::Shuffle(mCards);
}




void Add3(DHand* Hand, DCardNumber number)
{
	Hand->AddCard(DCard(DCC_DIAMONDS, number));
	Hand->AddCard(DCard(DCC_CLUBS, number));
	Hand->AddCard(DCard(DCC_HEARTS, number));
}

void DDeck::Deal(DHand* Hand)
{
	//for (int i = 0; i < DHAND_NUM_CARDS_NORMAL; i++)
	{
		//Hand->AddCard(mCards[i]);
	//	Hand->SetCards(mCards, DHAND_NUM_CARDS_NORMAL);
	}

	//Add3(Hand, DCN_3);
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_3));
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_4));
	Hand->AddCard(DCard(DCC_CLUBS, DCN_4));
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_5));
	Hand->AddCard(DCard(DCC_CLUBS, DCN_5));
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_6));
	Hand->AddCard(DCard(DCC_CLUBS, DCN_6));
	//Hand->AddCard(DCard(DCC_DIAMONDS, DCN_7));
	Add3(Hand, DCN_7);


	//Add3(Hand, DCN_8);
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_8));
	Hand->AddCard(DCard(DCC_CLUBS, DCN_8));
	Hand->AddCard(DCard(DCC_DIAMONDS, DCN_9));


	Add3(Hand, DCN_10);
	Add3(Hand, DCN_J);
	//Hand->SetCards(mCards, DHAND_NUM_CARDS_NORMAL);

	Hand->Sort();
}


BOOL DDeck::IsMaxHand(const DDeckCharacter* Owner, const DHand* Hand) const
{
	return FALSE;
}


int DDeck::NumHands(const DHand& Hand)
{
	BYTE Cards[DCN_RED_JOKER + 1];
	memset(Cards, 0, DCN_RED_JOKER + 1);
	for (int i = 0; i < Hand.Size(); ++i)
	{
		const DCard& Card = Hand[i];
		Cards[Card.Number]++;
	}
	DAISloverResult Result;
	return DAISlover::Slover(Cards, Result);
}

int DDeck::CalcCallStrength(const DHand& Hand) const
{

	const int RocketWeight = 8;
	const int BombWeight = 6;
	const int BigKingWeight = 4;
	const int SmallKingWeight = 3;
	const int Weight_2 = 2;

	int Strength = 0;

	BYTE CardSlots[DCN_RED_JOKER + 1];
	memset(CardSlots, 0, DCN_RED_JOKER + 1);
	for (int i = 0; i < Hand.Size(); ++i)
	{
		const DCard& Card = Hand[i];
		CardSlots[Card.Number]++;
	}


	int NumBombs = 0;
	for (int i = DCN_3; i < DCN_2; ++i)
	{
		if (CardSlots[i] == 4)
		{
			NumBombs++;
		}
	}

	Strength = NumBombs * BombWeight;

	int Num2 = (int)CardSlots[DCN_2];
	Strength += Num2 * Weight_2;


	if (CardSlots[DCN_BLACK_JOKER] && CardSlots[DCN_RED_JOKER])
	{
		Strength += RocketWeight;
	}else if (CardSlots[DCN_BLACK_JOKER])
	{
		Strength += SmallKingWeight;
	}else if (CardSlots[DCN_RED_JOKER])
	{
		Strength += BigKingWeight;
	}

	if (Strength >= 7)
	{
		return 3;
	}

	if (Strength >= 5)
	{
		return 2;
	}

	if (Strength >= 3)
	{
		return 1;
	}

	return 0;
}

DCardNumber TypeAtom::Is3_2(const DCard* Cards, int NumCards)
{
	if (NumCards != 5)
	{
		return DCN_NULL;
	}

	DCardNumber HandNumber = Cards[2].Number;	// 获得3个的值 

	if (Cards[0].Number == HandNumber)
	{
		// 333 22
		if (Cards[0].Number == Cards[1].Number && Cards[0].Number == Cards[2].Number &&		// 3
			Cards[3].Number == Cards[4].Number		// 2
			)
		{
			return Cards[0].Number;
		}
	}
	else
	{
		// 22 333
		if (Cards[2].Number == Cards[3].Number && Cards[2].Number == Cards[4].Number &&		// 3
			Cards[0].Number == Cards[1].Number		// 2
			)
		{
			return Cards[2].Number;
		}
	}
	return DCN_NULL;
}

DCardNumber TypeAtom::Is4_2(const DCard* Cards, int NumCards)
{
	if (NumCards == 6)
	{
		DCardNumber HandNumber = Cards[2].Number;
		if (Cards[0].Number == HandNumber)
		{
			// 4444 22
			if (Cards[0].Number == Cards[1].Number &&
				Cards[0].Number == Cards[2].Number &&
				Cards[0].Number == Cards[3].Number)
			{
				return Cards[0].Number;
			}
		}
		else if (Cards[1].Number == HandNumber)
		{
			// 2 4444 2
			if (Cards[1].Number == Cards[2].Number &&
				Cards[1].Number == Cards[3].Number &&
				Cards[1].Number == Cards[4].Number)
			{
				return Cards[1].Number;
			}
		}
		else
		{
			// 22 4444
			if (Cards[2].Number == Cards[3].Number &&
				Cards[2].Number == Cards[4].Number &&
				Cards[2].Number == Cards[5].Number)
			{
				return Cards[2].Number;
			}
		}

	}
	else if (NumCards == 8)
	{
		if (
			(Cards[0].Number == Cards[1].Number && Cards[0].Number == Cards[2].Number && Cards[0].Number == Cards[3].Number) &&
			(Cards[4].Number == Cards[5].Number) &&
			(Cards[6].Number == Cards[7].Number)
			)
		{
			// 4444 22 22 
			return Cards[0].Number;
		}

		if (
			(Cards[2].Number == Cards[3].Number && Cards[2].Number == Cards[4].Number && Cards[2].Number == Cards[5].Number) &&
			(Cards[0].Number == Cards[1].Number) &&
			(Cards[6].Number == Cards[7].Number)
			)
		{
			// 22 4444 22
			return Cards[2].Number;
		}

		if (
			(Cards[4].Number == Cards[5].Number && Cards[4].Number == Cards[6].Number && Cards[4].Number == Cards[7].Number) &&
			(Cards[0].Number == Cards[1].Number) &&
			(Cards[2].Number == Cards[3].Number)
			)
		{
			// 22 22 4444 
			return Cards[4].Number;
		}
	}

	return DCN_NULL;

}

DCardNumber TypeAtom::IsStraight2(const DCard* Cards, int NumCards)
{
	if (NumCards < 6)
	{
		return DCN_NULL;
	}

	if ((NumCards % 2) != 0)
	{
		return DCN_NULL;
	}

	DCardNumber Number = Cards[0].Number;
	if (Number > DCN_A)
	{
		return DCN_NULL;
	}

	if (Cards[0].Number != Cards[1].Number)
	{
		return DCN_NULL;
	}

	int i;
	for (i = 2; i < NumCards; i += 2)
	{
		if (Cards[i].Number != Cards[i + 1].Number)
		{
			return DCN_NULL;
		}

		if (Cards[i].Number != (DCardNumber)(Cards[i - 1].Number - 1))
		{
			return DCN_NULL;
		}

		VASSERT(Cards[i].Number != DCN_NULL);
	}
	return Cards[NumCards - 1].Number;
}

/**
* 是否3顺子, 不带翅膀
*/
DCardNumber TypeAtom::IsPlane_NoWing(const DCard* Cards, int NumCards, int& Len)
{
	if (NumCards < 6)
	{
		return DCN_NULL;
	}

	if ((NumCards % 3) != 0)
	{
		return DCN_NULL;
	}

	DCardNumber Number = Cards[0].Number;
	if (Number > DCN_A)
	{
		return DCN_NULL;
	}

	if (Cards[0].Number != Cards[1].Number || Cards[0].Number != Cards[2].Number)
	{
		return DCN_NULL;
	}

	int i;
	for (i = 3; i < NumCards; i += 3)
	{
		if (Cards[i].Number != Cards[i + 1].Number || Cards[i].Number != Cards[i + 2].Number)
		{
			return DCN_NULL;
		}

		if (Cards[i].Number != (DCardNumber)(Cards[i - 1].Number - 1))
		{
			return DCN_NULL;
		}

		VASSERT(Cards[i].Number != DCN_NULL);
	}

	Len = NumCards / 3;

	return Cards[NumCards - 1].Number;
}

DCardNumber TypeAtom::IsPlane_Wing(const DCard* Cards, int NumCards, int& Len)
{
	if (NumCards < 8)
	{
		return DCN_NULL;
	}

	DHand Hand3;
	int Last = NumCards - 2;
	int Index = 0;
	while (Index < Last)
	{
		if (Cards[Index].Number <= DCN_A && Cards[Index].Number == Cards[Index + 1].Number && Cards[Index].Number == Cards[Index + 2].Number)
		{
			// 找到一个三个. 
			Hand3.AddCards(&Cards[Index], 3);
			Index += 3;
		}
		else
		{
			Index++;
		}
	}

	int Num3 = Hand3.Size() / 3;	// 3 个的数量

	if (Num3 < 2)
	{
		return DCN_NULL;
	}

	Len = Num3;

	int NumX = NumCards - Hand3.Size();		// 剩余的跟牌数量
	if (NumX == 0 || (NumX % Num3) != 0)
	{
		return DCN_NULL;
	}

	NumX = NumX / Num3;
	VASSERT(NumX);
	if (NumX == 1)
	{
		return Hand3.last().Number;
	}
	else if (NumX == 2)
	{
		// 检查跟牌是否为对子
		DHand X;
		X.SetCards(Cards, NumCards);
		X.RemoveCards(Hand3.GetCards(), Hand3.Size());
		for (int Index = 0; Index < X.Size(); Index += 2)
		{
			if (X[Index].Number != X[Index + 1].Number)
			{
				return DCN_NULL;
			}
		}

		return Hand3.last().Number;
	}


	return DCN_NULL;
}

inline bool HasPlane(const BYTE* Slots, int Head, int Len)
{
	int Tail = Head + Len;
	for (int Pos = Head; Pos < Tail; ++Pos)
	{
		if (Slots[Pos] < 3)		
		{
			return false;
		}
	}
	return true;
}

DCardNumber TypeAtom::IsPlane_Follow(const DCard* Cards, int NumCards, int Len)
{
	VASSERT(Len >= 2);
	int XSize = NumCards - 3 * Len;
	XSize = XSize / Len;

	BYTE CardSlots[DCN_RED_JOKER + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	DCardNumber Max = DCN_NULL;	// 最大的非癞子牌.
	DCardNumber Min = DCN_2;
	for (int Index = 0; Index < NumCards; ++Index)
	{
		const DCard& Card = Cards[Index];
		CardSlots[Card.Number]++;
		if (Card.Number < DCN_2)
		{
			Max = VMax(Max, Card.Number);
			Min = VMin(Min, Card.Number);
		}
	}

	if (!IsValidPlane(CardSlots,XSize))
	{
		return DCN_NULL;
	}

	DHand PlaneHand;
	int MainPartTail = Max;
	int Head;
	while (MainPartTail >= Min)
	{
		Head = MainPartTail - Len + 1;
		if (Head < DCN_3)
		{
			break;
		}

		if (HasPlane(CardSlots, Head, Len))
		{
			return (DCardNumber)Head;
		}
		MainPartTail--;
	}

	return DCN_NULL;
}

DHandType DDeck::EvalHandType(const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen)
{
	StraightLen = 0;
	if (NumCards == 1)
	{
		HandValue = Cards[0].Number;
		return DHT_1;
	}

	if ((HandValue = TypeAtom::IsDouble(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_2;
	}

	if ((HandValue = TypeAtom::Is3(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_3;
	}

	if ((HandValue = TypeAtom::IsBomb(Cards, NumCards)) != DCN_NULL)
	{
		// bomb 必须在 3_1 之前调用. 否则 5555 可能会被当做  555 _ 5 
		return DHT_4;
	}

	if ((HandValue = TypeAtom::Is3_1(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_3_1;
	}
	if ((HandValue = TypeAtom::Is3_2(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_3_2;
	}
	
	if ((HandValue = TypeAtom::Is2K(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_2K;
	}
	if ((HandValue = TypeAtom::IsStraight(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_straight;
	}
	if ((HandValue = TypeAtom::IsStraight2(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_straight_2;
	}



	if ((HandValue = TypeAtom::IsPlane_NoWing(Cards, NumCards, StraightLen)) != DCN_NULL)
		{
			return DHT_straight_3;
		}
	if ((HandValue = TypeAtom::IsPlane_Wing(Cards, NumCards, StraightLen)) != DCN_NULL)
		{
			return DHT_straight_3;
		}
	

	
	if ((HandValue = TypeAtom::Is4_2(Cards, NumCards)) != DCN_NULL)
	{
		return DHT_4_2;
	}

	HandValue = DCN_NULL;
	return DHT_UNKNOWN;
}

namespace follow
{

	inline BOOL MaybeBomber(int NumCards){ return NumCards == 2 || NumCards == 4; }

	inline void GetBomb(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		// 无法用炸弹炸对王. 
		if (FollowNumber > DCN_2)
		{
			return;
		}
		if (NumCards == 2)
		{
			if (Cards[0].IsKing() && Cards[1].IsKing())
			{
				HandType = DHT_2K;
				HandValue = DCN_RED_JOKER;
				return;
			}
		}
		else if (NumCards == 4)
		{
			HandValue = TypeAtom::IsBomb(Cards, 4);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_4;
			}
		}
	}


	void Follow_DHT_1(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == 1)
		{
			if (Cards[0].Number > FollowNumber)
			{
				HandType = DHT_1;
				HandValue = Cards[0].Number;
				
			}
			return;
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_2(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == 2)
		{
			HandValue = TypeAtom::IsDouble(Cards, 2);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_2;
				return;
			}
			// maybe 2k.
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_3(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == 3)
		{
			HandValue = TypeAtom::Is3(Cards, NumCards);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_3;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_3_1(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == 4)
		{
			// bug fix. 必须优先判断炸弹, 否则可能KKKK 被判断成 KKK-K 
			HandValue = TypeAtom::IsBomb(Cards, 4);
			if (HandValue != DCN_NULL)
			{
				HandType = DHT_4;
				return;
			}

			HandValue = TypeAtom::Is3_1(Cards, NumCards);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_3_1;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_3_2(DCardNumber FollowNumber, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == 5)
		{
			HandValue = TypeAtom::Is3_2(Cards, NumCards);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_3_2;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_4_2(const DHand& FollowTarget, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == FollowTarget.Size())
		{
			HandValue = TypeAtom::Is4_2(Cards, NumCards);
			if (HandValue > FollowTarget.GetHandNumber())
			{
				HandType = DHT_4_2;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_straight(DCardNumber FollowNumber, int Len, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == Len)
		{
			HandValue = TypeAtom::IsStraight(Cards, NumCards);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_straight;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}
	void Follow_DHT_straight_2(DCardNumber FollowNumber, int Len, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == Len)
		{
			HandValue = TypeAtom::IsStraight2(Cards, NumCards);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_straight_2;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}
	void Follow_DHT_straight_3(DCardNumber FollowNumber, int Len, int SLen, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue)
	{
		if (NumCards == Len)
		{
			HandValue = TypeAtom::IsPlane_Follow(Cards, NumCards, SLen);
			if (HandValue > FollowNumber)
			{
				HandType = DHT_straight_3;
				return;
			}
		}

		if (MaybeBomber(NumCards))
		{
			GetBomb(DCN_NULL, Cards, NumCards, HandType, HandValue);
		}
	}

	void Follow_DHT_4(const DHand& FollowTarget, const DCard* Cards, int NumCards, DHandType& HandType, DCardNumber& HandValue, DCardNumber LaiziNumber)
	{
		// 对王的判断. 
		if (NumCards == 2)
		{
			if (Cards[0].IsKing() && Cards[1].IsKing())
			{
				HandType = DHT_2K;
				HandValue = DCN_RED_JOKER;
			}
			return;
		}

		// 炸弹的判断
		if (NumCards != 4)
		{
			return;
		}
		
		DCardNumber FollowNumber = FollowTarget.GetHandNumber();
		HandValue = TypeAtom::IsBomb(Cards, 4);


		if (LaiziNumber)
		{
			// 癞子场.  我们肯定是硬炸. 
			if (FollowNumber == LaiziNumber)
			{
				// 无法用非2K 打4个癞子. 
				return;
			}

			VASSERT(HandValue != LaiziNumber);		// 如果是4个癞子的情况, 走得是癞子的流程. 

			BOOL TargetIsSoftBomber = FALSE;
			for (int i = 0; i < FollowTarget.Size();++i)
			{
				if (FollowTarget[i].Number == LaiziNumber || FollowTarget[i].Color == DCC_LAIZI)
				{
					TargetIsSoftBomber = TRUE;
					break;
				}
			}

			if (TargetIsSoftBomber || HandValue > FollowNumber)
			{
				HandType = DHT_4;
				return;
			}
		}
		else
		{
			if (HandValue > FollowNumber)
			{
				HandType = DHT_4;
			}
		}
		
	}
}

DHandType DDeck::EvalHandTypeFollow(const DHand& FollowHand, const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen, DCardNumber LaiziNumber)
{
	DHandType FollowType = FollowHand.GetHandType();
	DCardNumber FollowNumber = FollowHand.GetHandNumber();
	DHandType ResultType = DHT_UNKNOWN;
	HandValue = DCN_NULL;
	StraightLen = 0;
	switch (FollowType)
	{
	case DHT_1:
	{
		follow::Follow_DHT_1(FollowNumber, Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_2:
	{
		follow::Follow_DHT_2(FollowNumber, Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_3:
	{
		follow::Follow_DHT_3(FollowNumber, Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_3_1:
	{
		follow::Follow_DHT_3_1(FollowNumber, Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_3_2:
	{
		follow::Follow_DHT_3_2(FollowNumber, Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_straight:
	{
		follow::Follow_DHT_straight(FollowNumber, FollowHand.Size(), Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_straight_2:
	{
		follow::Follow_DHT_straight_2(FollowNumber, FollowHand.Size(), Cards, NumCards, ResultType, HandValue);
		break;
	}
	case DHT_straight_3:
	{
		follow::Follow_DHT_straight_3(FollowNumber, FollowHand.Size(), FollowHand.GetStraightLen(), Cards, NumCards, ResultType, HandValue);
		if (ResultType == DHT_straight_3)
		{
			StraightLen = FollowHand.GetStraightLen();
		}
		break;
	}
	case DHT_4:
	{
		follow::Follow_DHT_4(FollowHand, Cards, NumCards, ResultType, HandValue, LaiziNumber);
		break;
	}
	case DHT_4_2:
	{
		follow::Follow_DHT_4_2(FollowHand, Cards, NumCards, ResultType, HandValue);
		break;
	}
	default:
		break;
	}

	if (ResultType == DHT_UNKNOWN)
	{
		HandValue = DCN_NULL;
		StraightLen = 0;
	}
	return ResultType;
}


namespace follow_laizi
{


	BOOL _GetBomb(const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Hands)
	{
		if ((RawCards.Size() + Laizi.Size()) != 4)
		{
			return FALSE;
		}

		int NumCards = 4 - Laizi.Size();

		if (NumCards == 0)
		{
			DCardNumber LaiziNumber = Laizi[0].Number;
			DHand& HBomb = Increment(Hands, DHand(DHT_4, LaiziNumber));
			HBomb.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			HBomb.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			HBomb.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			HBomb.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			return TRUE;
		}

		DCardNumber BombNumber = RawCards[0].Number;
		for (int i = 1; i < RawCards.Size(); ++i)
		{
			if (RawCards[i].Number != BombNumber)
			{
				return FALSE;
			}
		}
		DHand& HBomb = Increment(Hands, DHand(DHT_4, BombNumber));
		HBomb.AddCards(RawCards.GetCards(), RawCards.Size());
		for (int j = 0; j < Laizi.Size(); ++j)
		{
			HBomb.AddCard(DCard(DCC_LAIZI, BombNumber));
		}
		return TRUE;
	}



	BOOL GetBomb(DCardNumber FollowNumber, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		if (FollowNumber > DCN_2)
		{
			return FALSE;
		}

		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards == 2)
		{
			if (Laizi.Size())
			{
				return FALSE;
			}

			if (RawCards[0].IsKing() && RawCards[1].IsKing())
			{
				DHand& Ret = Increment(Result, DHand(DHT_2K, DCN_RED_JOKER));
				Ret.AddCard(DCard::BIG_KING);
				Ret.AddCard(DCard::SMALL_KING);
				return TRUE;
			}
		}
		else if (NumCards == 4)
		{
			return _GetBomb(RawCards, Laizi, Result);
		}
		return FALSE;
	}

	BOOL Follow_DHT_1(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		VASSERT(StraightLen == 0);
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();

		if (NumCards == 1)
		{
			DCard Card = RawCards.Size() ? RawCards[0] : Laizi[0];
			if (Card.Number > FollowNumber)
			{
				DHand& Ret = Increment(Result, DHand(DHT_1, Card.Number));
				Ret.AddCard(Card);
				return TRUE;
			}
			return FALSE;
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}

	BOOL Follow_DHT_2(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		VASSERT(StraightLen == 0);
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();

		if (NumCards == 2)
		{
			VArray<DHand> Doubles;
			DDeck::FillDouble(RawCards, Laizi, Doubles);
			BOOL Valid = FALSE;
			for (int i = 0; i < (int)Doubles.Size(); ++i)
			{
				const DHand& FH = Doubles[i];
				if (FH.GetHandNumber() > FollowNumber)
				{
					Valid = TRUE;
					Result.PushBack(FH);
				}
			}
			
			if (Valid)
			{
				return TRUE;		// 2K
			}
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}

	BOOL Follow_DHT_3(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		VASSERT(StraightLen == 0);
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();

		if (NumCards == 3)
		{
			VArray<DHand> Hands;
			DDeck::FillIs3(RawCards, Laizi, Hands);
			BOOL Valid = FALSE;
			for (int i = 0; i < (int)Hands.Size(); ++i)
			{
				const DHand& FH = Hands[i];
				if (FH.GetHandNumber() > FollowNumber)
				{
					Valid = TRUE;
					Result.PushBack(FH);
				}
			}
			return Valid;
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}

	BOOL Follow_DHT_3_1(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		VASSERT(StraightLen == 0);
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();

		if (NumCards == 4)
		{
			
			// 优先判断4个的炸弹. 
			if (_GetBomb(RawCards, Laizi, Result))
			{
				return TRUE;
			}

			VArray<DHand> Hands;
			DDeck::Compose3_1(RawCards, Laizi, Hands);
			BOOL Valid = FALSE;
			for (int i = 0; i < (int)Hands.Size(); ++i)
			{
				const DHand& FH = Hands[i];
				if (FH.GetHandNumber() > FollowNumber)
				{
					Valid = TRUE;
					Result.PushBack(FH);
				}
			}
			if (Valid)
			{
				return TRUE;	// bomb is 4.
			}
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}

	BOOL Follow_DHT_3_2(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		VASSERT(StraightLen == 0);
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();

		if (NumCards == 5)
		{
			VArray<DHand> Hands;
			DDeck::Compose3_2(RawCards, Laizi, Hands);
			BOOL Valid = FALSE;
			for (int i = 0; i < (int)Hands.Size(); ++i)
			{
				const DHand& FH = Hands[i];
				if (FH.GetHandNumber() > FollowNumber)
				{
					Valid = TRUE;
					Result.PushBack(FH);
				}
			}
			return Valid;
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}
	BOOL Follow_DHT_4_2(const DHand& Hand, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		// 能打得起个子的情况只有2中, 个子或者炸弹. 
		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards == Hand.Size())
		{
			DHand Hands;
			if (DDeck::Compose4_2(RawCards, Laizi, Hands) && Hands.GetHandNumber() > Hand.GetHandNumber())
			{
				Result.PushBack(Hands);
				return TRUE;
			}
		}

		if (NumCards == 2 || NumCards == 4)
		{
			return GetBomb(DCN_NULL, RawCards, Laizi, Result);
		}

		return FALSE;
	}
	BOOL Follow_DHT_4(const DHand& FollowTarget, DCardNumber LaiziNumber, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards != 2 && NumCards != 4)
		{
			return FALSE;
		}

		// 对王能打得起任何炸弹
		if (RawCards.Size() == 2 && Laizi.Size() == 0 && RawCards[0].IsKing() && RawCards[1].IsKing())
		{
			DHand& Ret = Increment(Result, DHand(DHT_2K, DCN_RED_JOKER));
			Ret.AddCard(DCard::BIG_KING);
			Ret.AddCard(DCard::SMALL_KING);
			return TRUE;
		}


		const DCardNumber FollowNumber = FollowTarget.GetHandNumber();
		if (FollowNumber > DCN_2 || FollowNumber == LaiziNumber)
		{
			return FALSE;
		}
		
		int TargetNumLaizi = 0;
		for (int i = 0; i < FollowTarget.Size(); ++i)
		{
			const DCard& C = FollowTarget[i];
			if (C.Color == DCardColor::DCC_LAIZI)//这里应该是判定花色
			{
				TargetNumLaizi++;
			}
		}

		if (TargetNumLaizi == 4)
		{
			// 只有对王能打得起. 
			return FALSE;
		}else if (TargetNumLaizi == 0)
		{
			// 硬炸. 
			if (Laizi.Size() == 4 )
			{
				DHand& Ret = Increment(Result, DHand(DHT_4, LaiziNumber));
				Ret.AddCards(Laizi.GetCards(), 4);
				return TRUE;
			}

			if (Laizi.Size())
			{
				return FALSE;//
			}


			DCardNumber BombNumber = TypeAtom::IsBomb(RawCards.GetCards(), RawCards.Size());
			if (BombNumber > FollowNumber)
			{
				DHand& Ret = Increment(Result, DHand(DHT_4, BombNumber));
				Ret.AddCards(RawCards.GetCards(), 4);
				return TRUE;
			}
			return FALSE;
		}
		else
		{
			DCardNumber BombNumber = TypeAtom::IsBomb(RawCards.GetCards(), RawCards.Size());
			if (Laizi.Size() == 0 && BombNumber)
			{
				// 硬炸
				DHand& Ret = Increment(Result, DHand(DHT_4, BombNumber));
				Ret.AddCards(RawCards.GetCards(), 4);
				return TRUE;
			}
			// 目标是软炸, 我们也可能是软炸. 
			VArray<DHand> Hands;
			DDeck::FillBom(RawCards, Laizi, Hands);
			BOOL Valid = FALSE;
			for (int i = 0; i < (int)Hands.Size(); ++i)
			{
				int HandNumLaizi = 0;
				const DHand& FH = Hands[i];
				if (FH.GetHandNumber() > FollowNumber )
				{
					Valid = TRUE;
					Result.PushBack(FH);
				}
			}

			return Valid;
		}
	}

	BOOL Follow_DHT_straight(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards != StraightLen)
		{
			if (NumCards == 2 || NumCards == 4)
			{
				return GetBomb(DCN_NULL, RawCards, Laizi, Result);
			}

			return FALSE;
		}

		VArray<DHand> Hands;
		DDeck::ComposeStraight(RawCards, Laizi, Hands);
		BOOL Valid = FALSE;
		for (int i = 0; i < (int)Hands.Size(); ++i)
		{
			const DHand& FH = Hands[i];
			if (FH.GetHandNumber() > FollowNumber)
			{
				Valid = TRUE;
				Result.PushBack(FH);
			}
		}
		return Valid;
	}

	BOOL Follow_DHT_straight_2(DCardNumber FollowNumber, int StraightLen, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards != StraightLen)
		{
			if (NumCards == 2 || NumCards == 4)
			{
				return GetBomb(DCN_NULL, RawCards, Laizi, Result);
			}

			return FALSE;
		}

		VArray<DHand> Hands;
		DDeck::ComposeStraight2(RawCards, Laizi, Hands);
		BOOL Valid = FALSE;
		for (int i = 0; i < (int)Hands.Size(); ++i)
		{
			const DHand& FH = Hands[i];
			if (FH.GetHandNumber() > FollowNumber)
			{
				Valid = TRUE;
				Result.PushBack(FH);
			}
		}
		return Valid;
	}

	BOOL Follow_DHT_straight_3(const DHand& FollowTarget, const DHand& RawCards, const DHand& Laizi, VArray<DHand>& Result)
	{
		int NumCards = RawCards.Size() + Laizi.Size();
		if (NumCards != FollowTarget.Size())
		{
			if (NumCards == 2 || NumCards == 4)
			{
				return GetBomb(DCN_NULL, RawCards, Laizi, Result);
			}
			return FALSE;
		}

		const int FollowStraightLen = FollowTarget.GetStraightLen();
		
		//2 :	6		8		10 
		//3 :	9		12		15
		//4 :	12		16		20 
		//5 :	15		20 
		//6 :	18
		const int NumFollowCards = FollowTarget.Size();
		int  XPartSize = -1;
		if (NumFollowCards == 8 || NumFollowCards == 9 || NumFollowCards == 16)
		{
			XPartSize = 1;
		}else if (NumFollowCards == 10 )
		{
			XPartSize = 2;
		}else if (NumFollowCards == 6 || NumFollowCards == 18)
		{
			XPartSize = 0;
		}else if (NumFollowCards == 12)
		{
			XPartSize = FollowStraightLen == 3 ? 1 : 0;
		}else if (NumFollowCards == 15)
		{
			XPartSize = FollowStraightLen == 3 ? 2 : 0;
		}else if (NumFollowCards == 20)
		{
			XPartSize = FollowStraightLen == 4 ? 2 : 1;
		}
		else
		{
			return FALSE;
		}
		

		VArray<DHand> Hands;
		DDeck::ComposePlane(RawCards, Laizi, Hands, FollowStraightLen, XPartSize);
		BOOL Valid = FALSE;
		for (int i = 0; i < (int)Hands.Size(); ++i)
		{
			const DHand& FH = Hands[i];
			if (FH.GetHandNumber() > FollowTarget.GetHandNumber())
			{
				Valid = TRUE;
				Result.PushBack(FH);
			}
		}
		return Valid;
	}
}

/**
 * 对于癞子跟牌逻辑的优化处理. 
 */
BOOL DDeck::EvalFollowHandForLaizi(const DHand& FollowTarget, const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Result)
{
	VASSERT(LaiziNumber != DCN_NULL);
	VASSERT(!FollowTarget.IsEmpty() && NumCards && Cards);

	const DCardNumber FollowNumber = FollowTarget.GetHandNumber();
	const DHandType FollowType = FollowTarget.GetHandType();
	const int FollowStraightLen = FollowTarget.GetStraightLen();


	DHand RawCards;
	DHand LaiziCards;
	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& C = Cards[i];
		if (C.Number != LaiziNumber)
		{
			RawCards.AddCard(C);
		}
		else
		{
			LaiziCards.AddCard(C);
		}
	}


#define CASE(Type) case Type: return follow_laizi::Follow_##Type(FollowNumber, FollowStraightLen, RawCards, LaiziCards, Result)

	switch (FollowType)
	{
		CASE(DHT_1); 
		CASE(DHT_2);
		CASE(DHT_3);
		CASE(DHT_3_1);
		CASE(DHT_3_2);
	case DHT_4_2: return follow_laizi::Follow_DHT_4_2(FollowTarget, RawCards, LaiziCards, Result);
	case DHT_straight:return follow_laizi::Follow_DHT_straight(FollowNumber, FollowTarget.Size(), RawCards, LaiziCards, Result);
	case DHT_straight_2:return follow_laizi::Follow_DHT_straight_2(FollowNumber, FollowTarget.Size(), RawCards, LaiziCards, Result);
	case DHT_straight_3:return follow_laizi::Follow_DHT_straight_3(FollowTarget, RawCards, LaiziCards, Result);
	case DHT_4:
		return follow_laizi::Follow_DHT_4(FollowTarget, LaiziNumber, RawCards, LaiziCards, Result);
	default:
		break;
	}

#undef CASE

	return FALSE;
}


BOOL DDeck::LaiZiEvalHandType(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	Results.Clear();
	DHand Temp;

	DDeck::FillDouble(RawCards, SelectedLaize, Results);
	DDeck::FillIs3(RawCards, SelectedLaize, Results);
	DDeck::FillBom(RawCards, SelectedLaize, Results);
	DDeck::Compose3_1(RawCards, SelectedLaize, Results);
	DDeck::Compose3_2(RawCards, SelectedLaize, Results);

	if (DDeck::Compose4_2(RawCards, SelectedLaize, Temp))
	{
		Results.PushBack(Temp);
	}
	DDeck::ComposeStraight(RawCards, SelectedLaize, Results);
	DDeck::ComposeStraight2(RawCards, SelectedLaize, Results);
	DDeck::ComposePlane(RawCards, SelectedLaize, Results);

	return !Results.IsEmpty();
}


BOOL DDeck::EvalHandWithLaizi(const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Results)
{
	VASSERT(Cards && NumCards && LaiziNumber != DCN_NULL);
	
	DHand RawCards;
	DHand Laizi;
	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& C = Cards[i];
		if (C.Number == LaiziNumber)
		{
			Laizi.AddCard(C);
		}
		else
		{
			RawCards.AddCard(C);
		}
	}
	Results.Clear();
	if (NumCards == 1)
	{
		DHand TH_Hand;
		TH_Hand.AddCard(Cards[0]);
		TH_Hand.SetHandNumber(Cards[0].Number);
		TH_Hand.SetHandType(DHT_1);
		Results.PushBack(TH_Hand);
		return TRUE;
	}

	DHand Temp;
	DDeck::FillDouble(RawCards, Laizi, Results);
	DDeck::FillIs3(RawCards, Laizi, Results);
	DDeck::FillBom(RawCards, Laizi, Results);
	DDeck::Compose3_1(RawCards, Laizi, Results);
	DDeck::Compose3_2(RawCards, Laizi, Results);

	if (DDeck::Compose4_2(RawCards, Laizi, Temp))
	{
		Results.PushBack(Temp);
	}

	DDeck::ComposeStraight(RawCards, Laizi, Results);
	DDeck::ComposeStraight2(RawCards, Laizi, Results);
	DDeck::ComposePlane(RawCards, Laizi, Results);

	return !Results.IsEmpty();
}

/**
 * 尝试将癞子从TailPos 位置开始, 往前放入补齐顺子, 如果补齐成功, 则返回顺子的头部牌编号, 否则返回DCN_NULL.  
 */
int SeedLaizi_Straight(BYTE* Slot, int NumLaizi, int TailPos, int Len)
{
	if (NumLaizi == 0  || NumLaizi > 4)
	{
		return DCN_NULL;
	}
	// 前面不允许有其它杂牌
	int Head = TailPos - Len + 1;
	for (int Pos = DCN_3; Pos < Head; ++Pos)
	{
		if (Slot[Pos])
		{
			return DCN_NULL;
		}
	}
	// 后面也不允许有其它杂牌.
	for (int Pos = TailPos + 1; Pos < DCN_2; ++Pos)
	{
		if (Slot[Pos])
		{
			return DCN_NULL;
		}
	}

	// 计算在区间内所需要的癞子数量. 
	int NumGaps = 0;
	for (int Pos = Head; Pos <= TailPos; ++Pos)
	{
		int NumNeed = 1 - Slot[Pos];
		VASSERT(NumNeed >= 0);
		NumGaps += NumNeed;
	}

	if (NumGaps != NumLaizi)
	{
		return DCN_NULL;
	}

	return Head;
}
//凑对子
void DDeck::FillDouble(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	if (RawCards.Size() + SelectedLaize.Size() != 2)
		return;

	if (SelectedLaize.Size() == 2)//一对癞子直接出
	{
		DHand& Ret = Increment(Results);
		Ret.SetHandType(DHT_2);
		Ret.SetHandNumber(SelectedLaize[0].Number);
		Ret.SetCards(SelectedLaize.GetCards(), 2);
	}else if (SelectedLaize.Size() == 1)
	{
		const DCard& RawCard = RawCards[0];
		if (!RawCard.IsKing())
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_2);
			Ret.SetHandNumber(RawCards[0].Number);
			Ret.AddCard(RawCards[0]);
			Ret.AddCard(DCard(DCC_LAIZI, RawCard.Number));
		}
	}else 
	{
		DCardNumber HandNumber = TypeAtom::IsDouble(RawCards.GetCards(), 2);
		if (HandNumber != DCN_NULL)
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_2);
			Ret.SetHandNumber(HandNumber);
			Ret.SetCards(RawCards.GetCards(), 2);
		}
	}
}

//凑3子
void DDeck::FillIs3(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	if (RawCards.Size() + SelectedLaize.Size() != 3)
		return;

	const int NumLaizi = SelectedLaize.Size();

	if (NumLaizi == 0)
	{
		// 没有用癞子
		DCardNumber HandNumber = TypeAtom::Is3(RawCards.GetCards(), 3);
		if (HandNumber != DCN_NULL)
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3);
			Ret.SetHandNumber(HandNumber);
			Ret.SetCards(RawCards.GetCards(), 3);
		}
	}else if (NumLaizi == 1)
	{
		// 用一个癞子, 那么原始牌必须为对子. 
		DCardNumber HandNumber = TypeAtom::IsDouble(RawCards.GetCards(), 2);
		if (HandNumber != DCN_NULL)
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3);
			Ret.SetHandNumber(HandNumber);
			Ret.SetCards(RawCards.GetCards(), 2);

			Ret.AddCard(DCard(DCC_LAIZI, HandNumber));
		}
	}else if (NumLaizi == 2)
	{
		// 用2个癞子, 那么原始牌必须为非王的个子.
		if (!RawCards[0].IsKing())
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3);
			Ret.SetHandNumber(RawCards[0].Number);
			Ret.AddCard(RawCards[0]);
			Ret.AddCard(DCard(DCC_LAIZI, RawCards[0].Number));
			Ret.AddCard(DCard(DCC_LAIZI, RawCards[0].Number));
		}
	}else if (NumLaizi == 3)
	{
		DHand& Ret = Increment(Results);
		Ret.SetHandType(DHT_3);
		Ret.SetHandNumber(SelectedLaize[0].Number);
		Ret.SetCards(SelectedLaize.GetCards(), 3);
	}
}

BOOL DDeck::Compose3_1(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	/**
	 * 能配成3x1 的情况, 必须是需要RawCards 里面有且仅有2种不同的数字牌.  对于 777 * 这样的牌, 我们视为炸弹, 而不是拼成3带1. 
	 */
	if (RawCards.Size() + SelectedLaize.Size() != 4)
		return FALSE;

	if (RawCards.Size() < 2)
	{
		// 炸弹
		return FALSE;
	}

	// 尝试找到这两种数字的牌. 
	DCardNumber RawNumbers[2] = {DCN_NULL, DCN_NULL};
	int RawCardNum[2] = { 0, 0 };		// 在3带1的规则里面, 这里能出现的情况是, 两者均>0 , 必须至少有一个为1. 
	{
		int NumRawCardTypes = 0;
		for (int Index = 0; Index < RawCards.Size(); ++Index)
		{
			const DCard& Card = RawCards[Index];
			int TIndex = -1;
			if (Card.Number == RawNumbers[0])
			{
				TIndex = 0;
			}
			else if (Card.Number == RawNumbers[1])
			{
				TIndex = 1;
			}

			if (TIndex == -1)
			{
				if (NumRawCardTypes == 2)
				{
					return FALSE;
				}
				RawNumbers[NumRawCardTypes] = Card.Number;
				RawCardNum[NumRawCardTypes] = 1;
				NumRawCardTypes++;
			}
			else
			{
				RawCardNum[TIndex]++;
			}
		}

		if (NumRawCardTypes != 2)
		{
			return FALSE;
		}

		// 还必须要求至少有一个数量为1. 
		if (RawCardNum[0] != 1 && RawCardNum[1] != 1)
		{
			return FALSE;
		}

		VASSERT(RawCardNum[0] != 0 && RawCardNum[1] != 0);
	}

	// 计算主体. 
	int MainPartIndex = -1;
	if (RawCardNum[0] == 1 && RawCardNum[1] == 1)
	{
		// 这种情况下. 取最大的非王牌当主体. 
		MainPartIndex = RawNumbers[0] > RawNumbers[1] ? 0 : 1;
		if (RawNumbers[MainPartIndex] > DCN_2)
		{
			// 最大的牌是王. 
			MainPartIndex = (MainPartIndex + 1) & 1;
		}
	}
	else
	{
		int XPartIndex = RawCardNum[0] == 1 ? 0 : 1;
		MainPartIndex = (XPartIndex + 1) & 1;
	}

	DCardNumber MainNumber = RawNumbers[MainPartIndex];

	DHand& Ret = Increment(Results);
	Ret.SetHandType(DHT_3_1);
	Ret.SetHandNumber(MainNumber);
	Ret.SetCards(RawCards.GetCards(), RawCards.Size());

	if (RawCardNum[MainPartIndex] == 1)
	{
		Ret.AddCard(DCard(DCC_LAIZI, MainNumber));
		Ret.AddCard(DCard(DCC_LAIZI, MainNumber));
	}
	else if (RawCardNum[MainPartIndex] == 2)
	{
		Ret.AddCard(DCard(DCC_LAIZI, MainNumber));
	}
	return TRUE;
}


struct ComposeContext
{
	DCardNumber		Card[2];
	int				CardNum[2];
	int				NumTypes;

	inline ComposeContext()
	{
		Card[0] = Card[1] = DCN_NULL;
		CardNum[0] = CardNum[1] = 0;
		NumTypes = 0;
	}

	inline int FindCard(DCardNumber Number) const
	{
		for (int Index = 0; Index < NumTypes; ++Index)
		{
			if (Card[Index] == Number)
			{
				return Index;
			}
		}
		return -1;
	}

	inline void AddCard(DCardNumber Number)
	{
		Card[NumTypes] = Number;
		CardNum[NumTypes] = 1;
		NumTypes++;
	}

	inline BOOL Init(const DHand& Cards)
	{
		for (int Index = 0; Index < Cards.Size(); ++Index)
		{
			const DCard& Card = Cards[Index];
			int TIndex = FindCard(Card.Number);
			if (TIndex == -1)
			{
				if (NumTypes >= 2)
				{
					return FALSE;
				}
				AddCard(Card.Number);
			}
			else
			{
				CardNum[TIndex]++;
			}
		}
		return TRUE;
	}

};

//凑3带2
BOOL DDeck::Compose3_2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	if (RawCards.Size() + SelectedLaize.Size() != 5)
		return FALSE;


	// 不能出现王. 
	for (int Index = 0; Index < RawCards.Size(); ++ Index)
	{
		if (RawCards[Index].IsKing())
		{
			return FALSE;
		}
	}


	const int NumLaizi = SelectedLaize.Size();
	switch (NumLaizi)
	{
	case 0:
	{
		DCardNumber HandNumber = TypeAtom::Is3_2(RawCards.GetCards(), 5);
		if (HandNumber != DCN_NULL)
		{
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3_2);
			Ret.SetHandNumber(HandNumber);
			Ret.SetCards(RawCards.GetCards(), 5);
			return TRUE;
		}
		return FALSE;
	}
	case 1:
	{
		// 666 5*    66 55*      6666* ???? 3个6带对6 ??
		ComposeContext Context;
		if (!Context.Init(RawCards))
		{
			return FALSE;
		}

		if (Context.NumTypes != 2)
		{
			return FALSE;
		}
		int MainPartIndex = -1;
		if (Context.CardNum[0] == 3 || Context.CardNum[1] == 3)
		{
			MainPartIndex = Context.CardNum[0] == 3 ? 1 : 0;
		}else 
		{
			VASSERT(Context.CardNum[0] == 2 && Context.CardNum[1] == 2);
			MainPartIndex = Context.Card[0] > Context.Card[1] ? 0 : 1;
		}
		DHand& Ret = Increment(Results);
		Ret.SetHandType(DHT_3_2);
		Ret.SetHandNumber(Context.Card[MainPartIndex]);
		Ret.SetCards(RawCards.GetCards(), RawCards.Size());

		if (Context.CardNum[MainPartIndex] == 2)
		{
			Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));
		}
		else
		{
			int XPartIndex = (MainPartIndex + 1) & 1;
			Ret.AddCard(DCard(DCC_LAIZI, Context.Card[XPartIndex]));
		}

		return TRUE;
	}
	case 2:
	{
		// 55 6 **   $ 555** $ 
		ComposeContext Context;
		if (!Context.Init(RawCards))
		{
			return FALSE;
		}

		if (Context.NumTypes == 1)
		{
			// 555 ** 
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3_2);
			Ret.SetHandNumber(Context.Card[0]);
			Ret.SetCards(RawCards.GetCards(), RawCards.Size());
			Ret.AddCards(SelectedLaize.GetCards(), SelectedLaize.Size());
		}
		else
		{
			// 55 6 **
			int MainPartIndex = Context.Card[0] > Context.Card[1] ? 0 : 1;
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3_2);
			Ret.SetHandNumber(Context.Card[MainPartIndex]);
			Ret.SetCards(RawCards.GetCards(), RawCards.Size());

			if (Context.CardNum[MainPartIndex] == 1)
			{
				Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));
				Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));
			}
			else
			{
				Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));

				int XPartIndex = (MainPartIndex + 1) & 1;
				Ret.AddCard(DCard(DCC_LAIZI, Context.Card[XPartIndex]));
			}

		}
		return TRUE;
	}
	case 3:
	{
		// 66 *** $ 56 ***
		ComposeContext Context;
		if (!Context.Init(RawCards))
		{
			return FALSE;
		}

		if (Context.NumTypes == 1)
		{
			// 66 ***
			DCardNumber LaiziNumber = SelectedLaize[0].Number;
			DCardNumber MainPartNumber = VMax(LaiziNumber, Context.Card[0]);
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3_2);
			Ret.SetHandNumber(MainPartNumber);
			Ret.SetCards(RawCards.GetCards(), RawCards.Size());

			if (MainPartNumber == LaiziNumber)
			{
				// 3个癞子当主体
				Ret.AddCards(SelectedLaize.GetCards(), SelectedLaize.Size());
			}
			else
			{
				Ret.AddCard(DCard(DCC_LAIZI, Context.Card[0]));		// 补充主体成3个. 
				Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));			// 其它的两个座位癞子
				Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
		}
		else
		{
			// 56 ***
			VASSERT(Context.CardNum[0] == 1  && Context.CardNum[1] == 1);
			int MainPartIndex = Context.Card[0] > Context.Card[1] ? 0 : 1;
			int XPartIndex = (MainPartIndex + 1) & 1;
			DHand& Ret = Increment(Results);
			Ret.SetHandType(DHT_3_2);
			Ret.SetHandNumber(Context.Card[MainPartIndex]);
			Ret.SetCards(RawCards.GetCards(), RawCards.Size());

			Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));		// 主体
			Ret.AddCard(DCard(DCC_LAIZI, Context.Card[MainPartIndex]));
			Ret.AddCard(DCard(DCC_LAIZI, Context.Card[XPartIndex]));		// X
			
		}
		return TRUE;
	}
	case 4:
	{
		DCardNumber RawNumber = RawCards[0].Number;
		DCardNumber LaiziNumber = SelectedLaize[0].Number;
		DCardNumber MainPartNumber = VMax(LaiziNumber, RawNumber);

		DHand& Ret = Increment(Results);
		Ret.SetHandType(DHT_3_2);
		Ret.SetHandNumber(MainPartNumber);
		Ret.AddCard(RawCards[0]);
		if (MainPartNumber == LaiziNumber)
		{
			// 3个癞子当主体
			Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));

			// 补一个
			Ret.AddCard(DCard(DCC_LAIZI, RawNumber));
		}
		else
		{
			// 补2个当主体
			Ret.AddCard(DCard(DCC_LAIZI, RawNumber));
			Ret.AddCard(DCard(DCC_LAIZI, RawNumber));

			// 余下2个癞子当对子
			Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			Ret.AddCard(DCard(DCC_LAIZI, LaiziNumber));
		}
		return TRUE;
	}
	default:
		break;
	}

	return FALSE;
}

/**
 * 寻找组成4个 
 */
inline int MainPartGaps_4(BYTE* CardSlot, int NumLaizi, int Number)
{
	if (CardSlot[Number] )
	{
		int Gaps = 4 - CardSlot[Number];
		if (Gaps <= NumLaizi)
		{
			return Gaps;
		}
	}
	return -1;
}

BOOL Compose4_2x1(const DHand& RawCards, const DHand& SelectedLaize, DHand& Hand)
{
	int Max = DCN_NULL;
	int Min = DCN_2;
	BYTE CardSlots[DCN_RED_JOKER + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int Index = 0; Index < RawCards.Size(); ++Index)
	{
		const DCard& Card = RawCards[Index];
		CardSlots[Card.Number]++;
		Max = VMax(Max, (int)Card.Number);
		Min = VMin(Min, (int)Card.Number);
	}

	int MainPart = Max;
	while (MainPart >= Min)
	{
		// 找主体
		int NumLaizi = SelectedLaize.Size();
		int NumGaps = MainPartGaps_4(CardSlots, NumLaizi, MainPart);
		if (NumGaps != -1)
		{
			// 找到一个4个的位置. 
			//int PrevIndex = (int)Results.Size();
			//Results.Increment(1);
			//DHand& Hand = Results[PrevIndex];
			Hand.Clear();
			Hand.SetHandType(DHT_4_2);
			Hand.SetHandNumber((DCardNumber)MainPart);
			Hand.SetCards(RawCards.GetCards(), RawCards.Size());
			for (int i = 0; i < NumGaps; ++i)
			{
				Hand.AddCard(DCard(DCC_LAIZI, (DCardNumber)MainPart));
			}

			return TRUE;
		}
		MainPart--;
	}


	return FALSE;
}

struct XPart4X2
{
	short NumCards;
	DCardNumber Number;
};

// 剩余的牌是否是4X2的对子.
inline BOOL IsRemainValidFor4x2(BYTE* CardSlot, int NumLaizi, int Number, int Min, int Max, XPart4X2 Double[2])
{
	int NumDouble = 0;
	DCardNumber ForDoubleNumbers[2];
	int NumDoubleAtoms = 0;
	for (int Index = Min; Index <= Max; ++Index)
	{
		if (Index == Number)
		{
			continue;
		}

		if (!CardSlot[Index])
		{
			// 当前槽位没有牌. 
			continue;
		}
		if (NumDouble >= 2)
		{
			return FALSE;
		}
		ForDoubleNumbers[NumDouble++] = (DCardNumber)Index;
		NumDoubleAtoms += (int)CardSlot[Index];
	}

	if ((NumDoubleAtoms + NumLaizi) != 4)
	{
		return FALSE;
	}

	// 特殊情况处理
	if (NumDouble == 0 && NumLaizi == 4)
	{
		Double[0].Number = Double[1].Number = DCN_NULL;
		Double[0].NumCards = Double[1].NumCards = 0;
		return TRUE;
	}

	if (NumDouble == 1)
	{
		if (NumDoubleAtoms == 4 )
		{
			Double[0].Number = Double[1].Number = ForDoubleNumbers[0];
			Double[0].NumCards = Double[1].NumCards = 2;
		}
		else if (NumDoubleAtoms == 3)
		{
			Double[0].Number = Double[1].Number = ForDoubleNumbers[0];
			Double[0].NumCards = 2;
			Double[1].NumCards = 1;
		}
		else if (NumDoubleAtoms == 2)
		{
			Double[0].Number = Double[1].Number = ForDoubleNumbers[0];
			Double[0].NumCards = Double[1].NumCards = 1;
		}
		else 
		{
			VASSERT(NumDoubleAtoms == 1);
			Double[0].Number = ForDoubleNumbers[0]; Double[0].NumCards = 1;
			Double[1].Number = DCN_NULL; Double[1].NumCards = 0;
			
		}
		return TRUE;
	}

	if (NumDouble == 2)
	{
		Double[0].Number = ForDoubleNumbers[0]; Double[0].NumCards = CardSlot[ForDoubleNumbers[0]];
		Double[1].Number = ForDoubleNumbers[1]; Double[1].NumCards = CardSlot[ForDoubleNumbers[1]];
		VASSERT(CardSlot[ForDoubleNumbers[0]] <= 2);
		VASSERT(CardSlot[ForDoubleNumbers[1]] <= 2);
		return TRUE;
	}

	return FALSE;
}

BOOL Compose4_2x2(const DHand& RawCards, const DHand& SelectedLaize, DHand& Hand)
{
	int Max = DCN_NULL;
	int Min = DCN_2;
	BYTE CardSlots[DCN_2 + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for (int Index = 0; Index < RawCards.Size(); ++Index)
	{
		const DCard& Card = RawCards[Index];
		if (Card.Number > DCN_2)
		{
			return FALSE;			// 4带2对时不能出现王. 
		}

		CardSlots[Card.Number]++;
		Max = VMax(Max, (int)Card.Number);
		Min = VMin(Min, (int)Card.Number);
	}

	int MainPart = Max;
	XPart4X2 DoubleNumber[2];
	const DCardNumber LaiziNumber = SelectedLaize[0].Number;
	while (MainPart >= Min)
	{
		// 找主体
		int NumLaizi = SelectedLaize.Size();
		int NumGaps = MainPartGaps_4(CardSlots, NumLaizi, MainPart);
		if (NumGaps != -1 && 
			IsRemainValidFor4x2(CardSlots, NumLaizi - NumGaps, MainPart, Min, Max, DoubleNumber)
			)
		{
			// 找到一个4个的位置. 
			//int PrevIndex = (int)Results.Size();
			//Results.Increment(1);
			//DHand& Hand = Results[PrevIndex];
			Hand.Clear();
			Hand.SetHandType(DHT_4_2);
			Hand.SetHandNumber((DCardNumber)MainPart);
			Hand.SetCards(RawCards.GetCards(), RawCards.Size());
			for (int i = 0; i < NumGaps; ++i)
			{
				Hand.AddCard(DCard(DCC_LAIZI, (DCardNumber)MainPart));
			}


			// 补充对子. 
			for (int i = 0; i < 2; ++i)
			{
				DCardNumber Double = DoubleNumber[i].Number;
				if (Double == DCN_NULL)
				{
					// 全部用癞子. 
					Hand.AddCard(DCard(DCC_LAIZI, LaiziNumber));
					Hand.AddCard(DCard(DCC_LAIZI, LaiziNumber));
				}
				else
				{
					VASSERT(DoubleNumber[i].NumCards);
					if (DoubleNumber[i].NumCards == 1)
					{
						// 补充一个癞子. 
						Hand.AddCard(DCard(DCC_LAIZI, Double));
					}
				}
			}


			return TRUE;
		}
		MainPart--;
	}

	return FALSE;
}

BOOL DDeck::Compose4_2(const DHand& RawCards, const DHand& SelectedLaize, DHand& Results)
{
	VASSERT(SelectedLaize.Size());

	int NumSel = RawCards.Size() + SelectedLaize.Size();
	if (NumSel == 6)
	{
		return Compose4_2x1(RawCards, SelectedLaize, Results);
	}else if (NumSel == 8)
	{
		return Compose4_2x2(RawCards, SelectedLaize, Results);
	}
	return FALSE;
}

//补顺子
void DDeck::ComposeStraight(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	// early exit
	int NumSel = RawCards.Size() + SelectedLaize.Size();
	if (NumSel < 5)
	{
		return;
	}
	int Max = DCN_NULL;
	BYTE CardSlots[DCN_2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int Index = 0; Index < RawCards.Size(); ++Index)
	{
		const DCard& Card = RawCards[Index];
		if (Card.Number >= DCN_2)
		{
			// 顺子中不能包含2以上的牌.
			return;
		}
		if (CardSlots[Card.Number])
		{
			// 如果选择的原始牌中包含重复牌, 则不可能组成顺子. 
			return;
		}

		CardSlots[Card.Number]++;
		Max = VMax(Max, (int)Card.Number);
	}

	const int VariantLen = (SelectedLaize.Size());
	const int StraightLen = NumSel;

	int TestTail = VMin<int>(Max + VariantLen, DCN_A);		// 尝试位置. 
	int TestHead;
	while (TestTail >= Max)
	{
		//
		TestHead = SeedLaizi_Straight(CardSlots, SelectedLaize.Size(), TestTail, StraightLen);
		if (TestHead != DCN_NULL)
		{
			// 找到一个有效的顺子.
			int PrevIndex = (int)Results.Size();
			Results.Increment(1);
			DHand& Straight = Results[PrevIndex];
			Straight.Clear();
			Straight.SetHandType(DHT_straight);
			Straight.SetHandNumber((DCardNumber)TestHead);

			Straight.AddCards(RawCards.GetCards(), RawCards.Size());

			// 填补癞子
			for (int Index = TestHead; Index <= TestTail; ++Index)
			{
				if (CardSlots[Index] == 0)
				{
					Straight.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				}
			}
			return;
		}

		TestTail--;

	}
}


/**
* 尝试将癞子从TailPos 位置开始, 往前放入补齐顺子, 如果补齐成功, 则返回顺子的头部牌编号, 否则返回DCN_NULL.
*/
int SeedLaizi_Straight2(BYTE* Slot, int NumLaizi, int TailPos, int Len)
{
	if (NumLaizi == 0 || NumLaizi > 4)
	{
		return DCN_NULL;
	}

	// 前面不允许有其它杂牌
	int Head = TailPos - Len + 1;
	for (int Pos = DCN_3; Pos < Head; ++Pos)
	{
		if (Slot[Pos])
		{
			return DCN_NULL;
		}
	}
	// 后面也不允许有其它杂牌.
	for (int Pos = TailPos + 1; Pos < DCN_2; ++Pos)
	{
		if (Slot[Pos])
		{
			return DCN_NULL;
		}
	}

	// 计算在区间内所需要的癞子数量. 
	int NumGaps = 0;
	for (int Pos = Head; Pos <= TailPos; ++Pos)
	{
		int NumNeed = 2 - Slot[Pos];
		VASSERT(NumNeed >= 0);
		NumGaps += NumNeed;
	}

	if (NumGaps != NumLaizi)
	{
		return DCN_NULL;
	}

	return Head;
}

void DDeck::ComposeStraight2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	int NumSel = RawCards.Size() + SelectedLaize.Size();
	if (NumSel < 6)
	{
		return;
	}
	if ((NumSel & 0x00000001))
	{
		return;
	}
	int Max = DCN_NULL, Min = DCN_2;
	BYTE CardSlots[DCN_2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int Index = 0; Index < RawCards.Size(); ++Index)
	{
		const DCard& Card = RawCards[Index];
		if (Card.Number >= DCN_2)
		{
			// 顺子中不能包含2以上的牌.
			return;
		}
		if (CardSlots[Card.Number] >= 2)
		{
			// 如果选择的原始牌中包含3个重复的牌
			return;
		}

		CardSlots[Card.Number]++;

		Max = VMax(Max, (int)Card.Number);
	}

	const int VariantLen = (SelectedLaize.Size() + 1) >> 1;
	const int StraightLen = NumSel >> 1;
	int TestTail = VMin<int>(Max + VariantLen, DCN_A);		// 尝试位置. 
	int TestHead;
	while (TestTail >= Max)
	{
		//
		TestHead = SeedLaizi_Straight2(CardSlots, SelectedLaize.Size(), TestTail, StraightLen);
		if (TestHead != DCN_NULL)
		{
			// 找到一个有效的顺子.
			int PrevIndex = (int)Results.Size();
			Results.Increment(1);
			DHand& Straight = Results[PrevIndex];
			Straight.Clear();
			Straight.SetHandType(DHT_straight_2);
			Straight.SetHandNumber((DCardNumber)TestHead);

			Straight.AddCards(RawCards.GetCards(), RawCards.Size());

			// 填补癞子
			for (int Index = TestHead; Index <= TestTail; ++Index)
			{
				if (CardSlots[Index] != 2)
				{
					if (CardSlots[Index])
					{
						Straight.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
					}
					else
					{
						Straight.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
						Straight.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
					}
					
				}
			}

		}

		TestTail--;

	}

}

bool IsValidPlane(const BYTE* CardSlots, int XSize)
{
	// 如果有2. 则2 只能成为配牌. 
	if (XSize == 0)
	{
		if (CardSlots[DCN_2] || CardSlots[DCN_BLACK_JOKER] || CardSlots[DCN_RED_JOKER])
		{
			return false;
		}
		return true;
	}else if (XSize == 1)
	{
		if (CardSlots[DCN_2] >= 2)
		{
			return false;
		}
	}else 
	{
		if (CardSlots[DCN_2] > 2)
		{
			return false;
		}
		if (CardSlots[DCN_BLACK_JOKER] || CardSlots[DCN_RED_JOKER])
		{
			// 如果有王的时候, 必须是组个子. 
			return false;
		}
	}

	return true;
}

/**
 * 判断主体需要的癞子缺口. 返回主体需要的癞子数量, 如果无法形成主体,返回-1
 */
int PlaneMainPartGaps(const BYTE* CardSlots, int MainPartLen, int NumLaizi, int Tail)
{
	const int Head = Tail - MainPartLen + 1;
	int NumGaps = 0;
	for (int Pos = Head; Pos <= Tail; ++Pos)
	{
		if (CardSlots[Pos] < 3)		// NOTE: CardSlots[Pos] 允许大于3. 
		{
			NumGaps += (3 - CardSlots[Pos]);
		}
	}

	if (NumGaps > NumLaizi)
	{
		// 无法用癞子补充缺口
		return -1;
	}
	return NumGaps;
}

bool ComposePlane_0(const BYTE* CardSlots, int NumX, int Tail, const DHand& RawCards, DHand& Result)
{
	Result.Clear();
	Result.AddCards(RawCards.GetCards(), RawCards.Size());

	// 加入癞子
	const int Head = Tail - NumX + 1;
	for (int Index = Head; Index <= Tail; ++Index)
	{
		if (CardSlots[Index] != 3)
		{
			if (CardSlots[Index] == 2)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else if (CardSlots[Index] == 1)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
		}
	}
	return true;
}

bool ComposePlane_1(const BYTE* CardSlots, int NumX, int NumLaizi, int Tail, const DCardNumber LaiziNumber, const DHand& RawCards, DHand& Result)
{
	Result.Clear();
	Result.AddCards(RawCards.GetCards(), RawCards.Size());
	// 加入癞子到主体中.
	const int Head = Tail - NumX + 1;
	for (int Index = Head; Index <= Tail; ++Index)
	{
		if (CardSlots[Index] != 3)
		{
			if (CardSlots[Index] == 2)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else if (CardSlots[Index] == 1)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
		}
	}

	// 加入X. 
	if (NumLaizi)
	{
		VASSERT(LaiziNumber != DCN_NULL);
		for (int i = 0; i < NumLaizi; ++i)
		{
			Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
		}

	}

	return true;
}

bool ComposePlane_2(const BYTE* CardSlots, int NumX, int NumLaizi, int Tail, const DCardNumber LaiziNumber, const DHand& RawCards, DHand& Result)
{
	const int Head = Tail - NumX + 1;
	BYTE Doubles[DCN_2 + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// 散牌. 
	int NumDoubles = 0;
	int XPartGaps = 0;
	for (int Index = DCN_3; Index <= DCN_2; ++Index)
	{
		if (Index >= Head && Index <= Tail)
		{
			// 处于主体中, 
			if (CardSlots[Index] == 4)
			{
				NumDoubles++;
				XPartGaps++;
				Doubles[Index] = 1;
			}
		}
		else
		{
			if (CardSlots[Index])
			{
				if (CardSlots[Index] == 4)
				{
					NumDoubles += 2;
				}else if (CardSlots[Index] == 3)
				{
					NumDoubles += 2;
					XPartGaps++;
				}
				else if (CardSlots[Index] == 2)
				{
					NumDoubles++;
				}
				else
				{
					NumDoubles++;
					XPartGaps++;
				}
				Doubles[Index] = CardSlots[Index];
			}
		}

	}

	if (XPartGaps > NumLaizi)
	{
		return false;
	}
	if (NumDoubles > NumX)
	{
		return false;
	}
	const int RemainLaiziForDoubles = NumLaizi - XPartGaps;
	int LaiziDouble = 0;	// 全部由癞子组成的对子数量
	if (NumDoubles == NumX)
	{
		// 
		if (XPartGaps != NumLaizi)
		{
			return false;
		}
	}
	else
	{
		// NumDoubles < NumX;	(*为癞子)
		// 考虑这种情况：	555666 3 ***   			NumDoubles = 1  RemainLaiziForDoubles = 2
		//					555666 33**				NumDoubles = 1  RemainLaiziForDoubles = 2
		//					555666 ****				NumDoubles = 0  RemainLaiziForDoubles = 4

		if (RemainLaiziForDoubles != 2 && RemainLaiziForDoubles != 4)
		{
			return false;
		}

		LaiziDouble = RemainLaiziForDoubles >> 1;

		if ((NumDoubles + LaiziDouble) != NumX)
		{
			return false;
		}
	}

	Result.Clear();
	Result.AddCards(RawCards.GetCards(), RawCards.Size());
	// 加入癞子到主体中.

	for (int Index = Head; Index <= Tail; ++Index)
	{
		if (CardSlots[Index] != 3)
		{
			if (CardSlots[Index] == 2)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else if (CardSlots[Index] == 1)
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
			else
			{
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
				Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));
			}
		}
	}

	// 加入X. 
	for (int Index = DCN_3; Index <= DCN_2; ++Index)
	{
		if (!Doubles[Index])
		{
			continue;
		}

		if (Doubles[Index] == 3 || Doubles[Index] == 1)
		{
			Result.AddCard(DCard(DCC_LAIZI, (DCardNumber)Index));	// 补充对子 
		}
	}

	
	if (LaiziDouble == 1)
	{
		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
	}else if (LaiziDouble == 2)
	{
		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));

		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
		Result.AddCard(DCard(DCC_LAIZI, LaiziNumber));
	}

	return true;
}


void DDeck::ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results, int NumX, int XPartSize)
{
	VASSERT(XPartSize >= 0 && XPartSize <= 2);
	VASSERT(NumX <= 6);
	const int NumCardNeed = NumX *(3 + XPartSize);
	const int NumCardSel = RawCards.Size() + SelectedLaize.Size();
	if (NumCardNeed != NumCardSel)
	{
		return;
	}

	const DCardNumber LaiziNumber = SelectedLaize.Size() ? SelectedLaize[0].Number : DCN_NULL;

	BYTE CardSlots[DCN_RED_JOKER + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	DCardNumber Max = DCN_NULL;	// 最大的非癞子牌.
	DCardNumber Min = DCN_2;
	for (int Index = 0; Index < RawCards.Size(); ++Index)
	{
		const DCard& Card = RawCards[Index];
		CardSlots[Card.Number]++;
		if (Card.Number < DCN_2)
		{
			Max = VMax(Max, Card.Number);
			Min = VMin(Min, Card.Number);
		}
	}

	// 
	if (!IsValidPlane(CardSlots, XPartSize))
	{
		return;
	}

	// 计算飞机最大的3个起始位置. 
	const int NumLaizi = SelectedLaize.Size();
	const int MainPartMax = VMin<int>(Max + 1, DCN_A);	// 最大的潜在主体. 
	const int MainPartMin = VMax<int>(Min - 1 + NumX - 1, DCN_3);	// 最小的潜在主体. 

	DHand PlaneHand;
	bool PlaneValid;
	int MainPartTail = MainPartMax;
	while (MainPartTail >= MainPartMin)
	{
		PlaneValid = false;
		// 尝试找到主体. 
		int MainPartLaizi = PlaneMainPartGaps(CardSlots, NumX, NumLaizi, MainPartTail);
		if (MainPartLaizi != -1)
		{
			// 找到一个主体, 检测组合部分是否合法.  
			if (XPartSize == 0)
			{
				// 什么都不带的情况
				if (MainPartLaizi == NumLaizi && ComposePlane_0(CardSlots, NumX, MainPartTail, RawCards, PlaneHand))
				{
					PlaneValid = true;
				}
			}else if (XPartSize == 1)
			{
				PlaneValid = ComposePlane_1(CardSlots, NumX, NumLaizi - MainPartLaizi, MainPartTail, LaiziNumber, RawCards, PlaneHand);
			}
			else
			{
				PlaneValid = ComposePlane_2(CardSlots, NumX, NumLaizi - MainPartLaizi, MainPartTail, LaiziNumber, RawCards, PlaneHand);
			}

			if (PlaneValid)
			{
				PlaneHand.SetHandType(DHT_straight_3);
				PlaneHand.SetHandNumber((DCardNumber)(MainPartTail - NumX + 1));
				PlaneHand.SetStraightLen(NumX);
				Results.PushBack(PlaneHand);
				return;	// 只获取一个结果. 
			}
		}




		MainPartTail--;
	}
}

void DDeck::ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{

	const int NumCards = RawCards.Size() + SelectedLaize.Size();
	if (NumCards < 6)
	{
		return;
	}

	//2 :	6		8		10 
	//3 :	9		12		15
	//4 :	12		16		20 
	//5 :	15		20 
	//6 :	18

	switch (NumCards)
	{
	case 6: ComposePlane(RawCards, SelectedLaize, Results, 2, 0); break;
	case 8: ComposePlane(RawCards, SelectedLaize, Results, 2, 1); break;
	case 10: ComposePlane(RawCards, SelectedLaize, Results, 2, 2); break;
	case 9: ComposePlane(RawCards, SelectedLaize, Results, 3, 0); break;
	case 12:
	{
		ComposePlane(RawCards, SelectedLaize, Results, 4, 0);
		ComposePlane(RawCards, SelectedLaize, Results, 3, 1);
		break;
	}
	case 15:
	{
		ComposePlane(RawCards, SelectedLaize, Results, 5, 0);
		ComposePlane(RawCards, SelectedLaize, Results, 3, 2);
		break;
	}
	case 16:
	{
		ComposePlane(RawCards, SelectedLaize, Results, 4, 1); 
		break;
	}
	case 18:
	{
		ComposePlane(RawCards, SelectedLaize, Results, 6, 0); 
		break;
	}
	case 20:
	{
		ComposePlane(RawCards, SelectedLaize, Results, 4, 2);
		ComposePlane(RawCards, SelectedLaize, Results, 5, 1);
		break;
	}
	default:
		return;
	}
}

//填充炸弹

void DDeck::FillBom(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results)
{
	DHand Straight = RawCards;
	DCardNumber  HandNumber = TypeAtom::Is2K(Straight.GetCards(), Straight.Size());
	if (HandNumber != DCN_NULL)
	{
		Straight.SetHandType(DHT_2K);
		Straight.SetHandNumber(DCN_RED_JOKER);
		Results.PushBack(Straight);
		return;
	}
	if (RawCards.Size() + SelectedLaize.Size() != 4)return;
	int limitlaizi = SelectedLaize.Size();//癞子的数量
	if (RawCards[0].Number > DCardNumber::DCN_2)return;
	if (limitlaizi == 0)//没有癞子
	{
		HandNumber = TypeAtom::IsBomb(RawCards.GetCards(), RawCards.Size());
		if (HandNumber != DCN_NULL)
		{
			Straight.SetHandType(DHT_4);
			Straight.SetHandNumber(HandNumber);
			Results.PushBack(RawCards);
		}
		return;
	}
	if (RawCards.Size() > 0)
	{
		for (int i = 0; i < limitlaizi; i++)
		{
			Straight.AddCard(DCard(DCardColor::DCC_LAIZI, RawCards[0].Number));
		}
	}
	else //四个癞子
	{
		for (int i = 0; i < limitlaizi; i++)
		{
			Straight.AddCard(DCard(DCardColor::DCC_LAIZI, SelectedLaize[0].Number));
		}
	}
	
	HandNumber = TypeAtom::IsBomb(Straight.GetCards(), Straight.Size());
	if (HandNumber != DCN_NULL)
	{
		Straight.SetHandType(DHT_4);
		Straight.SetHandNumber(HandNumber);
		Results.PushBack(Straight);
	}
}
//填充飞机不带翅膀

bool DDeck::InspectCardExist(DCard* card, int number)
{
	return true;
}

BOOL DDeck::CanFollowLastHand(const DHand& CurrHand, const DHand& LastHand, DCard LaiZi) const
{
	DHandType LastType = LastHand.GetHandType();
	DCardNumber LastNumber = LastHand.GetHandNumber();
	DHandType CurrHandType = CurrHand.GetHandType();
	DCardNumber CurrNumber = CurrHand.GetHandNumber();

	if (LastType == DHT_2K)
	{
		return FALSE;
	}

	if (LastType == CurrHandType)
	{
		switch (LastType)
		{
		case DHT_1:
		case DHT_2:
		case DHT_3:
		case DHT_3_1:
		case DHT_3_2:
		case DHT_4_2:
			return CurrNumber > LastNumber;
		case DHT_4:
			{
				if (LaiZi.IsValid())
				{
					if (CurrNumber == LaiZi.Number)
					{
						return TRUE;
					}
					else if (LastNumber == LaiZi.Number)
					{
						return FALSE;
					}
					else
					{
						bool CurrUsingLaizi = CurrHand.IsUsingLaiZi();
						bool LastUsingLaiZi = LastHand.IsUsingLaiZi();
						if (CurrUsingLaizi && !LastUsingLaiZi)
						{
							return FALSE;
						}
						if (!CurrUsingLaizi && LastUsingLaiZi)
						{
							return TRUE;
						}
					}
				}
				return CurrNumber > LastNumber;
			}
		case DHT_straight:
		case DHT_straight_2:
		case DHT_straight_3:
			return CurrNumber > LastNumber && CurrHand.Size() == LastHand.Size();
		default:
			return FALSE;
		}
	}

	if (CurrHandType == DHT_2K || CurrHandType == DHT_4)
	{
		return TRUE;
	}

	return FALSE;
}
//接
BOOL DDeck::CanFollowLastHand(const DHand& CurrHand, DCard LaiZi /* = DCard() */) const
{
	VASSERT(CurrHand.GetHandType() != DHT_UNKNOWN && CurrHand.GetHandNumber() != DCN_NULL);
	const DHand* LastHand = GetLastHand();
	if (LastHand == NULL)
	{
		return TRUE;
	}
	return CanFollowLastHand(CurrHand, *LastHand);
}



void DDeck::UpdateAIHint(int CharIndex, const DCard* PlayedCards, int NumCards)
{
	for (int Index = 0; Index < NumCards; ++Index)
	{
		const DCard& Card = PlayedCards[Index];
		mCardSlots[Card.Number]--;
		VASSERT(mCardSlots[Card.Number] >= 0);
	}
}

void DDeck::ResetAIHint()
{
	for (int i = DCN_3; i <= DCN_2; ++i)
	{
		mCardSlots[i] = 4;
	}
	mCardSlots[DCN_BLACK_JOKER] = mCardSlots[DCN_RED_JOKER] = 1;
}

DCardNumber DDeck::AI_QueryMaxCard() const
{
	for (int i = DCN_RED_JOKER; i >= DCN_3; --i)
	{
		if (mCardSlots[i])
		{
			return (DCardNumber)i;
		}
	}
	return DCN_NULL;
}

DCardNumber DDeck::AI_QueryMaxCard(const BYTE* CardSlots) const
{
	for (int i = DCN_RED_JOKER; i >= DCN_3; --i)
	{
		VASSERT(mCardSlots[i] >= CardSlots[i]);
		if (mCardSlots[i] > CardSlots[i])
		{
			return (DCardNumber)i;
		}
	}
	return DCN_NULL;
}

DCardNumber DDeck::AI_QueryCardGreatThen(const BYTE* CardSlots, DCardNumber Compare) const
{
	for (int i = DCN_RED_JOKER; i > Compare; --i)
	{
		if (CardSlots[i])
		{
			return (DCardNumber)i;
		}
	}
	return DCN_NULL;
}



bool DDeck::AI_Bomb(const BYTE* CardSlots, DCardNumber BomberNumber) const
{
	if (BomberNumber >= DCN_BLACK_JOKER)
	{
		return true;
	}

	int GreatBomber = 0;
	for (int i = DCN_2; i > BomberNumber; --i)
	{
		if (CardSlots[i] == 0 && mCardSlots[i] == 4)
		{
			GreatBomber++;
		}
	}

	// 对王在外面. 
	if (mCardSlots[DCN_BLACK_JOKER] && mCardSlots[DCN_RED_JOKER] &&
		CardSlots[DCN_BLACK_JOKER] == 0 && CardSlots[DCN_RED_JOKER] == 0)
	{
		GreatBomber++;
	}


	static const int BombRate[4] =
	{
		100,
		80,
		30,
		10
	};
	if (GreatBomber == 0)
	{
		return true;
	}
	if (GreatBomber > 3)
	{
		return false;
	}
	return Chance(BombRate[GreatBomber]);
}

int DDeck::AI_QueryRemainBombers(const BYTE* CardSlots, DCardNumber GreatThen) const
{
	int GreatBomber = 0;
	for (int i = DCN_2; i > GreatThen; --i)
	{
		if (CardSlots[i] == 0 && mCardSlots[i] == 4)
		{
			GreatBomber++;
		}
	}

	// 对王在外面. 
	if (mCardSlots[DCN_BLACK_JOKER] && mCardSlots[DCN_RED_JOKER] &&
		CardSlots[DCN_BLACK_JOKER] == 0 && CardSlots[DCN_RED_JOKER] == 0)
	{
		GreatBomber++;
	}
	return GreatBomber;
}

DCardNumber DDeck::AI_QueryMaxBomber(const BYTE* CardSlots, DCardNumber GreatThen) const
{
	if (GreatThen > DCN_2)
	{
		return DCN_NULL;
	}

	if (mCardSlots[DCN_BLACK_JOKER] && mCardSlots[DCN_RED_JOKER] &&
		CardSlots[DCN_BLACK_JOKER] == 0 && CardSlots[DCN_RED_JOKER] == 0)
	{
		return DCN_RED_JOKER;
	}

	for (int i = DCN_2; i > GreatThen; --i)
	{
		if (CardSlots[i] == 0 && mCardSlots[i] == 4)
		{
			return (DCardNumber)i;
		}
	}
	return DCN_NULL;
}

int DDeck::AI_QueryRemainBombers(DCardNumber GreatThen) const
{
	int NumBombers = 0;
	for (int i = DCN_2; i > GreatThen; --i)
	{
		if (mCardSlots[i] == 4)
		{
			NumBombers++;
		}
	}

	if (mCardSlots[DCN_BLACK_JOKER] && mCardSlots[DCN_RED_JOKER])
	{
		NumBombers++;
	}

	return NumBombers;
}

int DDeck::AI_Determ_Single_Max(const BYTE* CardSlots, DCardNumber SingleNumber, bool IncludeBomber ) const
{
	// 外面的炸弹数量
	int NumBombers = IncludeBomber ? AI_QueryRemainBombers(CardSlots, DCN_NULL) : 0;

	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > SingleNumber; --i)
	{
		int Remain = mCardSlots[i] - CardSlots[i];
		if (Remain && Remain != 4)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	int RemainKS = mCardSlots[DCN_BLACK_JOKER] - CardSlots[DCN_BLACK_JOKER];
	int RemainKB = mCardSlots[DCN_RED_JOKER] - CardSlots[DCN_RED_JOKER];
	if (RemainKS != RemainKB )
	{
		NumGreater++;	
	}

	NumGreater += NumBombers;

	/*int Rate = 0;
	if (NumGreater == 0)
	{
	return true;
	}
	else if (NumGreater == 1)
	{
	Rate = 80;
	}
	else if (NumGreater == 2)
	{
	Rate = 50;
	}
	else if (NumGreater == 3)
	{
	Rate = 20;
	}
	else
	{
	return false;
	}
	return Chance(Rate);*/
	return NumGreater;
}


bool DDeck::AI_Determ_Single_CanBeFollowByLord(const BYTE* OtherHand, DCardNumber SingleNumber) const
{
	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_RED_JOKER; i > SingleNumber; --i)
	{
		int Remain = OtherHand[i];
		if (Remain )		// don't include bomber.
		{
			NumGreater += Remain;
		}
	}

	if (NumGreater == 0)
	{
		return false;
	}

	if (SingleNumber >= DCN_A)
	{
		return NumGreater > 0;
	}else if ( SingleNumber >= DCN_J)
	{
		int Rate = NumGreater * 20;
		return Chance(Rate);
	}
	int Rate = NumGreater * 10;
	return Chance(Rate);
}


int DDeck::AI_Determ_Double_Greater(const BYTE* CardSlots, DCardNumber DoubleNumber) const
{
	// 外面的炸弹数量
	int NumBombers = AI_QueryRemainBombers(CardSlots, DCN_NULL);

	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > DoubleNumber; --i)
	{
		int Remain = mCardSlots[i] - CardSlots[i];
		if (Remain >= 2 && Remain != 4)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	NumGreater += NumBombers;
	return NumGreater;
}

int DDeck::AI_Determ_Double_Lesser(const BYTE* CardSlots, DCardNumber DoubleNumber) const
{
	int NumLesser = 0;	// 大于我的潜在对子数量
	for (int i = DCN_3; i < DoubleNumber; ++i)
	{
		int Remain = mCardSlots[i] - CardSlots[i];
		if (Remain >= 2 && Remain != 4)		// don't include bomber.
		{
			NumLesser++;
		}
	}
	return NumLesser;
}

bool DDeck::AI_Determ_Double_Is_Max(const BYTE* CardSlots, DCardNumber DoubleNumber) const
{
	int NumGreater = AI_Determ_Double_Greater(CardSlots, DoubleNumber);

	int Rate = 0;
	if (NumGreater == 0)
	{
		return true;
	}
	else if (NumGreater == 1)
	{
		Rate = 80;
	}
	else if (NumGreater == 2)
	{
		Rate = 50;
	}
	else if (NumGreater == 3)
	{
		Rate = 20;
	}
	else
	{
		return false;
	}
	return Chance(Rate);
}

int DDeck::AI_Determ_3_Greater(const BYTE* CardSlots, DCardNumber ThreeNumber) const
{
	// 外面的炸弹数量
	int NumBombers = AI_QueryRemainBombers(CardSlots, DCN_NULL);

	int NumGreater = 0;	// 大于我的潜在对子数量
	for (int i = DCN_2; i > ThreeNumber; --i)
	{
		int Remain = mCardSlots[i] - CardSlots[i];
		if (Remain == 3)		// don't include bomber.
		{
			NumGreater++;
		}
	}

	NumGreater += NumBombers;
	return NumGreater;
}

bool DDeck::AI_Determ_3_Is_Max(const BYTE* CardSlots, DCardNumber ThreeNumber) const
{
	int NumGreater = AI_Determ_3_Greater(CardSlots, ThreeNumber);

	int Rate = 0;
	if (NumGreater == 0)
	{
		return true;
	}
	else if (NumGreater == 1)
	{
		Rate = 90;
	}
	else if (NumGreater == 2)
	{
		Rate = 70;
	}
	else if (NumGreater == 3)
	{
		Rate = 30;
	}
	else
	{
		return false;
	}
	return Chance(Rate);
}

int DDeck::AI_Determ_S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}
	BYTE RemainCards[DCN_RED_JOKER + 1];
	for (int i = DCN_A; i >= Start; --i)
	{
		RemainCards[i] = mCardSlots[i] - CardSlots[i];
	}
	
	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight(RemainCards, i, Len))
		{
			NumValidStraights++;
		}
	}
	
	return NumValidStraights;

}

int DDeck::AI_Determ_2S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}
	BYTE RemainCards[DCN_RED_JOKER + 1];
	for (int i = DCN_A; i >= Start; --i)
	{
		RemainCards[i] = mCardSlots[i] - CardSlots[i];
	}

	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight2(RemainCards, i, Len))
		{
			NumValidStraights++;
		}
	}

	return NumValidStraights;
}


int DDeck::AI_Determ_3S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const
{
	int Tail = Head + Len - 1;
	if (Tail >= DCN_A)
	{
		return 0;
	}
	// 初始化剩余的牌堆. 
	int Start = Head + 1;
	int Head2 = DCN_A + 1 - Len;
	if (Head2 < Start)
	{
		return 0;
	}
	BYTE RemainCards[DCN_RED_JOKER + 1];
	for (int i = DCN_A; i >= Start; --i)
	{
		RemainCards[i] = mCardSlots[i] - CardSlots[i];
	}
	int NumValidStraights = 0;
	for (int i = Start; i <= Head2; ++i)
	{
		if (DAISlover::HasStraight3(RemainCards, i, Len))
		{
			NumValidStraights++;
		}
	}

	return NumValidStraights;
}

bool DDeck::AI_Determ_S_IsMax(const BYTE* CardSlots, DCardNumber Head, int Len) const
{
	//int NumValidStraights = 

	return true;
}

// for server.
bool DDeck::IsValidHand(const DHand& Hand)
{
	if (Hand.IsEmpty())
	{
		return true;
	}

	DHandType InHandType = Hand.GetHandType();
	if (InHandType == 0 || InHandType > DHT_2K)
	{
		return false;
	}

	DCardNumber InNumber = Hand.GetHandNumber();
	if (InNumber == DCN_NULL || InNumber > DCN_RED_JOKER)
	{
		return false;
	}

	DCardNumber CheckNumber = DCN_NULL;
	// 校验是否为合法牌型. 
	switch (InHandType)
	{
	case DHT_1:
	{
		if (Hand.Size() == 1)
		{
			CheckNumber = Hand[0].Number;
		}
		break;
	}
	case DHT_2:
	{
		if (Hand.Size() == 2)
		{
			CheckNumber = TypeAtom::IsDouble(Hand.GetCards(), 2);
		}
		break;
	}
	case DHT_3:
		if (Hand.Size() == 3)
		{
			CheckNumber = TypeAtom::Is3(Hand.GetCards(), 3);
		}
		break;
	case DHT_3_1:
		if (Hand.Size() == 4)
		{
			CheckNumber = TypeAtom::Is3_1(Hand.GetCards(), 4);
		}
		break;
	case DHT_3_2:
		if (Hand.Size() == 5)
		{
			CheckNumber = TypeAtom::Is3_2(Hand.GetCards(), 5);
		}
		break;
	case DHT_4_2:
		if (Hand.Size() == 6 || Hand.Size() == 8)
		{
			CheckNumber = TypeAtom::Is4_2(Hand.GetCards(), Hand.Size());
		}
		break;
	case DHT_straight:
		CheckNumber = TypeAtom::IsStraight(Hand.GetCards(), Hand.Size());
		break;
	case DHT_straight_2:
		CheckNumber = TypeAtom::IsStraight2(Hand.GetCards(), Hand.Size());
		break;
	case DHT_straight_3:
		if (Hand.GetStraightLen() >= 2)
		{
			CheckNumber = TypeAtom::IsPlane_Follow(Hand.GetCards(), Hand.Size(), Hand.GetStraightLen());
		}
		break;
	case DHT_4:
		if (Hand.Size() == 4)
		{
			CheckNumber = TypeAtom::IsBomb(Hand.GetCards(), Hand.Size());
		}
		break;
	case DHT_2K:
		if (Hand.Size() == 2)
		{
			CheckNumber = TypeAtom::Is2K(Hand.GetCards(), 2);
		}
		break;
	default:
		break;
	}

	if (CheckNumber != InNumber)
	{
		return false;
	}

	return true;
}


void DDeckCharacter::DisplayHandCard()
{

}




#if VDEBUG
#include "DHintUnslover.h"
namespace DeckTest
{
	void TestHint()
	{
		DHintUnslover Hint;
		 DCard MyCards[] = { DCard(DCC_LAIZI, DCN_Q), DCard(DCC_HEARTS, DCN_2), DCard(DCC_SPADES, DCN_A), DCard(DCC_CLUBS, DCN_A),
			DCard(DCC_DIAMONDS, DCN_K), DCard(DCC_SPADES, DCN_4), DCard(DCC_HEARTS, DCN_4), DCard(DCC_SPADES, DCN_3), DCard(DCC_CLUBS, DCN_3) };

		 Hint.Reset(MyCards, sizeof(MyCards) / sizeof(MyCards[0]), DCN_Q);

		 DCard FollowCards[] = { DCard(DCC_LAIZI, DCN_J), DCard(DCC_HEARTS, DCN_J), DCard(DCC_SPADES, DCN_J), DCard(DCC_CLUBS, DCN_J) };
		 VArray<DHand> Hands;
		 DHand FollowHand(DHT_4, DCN_J);
		 FollowHand.SetCards(FollowCards, 4);

		 Hint.GetValidHands(&FollowHand , Hands);
		 VASSERT(Hands.IsEmpty());
	}

	void TestFollowS2()
	{
		DHintUnslover Hint;
		DCard MyCards[] = { DCard(DCC_SPADES, DCN_K), DCard(DCC_HEARTS, DCN_K), DCard(DCC_SPADES, DCN_Q), DCard(DCC_CLUBS, DCN_Q),
			DCard(DCC_DIAMONDS, DCN_J), DCard(DCC_SPADES, DCN_J), DCard(DCC_HEARTS, DCN_10), DCard(DCC_SPADES, DCN_10), DCard(DCC_CLUBS, DCN_8) };

		Hint.Reset(MyCards, sizeof(MyCards) / sizeof(MyCards[0]), DCN_NULL);

		DCard FollowCards[] = { DCard(DCC_SPADES, DCN_4), DCard(DCC_HEARTS, DCN_4), DCard(DCC_SPADES, DCN_5), DCard(DCC_CLUBS, DCN_5), DCard(DCC_SPADES, DCN_6), DCard(DCC_CLUBS, DCN_6) };
		
		DHand FollowHand(DHT_straight_2, DCN_4);
		FollowHand.SetCards(FollowCards, 6);

		VArray<DHand> Hands;
		Hint.GetValidHands(&FollowHand, Hands);
		VASSERT(!Hands.IsEmpty());
	}

	void Test_ComposePlane()
	{
		VArray<DHand> Result;

		{
			Result.Clear();
			// 555 66 33 44 **
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), 
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5),
				DCard(DCC_CLUBS, DCN_4), DCard(DCC_DIAMONDS, DCN_4),
				DCard(DCC_CLUBS, DCN_3), DCard(DCC_DIAMONDS, DCN_3) };

			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A)};

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_straight_3);
			VASSERT(H.GetStraightLen() == 2);
			VASSERT(H.GetHandNumber() == DCN_5);
			Result.Clear();
		}

		{
			// 555 666 ****
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };

			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A) };

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			Result.Clear();
		}

		{
			// 555 666 3 ***
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5),
				DCard(DCC_DIAMONDS, DCN_3) };
			
			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A) };

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			Result.Clear();
		}
		{
			// 555 66 3 ****
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), 
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5),
				DCard(DCC_DIAMONDS, DCN_3) };

			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A) };

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			Result.Clear();
		}
		{
			// 555 666 33 **
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5),
				DCard(DCC_DIAMONDS, DCN_3), DCard(DCC_CLUBS, DCN_3) };

			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A), DCard(DCC_LAIZI, DCN_A)};

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			Result.Clear();
		}

		{
			// 555 666 333 *
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5),
				DCard(DCC_DIAMONDS, DCN_3), DCard(DCC_CLUBS, DCN_3), DCard(DCC_SPADES, DCN_3) };

			DCard LaiziCards[] = { DCard(DCC_LAIZI, DCN_A) };

			DHand RawHand;
			RawHand.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			DHand Laizi;
			Laizi.SetCards(LaiziCards, sizeof(LaiziCards) / sizeof(LaiziCards[0]));
			DDeck::ComposePlane(RawHand, Laizi, Result);

			VASSERT(!Result.IsEmpty());
			Result.Clear();
		}

	}


	void TestCompose2()
	{

	}


	void TestCompose3()
	{

	}


	void TestCompose3_1()
	{
		VArray<DHand> Result;
		DHand RawHands;
		DHand LaiziHands;
		const DCardNumber LaiziNumber = DCN_A;
		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 0;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{ 
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_1(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_1);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_5);
		}
	
		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_CLUBS, DCN_7) };
			const int NumLaizi = 1;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_1(RawHands, LaiziHands, Result);
			VASSERT(Result.IsEmpty());
		}

		{
			DCard MyCards[] = {DCard::SMALL_KING, DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 1;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_1(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_1);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_5);
		}

		{
			DCard MyCards[] = { DCard::SMALL_KING, DCard(DCC_HEARTS, DCN_5)};
			const int NumLaizi = 2;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_1(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_1);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_5);
		}

		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6), DCard(DCC_HEARTS, DCN_5) };
			const int NumLaizi = 2;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_1(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_1);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_6);
		}
	}


	void TestCompose3_2()
	{
		VArray<DHand> Result;
		DHand RawHands;
		DHand LaiziHands;
		const DCardNumber LaiziNumber = DCN_A;
		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 0;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			RawHands.Sort();
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_5);
		}

		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6),
				DCard(DCC_SPADES, DCN_5), DCard(DCC_CLUBS, DCN_7) };
			const int NumLaizi = 1;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(Result.IsEmpty());
		}

		{
			DCard MyCards[] = { DCard::SMALL_KING, DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 1;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			RawHands.Sort();
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(Result.IsEmpty());
		}

		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_6), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 1;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			RawHands.Sort();
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_6);
		}

		{
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_5), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 2;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_5);
		}

		{
			DCard MyCards[] = { DCard(DCC_SPADES, DCN_6), DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 2;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_6);
		}


		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_5), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 3;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == LaiziNumber);
		}

		{
			DCard MyCards[] = { DCard(DCC_HEARTS, DCN_6), DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 3;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == DCN_6);
		}
		{
			DCard MyCards[] = { DCard(DCC_CLUBS, DCN_5) };
			const int NumLaizi = 4;

			Result.Clear();
			RawHands.Clear();
			RawHands.SetCards(MyCards, sizeof(MyCards) / sizeof(MyCards[0]));
			LaiziHands.Clear();
			for (int l = 0; l < NumLaizi; ++l)
			{
				LaiziHands.AddCard(DCard(DCC_LAIZI, LaiziNumber));
			}
			DDeck::Compose3_2(RawHands, LaiziHands, Result);
			VASSERT(!Result.IsEmpty());
			VASSERT(Result.Size() == 1);
			const DHand& H = Result[0];
			VASSERT(H.GetHandType() == DHT_3_2);
			VASSERT(H.GetStraightLen() == 0);
			VASSERT(H.GetHandNumber() == LaiziNumber);
		}

	}
}

void DeckUnitTest()
{
	DeckTest::TestHint();
	DeckTest::TestCompose2();
	DeckTest::TestCompose3();
	DeckTest::TestCompose3_1();
	DeckTest::TestCompose3_2();
	DeckTest::TestFollowS2();
	DeckTest::Test_ComposePlane();

	{
		DHand RawCards;

		RawCards.AddCard(DCard(DCC_CLUBS, DCN_9));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_9));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_8));

		DHand Laize;
		Laize.AddCard(DCard(DCC_LAIZI, DCN_J));

		VArray<DHand> Results;

		//		Results =DDeck::LaiZiEvalHandType(RawCards, Laize);

		DDeck::Compose3_1(RawCards, Laize, Results);
		DDeck::Compose3_2(RawCards, Laize, Results);
		DDeck::ComposeStraight2(RawCards, Laize, Results);
		int i = 0;
	}

	{
		DHand RawCards;
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_6));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_7));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_8));

		DHand Laize;
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));

		VArray<DHand> Results;
		DDeck::ComposeStraight2(RawCards, Laize, Results);
	}


	{
		DHand RawCards;
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_6));
		RawCards.AddCard(DCard(DCC_HEARTS, DCN_6));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_7));
		RawCards.AddCard(DCard(DCC_HEARTS, DCN_7));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_8));

		DHand Laize;
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));

		VArray<DHand> Results;
		DDeck::ComposePlane(RawCards, Laize, Results);
	}

	{
		DHand RawCards;
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_6));
		RawCards.AddCard(DCard(DCC_HEARTS, DCN_6));
		RawCards.AddCard(DCard(DCC_DIAMONDS, DCN_6));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_7));
		RawCards.AddCard(DCard(DCC_HEARTS, DCN_7));
		RawCards.AddCard(DCard(DCC_DIAMONDS, DCN_7));
		RawCards.AddCard(DCard(DCC_CLUBS, DCN_8));
		RawCards.AddCard(DCard(DCC_HEARTS, DCN_8));
		RawCards.AddCard(DCard(DCC_DIAMONDS, DCN_8));

		DHand Laize;
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));
		Laize.AddCard(DCard(DCC_LAIZI, DCN_2));

		VArray<DHand> Results;
		DDeck::ComposePlane(RawCards, Laize, Results);

		// 上面这个结果应该包含2个. 666 777 888 999 无翅膀长度4的飞机 和 777 888 999 X 6 6 6 带3个6作为翅膀的长度为3的飞机 

		VASSERT(Results.Size() == 2);
	}
}

#endif
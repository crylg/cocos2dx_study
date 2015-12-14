#include "VStdAfx.h"
#include "DHintUnslover.h"
#define LAIZI(Number) DCard(DCC_LAIZI, (DCardNumber)(Number))

DHintUnslover::DHintUnslover()
{
	Clear();
}


DHintUnslover::~DHintUnslover()
{
}

void DHintUnslover::Clear()
{
	for (int i = 0; i <= DCN_RED_JOKER; ++i)
	{
		mCardSlots[i].NumCards = 0;
	}
	mNumLaizi = 0;
}

void DHintUnslover::Reset(const DCard* Cards, int NumCards,DCardNumber Laizi)
{
	for (int i = 0; i <= DCN_RED_JOKER; ++i)
	{
		mCardSlots[i].NumCards = 0;
	}
	mNumLaizi = 0;
	mCards.Clear();
	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& c = Cards[i];
		if (Laizi == c.Number)
		{
			mNumLaizi++;
		}
		else
		{
			mCardSlots[c.Number].AddCard(c);
			mCards.AddCard(c);
		}
	}

	mLaizi = Laizi;
}

void DHintUnslover::GetValidHands(const DHand* LastHand, VArray<DHand>& Hands) const
{
	DHandType HandType = LastHand->GetHandType();
	DCardNumber HandNumber = LastHand->GetHandNumber();

	switch (HandType)
	{
	case DHT_1: Hint_1(HandNumber, Hands); return;
	case DHT_2: Hint_2(HandNumber, Hands); return;
	case DHT_3: Hint_3(HandNumber, Hands); return;
	case DHT_3_1: Hint_3_1(HandNumber, Hands); return;
	case DHT_3_2: Hint_3_2(HandNumber, Hands); return;
	case DHT_4_2: Hint_4_2(HandNumber, Hands); return;
	case DHT_straight: Hint_straight(HandNumber,LastHand->Size(), Hands); return;
	case DHT_straight_2:Hint_straight2(HandNumber, LastHand->Size(), Hands); return;
	case DHT_straight_3:Hint_straight3(HandNumber, LastHand, Hands); return;
	case DHT_4:Hint_4(HandNumber,LastHand, Hands); return;
	case DHT_2K:
		break;
	default:
		break;
	}
}

void DHintUnslover::GetBombRaw(const DCardNumber Number, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
	{
		if (mCardSlots[i].NumCards == 4)
		{
			DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
			HBomb.AddCards(mCardSlots[i].Cards, 4);
		}
	}
}

void DHintUnslover::GetBomb(const DCardNumber Number, VArray<DHand>& Hands, bool AllowLaizi) const
{
	
	// 找出所有的4个. 
	for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
	{
		if (mCardSlots[i].NumCards == 4)
		{
			DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
			HBomb.AddCards(mCardSlots[i].Cards, 4);
		}
	}

	// 
	if (mNumLaizi && AllowLaizi)
	{
		GetBomb_L(Number, Hands);
	}

	// 找出对王
	if (mCardSlots[DCN_BLACK_JOKER].NumCards && mCardSlots[DCN_RED_JOKER].NumCards)
	{
		DHand& K2 = Increment(Hands, DHand(DHT_2K, DCN_RED_JOKER));
		K2.AddCard(DCard::BIG_KING);
		K2.AddCard(DCard::SMALL_KING);
	}
}

void DHintUnslover::Get2K(VArray<DHand>& Hands) const
{
	if (mCardSlots[DCN_BLACK_JOKER].NumCards && mCardSlots[DCN_RED_JOKER].NumCards)
	{
		DHand& K2 = Increment(Hands, DHand(DHT_2K, DCN_RED_JOKER));
		K2.AddCard(DCard::BIG_KING);
		K2.AddCard(DCard::SMALL_KING);
	}
}

void DHintUnslover::GetBomb_L(const DCardNumber Number, VArray<DHand>& Hands) const
{
	VASSERT(mNumLaizi && mLaizi);
	// 先看3张的. 
	for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
	{
		if (mCardSlots[i].NumCards == 3)
		{
			DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
			HBomb.AddCards(mCardSlots[i].Cards, 3);
			HBomb.AddCard(DCard(DCC_LAIZI, (DCardNumber)i));
		}
	}

	if (mNumLaizi >= 2)
	{
		// 找出所有的对子
		for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
		{
			if (mCardSlots[i].NumCards == 2)
			{
				DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
				HBomb.AddCards(mCardSlots[i].Cards, 2);
				HBomb.AddCard(LAIZI(i));
				HBomb.AddCard(LAIZI(i));
			}
		}
	}

	if (mNumLaizi >= 3)
	{
		// 和个子组合
		for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
		{
			if (mCardSlots[i].NumCards == 1)
			{
				DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
				HBomb.AddCards(mCardSlots[i].Cards, 1);
				HBomb.AddCard(LAIZI(i));
				HBomb.AddCard(LAIZI(i));
				HBomb.AddCard(LAIZI(i));
			}
		}
	}


	if (mNumLaizi == 4)
	{
		DHand& HBomb = Increment(Hands, DHand(DHT_4, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
	}
}


void DHintUnslover::GetBomb_L(const DCardNumber Number, VArray<DHand>& Hands, int NumLaizi) const
{
	int NumCards = 4 - NumLaizi;

	if (NumCards == 0)
	{
		DHand& HBomb = Increment(Hands, DHand(DHT_4, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		HBomb.AddCard(DCard(DCC_LAIZI, mLaizi));
		return;
	}

	for (int i = Number + 1; i < DCN_BLACK_JOKER; ++i)
	{
		if (mCardSlots[i].NumCards == NumCards)
		{
			DHand& HBomb = Increment(Hands, DHand(DHT_4, mCardSlots[i].Cards[0].Number));
			HBomb.AddCards(mCardSlots[i].Cards, NumCards);
			for (int j = 0; j < NumLaizi; ++j)
			{
				HBomb.AddCard(DCard(DCC_LAIZI, (DCardNumber)i));
			}
			VASSERT(HBomb.Size() == 4);
		}
	}
}

void DHintUnslover::Hint_1(const DCardNumber Number, VArray<DHand>& Hands) const
{
	VPODArray<DCard> PotentialCards;


	// 先选择单个
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		if (mCardSlots[i].NumCards == 1)
		{
			PotentialCards.PushBack(mCardSlots[i].Cards[0]);
		}
	}

	if (!(mCardSlots[DCN_BLACK_JOKER].NumCards && mCardSlots[DCN_RED_JOKER].NumCards))
	{
		if (mCardSlots[DCN_BLACK_JOKER].NumCards && DCN_BLACK_JOKER > Number)
		{
			PotentialCards.PushBack(mCardSlots[DCN_BLACK_JOKER].Cards[0]);
		}

		if (mCardSlots[DCN_RED_JOKER].NumCards && DCN_RED_JOKER > Number)
		{
			PotentialCards.PushBack(mCardSlots[DCN_RED_JOKER].Cards[0]);
		}
	}

	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		if (mCardSlots[i].NumCards == 2)
		{
			PotentialCards.PushBack(mCardSlots[i].Cards[0]);
		}
	}

	

	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		if (mCardSlots[i].NumCards == 3)
		{
			PotentialCards.PushBack(mCardSlots[i].Cards[0]);
		}
	}

	if ((mCardSlots[DCN_BLACK_JOKER].NumCards && mCardSlots[DCN_RED_JOKER].NumCards))
	{
		if (mCardSlots[DCN_BLACK_JOKER].NumCards && DCN_BLACK_JOKER > Number)
		{
			PotentialCards.PushBack(mCardSlots[DCN_BLACK_JOKER].Cards[0]);
		}

		if (mCardSlots[DCN_RED_JOKER].NumCards && DCN_RED_JOKER > Number)
		{
			PotentialCards.PushBack(mCardSlots[DCN_RED_JOKER].Cards[0]);
		}
	}


	if (!PotentialCards.IsEmpty())
	{
		for (int Index = 0; Index < (int)PotentialCards.Size(); ++Index)
		{
			const DCard& c = PotentialCards[Index];
			DHand& PH = Increment(Hands, DHand(DHT_1, c.Number));
			PH.AddCard(c);
		}
	}


	// add bomb / 2k

	GetBomb(DCN_NULL, Hands);
}




void DHintUnslover::Hint_2(const DCardNumber Number, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 2)
		{
			DHand& PH = Increment(Hands, DHand(DHT_2, s.Cards[0].Number));
			PH.AddCards(s.Cards, 2);
		}
	}

	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 3)
		{
			DHand& PH = Increment(Hands, DHand(DHT_2, s.Cards[0].Number));
			PH.AddCards(s.Cards, 2);
		}
	}

	if (mNumLaizi)
	{
		Hint_2_l(Number, Hands);
	}


	GetBomb(DCN_NULL, Hands);
}

void DHintUnslover::Hint_2_l(const DCardNumber Number, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 1)
		{
			DHand& PH = Increment(Hands, DHand(DHT_2, (DCardNumber)i));
			PH.AddCards(s.Cards, 1);
			PH.AddCard(LAIZI(i));
		}
	}
}

void DHintUnslover::Hint_3(const DCardNumber Number, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 3 && s.Cards[0].Number > Number)
		{
			DHand& PH = Increment(Hands, DHand(DHT_3, (DCardNumber)i));
			PH.AddCards(s.Cards, 3);
		}
	}

	if (mNumLaizi)
	{
		Hint_3_l(Number, Hands);
	}

	GetBomb(DCN_NULL, Hands);
}

void DHintUnslover::Hint_3_l(const DCardNumber Number, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 2)
		{
			DHand& PH = Increment(Hands, DHand(DHT_3, (DCardNumber)i));
			PH.AddCards(s.Cards, 2);
			PH.AddCard(LAIZI(i));
		}
	}

	// 用2个癞子
	if (mNumLaizi >= 2)
	{
		for (int i = Number + 1; i <= DCN_2; ++i)
		{
			const Slot& s = mCardSlots[i];
			if (s.NumCards == 1)
			{
				DHand& PH = Increment(Hands, DHand(DHT_3, (DCardNumber)i));
				PH.AddCards(s.Cards, 1);
				PH.AddCard(LAIZI(i));
				PH.AddCard(LAIZI(i));
			}
		}
	}
}

void DHintUnslover::FindX1(VArray<DHand>& PH3) const
{
	for (int Index = 0; Index < (int)PH3.Size(); ++Index)
	{
		DHand& h3 = PH3[Index];
		DCard X;
		// 跳出一个最小的个子
		for (int i = DCN_3; i <= DCN_RED_JOKER; ++i)
		{
			if (mCardSlots[i].NumCards == 1)
			{
				X = mCardSlots[i].Cards[0];
				break;
			}
		}

		if (X.Number == DCN_NULL)
		{
			for (int i = DCN_3; i <= DCN_2; ++i)
			{
				if (mCardSlots[i].NumCards == 2)
				{
					X = mCardSlots[i].Cards[0];
					break;
				}
			}
		}

		if (X.Number == DCN_NULL)
		{
			for (int i = DCN_3; i <= DCN_2; ++i)
			{
				if (mCardSlots[i].NumCards == 3 && mCardSlots[i].Cards[0].Number != h3.GetHandNumber())
				{
					X = mCardSlots[i].Cards[0];
					break;
				}
			}
		}

		if (X.Number != DCN_NULL)
		{
			h3.AddCard(X);
		}
		else
		{

		}
	}
}

void DHintUnslover::FindX1_L(VArray<DHand>& PH3) const
{
	for (int Index = 0; Index < (int)PH3.Size(); ++Index)
	{
		DHand& h3 = PH3[Index];
		DCardNumber MainPartNumber = h3.GetHandNumber();
		DCard X;
		// 跳出一个最小的个子
		for (int i = DCN_3; i <= DCN_RED_JOKER; ++i)
		{
			if (i == MainPartNumber)
			{
				continue;
			}
			if (mCardSlots[i].NumCards == 1 )
			{
				X = mCardSlots[i].Cards[0];
				break;
			}
		}

		if (X.Number == DCN_NULL)
		{
			for (int i = DCN_3; i <= DCN_2; ++i)
			{
				if (i == MainPartNumber)
				{
					continue;
				}

				if (mCardSlots[i].NumCards == 2)
				{
					X = mCardSlots[i].Cards[0];
					break;
				}
			}
		}

		if (X.Number == DCN_NULL)
		{
			for (int i = DCN_3; i <= DCN_2; ++i)
			{
				if (i == MainPartNumber)
				{
					continue;
				}

				if (mCardSlots[i].NumCards == 3 )
				{
					X = mCardSlots[i].Cards[0];
					break;
				}
			}
		}

		if (X.Number != DCN_NULL)
		{
			h3.AddCard(X);
		}
	}
}

void DHintUnslover::Hint_3_1(const DCardNumber Number, VArray<DHand>& Hands) const
{
	VArray<DHand> PH3;

	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 3)
		{
			DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
			PH.SetCards(s.Cards, 3);
		}
	}

	if (!PH3.IsEmpty())
	{
		FindX1(PH3);
		for (int Index = 0; Index < (int)PH3.Size(); ++Index)
		{
			DHand& h3 = PH3[Index];
			if (h3.Size() == 4)
			{
				DHand& PH = Increment(Hands, h3);
				PH.SetHandType(DHT_3_1);
			}
		}
	}

	if (mNumLaizi)
	{
		Hint_3_1_l(Number, Hands);
	}

	GetBomb(DCN_NULL, Hands);
}

void DHintUnslover::Hint_3_1_l(const DCardNumber Number, VArray<DHand>& Hands) const
{
	// 用一个癞子补齐对子
	VArray<DHand> PH3;
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 2)
		{
			DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
			PH.SetCards(s.Cards, 2);
			PH.AddCard(LAIZI(i));
		}
	}
	if (!PH3.IsEmpty())
	{
		FindX1_L(PH3);
		for (int Index = 0; Index < (int)PH3.Size(); ++Index)
		{
			DHand& h3 = PH3[Index];
			if (h3.Size() == 4)
			{
				DHand& PH = Increment(Hands, h3);
				PH.SetHandType(DHT_3_1);
			}
		}
	}

	// 用两个癞子补齐
	if (mNumLaizi >= 2)
	{
		PH3.Clear();
		for (int i = Number + 1; i <= DCN_2; ++i)
		{
			const Slot& s = mCardSlots[i];
			if (s.NumCards == 1)
			{
				DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
				PH.SetCards(s.Cards, 1);
				PH.AddCard(LAIZI(i));
				PH.AddCard(LAIZI(i));
			}
		}

		if (!PH3.IsEmpty())
		{
			FindX1_L(PH3);
			for (int Index = 0; Index < (int)PH3.Size(); ++Index)
			{
				DHand& h3 = PH3[Index];
				if (h3.Size() == 4)
				{
					DHand& PH = Increment(Hands, h3);
					PH.SetHandType(DHT_3_1);
				}
			}
		}
	}
}

void DHintUnslover::Hint_3_2(const DCardNumber Number, VArray<DHand>& Hands) const
{
	VArray<DHand> PH3;
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 3 && s.Cards[0].Number > Number)
		{
			DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
			PH.SetCards(s.Cards, 3);
		}
	}

	if (!PH3.IsEmpty())
	{
		for (int Index = 0; Index < (int)PH3.Size(); ++Index)
		{
			DHand& h3 = PH3[Index];
			DCardNumber X = DCN_NULL;
		
			for (int i = DCN_3; i <= DCN_2; ++i)
			{
				if (mCardSlots[i].NumCards == 2)
				{
					X = mCardSlots[i].Cards[0].Number;
					break;
				}
			}
			
			if (X == DCN_NULL)
			{
				for (int i = DCN_3; i <= DCN_2; ++i)
				{
					if (mCardSlots[i].NumCards == 3 && mCardSlots[i].Cards[0].Number != h3.GetHandNumber())
					{
						X = mCardSlots[i].Cards[0].Number;
						break;
					}
				}
			}

			if (X != DCN_NULL)
			{
				VASSERT(mCardSlots[X].NumCards >= 2);
				h3.AddCards(mCardSlots[X].Cards, 2);
				DHand& PH = Increment(Hands, h3);
				PH.SetHandType(DHT_3_2);

			}
			else
			{
				//VDelete h3;
			}
		}
	}

	if (mNumLaizi)
	{
		Hint_3_2_l(Number, Hands);
	}

	GetBomb(DCN_NULL, Hands);
}

void DHintUnslover::Hint_3_2_l(const DCardNumber Number, VArray<DHand>& Hands) const
{
	// 用1个癞子补齐主体
	VArray<DHand> PH3;
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 2)
		{
			DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
			PH.SetCards(s.Cards, 2);
			PH.AddCard(LAIZI(i));
		}
	}
	if (!PH3.IsEmpty())
	{
		for (int Index = 0; Index < (int)PH3.Size(); ++Index)
		{
			DHand& h3 = PH3[Index];
			FindX2(h3, mNumLaizi - 1, 1);
			if (h3.Size() == 5)
			{
				DHand& PH = Increment(Hands, h3);
				PH.SetHandType(DHT_3_2);
			}
		}
	}

	// 用两个癞子补主体
	if (mNumLaizi >= 2)
	{
		PH3.Clear();
		for (int i = Number + 1; i <= DCN_2; ++i)
		{
			const Slot& s = mCardSlots[i];
			if (s.NumCards == 1)
			{
				DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
				PH.SetCards(s.Cards, 1);
				PH.AddCard(LAIZI(i));
				PH.AddCard(LAIZI(i));
			}
		}
		if (!PH3.IsEmpty())
		{
			for (int Index = 0; Index < (int)PH3.Size(); ++Index)
			{
				DHand& h3 = PH3[Index];
				FindX2(h3, mNumLaizi - 2, 2);
				if (h3.Size() == 5)
				{
					DHand& PH = Increment(Hands, h3);
					PH.SetHandType(DHT_3_2);
				}
			}
		}
	}

	// 在4个中选择.  用癞子补齐额外的一个个子来作为对子. 
	PH3.Clear();
	for (int i = Number + 1; i <= DCN_2; ++i)
	{
		const Slot& s = mCardSlots[i];
		if (s.NumCards == 4)
		{
			DHand& PH = Increment(PH3, DHand(DHT_3, (DCardNumber)i));
			PH.SetCards(s.Cards, 3);
		}
	}
	if (!PH3.IsEmpty())
	{
		for (int Index = 0; Index < (int)PH3.Size(); ++Index)
		{
			DHand& h3 = PH3[Index];
			FindX2(h3, mNumLaizi, 0);
			if (h3.Size() == 5)
			{
				DHand& PH = Increment(Hands, h3);
				PH.SetHandType(DHT_3_2);
			}
		}
	}
}

void DHintUnslover::FindX2(DHand& h3, int NumLaizi, int MainPartNumLaizi) const
{
	const DCardNumber MainPartNumber = h3.GetHandNumber();
	DCardNumber X = DCN_NULL;
	// 先尝试不用癞子的查找. 
	for (int i = DCN_3; i <= DCN_2; ++i)
	{
		if (MainPartNumber == i)
		{
			continue;
		}
		if (mCardSlots[i].NumCards == 2)
		{
			X = mCardSlots[i].Cards[0].Number;
			break;
		}
	}
	if (X == DCN_NULL)
	{
		for (int i = DCN_3; i <= DCN_2; ++i)
		{
			if (MainPartNumber == i)
			{
				continue;
			}

			if (mCardSlots[i].NumCards == 3)
			{
				X = mCardSlots[i].Cards[0].Number;
				break;
			}
		}
	}

	if (X == DCN_NULL && NumLaizi)
	{
		// 尝试用癞子
		for (int i = DCN_3; i <= DCN_2; ++i)
		{
			if (MainPartNumber == i)
			{
				continue;
			}
			if (mCardSlots[i].NumCards == 1)
			{
				X = mCardSlots[i].Cards[0].Number;
				break;
			}
		}

		if (X == DCN_NULL)
		{
			int NumCardsInMain = mCardSlots[MainPartNumber].NumCards;
			if (NumCardsInMain == 4 && NumLaizi)
			{
				// 比如 9999 L -> 999 x 99
				X = MainPartNumber;
				
			}
		}
	}
	

	if (X == DCN_NULL && NumLaizi >= 2)
	{
		X = mLaizi;
	}


	if (X != DCN_NULL)
	{
		if (X == MainPartNumber)
		{
			VASSERT(mCardSlots[X].NumCards == 4);
			h3.AddCard(mCardSlots[MainPartNumber].Cards[3]);
			h3.AddCard(LAIZI(MainPartNumber));
		}
		else if (mCardSlots[X].NumCards >= 2 )
		{
			h3.AddCards(mCardSlots[X].Cards, 2);
		}
		else if (mCardSlots[X].NumCards == 1)
		{
			h3.AddCards(mCardSlots[X].Cards, 1);
			h3.AddCard(LAIZI(X));
		}
		else
		{
			VASSERT(X == mLaizi);
			h3.AddCard(LAIZI(X));
			h3.AddCard(LAIZI(X));
		}
	}
}


void DHintUnslover::Hint_4_2(const DCardNumber Number, VArray<DHand>& Hands) const
{
	GetBomb(DCN_NULL, Hands);


}

void DHintUnslover::Hint_straight(const DCardNumber Number, int Len, VArray<DHand>& Hands) const
{
	for (int i = Number + 1; i <= (DCN_2 - Len); ++i)
	{
		if (HasStraight(i, Len))
		{
			DHand& PH = Increment(Hands, DHand(DHT_straight, (DCardNumber)i));
			for (int si = 0; si < Len; ++si)
			{
				PH.AddCard(mCardSlots[i + si].Cards[0]);
			}
		}
	}

	// 获得炸弹. 
	GetBombRaw(DCN_NULL, Hands);

	// 
	if (mNumLaizi)
	{
		// 配1个癞子
		Hint_straight_l(Number, Len, Hands, 1);
		GetBomb_L(DCN_NULL, Hands, 1);

		if (mNumLaizi >= 2)
		{
			Hint_straight_l(Number, Len, Hands, 2);
			GetBomb_L(DCN_NULL, Hands, 2);
		}

		if (mNumLaizi >= 3)
		{
			Hint_straight_l(Number, Len, Hands, 3);
			GetBomb_L(DCN_NULL, Hands, 3);
		}

		if (mNumLaizi >= 4)
		{
			Hint_straight_l(Number, Len, Hands, 4);
			GetBomb_L(DCN_NULL, Hands, 4);
		}
	}

	

	Get2K(Hands);
}

bool DHintUnslover::HasStraight(int Start, int Len, int NumLaizi)const
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}

	int NumGaps = 0;
	for (int i = Start; i <= End; ++i)
	{
		if (mCardSlots[i].NumCards == 0)
		{
			NumGaps++;
		}
	}
	return NumGaps == NumLaizi;
}

void DHintUnslover::Hint_straight_l(const DCardNumber Number, int Len, VArray<DHand>& Hands, int NumLaizi) const
{
	for (int i = Number + 1; i <= (DCN_2 - Len); ++i)
	{
		if (HasStraight(i, Len, NumLaizi))
		{
			DHand& PH = Increment(Hands, DHand(DHT_straight, (DCardNumber)i));
			for (int si = 0; si < Len; ++si)
			{
				const Slot& s = mCardSlots[i + si];
				if (s.NumCards)
				{
					PH.AddCard(s.Cards[0]);
				}
				else
				{
					PH.AddCard(LAIZI(i+si));
				}
			}
		}
	}
}

void DHintUnslover::Hint_straight2(const DCardNumber Number, int Len, VArray<DHand>& Hands) const
{
	// 不配癞子
	int NumX = Len >> 1;
	for (int i = Number + 1; i <= (DCN_2 - NumX); ++i)
	{
		if (HasStraight2(i, NumX))
		{
			DHand& PH = Increment(Hands, DHand(DHT_straight_2, (DCardNumber)i));
			for (int si = 0; si < NumX; ++si)
			{
				PH.AddCards(mCardSlots[i + si].Cards, 2);
			}
		}
	}

	GetBombRaw(DCN_NULL, Hands);

	//
	if (mNumLaizi)
	{
		Hint_straight2_l(Number, NumX, Hands, 1);
		GetBomb_L(DCN_NULL, Hands, 1);

		if (mNumLaizi >= 2)
		{
			Hint_straight2_l(Number, NumX, Hands, 2);
			GetBomb_L(DCN_NULL, Hands, 2);
		}

		if (mNumLaizi >= 3)
		{
			Hint_straight2_l(Number, NumX, Hands, 3);
			GetBomb_L(DCN_NULL, Hands, 3);
		}

		if (mNumLaizi >= 4)
		{
			Hint_straight2_l(Number, NumX, Hands, 4);
			GetBomb_L(DCN_NULL, Hands, 4);
		}
	}

	Get2K(Hands);
}

bool DHintUnslover::HasStraight2(int Start, int Len, int NumLaizi) const
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}

	int NumGaps = 0;
	for (int i = Start; i <= End; ++i)
	{
		if (mCardSlots[i].NumCards < 2)
		{
			NumGaps += (2 - mCardSlots[i].NumCards);
		}
	}
	return NumGaps == NumLaizi;
}

void DHintUnslover::Hint_straight2_l(const DCardNumber Number, int Len, VArray<DHand>& Hands, int NumLaizi) const
{
	int Index = 0;
	for (int i = Number + 1; i <= (DCN_2 - Len); ++i)
	{
		if (HasStraight2(i, Len, NumLaizi))
		{
			DHand& PH = Increment(Hands, DHand(DHT_straight_2, (DCardNumber)i));
			for (int si = 0; si < Len; ++si)
			{
				Index = i + si;
				const Slot& s = mCardSlots[Index];
				if (s.NumCards >= 2)
				{
					PH.AddCards(s.Cards, 2);
				}
				else if (s.NumCards == 1)
				{
					PH.AddCard(s.Cards[0]);
					PH.AddCard(LAIZI(Index));
				}
				else
				{
					PH.AddCard(LAIZI(Index));
					PH.AddCard(LAIZI(Index));
				}
			}
		}
	}
}

void DHintUnslover::Hint_straight3(const DCardNumber Number, const DHand* LastHand , VArray<DHand>& Hands) const
{
	
	const int StraightLen = LastHand->GetStraightLen();
	VASSERT(StraightLen >= 2);
	int NumX = LastHand->Size() - StraightLen*3;
	const int XSize = NumX / StraightLen;
	VASSERT(NumX % StraightLen == 0);



	// 不带癞子
	VArray<DHand> H3S;
	bool CheckStraight = false;
	if (mCards.Size() >= LastHand->Size())
	{
		CheckStraight = true;
	}

	if (CheckStraight)
	{
		int Index;
		const int Tail = DCN_2 - StraightLen;
		for (int i = Number + 1; i <= Tail; ++i)
		{
			if (HasStraight3(i, StraightLen))
			{
				DHand& PH = Increment(H3S, DHand(DHT_straight_3, (DCardNumber)i, StraightLen));
				for (int si = 0; si < StraightLen; ++si)
				{
					PH.AddCards(mCardSlots[i + si].Cards, 3);
				}
			}
		}

		if (!H3S.IsEmpty())
		{
			for (int Index = 0; Index < (int)H3S.Size(); ++Index)
			{
				DHand& h3s = H3S[Index];

				if (XSize == 1)
				{
					if (FillPlane_Wing_X1(&h3s, StraightLen))
					{
						Hands.PushBack(h3s);
					}

				}
				else if (XSize == 2)
				{
					if (FillPlane_Wing_X2(&h3s, StraightLen))
					{
						Hands.PushBack(h3s);
					}
				}
				else
				{
					Hands.PushBack(h3s);
				}
			}
		}

		GetBombRaw(DCN_NULL, Hands);

		if (mNumLaizi)
		{

			Hint_straight3_l(Number, StraightLen, XSize, Hands, 1, H3S);
			GetBomb_L(DCN_NULL, Hands, 1);
			if (mNumLaizi >= 2)
			{
				Hint_straight3_l(Number, StraightLen, XSize, Hands, 2, H3S);
				GetBomb_L(DCN_NULL, Hands, 2);
			}

			if (mNumLaizi >= 3)
			{
				Hint_straight3_l(Number, StraightLen, XSize, Hands, 3, H3S);
				GetBomb_L(DCN_NULL, Hands, 3);
			}

			if (mNumLaizi >= 4)
			{
				Hint_straight3_l(Number, StraightLen, XSize, Hands, 4, H3S);
				GetBomb_L(DCN_NULL, Hands, 4);
			}
		}

	}
	else
	{
		GetBombRaw(DCN_NULL, Hands);

		if (mNumLaizi)
		{
			GetBomb_L(DCN_NULL, Hands, 1);
			if (mNumLaizi >= 2)
			{
				GetBomb_L(DCN_NULL, Hands, 2);
			}

			if (mNumLaizi >= 3)
			{
				GetBomb_L(DCN_NULL, Hands, 3);
			}

			if (mNumLaizi >= 4)
			{
				GetBomb_L(DCN_NULL, Hands, 4);
			}
		}

	}
	
	Get2K(Hands);
}

bool DHintUnslover::HasStraight3(int Start, int Len, int NumLaizi)const
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}

	int NumGaps = 0;
	for (int i = Start; i <= End; ++i)
	{
		if (mCardSlots[i].NumCards < 3)
		{
			NumGaps += (3 - mCardSlots[i].NumCards);
		}
	}
	return NumGaps == NumLaizi;
}

void DHintUnslover::Hint_straight3_l(const DCardNumber Number, int Len, int XPartSize, VArray<DHand>& Hands, int NumLaizi, VArray<DHand>& H3) const
{
	H3.Clear();
	int Index;
	const int Tail = DCN_2 - Len;
	for (int i = Number + 1; i <= Tail; ++i)
	{
		if (HasStraight3(i, Len, NumLaizi))
		{
			DHand& PH = Increment(H3, DHand(DHT_straight_3, (DCardNumber)i, Len));
			for (int si = 0; si < Len; ++si)
			{
				Index = i + si;
				const Slot& s = mCardSlots[Index];
				if (s.NumCards >= 3)
				{
					PH.AddCards(s.Cards, 3);
				}else if (s.NumCards == 2)
				{
					PH.AddCards(s.Cards, 2);
					PH.AddCard(LAIZI(Index));
				}
				else if (s.NumCards == 1)
				{
					PH.AddCard(s.Cards[0]);
					PH.AddCard(LAIZI(Index));
					PH.AddCard(LAIZI(Index));
				}
				else
				{
					PH.AddCard(LAIZI(Index));
					PH.AddCard(LAIZI(Index));
					PH.AddCard(LAIZI(Index));
				}

			}
		}
	}

	if (!H3.IsEmpty())
	{
		for (int Index = 0; Index < (int)H3.Size(); ++Index)
		{
			DHand& h3s = H3[Index];
#if 0
			if (XPartSize == 1)
			{
				if (FillPlane_Wing_X1_L(&h3s, StraightLen))
				{
					Hands.PushBack(h3s);
				}

			}
			else if (XPartSize == 2)
			{
				if (FillPlane_Wing_X2_L(&h3s, StraightLen))
				{
					Hands.PushBack(h3s);
				}
			}
			else
			{
				Hands.PushBack(h3s);
			}
#else 
			Hands.PushBack(h3s);
#endif 
		}
	}

}


bool IsInPlane(DHand* PlaneBody, DCardNumber Number)
{
	for (int Index = 0; Index < PlaneBody->Size(); ++Index)
	{
		if (PlaneBody->At(Index).Number == Number)
		{
			return true;
		}
	}
	return false;
}

bool DHintUnslover::FillPlane_Wing_X1(DHand* PlaneBody, int StraightLen) const
{
	DHand Xs;
	int NumX = StraightLen;

	for (int i = DCN_3; i <= DCN_2; ++i)
	{
		if (mCardSlots[i].NumCards == 1)
		{
			Xs.AddCard(mCardSlots[i].Cards[0]);
			if (Xs.Size() >= NumX)
			{
				break;
			}
		}
	}


	if (Xs.Size() < NumX)
	{
		for (int i = DCN_3; i <= DCN_2; ++i)
		{
			if (mCardSlots[i].NumCards == 2)
			{
				Xs.AddCard(mCardSlots[i].Cards[0]);
				if (Xs.Size() >= NumX)
				{
					break;
				}
				Xs.AddCard(mCardSlots[i].Cards[1]);
				if (Xs.Size() >= NumX)
				{
					break;
				}
			}
		}
	}

	if (Xs.Size() < NumX)
	{
		for (int i = DCN_3; i <= DCN_2; ++i)
		{
			if (mCardSlots[i].NumCards == 3 && !IsInPlane(PlaneBody, mCardSlots[i].Cards[0].Number))
			{
				Xs.AddCard(mCardSlots[i].Cards[0]);
				if (Xs.Size() >= NumX)
				{
					break;
				}
				Xs.AddCard(mCardSlots[i].Cards[1]);
				if (Xs.Size() >= NumX)
				{
					break;
				}

				Xs.AddCard(mCardSlots[i].Cards[2]);
				if (Xs.Size() >= NumX)
				{
					break;
				}
			}
		}
	}

	if (Xs.Size() != NumX)
	{
		return false;
	}
	
	PlaneBody->AddCards(Xs.GetCards(), Xs.Size());
	return true;
}

bool DHintUnslover::FillPlane_Wing_X2(DHand* PlaneBody, int StraightLen) const
{
	DHand Xs;
	int NumX = StraightLen * 2;

	for (int i = DCN_3; i <= DCN_2; ++i)
	{
		if (mCardSlots[i].NumCards == 2)
		{
			Xs.AddCard(mCardSlots[i].Cards[0]);
			Xs.AddCard(mCardSlots[i].Cards[1]);
			if (Xs.Size() >= NumX)
			{
				break;
			}
		}
	}


	if (Xs.Size() < NumX)
	{
		for (int i = DCN_3; i <= DCN_2; ++i)
		{
			if (mCardSlots[i].NumCards == 3 && !IsInPlane(PlaneBody, mCardSlots[i].Cards[0].Number))
			{
				Xs.AddCard(mCardSlots[i].Cards[0]);
				Xs.AddCard(mCardSlots[i].Cards[1]);
				if (Xs.Size() >= NumX)
				{
					break;
				}
			}
		}
	}

	if (Xs.Size() != NumX)
	{
		return false;
	}

	PlaneBody->AddCards(Xs.GetCards(), Xs.Size());
	return true;
}

bool DHintUnslover::FillPlane_Wing_X1_L(DHand* PlaneBody, int StraightLen) const
{
	return true;
}

bool DHintUnslover::FillPlane_Wing_X2_L(DHand* PlaneBody, int StraightLen) const
{
	return true;
}

void DHintUnslover::Hint_4(const DCardNumber Number, const DHand* LastHand, VArray<DHand>& Hands) const
{
	if (mLaizi == DCN_NULL)
	{
		GetBombRaw(Number, Hands);
		Get2K(Hands);
	}
	else
	{
		// 癞子场, 需要区别是否为软炸. 
		int NumLaizi = 0;
		for (int i = 0; i < LastHand->Size(); ++i)
		{
			const DCard& C = LastHand->At(i);
			if (C.Color == DCC_LAIZI)
			{
				NumLaizi++;
			}
		}

		if (NumLaizi == 4)
		{
			// 只有对王能打得起. 
			Get2K(Hands);
			return;
		}else if (NumLaizi == 0)
		{
			// 硬炸. 
			GetBombRaw(Number, Hands);
			if (mNumLaizi == 4)
			{
				GetBomb_L(DCN_NULL, Hands, 4);
			}
			Get2K(Hands);
		}
		else
		{
			// 软炸. 
			GetBombRaw(DCN_NULL, Hands);

			// 
			if (mNumLaizi)
			{
				GetBomb_L(Number, Hands, 1);

				if (mNumLaizi >= 2)
				{
					GetBomb_L(Number, Hands, 2);
				}

				if (mNumLaizi >= 3)
				{
					GetBomb_L(Number, Hands, 3);
				}

				if (mNumLaizi >= 4)
				{
					GetBomb_L(DCN_NULL, Hands, 4);
				}
			}

			Get2K(Hands);
		}

	}


	
}
#include "VStdAfx.h"
#include "DAISlover.h"
#include "DGameAI.h"

DAISlover::DAISlover()
	:mNumCards(0)
{
	
}

DAISlover::~DAISlover()
{
	ClearSlover();
	
}

void DAISlover::Init(const DCard* Cards, int NumCards)
{
	memset(mCardSlots, 0, DCN_RED_JOKER + 1);

	for (int i = 0; i < NumCards; ++i)
	{
		const DCard& Card = Cards[i];
		mCardSlots[Card.Number]++;
	}
	mNumCards = NumCards;
}

BOOL DAISlover::CanFollow(const DHand* Hand) const
{
	return FALSE;
}


void DAISlover::ClearSlover()
{
	m4Atoms.clear();
	m3Atoms.clear();
	m2Atoms.clear();
	m1Atoms.clear();
	m1SAtoms.clear();
	m2SAtoms.clear();
	m3SAtoms.clear();
	m2K.Len = 0;
	m2K.Number = DCN_NULL;
}

int DAISlover::SloverCallStrength(const DCard* Cards, int NumCards)
{
	/*mHand.Clear();
	mHand.SetCards(Cards, NumCards);
	mHand.Sort();
	m2KHand = Slover2K(&mHand);
	Slover4(&mHand);*/

	return 0;
}

void DAISlover::SloverTypes()
{
#if 1
	Slover(mCardSlots, *this);

#else 
	ClearSlover();

	BYTE CardSlot[DCN_RED_JOKER + 1];
	memcpy(CardSlot, mCardSlots, DCN_RED_JOKER + 1);
	Slover2K(CardSlot);
	Slover4(CardSlot);
	Slover3(CardSlot);
	Compositing3Straight(CardSlot);
	SloverStraight(CardSlot);
	Composition2Straights(CardSlot);
	SloverDouble(CardSlot);
	SloverSingle(CardSlot);
	RecompsitionStraight();
#if VDEBUG
	for (int i = DCN_3; i <= DCN_RED_JOKER; ++i)
	{
		VASSERT(CardSlot[i] == 0);
	}
#endif 
#endif 
}

void DAISlover::SortHands(DAtomList& Atoms)
{
	if (Atoms.empty())
	{
		return;
	}

	std::sort(Atoms.Atoms, Atoms.Atoms + Atoms.size(), [](const DAtom& a0, const DAtom& a1)->bool
	{
		return a0.Number > a1.Number;
	}
	);
}

void DAISlover::Slover2K(BYTE* CardSlots)
{
	if (CardSlots[DCN_BLACK_JOKER] && CardSlots[DCN_RED_JOKER])
	{
		m2K.Number = DCN_RED_JOKER;
		m2K.Len = 2;
		CardSlots[DCN_BLACK_JOKER] = 0;
		CardSlots[DCN_RED_JOKER] = 0;
	}
	else
	{
		m2K.Number = DCN_NULL;
		m2K.Len = 0;
	}
}

void DAISlover::Slover4(BYTE* CardSlots)
{
	for (int i = DCN_2; i >= DCN_3; --i)
	{
		if (CardSlots[i] == 4)
		{
			DAtom& a4 = m4Atoms.increment();
			a4.Len = 4;
			a4.Number = (DCardNumber)i;
			CardSlots[i] = 0;
		}
	}
}

void DAISlover::Slover3(BYTE* CardSlots)
{
	for (int i = DCN_2; i >= DCN_3; --i)
	{
		if (CardSlots[i] == 3)
		{
			DAtom& a3 = m3Atoms.increment();
			a3.Len = 3;
			a3.Number = (DCardNumber)i;
			CardSlots[i] = 0;
		}
	}
}


/*
 ˳�ӵĽ���㷨Ҫ�ܽ��һ���������
  3 4 5 6 7 8
      5 6 7 8 9 10 ...
  ��ô����Ӧ�����γ�����˳��: 345678 �� 56789 
  �������ǾͲ���һ�����γ����˳��(345678910), ����ֻ��һ���γ���С��5��˳����Ϊ˳�����, 
  Ȼ��ʣ�����, ��˳����׷��, �Ӷ��γɿ��ܵĸ�����˳��. 
*/
void DAISlover::SloverStraight(BYTE* CardSlots)
{

	bool Reslover3 = false;
	// ������3������(�Ѿ���ɷɻ������Ѿ������3�����Ƴ�)���뵽�б���, ׼�����. 
	for (int i = 0; i < (int)m3Atoms.size(); ++i)
	{
		const DAtom& A3 = m3Atoms[i];
		CardSlots[A3.Number] += 3;
		VASSERT(CardSlots[A3.Number] <= 4);
		Reslover3 = true;
	}
	m3Atoms.clear();

	while (true)
	{
		bool Continue = false;
		for (int i = DCN_3; i < DCN_J; ++i)
		{
			if (CardSlots[i] && CardSlots[i + 1] && CardSlots[i + 2] && CardSlots[i + 3] && CardSlots[i + 4])
			{
				// �ҵ�һ��˳��. 
				DAtom& AS1 = m1SAtoms.increment();
				AS1.Len = 5;
				AS1.Number = (DCardNumber)i;
				
				CardSlots[i]--;
				CardSlots[i + 1]--;
				CardSlots[i + 2]--;
				CardSlots[i + 3]--;
				CardSlots[i + 4]--;

				Continue = true;
				break;			// to next find. 
			}

		}

		if (!Continue)
		{
			break;
		}
	}

	if (m1SAtoms.empty())
	{
		if (Reslover3)
		{
			Slover3(CardSlots);
		}
		return;
	}

	// ����С���ƿ�ʼ, ��ͼ������չ��ĳ��˳����. 
	for (int i = DCN_3; i < DCN_2; ++i)
	{
		if (CardSlots[i])
		{
			for (int sidx = 0; sidx < m1SAtoms.size(); sidx++)
			{
				if (Straight_Extend(m1SAtoms[sidx], (DCardNumber)i))
				{
					CardSlots[i]--;
					break;		// �Ƿ����³���???
				}
			}

		}
	}

	SortHands(m1SAtoms);
	
	// �������3��. 
	if (Reslover3)
	{
		Slover3(CardSlots);
	}
}

BOOL DAISlover::Straight_Extend(DAtom& S, DCardNumber Number) const
{
	DCardNumber Tail = (DCardNumber)(S.Number + S.Len);
	if (Number == Tail)
	{
		S.Len++;
		return TRUE;
	}
	return FALSE;
}

void DAISlover::Composition2Straights(BYTE* CardSlots)
{
	/**
	 * ����˫˳�Ŀ���:
	 *    ����  3334567
	               45678 
		����������, ���ǲ���ϲ� ���ǲ��  33  34567 45678 ��Ȼ, ����һ��Ǳ�ڿ��ܾ��� 333-8  44556677 �������, ��ʱ����֧��.....
	 */
	// ����, ����Ƿ������������ȫһ����˳��:  34567  34567 ������ͬ, ����ɫ��ͬ
	for (int s0 = 0; s0 < (int)m1SAtoms.size(); s0++)
	{
		const DAtom& as0 = m1SAtoms[s0];
		for (int s1 = s0 + 1; s1 < (int)m1SAtoms.size(); s1++)
		{
			const DAtom& as1 = m1SAtoms[s1];
			if (as0.Len == as1.Len
				&& as0.Number == as1.Number)
			{
				// ����˳�ӵ�������ȫ�غ�
				DAtom& AS2 = m2SAtoms.increment();
				AS2.Number = as0.Number;
				AS2.Len = as0.Len;

				m1SAtoms.erase(s1);	// s1 > s0 , must erase s1 first!!!
				m1SAtoms.erase(s0);

				s0--;
				break;
			}
		}
	}

	// note: ��ʱ���ƶ�Ӧ����û���� BOMB , �ɻ�, ˳��, 3��. ��, 2 ����. 
	// Ҳ����˵, ֻ�и��ӺͶ���, ��������Щ���ҳ����ܵ�3���Ժ�4����. 
	Composition2Straights_4(CardSlots);
	Composition2Straights_3(CardSlots);

	SortHands(m2SAtoms);
}

void DAISlover::Composition2Straights_3(BYTE* CardSlots)
{
	int Index = 0;
	int Last = DCN_2 - 3;
	for (int Index = DCN_3; Index <= Last; Index++)
	{
		if (HasStraight2(CardSlots, Index, 3))
		{
			DAtom& AS2 = m2SAtoms.increment();
			AS2.Number = (DCardNumber)Index;
			AS2.Len = 3;
			CardSlots[Index] -= 2;
			CardSlots[Index + 1] -= 2;
			CardSlots[Index + 2] -= 2;
			Index--;	// �ӵ�ǰλ�����¿�ʼ???
		}
	}
}

void DAISlover::Composition2Straights_4(BYTE* CardSlots)
{
	int Index = 0;
	int Last = DCN_2 - 4;
	for (int Index = DCN_3; Index <= Last; Index++)
	{
		if (HasStraight2(CardSlots, Index, 4))
		{
			DAtom& AS2 = m2SAtoms.increment();
			AS2.Number = (DCardNumber)Index;
			AS2.Len = 4;

			CardSlots[Index] -= 2;
			CardSlots[Index+1] -= 2;
			CardSlots[Index+2] -= 2;
			CardSlots[Index+3] -= 2;
			Index--;	// �ӵ�ǰλ�����¿�ʼ???
		}
	}

}

bool DAISlover::HasStraight(const BYTE* CardSlots, int Start, int Len)
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}

	for (int i = Start; i <= End; ++i)
	{
		if (!CardSlots[i])
		{
			return false;
		}
	}
	return true;
}

bool DAISlover::HasStraight2(const BYTE* CardSlots, int Start, int Len)
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}
	for (int i = Start; i <= End; ++i)
	{
		if (CardSlots[i] < 2)
		{
			return false;
		}
	}
	return true;
}

bool DAISlover::HasStraight3(const BYTE* CardSlots, int Start, int Len)
{
	int End = Start + Len - 1;
	if (End >= DCN_2 || Start <= DCN_NULL)
	{
		return false;
	}
	for (int i = Start; i <= End; ++i)
	{
		if (CardSlots[i] != 3)
		{
			return false;
		}
	}
	return true;
}

int DAISlover::AI_Calc_3XValue(const DGameAIHand& AIHand)
{
	VASSERT(AIHand.Type == DHT_3_1 || AIHand.Type == DHT_3_2 || AIHand.Type == DHT_3);
	DCardNumber X = AIHand.X[0].X;
	if (AIHand.Type == DHT_3)
	{
		return AIHand.Number;
	}else 
	{
		if (AIHand.Number < DCN_J && X < DCN_J)
		{
			int Value = VMin(AIHand.Number, X);
			Value--;
			return Value;
		}

		if (AIHand.Number > DCN_J && X > DCN_J)
		{
			int Value = VMin(AIHand.Number, X);
			Value--;
			return Value;
		}
		int Value = VMax(AIHand.Number, X);
		Value--;
		return Value;

	}

	return 0;
}

DAtom DAISlover::StraightFollow(const DAtom& Straight, DCardNumber Head, int Len)
{
	if (Straight.Len < Len)
	{
		return DAtom();
	}

	if (Straight.Len == Len)
	{
		if (Straight.Number > Head)
		{
			return Straight;
		}
		return DAtom();
	}
	int Head1 = Head + 1;
	Head1 = VMax(Head1, (int)Straight.Number);
	int Tail1 = Head1 + Len - 1;
	int MyTail = Straight.Number + Straight.Len - 1;
	if (Tail1 <= MyTail)
	{
		return DAtom((DCardNumber)Head1, Len);
	}

	return DAtom();
}

BOOL DAISlover::IsDoubleStraight(const DCard* Cards, int NumCards) const
{
	if (NumCards< 6)
	{
		return FALSE;
	}
	if ((NumCards % 2) != 0 )
	{
		return FALSE;
	}
	int NumPairs = NumCards / 2;
	DCardNumber CardNumber = Cards[0].Number;

	for(int Pair = 0; Pair < NumPairs; ++Pair)
	{
		const DCard& c0 = Cards[Pair * 2];
		const DCard& c1 = Cards[Pair * 2 + 1];
		if (c0.Number != c1.Number)
		{
			return FALSE;
		}

		if (c0.Number != CardNumber)
		{
			return FALSE;
		}
		CardNumber = (DCardNumber)(CardNumber - 1);
		if (CardNumber == DCN_NULL)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void DAISlover::SloverDouble(BYTE* CardSlots)
{
	for (int i = DCN_2; i >= DCN_3; --i)
	{
		VASSERT(CardSlots[i] <= 2);
		if (CardSlots[i] == 2)
		{
			DAtom& A2 = m2Atoms.increment();
			A2.Len = 2;
			A2.Number = (DCardNumber)i;
			CardSlots[i] = 0;
		}
	}
}

void DAISlover::SloverSingle(BYTE* CardSlots)
{
	if (!m2K)
	{
		if (mCardSlots[DCN_RED_JOKER])
		{
			DAtom& A1 = m1Atoms.increment();
			A1.Len = 1;
			A1.Number = DCN_RED_JOKER;
			CardSlots[DCN_RED_JOKER] = 0;
		}

		if (mCardSlots[DCN_BLACK_JOKER])
		{
			DAtom& A1 = m1Atoms.increment();
			A1.Len = 1;
			A1.Number = DCN_BLACK_JOKER;
			CardSlots[DCN_BLACK_JOKER] = 0;
		}

		
	}

	for (int i = DCN_2; i >= DCN_3; --i)
	{
		VASSERT(CardSlots[i] <= 1);
		if (CardSlots[i] == 1)
		{
			DAtom& A1 = m1Atoms.increment();
			A1.Len = 1;
			A1.Number = (DCardNumber)i;
			CardSlots[i] = 0;
		}
	}

	

}

void DAISlover::RecompsitionStraight()
{
	// �������ȰѶ��ӿ��ǽ���, ������û����������. 33345678
}

//////////////////////////////////////////////////////////////////////////
void DAISlover::Compositing3Straight(BYTE* Cards)
{
	// �����3��, ��3������������1. ��ô���ǿ��ܴ���3˳. 
	// ����3˳. 
	if (m3Atoms.size() < 2)
	{
		return;
	}
	SortHands(m3Atoms);
	
	int iStart = 0;
	if (m3Atoms[iStart].Number == DCN_2)
	{
		iStart++;
		if (m3Atoms.size() < 3)
		{
			return;
		}
	}

	DCardNumber NextNumber = (DCardNumber)(m3Atoms[iStart].Number - 1);
	if (NextNumber == DCN_NULL)
	{
		return;
	}


	int iEnd = -1;
	int Index = iStart + 1;
	while (Index < (int)m3Atoms.size())
	{
		const DAtom& aCurr = m3Atoms[Index];
		if (aCurr.Number == NextNumber)
		{
			// �ҵ�һ�����Գ�˳��
			iEnd = Index;

			NextNumber = (DCardNumber)(NextNumber - 1);
			if (NextNumber == DCN_NULL)
			{
				break;
			}
			Index++;
			continue;
		}
		else
		{

			if (iEnd != -1)
			{
				// ����ǰ��˳��. ���뵽�б���. 
				DAtom& as3 = m3SAtoms.increment();
				as3.Number = m3Atoms[iEnd].Number;
				as3.Len = iEnd - iStart + 1;
				// remove from 3hand
				for (int sidx = iEnd; sidx >= iStart; --sidx)
				{
					m3Atoms.erase(sidx);
				}
			}
			iEnd = -1;
			iStart = Index;
			NextNumber = (DCardNumber)(aCurr.Number - 1);
			if (NextNumber == DCN_NULL)
			{
				break;
			}
			Index++;
		}
	}


	if (iEnd != -1)
	{
		// ����ǰ��˳��. ���뵽�б���.
		DAtom& as3 = m3SAtoms.increment();
		as3.Number = m3Atoms[iEnd].Number;
		as3.Len = iEnd - iStart + 1;
		// remove from 3hand
		for (int sidx = iEnd; sidx >= iStart; --sidx)
		{
			m3Atoms.erase(sidx);
		}
	}

}


DCardNumber DAISlover::AnyGreatThen(DCardNumber Num, bool NotBomber) const
{
	if (NotBomber)
	{
		for (int i = DCN_RED_JOKER; i > Num; --i)
		{
			if (mCardSlots[i])
			{
				if (i == DCN_RED_JOKER || i == DCN_BLACK_JOKER)
				{
					if (m2K)
					{
						continue;
					}
				}
				else if (mCardSlots[i] == 4)
				{
					continue;
				}

				return (DCardNumber)i;
			}
		}
	}
	else
	{
		for (int i = DCN_RED_JOKER; i > Num; --i)
		{
			if (mCardSlots[i])
			{
				return (DCardNumber)i;
			}
		}
	}
	
	return DCN_NULL;
}

DCardNumber DAISlover::AnyLessThen(DCardNumber Num, bool NotBomber) const
{

	if (NotBomber)
	{
		for (int i = DCN_3; i < Num; ++i)
		{
			if (mCardSlots[i])
			{
				if (i == DCN_RED_JOKER || i == DCN_BLACK_JOKER)
				{
					if (m2K)
					{
						continue;
					}
				}
				else if (mCardSlots[i] == 4)
				{
					continue;
				}

				return (DCardNumber)i;
			}
		}
	}
	else
	{
		for (int i = DCN_3; i < Num; ++i)
		{
			if (mCardSlots[i])
			{
				return (DCardNumber)i;
			}
		}
	}
	
	return DCN_NULL;
}

DCardNumber DAISlover::AnyLessThen(DCardNumber Num, DCardNumber Except, int NumLess) const
{
	for (int i = DCN_3; i < Num; ++i)
	{
		if (i != Except && mCardSlots[i] >= NumLess)
		{
			return (DCardNumber)i;
		}
	}
	return DCN_NULL;
}


struct DAISloverResult_Potention : public DAISloverResult
{
	int Weight;			// Ȩ��
	int NumHands;

	inline bool IsBetterThen(int OtherHands, int OtherWeight) const
	{
		if (NumHands > OtherHands)
		{
			return false;
		}else if (NumHands < OtherHands)
		{
			return true;
		}
		
		return Weight > OtherWeight;
	}

	void UpdateWeight()
	{
		NumHands = m1Atoms.size() + m2Atoms.size() + m3Atoms.size() + m4Atoms.size() + m1SAtoms.size() + m2SAtoms.size() + m3SAtoms.size();
		int NumX = m3Atoms.size();
		int Num3s = m3SAtoms.size();
		for (int Index = 0; Index < Num3s; ++Index)
		{
			const DAtom& A = m3SAtoms[Index];
			NumX += A.Len;
		}
		int NumValidX = m1Atoms.size() + m2Atoms.size();
		NumX = VMin(NumValidX, NumX);
		NumHands = NumHands - NumX;

		Weight = 0;

		Weight += CalcWeight(m1Atoms, 1);
		Weight += CalcWeight(m2Atoms, 2);
		Weight += CalcWeight(m3Atoms, 3);
		Weight += CalcWeight(m4Atoms, 7);

		Weight += CalcWeight(m1SAtoms, 4);
		Weight += CalcWeight(m2SAtoms, 4, 1.0f);
		Weight += CalcWeight(m3SAtoms, 8, 3.0f);
		//if (m2K)	// �������������, ӦΪ�޷��������. 
		//{
		//}

	}

	inline int CalcWeight(const DAtomList& List, int BaseWeight)
	{
		return List.size() * BaseWeight;
	}
	inline int CalcWeight(const DAtomList& List, int BaseWeight, float LenWeight)
	{
		int Weight = 0;
		for (int i = 0; i < List.size(); ++i)
		{
			float W = BaseWeight + List[i].Len * LenWeight;
			Weight += (int)W;
		}

		return Weight;
	}
};

enum
{
	CARE_BOMB = 0x01,
	CARE_PLANE = 0x02,
	CARE_THREE = 0x04,
	CARE_DOUBLE_STRAIGHT = 0x08,
	CARE_DOUBLE = 0x10,
};
static const int  cMaxPotentions = ((CARE_BOMB | CARE_PLANE | CARE_THREE | CARE_DOUBLE_STRAIGHT | CARE_DOUBLE) + 1);

// TODO : use TLS  to optimize for paral computation
struct DAISloverContext
{
	DAtomList Bombs;
	DAtomList Planes;
	DAtomList Threes;
	DAtomList DoubleStraights;
	DAtomList Doubles;

	DAISloverResult_Potention PotentionResults[cMaxPotentions];
	BOOL InitContext(const BYTE* _Cards)
	{
		memcpy(Cards, _Cards, DCN_RED_JOKER + 1);
		Clear();
		int NumCardsNotK2 = 0;
		for (int i = DCN_3; i < DCN_2; ++i)
		{
			if (Cards[i] == 4)
			{
				DAtom& A = Bombs.increment();
				A.Number = (DCardNumber)i;
				A.Len = 4;
			}else if (Cards[i] == 3)
			{
				DAtom& A = Threes.increment();
				A.Number = (DCardNumber)i;
				A.Len = 3;
			}else if (Cards[i] == 2)
			{
				DAtom& A = Doubles.increment();
				A.Number = (DCardNumber)i;
				A.Len = 2;
			}
			NumCardsNotK2 += Cards[i];
		}


		// ��Ϸɻ�. 
		if (Threes.size() >= 2)
		{
			CompositionPlanes();
		}
		
		if (Doubles.size() >= 3)
		{
			CompositionDoubleStraight();
		}

		return NumCardsNotK2;
	}

	inline void Clear()
	{
		Bombs.clear();
		Planes.clear();
		Threes.clear();
		DoubleStraights.clear();
		Doubles.clear();
		for (int i = 0; i < cMaxPotentions; ++i)
		{
			PotentionResults[i].Weight = 0;
			PotentionResults[i].NumHands = 0;
			PotentionResults[i].Clear();
		}
	}

	void CompositionPlanes()
	{
		// Note: ���ǵ�3�����Ǵ�С�������е�. 3- A
		int iStart = 0;
		if (Threes[iStart].Number == DCN_A)
		{
			// �����Ѿ������ٴ���. early exit.
			return;
		}

		DCardNumber NextNumber = (DCardNumber)(Threes[iStart].Number + 1);
		if (NextNumber >= DCN_2)
		{
			return;
		}

		int iEnd = -1;
		int Index = iStart + 1;
		while (Index < (int)Threes.size())
		{
			const DAtom& aCurr = Threes[Index];
			if (aCurr.Number == NextNumber)
			{
				// �ҵ�һ�����Գ�˳��
				iEnd = Index;
				NextNumber = (DCardNumber)(NextNumber + 1);
				if (NextNumber >= DCN_2)
				{
					break;
				}
				Index++;
				continue;
			}
			else
			{
				if (iEnd != -1)
				{
					// ����ǰ��˳��. ���뵽�б���. 
					DAtom& as3 = Planes.increment();
					as3.Len = iEnd - iStart + 1;
					as3.Number = (DCardNumber)(Threes[iEnd].Number - (iEnd - iStart));
					// remove from 3hand
					for (int sidx = iEnd; sidx >= iStart; --sidx)
					{
						Threes.erase(sidx);
					}
					Index = iStart;
				}
				iEnd = -1;
				iStart = Index;
				NextNumber = (DCardNumber)(Threes[Index].Number + 1);
				if (NextNumber >= DCN_2)
				{
					break;
				}
				Index++;
			}
		}


		if (iEnd != -1)
		{
			// ����ǰ��˳��. ���뵽�б���.
			DAtom& as3 = Planes.increment();
			as3.Len = iEnd - iStart + 1;
			as3.Number = (DCardNumber)(Threes[iEnd].Number - (iEnd - iStart));
			// remove from 3hand
			for (int sidx = iEnd; sidx >= iStart; --sidx)
			{
				Threes.erase(sidx);
			}
		}
	}

	int FindDoubleStraight(int StartIndex)
	{
		int LastIndex = Doubles.size() - 3;
		if (StartIndex > LastIndex)
		{
			return -1;
		}
		const DAtom& HeadAtom = Doubles[StartIndex];
		const DAtom& Atom1 = Doubles[StartIndex + 1];
		const DAtom& Atom2 = Doubles[StartIndex + 2];
		if (HeadAtom.Number + 1 == Atom1.Number &&
			Atom1.Number + 1 == Atom2.Number)
		{
			// valid. 
			int TailIndex = StartIndex + 2 ;
			int NextNumber = Atom2.Number + 1;
			while (TailIndex < (Doubles.size() - 1))
			{
				const DAtom& A = Doubles[TailIndex + 1];
				if (A.Number == NextNumber)
				{
					TailIndex++;
					NextNumber++;
				}
				else
				{
					break;
				}
			}
			return TailIndex;
		}
		return -1;
	}

	void CompositionDoubleStraight()
	{
		int Index = 0;
		while (Index < (Doubles.size() - 2))
		{
			int TailIndex = FindDoubleStraight(Index);
			if (TailIndex != -1)
			{
				// �ҵ�һ��˫˳
				DAtom& as2 = DoubleStraights.increment();
				as2.Number = Doubles[Index].Number;
				as2.Len = TailIndex - Index + 1;
				VASSERT(as2.Len >= 3);
				
				// �Ӷ������Ƴ�˫˳. 
				for (int i = TailIndex; i >= Index; --i)
				{
					Doubles.erase(i);
				}
			}
			else
			{
				Index++;
			}
		}
	}

	int Slover(DAISloverResult& BestResult)
	{
		int CareBomb = Bombs.empty() ? 0 : 1;
		int CarePlane = Planes.empty() ? 0 : 1;
		int CareThree = Threes.empty() ? 0 : 1;
		int CareDoubleStraight = DoubleStraights.empty() ? 0 : 1;
		int CareDoubles = Doubles.empty() ? 0 : 1;

		for (int Mask = 0; Mask < cMaxPotentions; ++Mask)
		{
			if (Mask & CARE_BOMB && !CareBomb)
			{
				continue;
			}

			if (Mask & CARE_PLANE && !CarePlane)
			{
				continue;
			}

			if (Mask& CARE_THREE && !CareThree)
			{
				continue;
			}

			if (Mask & CARE_DOUBLE_STRAIGHT && !CareDoubleStraight)
			{
				continue;
			}

			if (Mask & CARE_DOUBLE && !CareDoubles)
			{
				continue;
			}

			Slover_Kernel(Mask);
		}


		int BestIndex = -1;
		int BestHands = 100;
		int BestWeight = -1;
		for (int Mask = 0; Mask < cMaxPotentions; ++Mask)
		{
			DAISloverResult_Potention& SloverResult = PotentionResults[Mask];
			if (SloverResult.NumHands == 0)
			{
				// ��Ч.
				continue;
			}

			if (SloverResult.IsBetterThen(BestHands, BestWeight))
			{
				BestIndex = Mask;
				BestHands = SloverResult.NumHands;
				BestWeight = SloverResult.Weight;
			}
		}

		VASSERT(BestIndex != -1);
		const DAISloverResult_Potention& SrcBestResult = PotentionResults[BestIndex];
		BestResult.m2K = SrcBestResult.m2K;
		BestResult.m1Atoms = SrcBestResult.m1Atoms;
		BestResult.m2Atoms = SrcBestResult.m2Atoms;
		BestResult.m3Atoms = SrcBestResult.m3Atoms;
		BestResult.m4Atoms = SrcBestResult.m4Atoms;
		BestResult.m1SAtoms = SrcBestResult.m1SAtoms;
		BestResult.m2SAtoms = SrcBestResult.m2SAtoms;
		BestResult.m3SAtoms = SrcBestResult.m3SAtoms;

		return SrcBestResult.NumHands;
	}

	void Slover_Kernel(int Mask)
	{
		// �ָ���ԭʼ����
		memcpy(TempCards, Cards, DCN_RED_JOKER + 1);

		DAISloverResult_Potention& SloverResult = PotentionResults[Mask];

		if (Mask & CARE_BOMB)
		{
			// �������ը��, �����Ƴ�ը��.
			VASSERT(!Bombs.empty());
			for (int i = 0; i < Bombs.size(); ++i)
			{
				const DAtom& A = Bombs[i];
				VASSERT(TempCards[A.Number] == 4);
				TempCards[A.Number] -= 4;
				SloverResult.m4Atoms.push_back(A);
			}
		}

		if (Mask & CARE_PLANE)
		{
			// �����ɻ�
			for (int i = 0; i < Planes.size(); ++i)
			{
				const DAtom& A = Planes[i];
				int Tail = A.Number + A.Len;
				for (int j = A.Number; j < Tail; ++j)
				{
					VASSERT(TempCards[j] >= 3);
					TempCards[j] -= 3;
				}

				SloverResult.m3SAtoms.push_back(A);
			}
		}

		if (Mask& CARE_THREE)
		{
			for (int i = 0; i < Threes.size(); ++i)
			{
				const DAtom& A = Threes[i];
				VASSERT(TempCards[A.Number] >= 3);
				TempCards[A.Number] -= 3;

				SloverResult.m3Atoms.push_back(A);
			}
		}

		if (Mask & CARE_DOUBLE_STRAIGHT)
		{
			for (int i = 0; i < DoubleStraights.size(); ++i)
			{
				const DAtom& A = DoubleStraights[i];
				int Tail = A.Number + A.Len;
				for (int j = A.Number; j < Tail; ++j)
				{
					VASSERT(TempCards[j] >= 2);
					TempCards[j] -= 2;
				}
				SloverResult.m2SAtoms.push_back(A);
			}
		}

		if (Mask & CARE_DOUBLE)
		{
			for (int i = 0; i < Doubles.size(); ++i)
			{
				const DAtom& A = Doubles[i];
				VASSERT(TempCards[A.Number] >= 2);
				TempCards[A.Number] -= 2;
				SloverResult.m2Atoms.push_back(A);
			}
		}

		// ����ʣ�����ƶ��е���, ��������δ��ϵ���. ���ǽ������������. 

		SloverStraight(SloverResult, TempCards);

		// ��ʣ������������. 
		for (int i = DCN_3; i < DCN_2; ++i)
		{
			if (TempCards[i] == 4)
			{
				DAtom& A = SloverResult.m4Atoms.increment();
				A.Number = (DCardNumber)i;
				A.Len = 4;
			}
			else if (TempCards[i] == 3)
			{
				DAtom& A = SloverResult.m3Atoms.increment();
				A.Number = (DCardNumber)i;
				A.Len = 3;
			}
			else if (TempCards[i] == 2)
			{
				DAtom& A = SloverResult.m2Atoms.increment();
				A.Number = (DCardNumber)i;
				A.Len = 2;
			}
			else if (TempCards[i] == 1)
			{
				DAtom& A = SloverResult.m1Atoms.increment();
				A.Number = (DCardNumber)i;
				A.Len = 1;
			}
		}



		SloverResult.UpdateWeight();
	}

	void SloverStraight(DAISloverResult_Potention& Result, BYTE* CardSlots)
	{
		while (true)
		{
			bool Continue = false;
			for (int i = DCN_3; i < DCN_J; ++i)
			{
				if (CardSlots[i] && CardSlots[i + 1] && CardSlots[i + 2] && CardSlots[i + 3] && CardSlots[i + 4])
				{
					// �ҵ�һ��˳��. 
					DAtom& AS1 = Result.m1SAtoms.increment();
					AS1.Len = 5;
					AS1.Number = (DCardNumber)i;

					CardSlots[i]--;
					CardSlots[i + 1]--;
					CardSlots[i + 2]--;
					CardSlots[i + 3]--;
					CardSlots[i + 4]--;

					Continue = true;
					break;			// to next find. 
				}

			}

			if (!Continue)
			{
				break;
			}
		}

		if (Result.m1SAtoms.empty())
		{
			return;
		}

		// ����С���ƿ�ʼ, ��ͼ������չ��ĳ��˳����. 
		for (int i = DCN_3; i < DCN_2; ++i)
		{
			while (CardSlots[i])
			{
				bool Continue = false;
				for (int sidx = 0; sidx < Result.m1SAtoms.size(); sidx++)
				{
					DAtom& A = Result.m1SAtoms[sidx];
					int Tail = A.Number + A.Len;
					if (i == Tail)
					{
						A.Len++;
						CardSlots[i]--;
						Continue = true;
						break;;
					}
				}

				if (Continue)
				{
					continue;
				}
				else
				{
					break;
				}
			}
		}

		// ��34567 8910JQ ������˳�Ӻϲ��� 3-J
		for (int sidx = 0; sidx < (Result.m1SAtoms.size() -1); sidx++)
		{
			DAtom& A1 = Result.m1SAtoms[sidx];
			DAtom& A2 = Result.m1SAtoms[sidx + 1];
			int Tail1 = A1.Number + A1.Len;
			if (Tail1 == A2.Number)
			{
				A1.Len += A2.Len;
				Result.m1SAtoms.erase(sidx + 1);
				sidx--;
			}
		}

		

	}


	BYTE TempCards[DCN_RED_JOKER + 1];
	BYTE Cards[DCN_RED_JOKER + 1];
};

#define SLOVER_STATIC static 
//#define SLOVER_STATIC thread_local		// dont' supported in visual c++ 2013 by now. 





int DAISlover::Slover(const BYTE* Cards, DAISloverResult& BestResult)
{

	// �����ҳ�Ǳ�ڵ� ը��, �ɻ�, 3��, ���� 
	SLOVER_STATIC DAISloverContext SloverContext;
	BestResult.Clear();
	BOOL Slovable = SloverContext.InitContext(Cards);
	
	int NumHands = 0;
	if (Slovable)
	{
		NumHands = SloverContext.Slover(BestResult);
	}

	// ���������� W,2
	if (Cards[DCN_RED_JOKER] && Cards[DCN_BLACK_JOKER])
	{
		BestResult.m2K.Number = DCN_RED_JOKER;
		BestResult.m2K.Len = 2;
	}
	else if (Cards[DCN_RED_JOKER])
	{
		DAtom& A = BestResult.m1Atoms.increment();
		A.Number = DCN_RED_JOKER;
		A.Len = 1;
	}
	else if (Cards[DCN_BLACK_JOKER])
	{
		DAtom& A = BestResult.m1Atoms.increment();
		A.Number = DCN_BLACK_JOKER;
		A.Len = 1;
	}

	// 2 
	int Num2 = Cards[DCN_2];
	if (Num2 == 4)
	{
		DAtom& A = BestResult.m4Atoms.increment();
		A.Number = DCN_2;
		A.Len = 4;
	}
	else if (Num2 == 3)
	{
		DAtom& A = BestResult.m3Atoms.increment();
		A.Number = DCN_2;
		A.Len = 3;
	}
	else if (Num2 == 2)
	{
		DAtom& A = BestResult.m2Atoms.increment();
		A.Number = DCN_2;
		A.Len = 2;
	}
	else if (Num2 == 1)
	{
		DAtom& A = BestResult.m1Atoms.increment();
		A.Number = DCN_2;
		A.Len = 1;
	}


	SortHands(BestResult.m1Atoms);
	SortHands(BestResult.m2Atoms);
	SortHands(BestResult.m3Atoms);
	SortHands(BestResult.m4Atoms);
	SortHands(BestResult.m1SAtoms);
	SortHands(BestResult.m2SAtoms);
	SortHands(BestResult.m3SAtoms);

	return NumHands;
	// clear
	

	/*int Mask = 0;
	for (int b = 0; b <= CareBomb;++b)
	{
		for (int p = 0; p <= CarePlane; ++p)
		{
			for (int t = 0; t <= CareThree; ++t)
			{
				for (int ds = 0; ds <= CareDoubleStraight; ++ds)
				{
					for (int d = 0; d <= CareDoubles; ++d )
					{
					}
				}
			}
		}
	}*/

}
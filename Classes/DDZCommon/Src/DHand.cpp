#include "VStdAfx.h"
#include "DHand.h"

inline bool SortCards(const DCard& c0, const DCard& c1)
{
	if (c0.Number != c1.Number)
	{
		return c0.Number > c1.Number;
	}
	return c0.Color > c1.Color;
}

void DHand::Sort()
{
	std::sort(&mCards[0], &mCards[mNumCards], SortCards);
}

bool DHand::IsSubset(const DHand& Hand) const
{
	DHand TempHand = Hand;
	if (Size() > TempHand.Size())
	{
		return false;
	}
	for (int i = 0; i < Size(); ++i)
	{
		const DCard& Card = At(i);
		if (!TempHand.RemoveCard(Card))
		{
			return false;
		}
	}
	return true;
}

bool DHand::IsUsingLaiZi() const
{
	for (int i = 0; i < Size(); ++i)
	{
		const DCard& Card = At(i);
		if (Card.Color == DCC_LAIZI)
		{
			return true;
		}
	}
	return false;
}

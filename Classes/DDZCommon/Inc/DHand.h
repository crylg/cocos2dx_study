#pragma once
#include "DCard.h"
/**
 * 手牌 
 */
class DHand : public VAllocator
{
public:
	inline DHand() :mNumCards(0), mStraightLen(0),mHandType(DHT_UNKNOWN), mHandNumber(DCN_NULL)
	{

	}

	inline DHand(const DHand& Other) : mNumCards(Other.mNumCards), mStraightLen(Other.mStraightLen), mHandType(Other.mHandType), mHandNumber(Other.mHandNumber)
	{
		if (mNumCards)
		{
			memcpy(mCards, Other.mCards, sizeof(DCard) * mNumCards);
		}
	}
	inline DHand(DHandType HandType) :mNumCards(0), mStraightLen(0), mHandType(HandType), mHandNumber(DCN_NULL){}
	inline DHand(DHandType HandType, DCardNumber HandNumber) : mNumCards(0), mStraightLen(0), mHandType(HandType), mHandNumber(HandNumber){}
	inline DHand(DHandType HandType, DCardNumber HandNumber, int StraightLen) : mNumCards(0), mStraightLen(StraightLen), mHandType(HandType), mHandNumber(HandNumber){}
	inline DHand(DHandType HandType, const DHand* Other) : mNumCards(Other->mNumCards), mStraightLen(0), mHandType(HandType), mHandNumber(Other->mHandNumber)
	{
		if (mNumCards)
		{
			memcpy(mCards, Other->mCards, sizeof(DCard) * mNumCards);
		}
	}

	~DHand()
	{

	}

	void Clear() { mNumCards = 0; }

	void SetCards(const DCard* Cards, int NumCards)
	{
		VASSERT(NumCards <= DHAND_NUM_CARDS_MAX);
		mNumCards = NumCards;
		if (mNumCards)
		{
			memcpy(mCards, Cards, sizeof(DCard) * mNumCards);
		}
	}


	void AddCards(const DCard* Cards, int NumCards)
	{
		if (NumCards)
		{
			memcpy(mCards + mNumCards, Cards, sizeof(DCard) * NumCards);
			mNumCards += NumCards;
		}
	}



	/**
	 * 从牌中移除指定的牌.  
	 */
	int RemoveCards(const DCard* Cards, int NumCards)
	{
		int Removed = 0;
		DCard card;
		for (int i = 0; i < NumCards; ++i)
		{
			card.Code = Cards[i].Code;
			for (int Index = 0; Index < mNumCards; ++Index)
			{
				if (mCards[Index].Code == card.Code || (card.Color == mCards[Index].Color) && card.Color == DCC_LAIZI)
				{
					Remove(Index);
					Removed++;
					break;
				}
			}
		}
		return Removed;
	}

	void RemoveCards(DCardNumber Number)
	{
		for (int Index = mNumCards - 1; Index >= 0; --Index)
		{
			if (mCards[Index].Number == Number)
			{
				Remove(Index);
			}
		}
	}

	bool RemoveCard(const DCard& Card)
	{
		for (int i = 0; i < Size(); i++)
		{
			if (mCards[i] == Card)
			{
				Remove(i);
				return true;
			}
		}
		return false;
	}

	/**
	 * 从手牌中移除王和2. 
	 */
	void RemoveKingAnd2()
	{
		for (int Index = mNumCards - 1; Index >= 0; --Index)
		{
			if (mCards[Index].IsKinOr2())
			{
				Remove(Index);
			}
		}
	}

	void Sort();
	inline const DCard& operator[](int Index) const
	{
		return mCards[Index];
	}
	inline DCard& operator[](int Index)
	{
		return mCards[Index];
	}
	inline const DCard& At(int Index) const { return mCards[Index]; }

	inline const DCard* GetCards() const { return mCards; }
	inline operator DCard*() { return mCards; }
	inline operator const DCard*() const{ return mCards; }
	inline int Size() const { return mNumCards;  }
	inline BOOL IsEmpty() const { return mNumCards == 0; }

	inline const DCard& last() const { return mCards[mNumCards - 1]; }
	inline void pop_back() 
	{
		mNumCards--;
	}
	inline int FindCard(DCardNumber Number) const
	{
		for (int Index = 0; Index < mNumCards; ++Index)
		{
			if (mCards[Index].Number == Number)
			{
				return Index;
			}
		}
		return -1;
	}

	inline int FindCard(const DCard& Card) const
	{
		for (int Index = 0; Index < mNumCards; ++Index)
		{
			if (mCards[Index].Code == Card.Code)
			{
				return Index;
			}
		}
		return -1;
	}

	inline void AddCard(const DCard& Card)
	{
//		VASSERT(FindCard(Card) == -1);
		VASSERT(mNumCards < DHAND_NUM_CARDS_MAX );
		if (mNumCards >= DHAND_NUM_CARDS_MAX)
		{
			return;
		}
		mCards[mNumCards++] = Card;
	}

	inline void InsertAt(int Index, const DCard& Card)
	{
		VASSERT(Index <= mNumCards);
		VASSERT(mNumCards + 1 < DHAND_NUM_CARDS_MAX);
		// move prev data that after Index.
		if (Index < mNumCards)
		{
			VMemmove(mCards + Index + 1, mCards + Index, (mNumCards - Index) * sizeof(DCard));
		}
		mCards[Index] = Card;
		mNumCards++;
	}

	inline void Remove(int Index, int Count = 1)
	{
		VASSERT(Index >= 0 && Index < mNumCards);
		int MoveCount = mNumCards - (Index + Count);
		VASSERT(MoveCount >= 0);
		if (MoveCount)
		{
			memmove(mCards + Index, mCards + Index + Count, MoveCount * sizeof(DCard));
		}
		mNumCards -= Count;
		//mHandType = DHT_UNKNOWN;
	}




	inline void SetHandType(DHandType HandType) { mHandType = HandType; }
	inline DHandType GetHandType() const { return mHandType;  }

	inline void SetHandNumber(DCardNumber HandNumber)
	{
		mHandNumber = HandNumber;
	}

	inline DCardNumber GetHandNumber() const
	{
		return mHandNumber;
	}

	inline int GetStraightLen() const { return mStraightLen; }
	inline void SetStraightLen(int Len) { mStraightLen = (short)Len; }
	inline int GetNumCards() const
	{
		return mNumCards;
	}

	inline int RemoveByNumber(DCardNumber Number, int NumToRemove)
	{
		VASSERT(NumToRemove && NumToRemove <= 4);
		VASSERT(Number);
		int NumRemoved = 0;
		for (int i = mNumCards - 1; i >= 0; --i)
		{
			if (mCards[i].Number == Number)
			{
				Remove(i);
				NumRemoved++;
				if (NumRemoved == NumToRemove)
				{
					break;
				}
			}
		}
		
		return NumRemoved;
	}

	// 是否是另外一个hand的子集
	bool IsSubset(const DHand& Hand) const;
	// 是否有癞子代的牌
	bool IsUsingLaiZi() const;
private:
	DCard	mCards[DHAND_NUM_CARDS_MAX];
	short	mNumCards;
	short	mStraightLen;	// 当为顺子时的长度. 
	DHandType mHandType;
	DCardNumber mHandNumber;
	
};

/**
 * 将DHand序列化到Stream 中, 不包含手牌的牌型和大小信息. 
 */
inline void EncodeHand_CardOnly(const DHand& Hand, VByteStream& Stream)
{
	BYTE NumCards = (BYTE)Hand.Size();
	Stream << NumCards;
	for (BYTE Index = 0; Index < NumCards; ++Index)
	{
		Stream << Hand[Index].Code;
	}
}

/**
* 反序列化DHand, 不包含手牌的牌型和大小信息.
*/
inline void DecodeHand_CardOnly(DHand& Hand, VByteStream& Stream)
{
	BYTE NumCards;
	Stream >> NumCards;
	DCard Card;
	for (BYTE Index = 0; Index < NumCards; ++Index)
	{
		Stream >> Card.Code;
		Hand.AddCard(Card);
	}
}

/**
 * 序列化手牌, 包含每张牌, 和手牌牌型,大小.  
 */
inline void EncodeHand_Full(const DHand& Hand, VByteStream& Stream)
{
	EncodeHand_CardOnly(Hand, Stream);
	if (!Hand.IsEmpty())
	{
		BYTE HandType = Hand.GetHandType();
		BYTE HandValue = Hand.GetHandNumber();
		BYTE StraightLen = (BYTE)Hand.GetStraightLen();
		Stream << HandType << HandValue << StraightLen;
	}
	
}

inline void DecodeHand_Full(DHand& Hand, VByteStream& Stream)
{
	DecodeHand_CardOnly(Hand, Stream);
	if (!Hand.IsEmpty())
	{
		BYTE HandType;
		BYTE HandValue;
		BYTE StraightLen;
		Stream >> HandType >> HandValue >> StraightLen;
		Hand.SetHandType((DHandType)HandType);
		Hand.SetHandNumber((DCardNumber)HandValue);
		Hand.SetStraightLen((int)StraightLen);
	}
	else
	{
		Hand.SetHandType(DHT_UNKNOWN);
		Hand.SetHandNumber(DCN_NULL);
		Hand.SetStraightLen(0);
	}
	
}

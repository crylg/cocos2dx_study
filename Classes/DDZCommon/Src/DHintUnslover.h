#pragma once
#include "DNormalAI.h"

class DHintUnslover
{
public:
	DHintUnslover();
	~DHintUnslover();

	void Reset(const DCard* Cards, int NumCards, DCardNumber Laizi);
	void Clear();

	void GetValidHands(const DHand* LastHand, VArray<DHand>& Hands) const;


private:
	static inline DHand& Increment(VArray<DHand>& Hands)
	{
		int PrevIndex = (int)Hands.Size();
		Hands.Increment(1);
		DHand& Ret = Hands[PrevIndex];
		Ret.Clear();
		return Ret;
	}

	static inline DHand& Increment(VArray<DHand>& Hands, const DHand& Copy)
	{
		int PrevIndex = (int)Hands.Size();
		Hands.Increment(1, Copy);
		DHand& Ret = Hands[PrevIndex];
		return Ret;
	}
	inline DCard Laizi(int i) const { return DCard(DCC_LAIZI, (DCardNumber)i); }

	void GetBomb(const DCardNumber Number, VArray<DHand>& Hands, bool AllowLaizi = true) const;
	void GetBomb_L(const DCardNumber Number, VArray<DHand>& Hands) const;
	void GetBomb_L(const DCardNumber Number, VArray<DHand>& Hands, int NumLaizi) const;
	void Get2K(VArray<DHand>& Hands) const;
	void GetBombRaw(const DCardNumber Number, VArray<DHand>& Hands) const;

	void Hint_1(const DCardNumber Number, VArray<DHand>& Hands) const;
	
	void Hint_2(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_2_l(const DCardNumber Number, VArray<DHand>& Hands) const;

	void Hint_3(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_3_l(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_3_1(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_3_1_l(const DCardNumber Number, VArray<DHand>& Hands) const;
	void FindX1(VArray<DHand>& Hands) const;
	void FindX1_L(VArray<DHand>& Hands) const;

	void Hint_3_2(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_3_2_l(const DCardNumber Number, VArray<DHand>& Hands) const;
	void FindX2(DHand& Main, int NumLaizi, int MainPartNumLaizi) const;
	
	void Hint_4_2(const DCardNumber Number, VArray<DHand>& Hands) const;
	void Hint_straight(const DCardNumber Number, int Len, VArray<DHand>& Hands) const;

	void Hint_straight_l(const DCardNumber Number, int Len, VArray<DHand>& Hands, int NumLaizi) const;

	void Hint_straight2(const DCardNumber Number, int Len, VArray<DHand>& Hands) const;
	void Hint_straight2_l(const DCardNumber Number, int Len, VArray<DHand>& Hands, int NumLaizi) const;


	void Hint_straight3(const DCardNumber Number, const DHand* LastHand, VArray<DHand>& Hands) const;
	void Hint_straight3_l(const DCardNumber Number, int Len, int XPartSize, VArray<DHand>& Hands, int NumLaizi, VArray<DHand>& H3) const;
	void Hint_4(const DCardNumber Number, const DHand* LastHand, VArray<DHand>& Hands) const;

	bool HasStraight(int Start, int Len, int NumLaizi)const;

	bool HasStraight(int Start, int Len)const
	{
		int End = Start + Len - 1;
		if (End >= DCN_2 || Start <= DCN_NULL)
		{
			return false;
		}
		for (int i = Start; i <= End; ++i)
		{
			if (mCardSlots[i].NumCards == 0)
			{
				return false;
			}
		}
		return true;
	}

	bool HasStraight2(int Start, int Len, int NumLaizi)const;
	bool HasStraight2(int Start, int Len)const
	{
		int End = Start + Len - 1;
		if (End >= DCN_2 || Start <= DCN_NULL)
		{
			return false;
		}
		for (int i = Start; i <= End; ++i)
		{
			if (mCardSlots[i].NumCards < 2)
			{
				return false;
			}
		}
		return true;
	}
	bool HasStraight3(int Start, int Len, int NumLaizi)const;
	bool HasStraight3(int Start, int Len)const
	{
		int End = Start + Len - 1;
		if (End >= DCN_2 || Start <= DCN_NULL)
		{
			return false;
		}
		for (int i = Start; i <= End; ++i)
		{
			if (mCardSlots[i].NumCards != 3)
			{
				return false;
			}
		}
		return true;
	}

	bool FillPlane_Wing_X1(DHand* PlaneBody, int StraightLen) const;
	bool FillPlane_Wing_X2(DHand* PlaneBody, int StraightLen) const;

	bool FillPlane_Wing_X1_L(DHand* PlaneBody, int StraightLen) const;
	bool FillPlane_Wing_X2_L(DHand* PlaneBody, int StraightLen) const;
private:
	struct Slot
	{
		inline Slot() :NumCards(0){}
		int NumCards;
		DCard Cards[4];

		inline void AddCard(const DCard& c)
		{
			VASSERT(NumCards < 4);
			Cards[NumCards++] = c;
		}

		inline DCard PopBack()
		{
			VASSERT(NumCards);
			int Last = NumCards - 1;
			NumCards--;
			return Cards[Last];
		}
	};

	Slot mCardSlots[DCN_RED_JOKER + 1];
	DHand mCards;
	int mNumLaizi;
	DCardNumber mLaizi;
};


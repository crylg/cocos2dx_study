#pragma once
#include "DHand.h"
#include "DDeck.h"

#if defined(_DEBUG) || VDEBUG
#define AI_VERIFY(exp) assert(exp)
#else 
#define AI_VERIFY(exp) exp
#endif 


struct DGameAIHand;

typedef VPODArray<DHand*> DHandList;


struct DAISloverResult
{
	DAtomList m4Atoms;
	DAtomList m3Atoms;
	DAtomList m2Atoms;
	DAtomList m1Atoms;
	DAtomList m1SAtoms;
	DAtomList m2SAtoms;
	DAtomList m3SAtoms;
	DAtom m2K;
	
	inline void Clear()
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
};

/**
 * 拥有基础AI功能(包括提示)的玩家
 */
class DAISlover : public DAISloverResult, public DDeckAIHint
{
public:
	


	DAISlover();
	virtual ~DAISlover();
	
	//BOOL CanRetrieve(const DHand* MyHand, const DAISlover* Oppenont) const;

	void Init(const DCard* Cards, int NumCards);
	virtual void OnCardPlayed(const DCard* Cards, int NumCards) {}
	virtual int GetNumCards() const{return mNumCards;}
	inline int GetHandNum() const
	{
		int NumHands = m1Atoms.size();
		NumHands += (int)m2Atoms.size();
		NumHands += (int)m3Atoms.size();
		NumHands += (int)m4Atoms.size();
		NumHands += (int)m1SAtoms.size();
		NumHands += (int)m2SAtoms.size();
		NumHands += (int)m3SAtoms.size();
		NumHands += m2K.Len ? 1 : 0;
		NumHands -= NumXNeedFor_3x_And_Plane();
		return NumHands;
	}

	inline int GetRawHandNum() const
	{
		int NumHands = m1Atoms.size();
		NumHands += (int)m2Atoms.size();
		NumHands += (int)m3Atoms.size();
		NumHands += (int)m4Atoms.size();
		NumHands += (int)m1SAtoms.size();
		NumHands += (int)m2SAtoms.size();
		NumHands += (int)m3SAtoms.size();
		NumHands += m2K.Len ? 1 : 0;
		return NumHands;
	}


	inline int GetBombNum() const
	{
		int NumHands = m4Atoms.size();
		NumHands += m2K.Len ? 1 : 0;
		return NumHands;
	}

	/**
	* 获得手牌中非王和2的单张数量
	*/
	inline int GetSingleNum_Not_KingOr2() const
	{
		int Num = 0;
		for (int Index = 0; Index < m1Atoms.size(); ++Index)
		{
			DCardNumber Number = m1Atoms[Index].Number;
			if (Number < DCN_2)
			{
				Num++;
			}
		}
		return Num;
	}
	inline int GetDoubleNum_Not_2() const
	{
		int Num = 0;
		for (int Index = 0; Index < (int)m2Atoms.size(); ++Index)
		{
			if (m2Atoms[Index].Number != DCN_2)
			{
				Num++;
			}
		}
		return Num;
	}

	inline int GetNum2() const
	{
		return mCardSlots[DCN_2];
	}

	inline int GetNumKing() const
	{
		int Num = 0;
		if (mCardSlots[DCN_RED_JOKER])
		{
			Num++;
		}
		if (mCardSlots[DCN_BLACK_JOKER])
		{
			Num++;
		}
		return Num;
	}

	inline int GetNum3() const
	{
		int x = m3Atoms.size();
		int Num3s = m3SAtoms.size();
		for (int Index = 0; Index < Num3s; ++Index)
		{
			const DAtom& A = m3SAtoms[Index];
			x += A.Len;
		}
		return x;
	}


	// 从小到大选择
	DAtom Select_Ascend(const DAtomList& List, DCardNumber Number) const
	{
		for (int Index = List.size() - 1; Index >= 0; --Index)
		{
			if (List[Index].Number > Number)
			{
				return List[Index];
			}
		}
		return DAtom();
	}

	// 从大到小选择
	DAtom Select_Descend(const DAtomList& List, DCardNumber Number) const
	{
		for (int Index = 0; Index < List.size(); ++Index)
		{
			if (List[Index].Number > Number)
			{
				return List[Index];
			}
		}
		return DAtom();
	}

	DAtom SelectBomb(DCardNumber Number) const
	{
		DAtom Bomb = Select_Ascend(m4Atoms, Number);
		if (!Bomb.IsValid() && Has2King())
		{
			Bomb = m2K;
		}
		return Bomb;
	}

	/**
	 * 是否有大于Num 的牌  如果NotBomber 为true, 则忽略炸弹
	 */
	DCardNumber AnyGreatThen(DCardNumber Num, bool NotBomber = false) const;

	DCardNumber AnyLessThen(DCardNumber Num, bool NotBomber = false) const;

	DCardNumber AnyLessThen(DCardNumber Num, DCardNumber Except, int NumLess = 1) const;

	int SloverCallStrength(const DCard* Cards, int NumCards);

	/**
	* 解决玩家手牌中可能存在的牌型
	*/
	void SloverTypes();


	virtual BOOL CanFollow(const DHand* Hand) const;

	inline BOOL Has2King() const { return m2K.Len; }

	inline BOOL HasBomberOr2K() const { return m4Atoms.NumAtoms || m2K.Len; }

	/**
	* 飞机.3带1 等所需要的配牌(个子或者对子)数量
	*/
	int NumXNeedFor_3x_And_Plane() const
	{
		int x = m3Atoms.size();
		int Num3s = m3SAtoms.size();
		for (int Index = 0; Index < Num3s; ++Index)
		{
			const DAtom& A = m3SAtoms[Index];
			x += A.Len;
		}
		return x;
	}

	
	static bool HasStraight(const BYTE* CardSlots, int Start, int Len);
	static bool HasStraight2(const BYTE* CardSlots, int Start, int Len);
	static bool HasStraight3(const BYTE* CardSlots, int Start, int Len);

	/**
	 * 评估3带的牌值 
	 */
	static int AI_Calc_3XValue(const DGameAIHand& AIHand);

	static DAtom StraightFollow(const DAtom& Straight, DCardNumber Head, int Len);


	BOOL IsDoubleStraight(const DCard* Cards, int NumCards) const;

	// New Slover 
	static int Slover(const BYTE* Cards, DAISloverResult& BestResult);

protected:
	void ClearSlover();
	//////////////////////////////////////////////////////////////////////////
	// 解析出所有潜在的牌型
	//////////////////////////////////////////////////////////////////////////
	void Slover2K(BYTE* CardSlots);
	void Slover4(BYTE* CardSlots);
	void Slover3(BYTE* CardSlots);
	void SloverStraight(BYTE* CardSlots);
	BOOL Straight_Extend(DAtom& S, DCardNumber Number) const;
	

	// helper 从手牌中解析出一个顺子
	void Composition2Straights(BYTE* CardSlots);
	void Composition2Straights_3(BYTE* CardSlots);
	void Composition2Straights_4(BYTE* CardSlots);

	
	
	void SloverDouble(BYTE* CardSlots);
	void SloverSingle(BYTE* CardSlots);
	void RecompsitionStraight();
	void Compositing3Straight(BYTE* Cards);
	static void SortHands(DAtomList& Atoms);
private:
	
protected:
//	BYTE mCardSlots[DCN_RED_JOKER + 1];
	int mNumCards;
	private:
		
		virtual void DeleteThis() { delete this; }
		virtual const DAtomList& Get1Atoms() const { return m1Atoms; }
		virtual const DAtomList& Get2Atoms() const { return m2Atoms; }
		virtual const DAtomList& Get3Atoms() const { return m3Atoms; }
		virtual const DAtomList& Get4Atoms() const { return m4Atoms; }
		virtual const DAtomList& Get1SAtoms() const { return m1SAtoms; }
		virtual const DAtomList& Get2SAtoms() const { return m2SAtoms; }
		virtual const DAtomList& Get3SAtoms() const{ return m3SAtoms; }
		virtual bool Has2K() const { return m2K; }
};


#pragma once
#include "DCard.h"
#include "DHand.h"
#include "DGameDefine.h"
#include <ctime>


class DCardTypeSlover;
class GPlayer;
class DDeck;
class DAIHint;


// 2 Bytes.
struct DAtom
{
	union
	{
		struct
		{
			DCardNumber Number;
			BYTE Len;		// for straight.
		};
		WORD Value;
	};
	inline DAtom() :Value(0){}
	inline DAtom(const DAtom& other) : Value(other.Value){}
	inline DAtom(DCardNumber _Number, int _Len) : Number(_Number), Len(_Len) {}
	inline ~DAtom(){}
	inline DAtom& operator=(const DAtom& rhs)
	{
		Value = rhs.Value;
		return *this;
	}
	inline operator bool() const { return Value != 0; }
	inline bool IsValid() const { return Value != 0; }

	inline DCardNumber GetStraightLast() const
	{
		return (DCardNumber)((int)Number + Len - 1);
	}

};

// 41 Bytes.
struct DAtomList
{
	DAtom Atoms[20];		// 40 Bytes. no heap allocation. 
	BYTE NumAtoms;

	inline void clear() { NumAtoms = 0; }
	inline int size() const { return (int)NumAtoms; }
	inline bool empty() const { return NumAtoms == 0; }
	inline void push_back(const DAtom& a)
	{
		VASSERT(NumAtoms < 20);
		Atoms[NumAtoms++].Value = a.Value;
	}
	inline void erase(int Index)
	{
		VASSERT(Index + 1 <= NumAtoms);
		int MoveCount = NumAtoms - (Index + 1);
		if (MoveCount)
		{
			VMemmove(Atoms + Index, Atoms + Index + 1, MoveCount * sizeof(DAtom));
		}
		--NumAtoms;
	}

	inline DAtom& increment()
	{
		return Atoms[NumAtoms++];
	}
	inline DAtom& operator[](int Index)
	{
		VASSERT(Index >= 0 && Index < NumAtoms);
		return Atoms[Index];
	}
	inline const DAtom& operator[](int Index) const
	{
		VASSERT(Index >= 0 && Index < NumAtoms);
		return Atoms[Index];
	}
};

struct DDeckAIHint
{
protected:
	BYTE mCardSlots[DCN_RED_JOKER + 1];
public:
	virtual ~DDeckAIHint(){}
	virtual void DeleteThis() = 0;

	inline void Reset()
	{
		for (int i = DCN_3; i <= DCN_RED_JOKER; ++i)
		{
			mCardSlots[i] = 0;
		}
	}
	virtual void UpdateCards(const DCard* Cards, int NumCards);
	virtual void OnCardPlayed(const DCard* Cards, int NumCards);

	int NumBombersGreaterThen(DCardNumber Bomber) const;

	/**
	 * �ܷ��������? 
	 */
	bool CanFollow2(DCardNumber Number) const;


	bool IsRemainDouble() const;

	int AI_QueryRemainBombers(DCardNumber GreatThen) const;
	int AI_Determ_Single_Max(DCardNumber SingleNumber, bool IncludeBomber) const;
	int AI_Determ_Double_Greater(DCardNumber DoubleNumber) const;
	int AI_Determ_3_Greater(DCardNumber ThreeNumber) const;
	int AI_Determ_S_Greater(DCardNumber Head, int Len) const;
	int AI_Determ_2S_Greater(DCardNumber Head, int Len) const;
	int AI_Determ_3S_Greater(DCardNumber Head, int Len) const;


	int AI_QueryNumHands() const;

	int AI_QueryNumX() const;
protected:
	virtual const DAtomList& Get1Atoms() const = 0;
	virtual const DAtomList& Get2Atoms() const = 0;
	virtual const DAtomList& Get3Atoms() const = 0;
	virtual const DAtomList& Get4Atoms() const = 0;
	virtual const DAtomList& Get1SAtoms() const = 0;
	virtual const DAtomList& Get2SAtoms() const = 0;
	virtual const DAtomList& Get3SAtoms() const = 0;
	virtual bool Has2K() const = 0;
};

/**
 * ��������ҽ�ɫ. 
 * Note: �����ɫ������ֻ�ṩ������Ľ�����, ��Ӧ�û��ڿͻ���,���������и��Ե�ʵ��. 
 */
class DDeckCharacter
{
public:
	inline DDeckCharacter() :mCharType(DDCT_PRE_LORD), mDeck(NULL){}
	virtual ~DDeckCharacter()
	{
		
	}

	inline void SetDeck(DDeck* Deck) { mDeck = Deck; }
	inline DDeck* GetDeck() { return mDeck; }

	/**
	 * �ƾֿ�ʼ 
	 */
	virtual void OnRoundBegin() = 0;


	/**
	 *  ����
	 */
	virtual void OnDeal(const DCard* Cards, int NumCards) = 0;
	/**
	 * ������ 
	 */
	virtual void OnDealPockets(const DCard* Cards, int NumCards) = 0;

	/**
	* ��ʼ�е���
	*/
	virtual void BeginCallLord(int LimitSeconds,int CalledMaxScre=0) {}

	/**
	 * ���ѡ���˽е����ķ���.  
	 */
	virtual void OnCallLord(int Score) {}

	/**
	* �����е���
	*/
	virtual void EndCallLord() {}

	/**
	 * ��ʼ���ƽ׶� 
	 */
	virtual void OnStartPlayStage(){}
	virtual void OnEndPlayStage(){}
	/**
	 * ���ƽ׶�, Hand: ��ǰ�����ϵ�����, HandOwner : ��ǰ���ƵĴ����. 
	 * ���Ϊ��, ���ʶΪ����. 
	 */
	virtual void OnBeginPlay(const DHand* Hand, DDeckCharacter* HandOwner, int WaitSeconds) = 0;
	virtual void Play(const DHand* Hand){}
	virtual void OnWin(bool Win) = 0;

	virtual GPlayer* GetOwner() = 0;
	
	virtual int GetNumCards() const = 0;
	virtual const DCard& GetCard(int Index) const = 0;
	virtual const DCard* GetCards() const = 0;

	virtual DDeckAIHint& AI_GetHint() const = 0;

	virtual void SetCharType(DDeckCharacterType CharType) { mCharType = CharType; }
	inline DDeckCharacterType GetCharType() const { return mCharType;  }
	virtual void DisplayHandCard();//����

	

protected:
	DDeckCharacterType mCharType;		// ��ɫ���� ����, �����ϼ�, �����¼�.
	DDeck* mDeck;
	
};


int AIRand(int Max);
void  AISRand(size_t Seed);

/*
 ����һ������ 
*/
class DDeck
{
public:
	DDeck();
	virtual ~DDeck();
	/**
	*	��������
	*/
	void Reset();

	/**
	 * ϴ��
	 */
	void Shuffle();

	/**
	 * ����ϴ��. ����ƽ.  
	 */
	void ShuffleEx();
	/**
	 *  ����
	 */
	void Deal(DHand* Hand);

	/**
	 * ���Player ���¼�. 
	 */
	virtual DDeckCharacter* GetNextPlayer(DDeckCharacter* Player) const { return NULL; }

	virtual DDeckCharacter* GetPlayer(DDeckCharacterType CharType) const { return NULL; }
	
	/**
	 * Owner ����������, �Ƿ�Ϊ������� ???  �޷�������. 
	 */
	virtual BOOL IsMaxHand(const DDeckCharacter* Owner, const DHand* Hand) const;

	/**
	 * �������Ƶ�����, 
	 */
	virtual int CalcCallStrength(const DHand& Hand) const;

	/**
	 * �����Ѿ��������, ��ʼ���� 
	 */
	virtual void BeginPlay() = 0;

	virtual void Play(DDeckCharacter* Owner, DHand* Hand) = 0;
	virtual const DHand* GetLastHand() const = 0;
	virtual DDeckCharacter* GetLastHandOwner() const = 0;
	/**
	 * �����Ƶ�����. ���������͵�ֵ. Note: ����֮ǰ, ��Ҫȷ��Cards �Ѿ�����. 
	 */
	static DHandType EvalHandType(const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen);
	static DHandType EvalHandTypeFollow(const DHand& FollowHand, const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen, DCardNumber LaiziNumber);

	static bool IsValidHand(const DHand& Hand);
	/**
	 * ����ӳ���, ������ѡ�����ж���ƥ��ĸ�������. ���������Ч���ܴ����FollowTarget, ����TRUE, ���򷵻�FALSE. 
	 * �������TRUE, ��ôResult �л�������п��Դ�������͵����. 
	 */
	static BOOL EvalFollowHandForLaizi(const DHand& FollowTarget, const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Result);
	

	BOOL CanFollowLastHand(const DHand& CurrHand, const DHand& LastHand, DCard LaiZi = DCard()) const;

	BOOL CanFollowLastHand(const DHand& CurrHand, DCard LaiZi = DCard()) const;

	/**�������͡����������͵�ֵ**/
	static BOOL LaiZiEvalHandType(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL EvalHandWithLaizi(const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Results);
	/**
	 * �Ƿ���Դ����ǰ������?  
	 */
	//virtual BOOL CanPlay(DDeckCharacter* Owner, const DCard* Cards, int NumCards);

	virtual bool IsMultiplayer() const { return true; }

	static int NumHands(const DHand& Hand);

#pragma region "AI Hint"
public:
	bool AI_CreateAIHint();
	void AI_DestoryAIHint();
	DDeckAIHint* AI_GetHint() const { return mDeckAIHint; }
	/**
	 * AI ��ѯ���Ͽ��ܴ��ڵ�ը������ 
	 */
	int AI_QueryRemainBombers(DCardNumber GreatThen) const;

	/**
	 * AI ��ѯ������������ 
	 */
	DCardNumber AI_QueryMaxCard() const;

	/**
	 * ��ѯ������������������. 
	 * @param CardSlots �Լ�����.
	 */
	DCardNumber AI_QueryMaxCard(const BYTE* CardSlots) const;

	/**
	 * AI �Ƿ���BomberNumber��ը��ը����ը���� ???  
	 */
	bool AI_Bomb(const BYTE* CardSlots, DCardNumber BomberNumber) const;
	bool AI_Determ_Bomb_Is_Max(const BYTE* CardSlots, DCardNumber BomberNumber) const;

	int AI_Determ_Single_Max(const BYTE* CardSlots, DCardNumber SingleNumber, bool IncludeBomber = true) const;

	bool AI_Determ_Single_CanBeFollowByLord(const BYTE* LordHand, DCardNumber SingleNumber) const;

	bool AI_Determ_Double_Is_Max(const BYTE* CardSlots, DCardNumber DoubleNumber) const;
	int AI_Determ_Double_Greater(const BYTE* CardSlots, DCardNumber DoubleNumber) const;
	int AI_Determ_Double_Lesser(const BYTE* CardSlots, DCardNumber DoubleNumber) const;

	/**
	 * AI �����Լ������ƺ��Ѿ��������, �ж�����2��������, ���ܴ����3��������.  
	 */
	int AI_Determ_3_Greater(const BYTE* CardSlots, DCardNumber ThreeNumber) const;
	bool AI_Determ_3_Is_Max(const BYTE* CardSlots, DCardNumber DoubleNumber) const;

	int AI_Determ_S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	int AI_Determ_2S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	int AI_Determ_3S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	bool AI_Determ_S_IsMax(const BYTE* CardSlots, DCardNumber Head, int Len) const;

	int AI_QueryRemainBombers(const BYTE* CardSlots, DCardNumber GreatThen) const;

	/**
	 * ���ҳ��ҵ�����, ������Ǳ�ڵ����ը��(�����GreatThen��) 
	 */
	DCardNumber AI_QueryMaxBomber(const BYTE* CardSlots, DCardNumber GreatThen) const;

	DCardNumber AI_QueryCardGreatThen(const BYTE* CardSlots, DCardNumber Compare) const;
	/**
	 *  
	 */
	inline int AI_QueryRemainCards(const DCardNumber Number) const { return mCardSlots[Number]; }
protected:
	void UpdateAIHint(int Index, const DCard* PlayedCards, int NumCards);
	void ResetAIHint();
	char mCardSlots[DCN_RED_JOKER + 1];	// for AI Hint. 
#pragma endregion "AI Hint"


public:
	/**
	 * AI �����Լ������� 
	 */
	inline void SetLaiZiNumber(DCardNumber laizi)
	{
		mLaiZi = laizi;
	}

	inline DCardNumber GetLaiZiNumber() const
	{
		return mLaiZi;
	}

	virtual void SetDeckMode(EDGameMode mode)  { Deckmode = mode;};
	virtual EDGameMode GetDeckMode()  { return Deckmode;};

	////��˳��

	static void FillDouble(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static void FillIs3(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL Compose3_1(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL Compose3_2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);

	static BOOL Compose4_2(const DHand& RawCards, const DHand& SelectedLaize, DHand& Results);

	static void ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);

	static void  ComposeStraight(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	// NumX �ɻ�������3��������
	static void  ComposeStraight2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	// XPartSize : 0 : �������, 1: ������ 2: ������
	static VArray<DHand> FillStraight(const DHand& card, const DHand& laizi, int num, int laizinum);
	static void ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results, int NumX, int XPartSize);





	static VArray<DHand> FillStraight2(const DHand& card, const DHand& laizi, VArray<DHand>& Results);

	static void FillBom(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	//���ɻ��������
	static VArray<DHand> FillIsPlane_NoWing(const DHand& card, const DHand& laizi, int num, int laizinum);
	//�������һ
	static VArray<DHand> FillIsPlane_Wing(const DHand& card, const DHand& laizi, int num, int laizinum);

	bool InspectCardExist(DCard* card, int number);


protected:
	DCard mCards[DDECK_NUM_CARDS];			// ��������Ч����. 
	DCard mPlayedCards[DDECK_NUM_CARDS];	// �Ѿ��������. open cards.
	int mNumPlayedCards;					
	DCard mPockets[DDECK_NUM_POCKETS];		// ����
	int mNumCards;
	EDGameMode Deckmode;
	DDeckAIHint* mDeckAIHint;
	DCardNumber mLaiZi;
	//DHand mLastHand;
};


class TypeAtom
{
public:
	// Note: ����֮ǰ, ȷ�� Cards �Ѿ����Ƶ����ֽ��еݼ�����

	static inline DCardNumber Is2K(const DCard* Cards, int NumCards)
	{
		if (NumCards == 2 &&
			((Cards[0] == DCard::BIG_KING && Cards[1] == DCard::SMALL_KING)
			|| (Cards[1] == DCard::BIG_KING && Cards[0] == DCard::SMALL_KING)
			)
			)
		{
			return DCN_RED_JOKER;
		}
		return DCN_NULL;
	}

	static inline DCardNumber IsBomb(const DCard* Cards, int NumCards)
	{
		if (NumCards == 4 &&
			Cards[0].Number == Cards[1].Number &&
			Cards[0].Number == Cards[2].Number &&
			Cards[0].Number == Cards[3].Number)
		{
			return Cards[0].Number;
		}
		return DCN_NULL;
	}

	static inline DCardNumber IsDouble(const DCard* Cards, int NumCards)
	{
		if (NumCards == 2 && Cards[0].Number == Cards[1].Number)
		{
			return Cards[0].Number;
		}
		return DCN_NULL;
	}

	static inline DCardNumber Is3(const DCard* Cards, int NumCards)
	{
		if (NumCards == 3 && Cards[0].Number == Cards[1].Number && Cards[0].Number == Cards[2].Number)
		{
			return Cards[0].Number;
		}
		return DCN_NULL;
	}

	static inline DCardNumber Is3_1(const DCard* Cards, int NumCards)
	{
		if (NumCards != 4)
		{
			return DCN_NULL;
		}

		if (Cards[0].Number == Cards[1].Number && Cards[0].Number == Cards[2].Number)
		{
			// 333 1
			return Cards[0].Number;
		}

		if (Cards[1].Number == Cards[2].Number && Cards[1].Number == Cards[3].Number)
		{
			// 1 333
			return Cards[1].Number;
		}

		return DCN_NULL;
	}

	static DCardNumber Is3_2(const DCard* Cards, int NumCards);
	static DCardNumber Is4_2(const DCard* Cards, int NumCards);
	static inline DCardNumber IsStraight(const DCard* Cards, int NumCards)
	{
		if (NumCards < 5)
		{
			return DCN_NULL;
		}

		DCardNumber Number = Cards[0].Number;
		if (Number > DCN_A)
		{
			return DCN_NULL;
		}

		for (int i = 1; i < NumCards; ++i)
		{
			if (Cards[i].Number != (DCardNumber)(Cards[i - 1].Number - 1))
			{
				return DCN_NULL;
			}
			VASSERT(Cards[i].Number != DCN_NULL);
		}
		return Cards[NumCards - 1].Number;
	}

	static DCardNumber IsStraight2(const DCard* Cards, int NumCards);
	/**
	* �Ƿ�3˳��, �������
	*/
	static DCardNumber IsPlane_NoWing(const DCard* Cards, int NumCards, int& Len);
	static DCardNumber IsPlane_Wing(const DCard* Cards, int NumCards, int& Len);

	static DCardNumber IsPlane_Follow(const DCard* Cards, int NumCards, int Len);
};

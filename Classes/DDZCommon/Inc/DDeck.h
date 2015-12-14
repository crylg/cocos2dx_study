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
	 * 能否打得起对子? 
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
 * 斗地主玩家角色. 
 * Note: 这个角色将仅仅只提供最基本的借口设计, 将应该会在客户端,服务器端有各自的实现. 
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
	 * 牌局开始 
	 */
	virtual void OnRoundBegin() = 0;


	/**
	 *  发牌
	 */
	virtual void OnDeal(const DCard* Cards, int NumCards) = 0;
	/**
	 * 发底牌 
	 */
	virtual void OnDealPockets(const DCard* Cards, int NumCards) = 0;

	/**
	* 开始叫地主
	*/
	virtual void BeginCallLord(int LimitSeconds,int CalledMaxScre=0) {}

	/**
	 * 玩家选择了叫地主的分数.  
	 */
	virtual void OnCallLord(int Score) {}

	/**
	* 结束叫地主
	*/
	virtual void EndCallLord() {}

	/**
	 * 开始出牌阶段 
	 */
	virtual void OnStartPlayStage(){}
	virtual void OnEndPlayStage(){}
	/**
	 * 出牌阶段, Hand: 当前桌面上的手牌, HandOwner : 当前手牌的打出者. 
	 * 如果为空, 则标识为出牌. 
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
	virtual void DisplayHandCard();//明牌

	

protected:
	DDeckCharacterType mCharType;		// 角色类型 地主, 地主上家, 地主下家.
	DDeck* mDeck;
	
};


int AIRand(int Max);
void  AISRand(size_t Seed);

/*
 描述一个牌桌 
*/
class DDeck
{
public:
	DDeck();
	virtual ~DDeck();
	/**
	*	重置牌桌
	*/
	void Reset();

	/**
	 * 洗牌
	 */
	void Shuffle();

	/**
	 * 特殊洗牌. 不公平.  
	 */
	void ShuffleEx();
	/**
	 *  发牌
	 */
	void Deal(DHand* Hand);

	/**
	 * 获得Player 的下家. 
	 */
	virtual DDeckCharacter* GetNextPlayer(DDeckCharacter* Player) const { return NULL; }

	virtual DDeckCharacter* GetPlayer(DDeckCharacterType CharType) const { return NULL; }
	
	/**
	 * Owner 如果打出手牌, 是否为最大手牌 ???  无法被接上. 
	 */
	virtual BOOL IsMaxHand(const DDeckCharacter* Owner, const DHand* Hand) const;

	/**
	 * 计算手牌的力量, 
	 */
	virtual int CalcCallStrength(const DHand& Hand) const;

	/**
	 * 底牌已经发送完成, 开始出牌 
	 */
	virtual void BeginPlay() = 0;

	virtual void Play(DDeckCharacter* Owner, DHand* Hand) = 0;
	virtual const DHand* GetLastHand() const = 0;
	virtual DDeckCharacter* GetLastHandOwner() const = 0;
	/**
	 * 计算牌的牌型. 并返回牌型的值. Note: 调用之前, 需要确保Cards 已经排序. 
	 */
	static DHandType EvalHandType(const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen);
	static DHandType EvalHandTypeFollow(const DHand& FollowHand, const DCard* Cards, int NumCards, DCardNumber& HandValue, int& StraightLen, DCardNumber LaiziNumber);

	static bool IsValidHand(const DHand& Hand);
	/**
	 * 在癞子场中, 根据所选牌型判断最匹配的跟牌牌型. 如果跟牌有效且能打得起FollowTarget, 返回TRUE, 否则返回FALSE. 
	 * 如果返回TRUE, 那么Result 中会填充所有可以打得起牌型的组合. 
	 */
	static BOOL EvalFollowHandForLaizi(const DHand& FollowTarget, const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Result);
	

	BOOL CanFollowLastHand(const DHand& CurrHand, const DHand& LastHand, DCard LaiZi = DCard()) const;

	BOOL CanFollowLastHand(const DHand& CurrHand, DCard LaiZi = DCard()) const;

	/**计算牌型。并返回牌型的值**/
	static BOOL LaiZiEvalHandType(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL EvalHandWithLaizi(const DCard* Cards, int NumCards, DCardNumber LaiziNumber, VArray<DHand>& Results);
	/**
	 * 是否可以打出当前的手牌?  
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
	 * AI 查询桌上可能存在的炸弹数量 
	 */
	int AI_QueryRemainBombers(DCardNumber GreatThen) const;

	/**
	 * AI 查询桌面上最大的牌 
	 */
	DCardNumber AI_QueryMaxCard() const;

	/**
	 * 查询其它两家牌中最大的牌. 
	 * @param CardSlots 自己的牌.
	 */
	DCardNumber AI_QueryMaxCard(const BYTE* CardSlots) const;

	/**
	 * AI 是否用BomberNumber的炸弹炸掉非炸弹牌 ???  
	 */
	bool AI_Bomb(const BYTE* CardSlots, DCardNumber BomberNumber) const;
	bool AI_Determ_Bomb_Is_Max(const BYTE* CardSlots, DCardNumber BomberNumber) const;

	int AI_Determ_Single_Max(const BYTE* CardSlots, DCardNumber SingleNumber, bool IncludeBomber = true) const;

	bool AI_Determ_Single_CanBeFollowByLord(const BYTE* LordHand, DCardNumber SingleNumber) const;

	bool AI_Determ_Double_Is_Max(const BYTE* CardSlots, DCardNumber DoubleNumber) const;
	int AI_Determ_Double_Greater(const BYTE* CardSlots, DCardNumber DoubleNumber) const;
	int AI_Determ_Double_Lesser(const BYTE* CardSlots, DCardNumber DoubleNumber) const;

	/**
	 * AI 根据自己的手牌和已经打出的牌, 判断其它2家手牌中, 可能打得起3个的数量.  
	 */
	int AI_Determ_3_Greater(const BYTE* CardSlots, DCardNumber ThreeNumber) const;
	bool AI_Determ_3_Is_Max(const BYTE* CardSlots, DCardNumber DoubleNumber) const;

	int AI_Determ_S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	int AI_Determ_2S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	int AI_Determ_3S_Greater(const BYTE* CardSlots, DCardNumber Head, int Len) const;
	bool AI_Determ_S_IsMax(const BYTE* CardSlots, DCardNumber Head, int Len) const;

	int AI_QueryRemainBombers(const BYTE* CardSlots, DCardNumber GreatThen) const;

	/**
	 * 查找除我的牌外, 桌面上潜在的最大炸弹(打得起GreatThen的) 
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
	 * AI 觉得自己可以跑 
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

	////补顺子

	static void FillDouble(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static void FillIs3(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL Compose3_1(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	static BOOL Compose3_2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);

	static BOOL Compose4_2(const DHand& RawCards, const DHand& SelectedLaize, DHand& Results);

	static void ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);

	static void  ComposeStraight(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	// NumX 飞机主体中3个的数量
	static void  ComposeStraight2(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	// XPartSize : 0 : 不带翅膀, 1: 带单个 2: 带对子
	static VArray<DHand> FillStraight(const DHand& card, const DHand& laizi, int num, int laizinum);
	static void ComposePlane(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results, int NumX, int XPartSize);





	static VArray<DHand> FillStraight2(const DHand& card, const DHand& laizi, VArray<DHand>& Results);

	static void FillBom(const DHand& RawCards, const DHand& SelectedLaize, VArray<DHand>& Results);
	//填充飞机不带翅膀
	static VArray<DHand> FillIsPlane_NoWing(const DHand& card, const DHand& laizi, int num, int laizinum);
	//填充三带一
	static VArray<DHand> FillIsPlane_Wing(const DHand& card, const DHand& laizi, int num, int laizinum);

	bool InspectCardExist(DCard* card, int number);


protected:
	DCard mCards[DDECK_NUM_CARDS];			// 牌桌上有效的牌. 
	DCard mPlayedCards[DDECK_NUM_CARDS];	// 已经打出的牌. open cards.
	int mNumPlayedCards;					
	DCard mPockets[DDECK_NUM_POCKETS];		// 底牌
	int mNumCards;
	EDGameMode Deckmode;
	DDeckAIHint* mDeckAIHint;
	DCardNumber mLaiZi;
	//DHand mLastHand;
};


class TypeAtom
{
public:
	// Note: 调用之前, 确保 Cards 已经按牌的数字进行递减排序

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
	* 是否3顺子, 不带翅膀
	*/
	static DCardNumber IsPlane_NoWing(const DCard* Cards, int NumCards, int& Len);
	static DCardNumber IsPlane_Wing(const DCard* Cards, int NumCards, int& Len);

	static DCardNumber IsPlane_Follow(const DCard* Cards, int NumCards, int Len);
};

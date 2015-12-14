#pragma once
#include "DGameAI.h"
#include "DAISlover.h"


enum EHandSource
{
	HS_1	= 0x00000001,
	HS_2	= 0x00000002,
	HS_3	= 0x00000004,
	HS_4	= 0x00000008,
	HS_1S	= 0x00000010,
	HS_2S	= 0x00000020,
	HS_3S	= 0x00000040,
	HS_2K	= 0x00000080,
};

#define PEEK_SOURCE_MASK 0x000000FF

#define PEEK_FLAG_BOMB 0x00000100

static inline DGameAIHand MakeBomb(const DAtom& bomb)
{
	if (bomb.Len == 4)
	{
		return DGameAIHand(DHT_4, bomb.Number, 4);
	}
	else
	{
		VASSERT(bomb.Len == 2 && bomb.Number == DCN_RED_JOKER);
		return DGameAIHand(DHT_2K, bomb.Number, 2);
	}
}


struct DAIPotentialHand
{
	DAIPotentialHand() :Weight(0){}
	DAIPotentialHand(DHandType Type, DCardNumber Number, int Len) : Hand(Type, Number, Len), Weight(0){}
	DAIPotentialHand(DHandType Type, DCardNumber Number, int Len, EHandSource _Source) : Hand(Type, Number, Len), Weight(0),Source(_Source){}
	inline bool operator < (const DAIPotentialHand& Other) const
	{
		return Weight < Other.Weight;
	}

	DGameAIHand Hand;
	EHandSource Source;
	int		Weight;
};

typedef VPODArray<DAIPotentialHand> DAIPotentialHandList;
void PHSort(DAIPotentialHandList& Hands);
/**
 * 常规模式下的AI(无癞子) 
 * 我不觉得我们能做好有癞子的AI..... 但是现在先如此设计. 在后面可能会给出一个设定: 癞子场不允许AI进入. 
 */
class DNormalAI : public DGameAI, public DAISlover
{
public:
	DNormalAI(DDeckCharacter* Owner);
	virtual ~DNormalAI();

	virtual void UpdateCards(const DCard* Cards, int NumCards);

	/**
	* 决定跟手牌, AI必须选择一个合适的手牌打出. 如果返回NULL, 则表示AI 放弃跟牌(pass)
	* @param Hand		当前的手牌
	* @param HandOwner 当前手牌打出的角色
	*/
	virtual bool FollowHand(const DHand* Hand, DDeckCharacter* HandOwner, DHand& Result) override;

	/**
	* 出牌.
	*/
	virtual DGameAIHand PlayHand() const;
	virtual int CalcCallLordScore() const;
	virtual DDeckAIHint* GetAIHint() override { return this; }
public:
	//bool IsMaxHand();

	/**
	 * 炸掉后剩余的个子能够从指定牌中逃出去. 
	 */
	BOOL CanEscapeAfterBomb(DCardNumber RemainCardNumber) const;

	/**
	 * 选择一个打得起Number的BOMBER 且别人打不起的炸弹 
	 */
	DAtom Peek_MaxBomb(DCardNumber Number) const;

	/**
	 * 选择一个打得起Number 且能收回的炸弹 
	 */
	DAtom Peek_Bomb_CanRec(DCardNumber Number) const;

	/**
	 * 看看能不能打出一手牌, 能让自己剩余的牌全逃出??? 
	 */
	DGameAIHand Play_Hand_To_Escape(const BYTE* CardSlots) const;
	DGameAIHand Play_Hand_Normal() const;

	DGameAIHand TryEscape(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand TryEscape_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;

	/**
	 * AI 尝试跟3个,并且尝试自己逃脱.  
	 */
	DGameAIHand TryEscape_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand TryEscape_3X1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand TryEscape_3X2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;

	/**
	 * 尝试尽可能打断当前个子, 并且避免剩余的个子跑出. 
	 */
	DGameAIHand TryBreak_1_1(DCardNumber FollowNumber, DCardNumber RemainNumber) const;
	DGameAIHand TryBreak_2(DCardNumber FollowNumber, DCardNumber RemainNumber) const;
	void Peek_1(DAIPotentialHandList& PList, DCardNumber Number, UINT PeekFlags) const;


	bool IsBombCanRec(DCardNumber Bomber) const;

	bool IsHandMax() const;
private:
	bool GetHand_ToFollow(const DHand* ToFollow, const DDeckCharacter* HandOwner, DHand& Hand);
	
protected: 

	virtual DGameAIHand GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;


	void PH_Bomb(const DCardNumber Number, DAIPotentialHandList PotentialHands) const;
protected:

	inline BOOL Chance(int rate) const
	{
		int r = AIRand(100);
		return rate > r;
	}

	

	inline BOOL IsFollowFriend(const DDeckCharacter* HandOwner) const
	{
		if (HandOwner->GetCharType() == DDCT_LORD)
		{
			return FALSE;
		}
		return mOwner->GetCharType() != DDCT_LORD;
	}
	/**
	 * helper function
	 * 返回拥有者是否为地主
	 */
	inline BOOL IsLord() const { return mOwner->GetCharType() == DDCT_LORD; }
	/**
	 * 是否为地主的上家 
	 */
	inline BOOL IsPreLord() const { return mOwner->GetCharType() == DDCT_PRE_LORD; }
	/**
	 * 是否为地主的下家 
	 */
	inline BOOL IsPostLord() const { return mOwner->GetCharType() == DDCT_POST_LORD; }

	/**
	 * 是否对手仅仅一张牌? 
	 */
	BOOL IsAnyOpponentHasOnlyOne() const;


	
	DAtom Peek1_FromStraight(DCardNumber Number) const;
	DAtom Peek1_FromStraightLessThen(DCardNumber Number) const;
	DAtom Peek1_FromDouble(DCardNumber Number) const;
	DAtom Peek1_FromDoubleLessThen(DCardNumber Number) const;
	DAtom Peek1_From3(DCardNumber Number) const;
	DGameAIHand FHand1_Normal(DCardNumber Number, bool AllowBomb) const;
	int GetBombRateByHand() const;

	DAtom Peek2_LessThen(DCardNumber Number) const;
	DAtom Peek2_FromDoubleStaight(DCardNumber Number) const;
	DAtom Peek2_FromDoubleStaight_LessThen(DCardNumber Number) const;
	DCardNumber Peek2_Force(DCardNumber Number, bool Ascend = true) const;
	//DCardNumber Peek2_CompositionSingleAndStraight(DCardNumber Number) const;
	/**
	 * 最基本的跟对子 
	 */
	DGameAIHand FHand2_Normal(DCardNumber Number, bool AllowBomb) const;


	DGameAIHand Peek_3_1(DCardNumber FollowNumber) const;
	DGameAIHand Peek_3_2(DCardNumber FollowNumber) const;
	DGameAIHand Peek_3_1_Descend(DCardNumber FollowNumber) const;
	DGameAIHand Peek_3_2_Descend(DCardNumber FollowNumber) const;

	DCardNumber FH_Peek_X1_For_3_1(bool SpliteDouble) const;
	DCardNumber FH_Peek_X2_For_3_2() const;
	DGameAIHand Peek_3_x() const;
	
	/**
	 * 尝试从3顺中拆掉一个3来 
	 */
	DAtom Peek3_From3S(DCardNumber FollowNumber, bool Ascender = true) const;

	/**
	* 尝试找到一个合适的3顺(飞机)
	*/
	DGameAIHand Peek_3_straight() const;
	bool Peek_3_straight_1(DAIXInfo* XInfos, int x) const;		// 从单个中组合
	bool Peek_3_straight_1_2(DAIXInfo* XInfos, int x) const;	// 从单个和对子中组合
	bool Peek_3_straight_1_s(DAIXInfo* XInfos, int x) const;	// 飞机. 从单个和顺子中组合
	bool Peek_3_straight_2(DAIXInfo* XInfos, int x) const;		// 从对子中组合
	bool Peek_3_straight_2_s(DAIXInfo* XInfos, int x) const;	// 对双顺和对子中组合

	DGameAIHand Peek_Straight(DCardNumber Number, int Len) const;
	DGameAIHand Peek_Straight2(DCardNumber Number, int Len) const;
	DGameAIHand Peek_Straight3(DCardNumber Number, int Len) const;

	//virtual DHand* GetHand
	DGameAIHand PlayHand_Min_NoSingle(DCardNumber Remain1Number) const;
	DGameAIHand PlayHand_Min_GuardSingle(DCardNumber Remain1Number) const;
	DGameAIHand PlayHand_GuardDouble(DCardNumber RemainDoubleNumber) const;
	void GetAllPlayableHands(DAIPotentialHandList& Hands) const;
	virtual DGameAIHand GetHand_Min() const;
	DGameAIHand GetStraight() const;
	DGameAIHand GetStraight2() const;


	bool AI_Determ_CanEscape(const DGameAIHand& AfterHand = DGameAIHand::PASS) const;
#if 0

	

	/**
	 * 获得较小的非单张牌.  
	 */
	virtual DHand* GetHand_Min_NoSingle();

	/**
	 * 获得最有价值的一手手牌 
	 */
	virtual DHand* GetMostStrengthHand();
	//virtual UINT CalcHandValue(const DCards* Cards, int NumCards);


	
	/**
	 * 尝试找出一个合适的3带1/2
	 */
	virtual DHand* GetHand_3_x() const;



	DHand* GetHand_1() const;

	DHand* GetHand_2() const;

	DHand* GetHand_straight() const;

	DHand* GetHand_2_straight() const;

	DHand* GetHand_Bomb() const;

	DHand* GetHand_2K() const;
#endif 
	
	int GetBomb_2_Rate(DCardNumber Number) const;
	
	//virtual DHand* FollowHand_1_Normal(DCardNumber Number, BOOL FollowFriend) const;

	/**
	 *  
	 */
	//virtual DHand* FollowHand_1_Bomb(DCardNumber Number) const;

	/**
	 * 尝试在保留X的情况下跟牌 
	 */
	DGameAIHand GetHand_1_ToFollow_CareX(DCardNumber Number) const;
	DGameAIHand GetHand_1_ToFollow_DontCareX(DCardNumber Number) const;
	/**
	 * 从对子中拆出个子 
	 */
	DHand* GetHand_1_Split2(DCardNumber Number) const;

	DHand* GetHand_1_Splits(DCardNumber Number) const;

	DHand* GetHand_1_Max(DCardNumber Number) const;

	DHand* FH_Peek_2(DCardNumber Number) const;
	DHand* FH_Peek_2_Split2S(DCardNumber Number) const;

	DHand* FH_Peek_3(DCardNumber Number) const;
	DHand* FH_Peek_3_1(DCardNumber Number) const;


	DHand* FH_Peek_3_2(DCardNumber Number) const;
	int GetBomb_3_Rate(DCardNumber Number) const;


	DHand* FH_Peek_Bomb(DCardNumber Number) const;

	//DHand* FH_Peek_1_ToFollow(DCardNumber Number, DCardNumber BiggerThen);
private:		
	struct AICardSlot
	{
		inline AICardSlot() :NumCards(0){}
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

	mutable AICardSlot mAICardSlot[DCN_RED_JOKER + 1];	// 用于保存玩家当前手牌, 将DGameAIHand 重新翻译成可识别的牌. 
	void TranslateHandFromGameAIHand(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_1(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_2(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_3(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_3x1(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_3x2(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_4(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_4x2(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_1s(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_2s(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_3s(const DGameAIHand& AIHand, DHand& Hand) const;
	void TranslateHand_2k(const DGameAIHand& AIHand, DHand& Hand) const;
	void _GetCard(DHand& Hand, const DCardNumber Number, int Num) const;
	
};


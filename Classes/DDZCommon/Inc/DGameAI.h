#pragma once

#include "DDeck.h"

#if defined(_WIN32) && VDEBUG
#define DAI_DEBUG 1
#else 
#define DAI_DEBUG 0
#endif 


struct DAIXInfo
{
	inline DAIXInfo() :X(DCN_NULL), XLen(0){}
	inline DAIXInfo(DCardNumber _inN, int Len) : X(_inN), XLen(Len){}
	DCardNumber		X;
	char XLen;
};

struct DGameAIHand
{
	inline DGameAIHand() :Type(DHT_UNKNOWN), Number(DCN_NULL), Len(0){}
	/*inline DGameAIHand(const DGameAIHand& AIHand) 
		: Type(AIHand.Type), Number(AIHand.Number), Len(AIHand.Len)
		, X[0](AIHand.X[0])
	{
	}*/
	inline DGameAIHand(DHandType _Type, DCardNumber _Number, int _Len)
		: Type(_Type), Number(_Number), Len(_Len)
	{
	}
	inline DGameAIHand(DHandType _Type, DCardNumber _Number, int _Len, const DAIXInfo& _inX)
		: Type(_Type), Number(_Number), Len(_Len)
	{
		X[0].X = _inX.X;
		X[0].XLen = _inX.XLen;
	}

	inline ~DGameAIHand(){}
	inline operator bool() const
	{
		return Type != DHT_UNKNOWN;
	}

	inline void Invalid()
	{
		Type = DHT_UNKNOWN;
	}

	inline bool IsValid() const { return Type != DHT_UNKNOWN; }
	inline bool IsBreak() const { return Type == DHT_UNKNOWN && Len == -1; }
	
	DHandType		Type;
	DCardNumber		Number;
	short			Len;
	DAIXInfo		X[4];
	static const DGameAIHand PASS;
	static const DGameAIHand BREAK;
};


class DGameAI
{
public:
	DGameAI(DDeckCharacter* Owner);
	virtual ~DGameAI();
	virtual void Release();

	static DGameAI* CreateAI(DDeckCharacter* Owner);
	static DDeckAIHint* CreatePlayerAIHint();

	virtual void UpdateCards(const DCard* Cards, int NumCards) = 0;


	/**
	 * 决定跟手牌, AI必须选择一个合适的手牌打出. 如果返回NULL, 则表示AI 放弃跟牌(pass)
	 * @param Hand		当前的手牌
	 * @param HandOwner 当前手牌打出的角色
	 */
	virtual bool FollowHand(const DHand* Hand, DDeckCharacter* HandOwner, DHand& Result) = 0;


	/**
	 *  
	 */
	static int GetHandPower(const DCard* Cards);

	virtual int CalcCallLordScore() const;
	
	virtual DDeckAIHint* GetAIHint() { return nullptr; }
protected:
	DDeckCharacter* mOwner;
	DDeck* mDeck;
};

/**
 * 用于提示有效的牌型 
 */
class DHint
{
public:
	DHint();
	~DHint();

	inline BOOL CanHint() const { return mHintIndex != -1; }

	BOOL HintInit(const DCard* Cards, int NumCards, const DHand* LastHand, DCardNumber Laizi = DCN_NULL);


	const DHand* Hint() const;

	void HintClose();


	void Shutdown()
	{
		HintClose();
		mValidHands.Free();
	}
private:
	VArray<DHand> mValidHands;
	mutable int mHintIndex;
	class DHintUnslover* mHintUnslover;
	class DHintSlover* mHintSlover;
};
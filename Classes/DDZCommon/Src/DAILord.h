#pragma once
#include "DNormalAI.h"

// 地主的AI规则
class DAILord : public DNormalAI
{
public:
	DAILord(DDeckCharacter* Owner);
	~DAILord();
protected:
	BOOL IsAnyOpponentHasOnlyOne() const;
	BOOL IsAnyOpponentWarning() const;
	DCardNumber GetGuardSingle() const;
	DDeckAIHint* GetGuardOpponent(const DDeckAIHint& PrevLord, const DDeckAIHint& PostLord) const;

	virtual DGameAIHand PlayHand() const override;
	DGameAIHand Play_Hand_To_Escape() const;
	DGameAIHand Play_Normal() const;

	virtual DGameAIHand GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand GuardOpponent_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;

	virtual DGameAIHand GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand GuardOpponent_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;

	//virtual DHand* GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	//virtual DHand* GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
};


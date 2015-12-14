#pragma once
#include "DAIFarmer.h"

class DAIPreLord : public DAIFarmer
{
public:
	DAIPreLord(DDeckCharacter* Owner);
	~DAIPreLord();

	struct OpposeLord
	{
		OpposeLord() :NeedOppose(false){}
		DCardNumber RemainNumber;
		bool RemainIsDouble;
		bool NeedOppose;
		int NumRemain;

	};

	bool NeedBombLord(DDeckCharacter* Lord, float BombRateFactor = 1.0f) const;

	bool CanWin() const;

	DGameAIHand PlayHand() const override;
	DGameAIHand Play_GuardLord() const;
	DGameAIHand Play_AssistFriend() const;
	DGameAIHand Play_Normal() const;

	virtual DGameAIHand GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand GuardLord_1(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const;
	DGameAIHand TryPrevent1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;

	
	void TryOpposeLord(const DHand* ToFollow, const DDeckCharacter* HandOwner, OpposeLord& Fallback) const;

	virtual DGameAIHand GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand GuardLord_2(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const;
	DGameAIHand Get2_FroLord(DCardNumber FollowNumber, const DDeckAIHint& LordHint) const;


	virtual DGameAIHand GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand GuardLord_3(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const;


	virtual DGameAIHand GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	
	//DGameAIHand Get3_FroLord(DCardNumber FollowNumber, const DDeckAIHint& LordHint) const;



	virtual DGameAIHand GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
};


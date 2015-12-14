#pragma once
#include "DNormalAI.h"

class DAIFarmer : public DNormalAI
{
public:
	DAIFarmer(DDeckCharacter* Owner);
	~DAIFarmer();

	DDeckCharacter* GetFriend() const;
	DDeckCharacter* GetLord() const;
	DHand* PlayHand();
	inline BOOL IsFriend(const DDeckCharacter* Other) const
	{
		return (Other->GetCharType() != DDCT_LORD);
	}
	DGameAIHand Play_Hand_To_Escape(const DDeckAIHint& LordHint) const;
	//DGameAIHand TryEscape_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
//	virtual DHand* GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;


	BOOL HasSingleSmallerThen(DCardNumber Number, DCardNumber ExceptBomber = DCN_NULL) const;


	
};


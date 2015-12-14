#pragma once
#include "DAIFarmer.h"

class DAIPostLord : public DAIFarmer
{
public:
	DAIPostLord(DDeckCharacter* Owner);
	~DAIPostLord();
	DGameAIHand PlayHand() const override;

#pragma region "����"
	virtual DGameAIHand GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const override;

	DGameAIHand TryHelpEscape_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	DGameAIHand TryLetEscape1AfterBomb_1(DCardNumber FollowNumber, const DDeckCharacter* Friend) const;
	DGameAIHand TryLetEscape1AfterPlus_1(const DHand* ToFollow, const DDeckCharacter* Friend) const;
	DGameAIHand TryHelpEscape1AfterBomb_1(const DHand* ToFollow, const DDeckCharacter* Friend) const;
	DGameAIHand TryHelpEscape1AfterPlus_1(const DHand* ToFollow, const DDeckCharacter* Friend) const;

	DGameAIHand TryPrevent_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
#pragma endregion "����"
	//////////////////////////////////////////////////////////////////////////
	// �Ӷ���
	//////////////////////////////////////////////////////////////////////////
#pragma region "����"
	virtual DGameAIHand GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const override;
	DGameAIHand TryHelpEscape_2(const DHand* ToFollow, const DDeckCharacter* HandOwner, bool& Continue) const;

	// �¼�ʣ��1������, ���޷�����ǰ�Ƶ������, ��ը��Э����ʤ�� 
	DGameAIHand TryHelpEscape_Remain1_WithBomb(const DHand* ToFollow, const DDeckCharacter* Friend) const;
	DGameAIHand TryHelpEscape_Remain2_WithBomb(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber RemainDoubleNumber) const;

	DGameAIHand TryHelpEscape_Remain1_With2(const DHand* ToFollow, const DDeckCharacter* Friend, bool& Continue) const;
	DGameAIHand TryLetEscape2AfterBomb_2(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber FriendRemainDoubleNumber) const;
	bool AI_Determ_Double_Can_Escape(DCardNumber FriendRemainDoubleNumber, float RateScale) const;
	DGameAIHand TryLetEscape2AfterPlus_2(const DHand* ToFollow, const DDeckCharacter* Friend, DCardNumber FriendRemainDoubleNumber) const;
	DGameAIHand TryHelpEscape_Remain2_2(const DHand* ToFollow, const DDeckCharacter* Friend) const;
	DGameAIHand TryPrevent_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
#pragma endregion "����"

	enum HelpAction
	{
		DontCare = 0,			// ��Ҫ�ں��¼�, �Լ����Լ���.
		NeedPassOneHand,		// �¼���Ҫ��һ��, ��������. 
		CallHelp,				// �¼��޷�����, �����ܴ��Ƹ���
		
	};
	DGameAIHand TryHelpEscapeCommon(const DHand* ToFollow, const DDeckCharacter* HandOwner, int& NextAction) const;
#pragma region "3"
	virtual DGameAIHand GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const override;
	virtual DGameAIHand GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const override;
	virtual DGameAIHand GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const override;
#pragma endregion "3"

	virtual DGameAIHand GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	virtual DGameAIHand GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const;
	

	virtual DGameAIHand Play_Help() const;
	virtual DGameAIHand Play_Prevent() const;

};


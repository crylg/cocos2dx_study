#include "VStdAfx.h"
#include "DAIFarmer.h"


DAIFarmer::DAIFarmer(DDeckCharacter* Owner) :DNormalAI(Owner)
{

}


DAIFarmer::~DAIFarmer()
{
}

DDeckCharacter* DAIFarmer::GetFriend() const
{
	if (mOwner->GetCharType() == DDCT_PRE_LORD)
	{
		return mDeck->GetPlayer(DDCT_POST_LORD);
	}
	else
	{
		return mDeck->GetPlayer(DDCT_PRE_LORD);
	}
	
}

DDeckCharacter* DAIFarmer::GetLord() const
{
	return mDeck->GetPlayer(DDCT_LORD);
}


DHand* DAIFarmer::PlayHand()
{
	

	return NULL;
}


/**
* �ж��߼�:
����������. 
��ȥ3���ĸ��ӻ����. 
Ȼ��, ���ֻ��2��, ��ô��Ȼһ����������. �����, ���Ǵ������. 
*/
DGameAIHand DAIFarmer::Play_Hand_To_Escape(const DDeckAIHint& LordHint) const
{
	int NumHands = GetRawHandNum();
	int NumX = NumXNeedFor_3x_And_Plane();

	if (m2K)
	{
		NumHands--;
	}

	if (NumX > 1)
	{
		//Ŀǰ ��������ֻ��һ��3�������.
		return DGameAIHand::PASS;
	}else if (NumX == 1)
	{
		NumHands--;
	}

	if (NumHands > 2)
	{
		// ʣ���������ڿ��Ƿ�Χ֮��. 
		return DGameAIHand::PASS;
	}


		// ���ȳ�3��.
	for (int i = 0; i < m3Atoms.size(); ++i)
	{
		const DAtom& A = m3Atoms[i];
		int Greater = LordHint.AI_Determ_3_Greater(A.Number);
		if (Greater == 0)
		{
			DGameAIHand AIHand = Peek_3_x();
			if (AIHand)
			{
				return AIHand;
			}
		}
	}

	for (int i = 0; i < m1SAtoms.size(); ++i)
	{
		const DAtom& A = m1SAtoms[i];
		int Greater = LordHint.AI_Determ_S_Greater(A.Number, A.Len);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_straight, A.Number, A.Len);
		}
	}

	for (int i = 0; i < m2SAtoms.size(); ++i)
	{
		const DAtom& A = m2SAtoms[i];
		int Greater = LordHint.AI_Determ_2S_Greater(A.Number, A.Len);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_straight_2, A.Number, A.Len);
		}
	}

	for (int i = 0; i < m3SAtoms.size(); ++i)
	{
		const DAtom& A = m3SAtoms[i];
		int Greater = LordHint.AI_Determ_3S_Greater(A.Number, A.Len);
		if (Greater == 0)
		{
			return Peek_3_straight();
		}
	}


	for (int i = 0; i < m4Atoms.size(); ++i)
	{
		const DAtom& A = m4Atoms[i];
		int Greater = LordHint.AI_QueryRemainBombers(A.Number);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_4, A.Number, 4);
		}
	}

	for (int i = 0; i < m2Atoms.size(); ++i)
	{
		const DAtom& A = m2Atoms[i];
		int Greater = LordHint.AI_Determ_Double_Greater(A.Number);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_2, A.Number, 2);
		}
	}

	// ���Կ����Ƿ���һ��������, �����, ��������. 
	for (int i = 0; i < m1Atoms.size(); ++i)
	{
		const DAtom& A = m1Atoms[i];
		int Greater = LordHint.AI_Determ_Single_Max(A.Number, true);
		if (Greater == 0)
		{
			return DGameAIHand(DHT_1, A.Number, 1);
		}
	}

	

	


	return DGameAIHand::PASS;
}

#if 0

DHand* DAIFarmer::GetHand_ToFollow_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	BOOL IsFriendHand = IsFriend(HandOwner);
	DCardNumber Number = ToFollow->GetHandNumber();
	
	if (mHand.Size() == 1 && mHand[0].Number > Number)
	{
		DHand* Result = VNew DHand(DHT_1, mHand[0].Number);
		Result->AddCard(mHand[0]);
		return Result;
	}

	//if (mHand.last() == 2)
	//{

	//}

	if (IsFriendHand && Number >= DCN_A)
	{
		return NULL;
	}

	DHand* Result = FollowHand_1_Normal(Number, IsFriendHand);
	if (Result)
	{
		return Result;
	}

	return NULL;
}


DHand* DAIFarmer::GetHand_ToFollow_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_2(ToFollow,HandOwner);
}

DHand* DAIFarmer::GetHand_ToFollow_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_3_1(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3_1(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_3_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_3_2(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_straight(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_straight_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight_2(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_straight_3(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_straight_3(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_4(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_4(ToFollow, HandOwner);
}
DHand* DAIFarmer::GetHand_ToFollow_4_2(const DHand* ToFollow, const DDeckCharacter* HandOwner) const
{
	return DNormalAI::GetHand_ToFollow_4_2(ToFollow, HandOwner);
}


#endif 
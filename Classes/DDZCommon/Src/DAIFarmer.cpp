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
* 判断逻辑:
除掉对王后. 
除去3带的个子或对子. 
然后, 如果只有2手, 那么必然一手是最大的牌. 如果是, 我们打出最大的. 
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
		//目前 仅仅处理只有一个3带的情况.
		return DGameAIHand::PASS;
	}else if (NumX == 1)
	{
		NumHands--;
	}

	if (NumHands > 2)
	{
		// 剩余手数不在考虑范围之内. 
		return DGameAIHand::PASS;
	}


		// 优先出3带.
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

	// 尝试看看是否有一手是最大的, 如果是, 打出最大手. 
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
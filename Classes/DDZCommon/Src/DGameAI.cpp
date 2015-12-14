#include "VStdAfx.h"
#include "DGameAI.h"
#include "DAILord.h"
#include "DAIPreLord.h"
#include "DAIPostLord.h"

const DGameAIHand DGameAIHand::PASS(DHT_UNKNOWN, DCN_NULL, 0);
const DGameAIHand DGameAIHand::BREAK(DHT_UNKNOWN, DCN_NULL, -1);

DGameAI* DGameAI::CreateAI(DDeckCharacter* Owner)
{
	VASSERT(Owner);
	DDeckCharacterType CharType = Owner->GetCharType();
	if (CharType == DDCT_LORD)
	{
		return new DAILord(Owner);
	}else if (CharType == DDCT_PRE_LORD)
	{
		return new DAIPreLord(Owner);
	}else if (CharType == DDCT_POST_LORD)
	{
		return new DAIPostLord(Owner);
	}
	return NULL;
}

DDeckAIHint* DGameAI::CreatePlayerAIHint()
{
	DAISlover* Slover = new DAISlover;
	return Slover;
}


DGameAI::DGameAI(DDeckCharacter* Owner)
	:mOwner(Owner)
	, mDeck(Owner->GetDeck())
{

}


DGameAI::~DGameAI()
{
}

void DGameAI::Release()
{
	delete this; 
}
 

int DGameAI::CalcCallLordScore() const
{
	return 0;
}
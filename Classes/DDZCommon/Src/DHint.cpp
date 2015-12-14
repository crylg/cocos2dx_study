#include "VStdAfx.h"
#include "DGameAI.h"
#include "DHintUnslover.h"
class DHintSlover
{

};


DHint::DHint()
{
	mHintUnslover = new DHintUnslover;
//	mHintSlover = new DHintSlover;
	mHintIndex = -1;
}

DHint::~DHint()
{
	//HintClose();
	//delete mHintSlover;
	delete mHintUnslover;
}

BOOL DHint::HintInit(const DCard* Cards, int NumCards, const DHand* LastHand, DCardNumber Laizi)
{
	VASSERT(LastHand && Cards && NumCards);
	HintClose();
	mHintUnslover->Reset(Cards, NumCards, Laizi);
	mHintUnslover->GetValidHands(LastHand, mValidHands);

	if (!mValidHands.IsEmpty())
	{
		mHintIndex = 0;
		return TRUE;
	}

	return FALSE;
}

const DHand* DHint::Hint() const
{
	if(mHintIndex == -1) return NULL;
	const DHand& Result = mValidHands[mHintIndex];
	mHintIndex = mHintIndex + 1;
	if (mHintIndex == (int)mValidHands.Size())
	{
		mHintIndex = 0;
	}
	return &Result;
}

void DHint::HintClose()
{
	mHintIndex = -1;
	mValidHands.Clear();
}

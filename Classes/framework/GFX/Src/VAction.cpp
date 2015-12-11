#include "VStdAfx.h"
#include "VAction.h"
USING_NS_CC;

VParaMove* VParaMove::create(const cocos2d::Point& DestPoint, float Duration)
{
	VParaMove *pRet = VNew VParaMove();
	if (pRet && pRet->init(Duration, DestPoint))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

bool VParaMove::init(float d, const cocos2d::Point& DestPoint)
{
	if (CCActionInterval::initWithDuration(d))
	{
		mDestPos = DestPoint;
		return true;
	}
	return false;
}

void VParaMove::startWithTarget(Node *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	mFromPos = pTarget->getPosition();

	// Note: 我们并不是一个完整的抛物线. 而是取抛物线在前阶段的一半进行模拟. 物体到达目标点位置时依然有上升的速度. 

	mVelocityX = (mDestPos.x - mFromPos.x) / _duration;

	
	if (mDestPos.y >= mFromPos.y)
	{
		// 目标点在上方.
		float H = mDestPos.y - mFromPos.y;

		mAccY = -(H * 2.0f) / (_duration * _duration);
		mVelocityY = (H - (0.5*mAccY*_duration*_duration)) / _duration;

	}

}

void VParaMove::update(float dt)
{
	dt = dt * _duration;
	Point Curr;
	Curr.x = mFromPos.x + mVelocityX*dt;
	Curr.y = mFromPos.y + mVelocityY*dt + 0.5f * mAccY * dt * dt;
	_target->setPosition(Curr);
}

void VParaMove::stop(void)
{
	// Action is done, reset clip position  
	CCActionInterval::stop();
}

ActionInterval* VParaMove::clone() const
{
	VParaMove* move = VNew VParaMove;
	move->init(_duration, mDestPos);
	move->autorelease();
	return move;
}


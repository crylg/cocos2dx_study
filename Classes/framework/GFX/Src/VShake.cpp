#include "VStdAfx.h"
#include "VShake.h"
USING_NS_CC;

VShake::VShake() : m_strength_x(0), m_strength_y(0), m_initial_x(0), m_initial_y(0)
{
}

VShake* VShake::create(float d, float strength)
{
	// call other construction method with twice the same strength   
	return createWithStrength(d, strength, strength);
}

VShake* VShake::createWithStrength(float duration, float strength_x, float strength_y)
{
	VShake *pRet = VNew VShake();
	if (pRet && pRet->initWithDuration(duration, strength_x, strength_y))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

bool VShake::initWithDuration(float duration, float strength_x, float strength_y)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_strength_x = strength_x;
		m_strength_y = strength_y;
		return true;
	}
	return false;
}

static float fgRangeRand(float min, float max)
{
	float rnd = ((float)rand() / (float)RAND_MAX);
	return rnd*(max - min) + min;
}

void VShake::update(float dt)
{
	float randx = fgRangeRand(-m_strength_x, m_strength_x)*dt;
	float randy = fgRangeRand(-m_strength_y, m_strength_y)*dt;

	// move the target to a shaked position   
	_target->setPosition(Vec2(m_initial_x + randx,
		m_initial_y + randy));
}

void VShake::startWithTarget(Node *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	// save the initial position   
	m_initial_x = pTarget->getPosition().x;
	m_initial_y = pTarget->getPosition().y;
}

void VShake::stop(void)
{
	// Action is done, reset clip position  
	_target->setPosition(Vec2(m_initial_x, m_initial_y));
	CCActionInterval::stop();
}

ActionInterval* VShake::clone() const
{
	VShake* shake = VNew VShake;
	shake->initWithDuration(_duration, m_strength_x, m_strength_y);
	shake->autorelease();
	return shake;
}


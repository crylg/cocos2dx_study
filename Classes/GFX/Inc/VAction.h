#pragma once

/**
 * Å×ÎïÏßÒÆ¶¯ 
 */
class VCoreApi VParaMove : public cocos2d::ActionInterval, public VAllocator
{
public:
	VParaMove() :mVelocityX(0.0f), mVelocityY(0.0f), mAccY(0.0F){}
	// Create the action with a time and a strength (same in x and y)   
	static VParaMove* create(const cocos2d::Point& DestPoint, float Duration);
	// Create the action with a time and strengths (different in x and y)   
	bool init(float d, const cocos2d::Point& DestPoint);

	virtual ActionInterval *clone() const override;
protected:
	virtual void DeleteThis(){ VDelete this; }
	void startWithTarget(cocos2d::Node *pTarget);
	void update(float time);
	void stop(void);
	float mVelocityX;
	float mVelocityY;
	float mAccY;
	cocos2d::Point mFromPos;
	cocos2d::Point mDestPos;
};



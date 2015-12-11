#pragma once

class VCoreApi VShake : public cocos2d::ActionInterval, public VAllocator
{
public:
	VShake();
	// Create the action with a time and a strength (same in x and y)   
	static VShake* create(float d, float strength);

	// Create the action with a time and strengths (different in x and y)   
	static VShake* createWithStrength(float d, float strength_x, float strength_y);
	bool initWithDuration(float d, float strength_x, float strength_y);

	virtual ActionInterval *clone() const override;
protected:
	virtual void DeleteThis(){ VDelete this; }
	void startWithTarget(cocos2d::Node *pTarget);
	void update(float time);
	void stop(void);
	// Initial position of the shaked node   
	float m_initial_x, m_initial_y;
	// Strength of the action   
	float m_strength_x, m_strength_y;
};


#pragma once
#include "VAvataInfo.h"
class VAvataInfoOther :
	public VAvataInfo
{
public:
	VAvataInfoOther();
	~VAvataInfoOther();
	CREATE_FUNC(VAvataInfoOther);
	virtual void initData(void* data);
	virtual void resetData();
protected:
	enum eTag
	{
		avata_null_sprite = 10004,
		farmer_left_sprite = 1394,
		farmer_right_sprite = 82
	};
	virtual void requestChildTag();
	virtual void initRootViewAndrootOfRequestNode();
	cocos2d::Sprite* _farmerSpriteRight;
	cocos2d::Sprite* _farmerSpriteLeft;
	cocos2d::Sprite* _avataNullSprite;
};


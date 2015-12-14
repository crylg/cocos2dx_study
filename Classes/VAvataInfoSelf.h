#pragma once
#include "VAvataInfo.h"
class VAvataInfoSelf :
	public VAvataInfo
{
public:
	VAvataInfoSelf();
	~VAvataInfoSelf();
	CREATE_FUNC(VAvataInfoSelf);
	virtual void initData(void* data);
	virtual void resetData();
protected:
	enum eTag
	{
		farmer_sprite = 1491,
		money_button=1495,
		chat_button=1492
	};
	virtual void requestChildTag();
	virtual void initRootViewAndrootOfRequestNode();
	cocos2d::Sprite* _farmerSprite;
	cocos2d::ui::Button* _moneyButton;
	cocos2d::ui::Button* _chatButton;
};


#pragma once
#include "cocos2d.h"
class VAvataInfo :public VView
{
public:
	static VAvataInfo* create();
	bool init();
protected:
	cocos2d::ui::Text* _nameText;
	cocos2d::ui::Text* _moneyText;
	cocos2d::Sprite* _avataSprite;
	cocos2d::Sprite* _farmerSpriteRight;
	cocos2d::Sprite* _farmerSpriteLeft;
};


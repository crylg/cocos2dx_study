#pragma once
#include "cocos2d.h"
class VAvataInfo :public VView
{
public:
	//static VAvataInfo* create();
	CREATE_FUNC(VAvataInfo);
	//void initLayout(cocos2d::Node* parent);
	virtual void initData(void* data);
	virtual void resetData();
protected:
	enum eTag
	{
		avata_sprite = 10001,
		avata_null_sprite = 10004,
		farmer_left_sprite = 1394,
		farmer_right_sprite = 82,
		name_text = 10002,
		money_text = 10003,
		head = 10010
	};
	bool init();
	cocos2d::ui::Text* _nameText;
	cocos2d::ui::Text* _moneyText;
	cocos2d::Sprite* _avataSprite;
	cocos2d::Node* _rootView;
	cocos2d::Node* _rootOfRequestNode;
	virtual void requestChildTag();
	virtual void initRootViewAndrootOfRequestNode();
};


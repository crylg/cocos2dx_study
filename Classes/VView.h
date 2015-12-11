#pragma once
#include "stdafx.h"
#include "cocos2d.h"
class VView:public cocos2d::Node
{
public:
	cocos2d::Node* LoadView(const char* FileName);
	//cocos2d::Node* getSkin();
	//~VView();
protected:
	//cocos2d::Node* _skin;
};


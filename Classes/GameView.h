#pragma once
//ÓÎÏ·Ö÷³¡¾°
#include "cocos2d.h"
USING_NS_CC;
class PlayerInfoView;
class GameView:public cocos2d::Layer
{
	
public:
	GameView();
	~GameView();
public:

	static cocos2d::Scene* createScene();

	virtual bool init();
	CREATE_FUNC(GameView);
private:
	PlayerInfoView* _playerInfoView;
};


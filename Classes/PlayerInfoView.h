#pragma once
#include "cocos2d.h"

class VAvataInfo;//旁边的信息
class PlayerInfoView :public VView
{

public:
	//static PlayerInfoView* create();
	CREATE_FUNC(PlayerInfoView);

	enum ePlayerType
	{
		Self,
		Other
	};
	enum ePosition
	{
		RB,
		RT,
		LT,
		LB,
		P_Self
	};
	bool init();
	void layout(ePlayerType type, ePosition p);
private:
	//是否为庄家
	bool _juddgeIsFarmer;
	//VAvata* _vAvata;
	VAvataInfo* _vAvataInfo;
	cocos2d::Node* _vBg;//背景
	void createBg(ePlayerType type);
	void createAvataInfo(ePlayerType type);
	cocos2d::Vec2 getPositionByEPosition(ePosition p);
	//void createAvata();
	
};
//playerInfoView implements INode
//playerInfoView->getSkin return INode
//addChild(INode)
//parent.addChild(_playerInfoView->getSkin());//使所有加载成为代理加载,没必要,就算重载也得把INode属性全重写 

